#include "cameraconfigdialog.h"
#include "ui_cameraconfigdialog.h"
#include <QCamera>

CameraConfigDialog::CameraConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CameraConfigDialog),
    m_device("")
{
    ui->setupUi(this);
    m_mirror = false;
}

CameraConfigDialog::~CameraConfigDialog()
{
    delete ui;
}

int CameraConfigDialog::exec() {
    this->ui->cbCameraDevices->clear();
    this->ui->cbCameraDevices->addItem("");

    foreach (const QByteArray &deviceName, QCamera::availableDevices()) {
        this->ui->cbCameraDevices->addItem(deviceName);
    }

    return QDialog::exec();
}

QByteArray CameraConfigDialog::getDevice() {
    return m_device;
}

bool CameraConfigDialog::getMirror() {
    return m_mirror;
}

void CameraConfigDialog::on_buttonBox_accepted() {
    m_device.clear();
    m_device.append(this->ui->cbCameraDevices->currentText());

    if (this->ui->checkMirror->isChecked())
        m_mirror = true;
    else
        m_mirror = false;
}
