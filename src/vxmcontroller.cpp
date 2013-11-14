#include "vxmcontroller.h"

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
    isConnected = true;
    emit serialConnected();
    emit serialReady();
}

void VXMController::closeSerialConnection() {
    isConnected = false;
    emit serialDisconnected();
}

bool VXMController::isSerialOpen() {
    return isConnected;
}

void VXMController::move(Direction d, int units) {
    emit serialBusy();
}

void VXMController::setXStepsPerUnit(double steps) {
    this->xStepsPerUnit = steps;
}

void VXMController::setYStepsPerUnit(double steps) {
    this->yStepsPerUnit = steps;
}

void VXMController::serialReadyReadSlot() {
    QByteArray data = "";
    while (!serialConnection->atEnd()) {
        data += serialConnection->read(20);
    }
    if (data == "^") {
        emit serialReady();
    }
}
