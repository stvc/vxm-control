#include "vxmcontroller.h"

VXMController::VXMController(QObject *parent) :
    QObject(parent)
{
   serialConnection = new QSerialPort();
}

void VXMController::openSerialConnection(SerialSettings s) {
    emit serialConnected();
}

void VXMController::closeSerialConnection() {
    emit serialDisconnected();
}

void VXMController::moveUp(int steps) {
}
void VXMController::moveDown(int steps) {
}
void VXMController::moveLeft(int steps) {
}
void VXMController::moveRight(int steps) {
}
