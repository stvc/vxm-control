#include "serialconfigdialog.h"
#include "ui_serialconfigdialog.h"

#include <QtSerialPort/QSerialPortInfo>

SerialConfigDialog::SerialConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SerialConfigDialog)
{
    ui->setupUi(this);
    hasBeenConfigured = false;

    updateSettings();
}

SerialConfigDialog::~SerialConfigDialog()
{
    delete ui;
}

int SerialConfigDialog::exec() {
    ui->cbPort->clear();
    ui->cbPort->addItem("");
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        ui->cbPort->addItem(info.portName());
    }
    ui->cbPort->setCurrentText(currentSettings.portName);
    if (ui->cbPort->currentText() == "")
        ui->labelPortInfo->setText("");
    return QDialog::exec();
}

void SerialConfigDialog::setSerialDevice(QString portname) {
    currentSettings.portName = portname;
    hasBeenConfigured = true;
}

bool SerialConfigDialog::getHasBeenConfigured() {
    return hasBeenConfigured;
}

VXMController::SerialSettings SerialConfigDialog::getSerialSettings() const {
    return currentSettings;
}

void SerialConfigDialog::on_buttonBox_accepted() {
    if (ui->cbPort->currentText() != "") {
        hasBeenConfigured = true;
        updateSettings();
    }
}

void SerialConfigDialog::on_buttonBox_rejected() {
}

void SerialConfigDialog::on_cbPort_currentTextChanged() {
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        if (info.portName() == ui->cbPort->currentText()) {
            ui->labelPortInfo->setText(info.description());
        }
    }
}

void SerialConfigDialog::updateSettings() {
    currentSettings.portName = this->ui->cbPort->currentText();
    currentSettings.baudRate = this->ui->cbBaudRate->currentText().toInt();
    switch (this->ui->cbData->currentIndex()) {
        case 0:
            currentSettings.dataBits = QSerialPort::Data5;
            break;
        case 1:
            currentSettings.dataBits = QSerialPort::Data6;
            break;
        case 2:
            currentSettings.dataBits = QSerialPort::Data7;
            break;
        case 3:
            currentSettings.dataBits = QSerialPort::Data8;
            break;
    }
    switch (this->ui->cbParity->currentIndex()) {
        case 0:
            currentSettings.parity = QSerialPort::NoParity;
            break;
        case 1:
            currentSettings.parity = QSerialPort::OddParity;
            break;
        case 3:
            currentSettings.parity = QSerialPort::EvenParity;
            break;
        case 4:
            currentSettings.parity = QSerialPort::MarkParity;
            break;
        case 5:
            currentSettings.parity = QSerialPort::SpaceParity;
            break;
    }
    switch (this->ui->cbStop->currentIndex()) {
        case 0:
            currentSettings.stopBits = QSerialPort::OneStop;
            break;
        case 1:
            currentSettings.stopBits = QSerialPort::OneAndHalfStop;
            break;
        case 2:
            currentSettings.stopBits = QSerialPort::TwoStop;
            break;
    }
    switch (this->ui->cbFlowControl->currentIndex()) {
        case 0:
            currentSettings.flowControl = QSerialPort::NoFlowControl;
            break;
        case 1:
            currentSettings.flowControl = QSerialPort::HardwareControl;
            break;
        case 2:
            currentSettings.flowControl = QSerialPort::SoftwareControl;
            break;
    }
}

