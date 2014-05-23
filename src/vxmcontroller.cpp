#include "vxmcontroller.h"
#include <sstream>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

VXMController::VXMController(QObject *parent) :
    QObject(parent)
{
   m_serialConnection = new QSerialPort();
   m_connected = false;
   m_maxSpeed = 2000;

   connect(m_serialConnection, SIGNAL(readyRead()), this, SLOT(serialReadyReadSlot()));
}

void VXMController::openSerialConnection(SerialSettings s) {
    m_serialConnection->setPortName(s.portName);
    if (m_serialConnection->open(QIODevice::ReadWrite)) {
        if (m_serialConnection->setBaudRate(s.baudRate)
                && m_serialConnection->setDataBits(s.dataBits)
                && m_serialConnection->setParity(s.parity)
                && m_serialConnection->setStopBits(s.stopBits)
                && m_serialConnection->setFlowControl(s.flowControl)) {

            m_connected = true;
            emit serialConnected();
            if (loggedWrite("F V\n") == -1)
                QMessageBox::critical(0, "Error", "Could not write to serial port");
        }
    }
}

void VXMController::closeSerialConnection() {
    if (m_serialConnection->isOpen()) {
        m_serialConnection->close();
    }
    m_connected = false;
    emit serialDisconnected();
}

bool VXMController::isSerialOpen() {
    return m_connected;
}

void VXMController::move(QPoint p) {
    if (p.x() == 0 && p.y() == 0)
        return;

    emit serialBusy();

    MovementVect mv = generateMovementVect(p);

    std::ostringstream comm;
    comm << "C" << std::endl; // start by clearning the previous program
    comm << "S3M" << mv.xSpeed << ",S1M" << mv.ySpeed; // set the speeds for each motor
    comm << ",(I3M" << mv.xDist << ",I1M" << mv.yDist << ",)"; // run both movement commands simultainiously
    comm << ",R" << std::endl; // run program

    QByteArray data(comm.str().c_str());

    if (loggedWrite(data) == -1) {
        QMessageBox::critical(0, "Error", "Could not write to serial port");
    }
}

void VXMController::savePosition() {
    QByteArray data("C\nIA1M-0,IA3M-0,R");
    if (loggedWrite(data) == -1) {
        QMessageBox::critical(0, "Error", "Could not write to serial port");
    }
}

void VXMController::returnToSavedPosition() {
    QByteArray data("C\nIA1M0,IA3M0,R");
    if (loggedWrite(data) == -1) {
        QMessageBox::critical(0, "Error", "Could not write to serial port");
    }
}


void VXMController::newQueue() {
    m_estimatedTime = 0;

    m_queuedCommands.clear();
    m_queuedLines.clear();
    m_queuedCurves.clear();
    m_queuedPauses.clear();
}

void VXMController::addSavePositionToQueue() {
    m_queuedCommands.push_back(SAVE_POSITION);
}

void VXMController::addReturnToQueue() {
    m_queuedCommands.push_back(RETURN_TO_SAVED_POSITION);
    // this adds execution time, but the amount of time is non-deterministic
    // This command should only be put at the end of the queue in order to make
    // its execution time irrelevant (entity has already been drawn before it
    // executes)
}

void VXMController::addMoveToQueue(QPoint p) {
    MovementVect mv = generateMovementVect(p);
    m_estimatedTime += mv.time;
    m_queuedLines.push_back(mv);
    m_queuedCommands.push_back(LINE);
}

void VXMController::addCurveToQueue(std::list<QPoint> ps) {
    if (m_queuedCurves.size() >= 4) {
        // controller can only have 4 programs at a time
        // maybe there sould be some sort of error indicator?
        return;
    }
    double time = 0;
    std::list<MovementVect> curve;
    for (std::list<QPoint>::iterator it = ps.begin(); it != ps.end(); it++) {
        MovementVect mv = generateMovementVect(*it);
        time += mv.time;
        curve.push_back(mv);
    }
    m_estimatedTime += time;
    m_queuedCurves.push_back(curve);
    m_queuedCommands.push_back(CURVE);
}

void VXMController::addPauseToQueue(int t) {
    m_queuedPauses.push_back(t);
    m_queuedCommands.push_back(PAUSE);
    m_estimatedTime += t;
}

void VXMController::addOutputHighToQueue() {
    m_queuedCommands.push_back(OUTPUT_HIGH);
}

void VXMController::addOutputLowToQueue() {
    m_queuedCommands.push_back(OUTPUT_LOW);
}

double VXMController::getEstimatedExecTime() {
    return m_estimatedTime;
}

void VXMController::execQueue() {
    if (m_queuedCurves.empty())
        return;

    emit serialBusy();

    std::ostringstream mp[5]; // master programs 0-4
    std::ostringstream sp[5]; // slave programs 0-4
    int cpi = 0; // current program index
    bool inContinuousMode = false;

    std::list<MovementVect>::iterator line_it = m_queuedLines.begin();
    std::list<std::list<MovementVect> >::iterator curve_it = m_queuedCurves.begin();
    std::list<int>::iterator pause_it = m_queuedPauses.begin();

    for (std::list<CommandType>::iterator command_it = m_queuedCommands.begin(); command_it != m_queuedCommands.end(); command_it++) {
        if ((*command_it) == LINE ) {
            if (inContinuousMode) {
                // done drawing curves, leave continuous mode
                mp[cpi] << "U99,";
                sp[cpi] << "U99,";
                inContinuousMode = false;
            }
            MovementVect mv = *line_it;
            mp[0] << "(S3M" << mv.xSpeed << ",I" << mv.xDist
                  << ",S1M" << mv.ySpeed << ",I" << mv.yDist << ",)";
            line_it++;
        }
        else if ((*command_it) == CURVE ) {
            cpi += 1; // increment current program number
            if (inContinuousMode) { // continue from previous curve
                // end previous program with a jump to this program
                mp[cpi-1] << "J" << cpi << ",";
                sp[cpi-1] << "J" << cpi << ",";
            }
            else {
                // set acceleration and begin continuous mode
                mp[cpi] << "A1M10,U77,";
                sp[cpi] << "A1M10,U77,";
                inContinuousMode = true;
                mp[0] << "JM" << cpi << ",";
            }
            std::list<MovementVect> mvs = *curve_it;
            for (std::list<MovementVect>::iterator it = mvs.begin(); it != mvs.end(); it++) {
                // if either x or y distance is 0, then pause that motor's
                // program for the required length of time
                if (it->xSpeed == 0)
                    sp[cpi] << "P-" << it->time << ", ";
                else
                    sp[cpi] << "S1M" << it->xSpeed << ",I" << it->xDist << ", ";
                if (it->ySpeed == 0)
                    mp[cpi] << "P-" << it->time << ", ";
                else
                    mp[cpi] << "S1M" << it->ySpeed << ",I" << it->yDist << ", ";
            }

            curve_it++;
        }
        else if ((*command_it) == PAUSE ) {
            int t = *pause_it;
            mp[0] << "P-" << t << ",";
            pause_it++;
        }
        else if ((*command_it) == SAVE_POSITION) {
            mp[0] << "IA1M-0,IA3M-0,";
        }
        else if ((*command_it) == RETURN_TO_SAVED_POSITION) {
            mp[0] << "S1M" << m_maxSpeed << ",IA1M0,"
                  << "S3M" << m_maxSpeed << ",IA3M0,";
        }
        else if ((*command_it) == OUTPUT_HIGH) {
            // not implemented
        }
        else if ((*command_it) == OUTPUT_LOW) {
            // not implemented
        }
    }

    // bring it all together
    std::ostringstream output;
    output << "F" << std::endl;
    for (int i=1; i<=cpi; i++) {
        output << "[PM-" << i << "," << sp[i].str()
               << "]PM-" << i << "," << mp[i].str();
    }
    output << "PM-0," << mp[0] << "R" << std::endl;

    QByteArray finalProgram(output.str().c_str());

    // write program to serial
    if (loggedWrite(finalProgram) == -1)
        QMessageBox::critical(0, "Error", "Could not write to serial port");
}

void VXMController::serialReadyReadSlot() {
    QByteArray data("");
    while (!m_serialConnection->atEnd()) {
        data.append(m_serialConnection->read(20));
    }
    // log read data
    QFile logFile("serialLog.txt");
    if (logFile.open(QFile::WriteOnly | QFile::Append)) {
        QTextStream log(&logFile);
        log << " IN: " << data << endl;
        logFile.close();
    }

    if (data == "R")
        emit serialReady();

    if (data == "^") {
        loggedWrite("C\n");
        emit serialReady();
    }
}

int VXMController::loggedWrite(QByteArray data) {
    // write data to log file
    QFile logFile("serialLog.txt");
    if (logFile.open(QFile::WriteOnly | QFile::Append)) {
        QTextStream log(&logFile);
        log << "OUT: " << data << endl;
        logFile.close();
    }
    // write data to serial port
    return m_serialConnection->write(data);
}

VXMController::MovementVect VXMController::generateMovementVect(QPoint p) {
    MovementVect r;

    double dtime = lengthOfVect(p) / m_maxSpeed;

    double xSpeed;
    double ySpeed;

    if (dtime > 0) {
        xSpeed = p.x() / dtime;
        ySpeed = p.y() / dtime;
    }
    else {
        xSpeed = 0;
        ySpeed = 0;
    }

    if (xSpeed < 0) xSpeed *= -1;
    if (ySpeed < 0) ySpeed *= -1;

    r.time = qFloor(dtime * 10000 + 0.5);
    r.xSpeed = qFloor(xSpeed + 0.5);
    r.ySpeed = qFloor(ySpeed + 0.5);
    r.xDist = p.x();
    r.yDist = p.y();

    return r;
}

double VXMController::lengthOfVect(QPoint p) {
    return qSqrt(qPow(p.x(), 2) + qPow(p.y(), 2));
}

