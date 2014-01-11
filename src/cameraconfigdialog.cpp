#include "cameraconfigdialog.h"
#include "ui_cameraconfigdialog.h"
#include <QCamera>

CameraConfigDialog::CameraConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CameraConfigDialog),
    device("")
{
    ui->setupUi(this);
    mirror = false;
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
    return device;
}

bool CameraConfigDialog::getMirror() {
    return mirror;
}

void CameraConfigDialog::on_buttonBox_accepted() {
    device.clear();
    device.append(this->ui->cbCameraDevices->currentText());

    if (this->ui->checkMirror->isChecked())
        mirror = true;
    else
        mirror = false;
}
