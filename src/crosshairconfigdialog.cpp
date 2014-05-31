#include "crosshairconfigdialog.h"
#include "ui_crosshairconfigdialog.h"

CrosshairConfigDialog::CrosshairConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CrosshairConfigDialog)
{
    ui->setupUi(this);
}

CrosshairConfigDialog::~CrosshairConfigDialog()
{
    delete ui;
}

void CrosshairConfigDialog::setCurrentSliderValues(int x, int y) {
    ui->hSlideX->setValue(x);
    ui->hSlideY->setValue(y);
}

int CrosshairConfigDialog::getXSlideValue() {
    return ui->hSlideX->value();
}

int CrosshairConfigDialog::getYSlideValue() {
    return ui->hSlideY->value();
}

void CrosshairConfigDialog::on_hSlideX_valueChanged(int value) {
    ui->labelXSlideVal->setText(QString::number(value) + " %");
    emit crosshairConfigChanged(value, ui->hSlideY->value());
}

void CrosshairConfigDialog::on_hSlideY_valueChanged(int value) {
    ui->labelYSlideVal->setText(QString::number(value) + " %");
    emit crosshairConfigChanged(ui->hSlideX->value(), value);
}

