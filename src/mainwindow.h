#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "cameraconfigdialog.h"
#include "customvideosurface.h"
#include "drawableviewfinder.h"
#include "pointtranslator.h"
#include "serialconfigdialog.h"
#include "vxmcontroller.h"
#include <list>
#include <QActionGroup>
#include <QCamera>
#include <QCameraViewfinder>
#include <QLabel>
#include <QMainWindow>
#include <QPoint>
#include <QSettings>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum {MANUAL_MOVE, DRAW_ENTITIES};
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    // menu bar actions
    void on_actionE_xit_triggered();
    void on_actionSerialConfig_triggered();
    void on_actionCameraConfig_triggered();

    void on_btnConnect_clicked();
    void on_btnCalibrate_clicked();
    void on_btnMove_clicked();
    void on_btnGrpDrawType_buttonClicked(int);
    void on_btnCalMoveX_clicked();
    void on_btnCalMoveY_clicked();
    void on_btnCalSave_clicked();

    void controller_connected();
    void controller_disconnected();
    void controller_ready();
    void controller_busy();

    void camera_error(QCamera::Error);

    void drawing_updated();
    void entity_added();
    void viewfinder_point_clicked();
    void calibration_step_updated();
    void toolbar_action_triggered(QAction*);

signals:
    void updateCalibrationStep();

protected:
    void resizeEvent(QResizeEvent *);

private:
    void toggleManualControls(bool);
    void refreshMoveBtnState();
    void drawEntity(DrawableEntity*, PointTranslator);
    std::list<QPoint> controlPointsToVectors(std::list<QPoint>);

    QActionGroup *toolbarActions;

    Ui::MainWindow *ui;
    SerialConfigDialog *serialDialog;
    CameraConfigDialog *cameraDialog;

    QCamera *camera;
    CustomVideoSurface *videoSurface;
    DrawableViewfinder *shapeDrawer;

    QLabel *labelConnectionStatus;
    VXMController *controller;

    QPoint crossHairs;

    // stuff for calibrating controller
    int    m_calibrationStep;
    QPoint m_tmpPoint;
    double m_tmpXStepsPerFOV;
    double m_tmpYStepsPerFOV;

    PointTranslator m_translator;
    bool m_entitiesQueuedForDrawing;
    std::list<DrawableEntity*> *m_entitiesToDraw;
    std::list<DrawableEntity*>::iterator m_currentEntity;

    QSettings appSettings;
};

#endif
