#ifndef VXMCONTROLLER_H
#define VXMCONTROLLER_H

#include <QObject>
#include <QtSerialPort/QSerialPort>

class VXMController : public QObject {

    Q_OBJECT

public:
    enum Direction { MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT };
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

    void batchMoveNew();
    void batchMoveAddMovement(Direction, int);
    void batchMoveExec();

    bool isSerialOpen();

signals:
    void serialConnected();
    void serialDisconnected();
    void serialReady();
    void serialBusy();

private slots:
    void serialReadyReadSlot();

private:
    int loggedWrite(QByteArray);

    QSerialPort *serialConnection;
    bool isConnected;
    bool enteredProgram;

    QByteArray batchMovement;
};

#endif
