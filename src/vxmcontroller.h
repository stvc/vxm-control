#ifndef VXMCONTROLLER_H
#define VXMCONTROLLER_H

#include <QObject>
#include <QtSerialPort/QSerialPort>

class VXMController : public QObject {

    Q_OBJECT

public:
    struct SerialSettings {
        QString portName;
        qint32 baudRate;
        QSerialPort::DataBits dataBits;
        QSerialPort::Parity parity;
        QSerialPort::StopBits stopBits;
        QSerialPort::FlowControl flowControl;
    };

    VXMController(QObject *parent = 0);

    void openSerialConnection(SerialSettings settings);
    void closeSerialConnection();
    void moveUp(int);
    void moveDown(int);
    void moveLeft(int);
    void moveRight(int);

signals:
    void serialConnected();
    void serialDisconnected();

private:
    QSerialPort *serialConnection;
};

#endif
