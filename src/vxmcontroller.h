#ifndef VXMCONTROLLER_H
#define VXMCONTROLLER_H

#include <QObject>
#include <QtSerialPort/QSerialPort>

class VXMController : public QObject {

    Q_OBJECT

public:
    enum Direction { UP, DOWN, LEFT, RIGHT };
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
    void move(Direction, int);
    void setXStepsPerUnit(double);
    void setYStepsPerUnit(double);
    bool isSerialOpen();

signals:
    void serialConnected();
    void serialDisconnected();

private:
    QSerialPort *serialConnection;
    double xStepsPerUnit;
    double yStepsPerUnit;
    bool isConnected;
};

#endif
