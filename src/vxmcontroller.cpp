#include "vxmcontroller.h"
#include <QMessageBox>

VXMController::VXMController(QObject *parent) :
    QObject(parent)
{
   serialConnection = new QSerialPort();
   xStepsPerUnit = 1;
   yStepsPerUnit = 1;
   isConnected = false;

   connect(serialConnection, SIGNAL(readyRead()), this, SLOT(serialReadyReadSlot()));
}

void VXMController::openSerialConnection(SerialSettings s) {
    serialConnection->setPortName(s.portName);
    if (serialConnection->open(QIODevice::ReadWrite)) {
        if (serialConnection->setBaudRate(s.baudRate)
                && serialConnection->setDataBits(s.dataBits)
                && serialConnection->setParity(s.parity)
                && serialConnection->setStopBits(s.stopBits)
                && serialConnection->setFlowControl(s.flowControl)) {

            isConnected = true;
            emit serialConnected();
            if (serialConnection->write("F V\n") == -1)
                QMessageBox::critical(0, "Error", "Could not write to serial port");
        }
    }
}

void VXMController::closeSerialConnection() {
    if (serialConnection->isOpen()) {
        serialConnection->close();
    }
    isConnected = false;
    emit serialDisconnected();
}

bool VXMController::isSerialOpen() {
    return isConnected;
}

void VXMController::move(Direction d, int units) {
    emit serialBusy();
    QByteArray data("F I");
    QByteArray steps;
    switch (d) {
        case MOVE_UP:
            data.append("1M-");
            steps = QByteArray::number((int) (units * yStepsPerUnit));
            break;
        case MOVE_DOWN:
            data.append("1M");
            steps = QByteArray::number((int) (units * yStepsPerUnit));
            break;
        case MOVE_LEFT:
            data.append("2M-");
            steps = QByteArray::number((int) (units * xStepsPerUnit));
            break;
        case MOVE_RIGHT:
            data.append("2M");
            steps = QByteArray::number((int) (units * xStepsPerUnit));
            break;
    }

    while (steps.length() < 3) {
        steps.prepend('0');
    }
    data.append(steps);
    data.append(",R\n");
    if (serialConnection->write(data) == -1) {
        QMessageBox::critical(0, "Error", "Could not write to serial port");
    }
}

void VXMController::setXStepsPerUnit(double steps) {
    this->xStepsPerUnit = steps;
}

void VXMController::setYStepsPerUnit(double steps) {
    this->yStepsPerUnit = steps;
}

void VXMController::serialReadyReadSlot() {
    QByteArray data("");
    while (!serialConnection->atEnd()) {
        data.append(serialConnection->read(20));
    }
    if (data == "^" || data == "R") {
        emit serialReady();
    }
}
