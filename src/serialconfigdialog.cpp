#include "serialconfigdialog.h"
#include "ui_serialconfigdialog.h"

#include <QtSerialPort/QSerialPortInfo>

SerialConfigDialog::SerialConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SerialConfigDialog)
{
    ui->setupUi(this);
    m_hasBeenConfigured = false;

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
    ui->cbPort->setCurrentText(m_currentSettings.portName);
    if (ui->cbPort->currentText() == "")
        ui->labelPortInfo->setText("");
    return QDialog::exec();
}

void SerialConfigDialog::setSerialDevice(QString portname) {
    m_currentSettings.portName = portname;
    m_hasBeenConfigured = true;
}

bool SerialConfigDialog::getHasBeenConfigured() {
    return m_hasBeenConfigured;
}

VXMController::SerialSettings SerialConfigDialog::getSerialSettings() const {
    return m_currentSettings;
}

void SerialConfigDialog::on_buttonBox_accepted() {
    if (ui->cbPort->currentText() != "") {
        m_hasBeenConfigured = true;
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
    m_currentSettings.portName = this->ui->cbPort->currentText();
    m_currentSettings.baudRate = this->ui->cbBaudRate->currentText().toInt();
    switch (this->ui->cbData->currentIndex()) {
        case 0:
            m_currentSettings.dataBits = QSerialPort::Data5;
            break;
        case 1:
            m_currentSettings.dataBits = QSerialPort::Data6;
            break;
        case 2:
            m_currentSettings.dataBits = QSerialPort::Data7;
            break;
        case 3:
            m_currentSettings.dataBits = QSerialPort::Data8;
            break;
    }
    switch (this->ui->cbParity->currentIndex()) {
        case 0:
            m_currentSettings.parity = QSerialPort::NoParity;
            break;
        case 1:
            m_currentSettings.parity = QSerialPort::OddParity;
            break;
        case 3:
            m_currentSettings.parity = QSerialPort::EvenParity;
            break;
        case 4:
            m_currentSettings.parity = QSerialPort::MarkParity;
            break;
        case 5:
            m_currentSettings.parity = QSerialPort::SpaceParity;
            break;
    }
    switch (this->ui->cbStop->currentIndex()) {
        case 0:
            m_currentSettings.stopBits = QSerialPort::OneStop;
            break;
        case 1:
            m_currentSettings.stopBits = QSerialPort::OneAndHalfStop;
            break;
        case 2:
            m_currentSettings.stopBits = QSerialPort::TwoStop;
            break;
    }
    switch (this->ui->cbFlowControl->currentIndex()) {
        case 0:
            m_currentSettings.flowControl = QSerialPort::NoFlowControl;
            break;
        case 1:
            m_currentSettings.flowControl = QSerialPort::HardwareControl;
            break;
        case 2:
            m_currentSettings.flowControl = QSerialPort::SoftwareControl;
            break;
    }
}

