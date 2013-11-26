#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    shapeStart(0,0),
    shapeEnd(0,0)
{
    ui->setupUi(this);
    serialDialog = new SerialConfigDialog(this);
    cameraDialog = new CameraConfigDialog(this);
    labelConnectionStatus = new QLabel("Status: Not Connected");
    labelConnectionStatus->setMinimumWidth(200);
    ui->statusbar->addPermanentWidget(labelConnectionStatus);

    // setup radio button Id numbers
    ui->btnGrpDirection->setId(ui->radioDown, VXMController::MOVE_DOWN);
    ui->btnGrpDirection->setId(ui->radioUp, VXMController::MOVE_UP);
    ui->btnGrpDirection->setId(ui->radioLeft, VXMController::MOVE_LEFT);
    ui->btnGrpDirection->setId(ui->radioRight, VXMController::MOVE_RIGHT);
    ui->btnGrpDrawType->setId(ui->radioDrawManual, DRAW_MANUAL);
    ui->btnGrpDrawType->setId(ui->radioDrawLine, DRAW_LINE);
    ui->btnGrpDrawType->setId(ui->radioDrawRectangle, DRAW_RECT);

    QList<QByteArray> cameraDevices = QCamera::availableDevices();
    camera = new QCamera(cameraDevices.first());
    viewFinder = new QCameraViewfinder();
    camera->setViewfinder(viewFinder);

    shapeDrawer = new DrawableViewfinder(viewFinder);
    shapeDrawer->setGeometry(viewFinder->geometry());

    QVBoxLayout *l = new QVBoxLayout();
    l->addWidget(viewFinder);
    ui->displayFrame->setLayout(l);



    shapeDrawn = false;

    camera->start();

    controller = new VXMController();
    connect(controller, SIGNAL(serialConnected()), this, SLOT(controller_connected()));
    connect(controller, SIGNAL(serialDisconnected()), this, SLOT(controller_disconnected()));
    connect(controller, SIGNAL(serialReady()), this, SLOT(controller_ready()));
    connect(controller, SIGNAL(serialBusy()), this, SLOT(controller_busy()));

    connect(camera, SIGNAL(error(QCamera::Error)), this, SLOT(camera_error(QCamera::Error)));

    connect(shapeDrawer, SIGNAL(pointsChanged()), this, SLOT(drawing_updated()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionSerialConfig_triggered() {
    if (controller->isSerialOpen()) {
        controller->closeSerialConnection();
    }
    if (serialDialog->exec()) {
    }
}

void MainWindow::on_actionCameraConfig_triggered() {
    if (cameraDialog->exec()) {
        camera->stop();
        if (cameraDialog->getDevice().length() > 0) {
            camera = new QCamera(cameraDialog->getDevice());
            camera->setViewfinder(viewFinder);
            camera->start();
        }
    }
}

void MainWindow::on_btnConnect_clicked() {
    if (!serialDialog->getHasBeenConfigured()) {
        QMessageBox::critical(this, "Warning", "The serial connection has not been configured");
        return;
    }
    if (controller->isSerialOpen()) {
        controller->closeSerialConnection();
    }
    else {
        controller->openSerialConnection(serialDialog->getSerialSettings());
    }
}

void MainWindow::on_btnGrpDrawType_buttonClicked(int id) {
    shapeDrawer->resetPoints();
    switch (id) {
        case DRAW_MANUAL:
            shapeDrawer->setShape(DrawableViewfinder::None);
            toggleManualControls(true);
            break;
        case DRAW_LINE:
            shapeDrawer->setShape(DrawableViewfinder::Line);
            toggleManualControls(false);
            break;
        case DRAW_RECT:
            shapeDrawer->setShape(DrawableViewfinder::Rectangle);
            toggleManualControls(false);
            break;
        default:
            break;
    }
    refreshMoveBtnState();
}

void MainWindow::on_btnDoMove_clicked() {
    VXMController::Direction d;
    if (ui->radioUp->isChecked())
        d = VXMController::MOVE_UP;
    else if (ui->radioDown->isChecked())
        d = VXMController::MOVE_DOWN;
    else if (ui->radioRight->isChecked())
        d = VXMController::MOVE_RIGHT;
    else
        d = VXMController::MOVE_LEFT;
    int s = ui->spinBoxSteps->value();
    controller->move(d,s);
}

void MainWindow::controller_connected() {
    this->ui->btnConnect->setText("Disconnect");
    this->labelConnectionStatus->setText("Status: Connected");
}

void MainWindow::controller_disconnected() {
    this->ui->btnConnect->setText("Connect");
    refreshMoveBtnState();
    this->labelConnectionStatus->setText("Status: Not Connected");
}

void MainWindow::controller_ready() {
    refreshMoveBtnState();
}

void MainWindow::controller_busy() {
    this->ui->btnDoMove->setEnabled(false);
}

void MainWindow::camera_error(QCamera::Error /* e */) {
    this->ui->statusbar->showMessage("camera error");
}

void MainWindow::drawing_updated() {
    shapeStart = shapeDrawer->getStartPoint();
    shapeEnd = shapeDrawer->getEndPoint();
    this->ui->statusbar->showMessage("signal emitted");
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    QRect geo(0,0,0,0);
    geo.setWidth(ui->displayFrame->width());
    geo.setHeight(ui->displayFrame->height());
    geo.setWidth(viewFinder->width());
    geo.setHeight(viewFinder->height());
    shapeDrawer->setGeometry(geo);


}

void MainWindow::toggleManualControls(bool b) {
    ui->radioDown->setEnabled(b);
    ui->radioUp->setEnabled(b);
    ui->radioLeft->setEnabled(b);
    ui->radioRight->setEnabled(b);
    ui->spinBoxSteps->setEnabled(b);
    ui->labelSteps->setEnabled(b);
}

void MainWindow::refreshMoveBtnState() {
    this->ui->btnDoMove->setEnabled(false);

    // do nothing if controller isn't connected
    if (!this->controller->isSerialOpen())
        return;
    // do nothing if controller hasn't been calibrated
    if (!this->controller->hasControllerBeenCalibrated())
        return;
    // make sure that either user is moving manually, or user has drawn a shape
    if (shapeDrawn || this->ui->radioDrawManual->isChecked())
        this->ui->btnDoMove->setEnabled(true);
}
