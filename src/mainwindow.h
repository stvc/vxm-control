#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "cameraconfigdialog.h"
#include "serialconfigdialog.h"
#include "drawableviewfinder.h"
#include "vxmcontroller.h"
#include <QMainWindow>
#include <QLabel>
#include <QCamera>
#include <QCameraViewfinder>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum {DRAW_MANUAL, DRAW_LINE, DRAW_RECT};
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionSerialConfig_triggered();
    void on_actionCameraConfig_triggered();
    void on_btnConnect_clicked();
    void on_btnDoMove_clicked();
    void on_btnGrpDrawType_buttonClicked(int);

    void controller_connected();
    void controller_disconnected();
    void controller_ready();
    void controller_busy();

    void camera_error(QCamera::Error);

    void updatePoints();

private:
    void toggleManualControls(bool);
    void refreshMoveBtnState();

    Ui::MainWindow *ui;
    SerialConfigDialog *serialDialog;
    CameraConfigDialog *cameraDialog;

    QCamera *camera;
    QCameraViewfinder *viewFinder;
    DrawableViewfinder *shapeDrawer;

    QLabel *labelConnectionStatus;
    VXMController *controller;

    QPoint shapeStart;
    QPoint shapeEnd;
    bool shapeDrawn;
};

#endif
