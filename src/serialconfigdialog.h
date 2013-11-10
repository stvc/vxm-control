#ifndef SERIALCONFIGDIALOG_H
#define SERIALCONFIGDIALOG_H

#include <QDialog>
#include <QtSerialPort/QSerialPort>

namespace Ui {
class SerialConfigDialog;
}

class SerialConfigDialog : public QDialog
{
    Q_OBJECT

    struct SerialSettings {
        QString portName;
        qint32 baudRate;
        QSerialPort::DataBits dataBits;
        QSerialPort::Parity parity;
        QSerialPort::StopBits stopBits;
        QSerialPort::FlowControl flowControl;
    };

public:
    explicit SerialConfigDialog(QWidget *parent = 0);
    ~SerialConfigDialog();

    SerialSettings getSerialSettings() const;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    void updateSettings();

private:
    Ui::SerialConfigDialog *ui;
    bool hasBeenConfigured;
    SerialSettings currentSettings;
};

#endif // SERIALCONFIGDIALOG_H
