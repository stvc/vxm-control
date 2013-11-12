#include "vxmcontroller.h"

VXMController::VXMController(QObject *parent) :
    QObject(parent)
{
   serialConnection = new QSerialPort();
   xStepsPerUnit = 1;
   yStepsPerUnit = 1;
   isConnected = false;
}

void VXMController::openSerialConnection(SerialSettings s) {
    isConnected = true;
    emit serialConnected();
}

void VXMController::closeSerialConnection() {
    isConnected = false;
    emit serialDisconnected();
}

bool VXMController::isSerialOpen() {
    return isConnected;
}

void VXMController::move(Direction d, int units) {
}

void VXMController::setXStepsPerUnit(double steps) {
    this->xStepsPerUnit = steps;
}

void VXMController::setYStepsPerUnit(double steps) {
    this->yStepsPerUnit = steps;
}
