#ifndef SERIALCONFIGDIALOG_H
#define SERIALCONFIGDIALOG_H

#include "vxmcontroller.h"
#include <QDialog>
#include <QtSerialPort/QSerialPort>

namespace Ui {
class SerialConfigDialog;
}

class SerialConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SerialConfigDialog(QWidget *parent = 0);
    ~SerialConfigDialog();

    int exec();

    void setSerialDevice(QString);
    bool getHasBeenConfigured();
    VXMController::SerialSettings getSerialSettings() const;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_cbPort_currentTextChanged();

private:
    void updateSettings();

private:
    Ui::SerialConfigDialog *ui;
    bool m_hasBeenConfigured;
    VXMController::SerialSettings m_currentSettings;
};

#endif // SERIALCONFIGDIALOG_H
