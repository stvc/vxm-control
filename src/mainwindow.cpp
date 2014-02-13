#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    shapeStart(0,0),
    shapeEnd(0,0),
    appSettings(QSettings::IniFormat, QSettings::UserScope, "vxmcontroller")
{
    ui->setupUi(this);
    serialDialog = new SerialConfigDialog(this);
    // check if serial is already configured
    if (!appSettings.value("serialDevice/portName").isNull())
        serialDialog->setSerialDevice(appSettings.value("serialDevice/portName").toString());

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
    shapeDrawer = new DrawableViewfinder();
    videoSurface = new CustomVideoSurface(shapeDrawer, this);
    camera->setViewfinder(videoSurface);

    QVBoxLayout *l = new QVBoxLayout();
    l->addWidget(shapeDrawer);
    ui->displayFrame->setLayout(l);

    shapeDrawn = false;
    inCalibrationMode = false;
    calibrationStep = 0;

    camera->start();

    controller = new VXMController();
    connect(controller, SIGNAL(serialConnected()), this, SLOT(controller_connected()));
    connect(controller, SIGNAL(serialDisconnected()), this, SLOT(controller_disconnected()));
    connect(controller, SIGNAL(serialReady()), this, SLOT(controller_ready()));
    connect(controller, SIGNAL(serialBusy()), this, SLOT(controller_busy()));

    connect(camera, SIGNAL(error(QCamera::Error)), this, SLOT(camera_error(QCamera::Error)));

    connect(shapeDrawer, SIGNAL(pointsChanged()), this, SLOT(drawing_updated()));
    connect(this, SIGNAL(calibrationStepChanged()), this, SLOT(calibration_step_updated()));

    // temporary while I create the calibration routine
//    ui->btnCalibrate->setEnabled(true);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_actionE_xit_triggered() {
    this->close();
}

void MainWindow::on_actionSerialConfig_triggered() {
    if (controller->isSerialOpen()) {
        controller->closeSerialConnection();
    }
    if (serialDialog->exec()) {
        // save settings
        appSettings.setValue("serialDevice/portName", serialDialog->getSerialSettings().portName);
    }
}

void MainWindow::on_actionCameraConfig_triggered() {
    if (cameraDialog->exec()) {
        camera->stop();
        if (cameraDialog->getDevice().length() > 0) {
            delete camera;
            camera = new QCamera(cameraDialog->getDevice());
            camera->setViewfinder(videoSurface);
        }
        camera->start();
        videoSurface->mirror(cameraDialog->getMirror());
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

void MainWindow::on_btnCalibrate_clicked() {
    if (ui->btnCalibrate->text() == "Cancel") {
        ui->stackedWidget->setCurrentWidget(ui->pageMove);
        ui->btnCalibrate->setText("Calibrate");
        shapeDrawer->resetPoints();
        inCalibrationMode = false;
        calibrationStep = 0;
        emit calibrationStepChanged();
        ui->btnGrpDrawType->buttonClicked(ui->btnGrpDrawType->checkedId());
    }
    else {
        ui->stackedWidget->setCurrentWidget(ui->pageCalibrate);
        ui->btnCalibrate->setText("Cancel");
        shapeDrawer->setShape(DrawableViewfinder::StartPoint);
        inCalibrationMode = true;
        calibrationStep = 0;
        emit calibrationStepChanged();
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

void MainWindow::on_btnMove_clicked() {
    // tmp work around for crosshair config being shoddy
    crossHairs = QPoint(shapeDrawer->width()/2, shapeDrawer->height()/2);

    DrawableViewfinder::Shape shape = shapeDrawer->getShape();
    if (shape == DrawableViewfinder::None) {
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
    else if (shape == DrawableViewfinder::Line) {
        // points should be up to date from drawing_updated() slot
        // TODO: check step calculation logic
        double x = shapeStart.x() - crossHairs.x();
        int xSteps = (int)((x / shapeDrawer->width()) * appSettings.value("calibration/widthInSteps", 1).toInt() + 0.5);
        double y = shapeStart.y() - crossHairs.y();
        int ySteps = (int)((y / shapeDrawer->height()) * appSettings.value("calibration/heightInSteps", 1).toInt() + 0.5);
        controller->batchMoveNew();
        controller->batchMoveAddMovement(VXMController::MOVE_RIGHT, xSteps);
        controller->batchMoveAddMovement(VXMController::MOVE_DOWN, ySteps);
        x = shapeEnd.x() - shapeStart.x();
        xSteps = (int)((x / shapeDrawer->width()) * appSettings.value("calibration/widthInSteps", 1).toInt() + 0.5);
        y = shapeEnd.y() - shapeStart.y();
        ySteps = (int)((y / shapeDrawer->height()) * appSettings.value("calibration/heightInSteps", 1).toInt() + 0.5);
        controller->batchMoveAddMovement(VXMController::MOVE_RIGHT, xSteps);
        controller->batchMoveAddMovement(VXMController::MOVE_DOWN, ySteps);
        controller->batchMoveExec();
    }
    else if (shape == DrawableViewfinder::Rectangle) {
        // TODO: check step calculation logic
        double x = shapeStart.x() - crossHairs.x();
        int xSteps = (int)((x / shapeDrawer->width()) * appSettings.value("calibration/widthInSteps", 1).toInt() + 0.5);
        double y = shapeStart.y() - crossHairs.y();
        int ySteps = (int)((y / shapeDrawer->height()) * appSettings.value("calibration/heightInSteps", 1).toInt() + 0.5);
        controller->batchMoveNew();
        controller->batchMoveAddMovement(VXMController::MOVE_RIGHT, xSteps);
        controller->batchMoveAddMovement(VXMController::MOVE_DOWN, ySteps);
        double width = shapeEnd.x() - shapeStart.x();
        int widthSteps = (int)((width / shapeDrawer->width()) * appSettings.value("calibration/widthInSteps", 1).toInt() + 0.5);
        double height = shapeEnd.y() - shapeStart.y();
        int heightSteps = (int)((height / shapeDrawer->height()) * appSettings.value("calibration/heightInSteps", 1).toInt() + 0.5);
        controller->batchMoveAddMovement(VXMController::MOVE_RIGHT, widthSteps);
        controller->batchMoveAddMovement(VXMController::MOVE_DOWN, heightSteps);
        controller->batchMoveAddMovement(VXMController::MOVE_LEFT, widthSteps);
        controller->batchMoveAddMovement(VXMController::MOVE_UP, heightSteps);
        controller->batchMoveExec();
    }
}

void MainWindow::on_btnCalMoveX_clicked() {
    ui->btnCalMoveX->setEnabled(false);
    ui->spinBoxCalXSteps->setEnabled(false);
    controller->move(VXMController::MOVE_RIGHT, ui->spinBoxCalXSteps->value());
}

void MainWindow::on_btnCalMoveY_clicked() {
    ui->btnCalMoveY->setEnabled(false);
    ui->spinBoxCalYSteps->setEnabled(false);
    controller->move(VXMController::MOVE_DOWN, ui->spinBoxCalYSteps->value());
}

void MainWindow::on_btnCalSave_clicked() {

    if (tmpXStepsPerFOV < 0)
        tmpXStepsPerFOV *= -1;
    if (tmpYStepsPerFOV < 0)
        tmpYStepsPerFOV *= -1;

    appSettings.setValue("calibration/widthInSteps", tmpXStepsPerFOV);
    appSettings.setValue("calibration/heightInSteps", tmpYStepsPerFOV);

    // crossHairs = shapeDrawer->getEndPoint();
    crossHairs = QPoint(shapeDrawer->width()/2, shapeDrawer->height()/2);

    // reset ui
    ui->btnCalibrate->click();
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
    if (inCalibrationMode) {
        calibrationStep++;
        emit calibrationStepChanged();
    }
    else {
        refreshMoveBtnState();
        ui->btnCalibrate->setEnabled(true);
    }
    this->ui->btnCalibrate->setEnabled(true);
}

void MainWindow::controller_busy() {
    this->ui->btnMove->setEnabled(false);
    this->ui->btnCalibrate->setEnabled(false);
}

void MainWindow::camera_error(QCamera::Error /* e */) {
    this->ui->statusbar->showMessage("camera error");
}

void MainWindow::drawing_updated() {
    shapeStart = shapeDrawer->getStartPoint();
    shapeEnd = shapeDrawer->getEndPoint();

    // if in calibration mode, advance to the next step
    if (inCalibrationMode) {
        calibrationStep++;
        emit calibrationStepChanged();
    }
    else if (ui->btnGrpDrawType->checkedId() != DRAW_MANUAL) {
        shapeDrawn = true;
    }
    refreshMoveBtnState();
}

void MainWindow::calibration_step_updated() {
    double sections = 1;
    switch (calibrationStep) {
        case 0:
            // Instruction1: Set point
            // disable all but the first step and set drawer's shape
            ui->labelInstruction1->setEnabled(true);
            ui->labelInstruction2->setEnabled(false);
            ui->labelInstruction3->setEnabled(false);
            ui->labelInstruction4->setEnabled(false);
            ui->labelInstruction5->setEnabled(false);
            ui->labelInstruction6->setEnabled(false);
            ui->spinBoxCalXSteps->setEnabled(false);
            ui->spinBoxCalYSteps->setEnabled(false);
            ui->btnCalMoveX->setEnabled(false);
            ui->btnCalMoveY->setEnabled(false);
            ui->btnCalSave->setEnabled(false);
            shapeDrawer->setShape(DrawableViewfinder::StartPoint);
            break;
        case 1:
            // Instruction2: Move in X direction
            ui->labelInstruction1->setEnabled(false);
            ui->labelInstruction2->setEnabled(true);
            ui->btnCalMoveX->setEnabled(true);
            ui->spinBoxCalXSteps->setEnabled(true);
            shapeDrawer->freezePoints(true);
            break;
        case 2:
            // Instruction3: Mark new Point
            ui->labelInstruction2->setEnabled(false);
            ui->btnCalMoveX->setEnabled(false);
            ui->spinBoxCalXSteps->setEnabled(false);
            ui->labelInstruction3->setEnabled(true);
            shapeDrawer->setShape(DrawableViewfinder::EndPoint);
            shapeDrawer->freezePoints(false);
            break;
        case 3:
            sections = shapeDrawer->width() / (shapeDrawer->getEndPoint().x() - shapeDrawer->getStartPoint().x());
            tmpXStepsPerFOV = ui->spinBoxCalXSteps->value() * sections;
            // Instruction4: Move in Y direction
            ui->labelInstruction3->setEnabled(false);
            ui->labelInstruction4->setEnabled(true);
            ui->btnCalMoveY->setEnabled(true);
            ui->spinBoxCalYSteps->setEnabled(true);
            shapeDrawer->setShape(DrawableViewfinder::EndPoint);
            shapeDrawer->setStartPoint(shapeDrawer->getEndPoint());
            shapeDrawer->freezePoints(true);
            break;
        case 4:
            // Instruction5: Mark new location
            ui->labelInstruction4->setEnabled(false);
            ui->btnCalMoveY->setEnabled(false);
            ui->spinBoxCalYSteps->setEnabled(false);
            ui->labelInstruction5->setEnabled(true);
            shapeDrawer->setShape(DrawableViewfinder::EndPoint);
            shapeDrawer->freezePoints(false);
            break;
        case 5:
            sections = shapeDrawer->height() / (shapeDrawer->getEndPoint().y() - shapeDrawer->getStartPoint().y());
            tmpYStepsPerFOV = ui->spinBoxCalYSteps->value() * sections;
            // Instruction6: Mark crosshairs
            ui->labelInstruction5->setEnabled(false);
            ui->labelInstruction6->setEnabled(true);
            shapeDrawer->resetPoints();
            shapeDrawer->setShape(DrawableViewfinder::SinglePoint);
            break;
        case 6:
            // save calibration
            ui->labelInstruction6->setEnabled(false);
            ui->btnCalSave->setEnabled(true);
            break;
        default:
            calibrationStep = 0;
            emit calibrationStepChanged();
    }
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    QRect geo(0,0,0,0);
    geo.setWidth(ui->displayFrame->width());
    geo.setHeight(ui->displayFrame->height());
    shapeDrawer->setGeometry(geo);

    if (inCalibrationMode) {
        ui->btnCalibrate->click();
    }
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
    this->ui->btnMove->setEnabled(false);

    // do nothing if controller isn't connected
    if (!this->controller->isSerialOpen())
        return;

    // do nothing if controller hasn't been calibrated
    if (appSettings.value("calibration/widthInSteps").isNull() || appSettings.value("calibration/heightInSteps").isNull())
        return;

    // make sure that either user is moving manually, or user has drawn a shape
    if (shapeDrawn || this->ui->radioDrawManual->isChecked())
        this->ui->btnMove->setEnabled(true);
}
