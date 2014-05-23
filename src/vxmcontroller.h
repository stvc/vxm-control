#ifndef VXMCONTROLLER_H
#define VXMCONTROLLER_H

#include <list>
#include <QtCore/qmath.h>
#include <QObject>
#include <QPoint>
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

    enum CommandType { LINE, CURVE, PAUSE, SAVE_POSITION, RETURN_TO_SAVED_POSITION,/* START_CONTINUOUS_MODE,
        END_CONTINUOUS_MODE,*/ OUTPUT_HIGH, OUTPUT_LOW };

    struct MovementVect {
        int time; // in tenths of a millisecond
        int xSpeed;
        int ySpeed;
        int xDist;
        int yDist;
    };

    VXMController(QObject *parent = 0);

    void openSerialConnection(SerialSettings settings);
    void closeSerialConnection();

    void move(QPoint); // QPoint is a vector
    void savePosition();
    void returnToSavedPosition();

    void newQueue();
    void addSavePositionToQueue();
    void addReturnToQueue();
    void addMoveToQueue(QPoint); // QPoint is a vector
    void addCurveToQueue(std::list<QPoint>); // curve is list of vectors (relative points, not absolute)
    void addPauseToQueue(int); // pause for tenths of a millisecond
    void addOutputHighToQueue();
    void addOutputLowToQueue();
    double getEstimatedExecTime(); // in tenths of a milliseconds
    void execQueue();

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
    MovementVect generateMovementVect(QPoint);
    double lengthOfVect(QPoint);

    QSerialPort *m_serialConnection;
    bool m_connected;

    int m_maxSpeed;
    int m_estimatedTime; // tenths of a milliseconds

    std::list<CommandType> m_queuedCommands;
    std::list<MovementVect> m_queuedLines;
    std::list<std::list<MovementVect> > m_queuedCurves;
    std::list<int> m_queuedPauses;
};

#endif
