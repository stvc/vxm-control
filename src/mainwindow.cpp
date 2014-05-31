#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // set up toolbar icons
    ui->actionPointerTool->setIcon(QIcon(":/toolbarIcons/toolbarIcons/arrow.png"));
    ui->actionPointerTool->setData("pointer");
    ui->actionLineTool->setIcon(QIcon(":/toolbarIcons/toolbarIcons/draw-line.png"));
    ui->actionLineTool->setData("line");
    ui->actionRectangleTool->setIcon(QIcon(":/toolbarIcons/toolbarIcons/draw-rectangle.png"));
    ui->actionRectangleTool->setData("rectangle");
    ui->actionPolygonTool->setIcon(QIcon(":/toolbarIcons/toolbarIcons/draw-polygon.png"));
    ui->actionPolygonTool->setData("polygon");
    ui->actionCircleTool->setIcon(QIcon(":/toolbarIcons/toolbarIcons/draw-circle.png"));
    ui->actionCircleTool->setData("circle");
    ui->actionCurveTool->setIcon(QIcon(":/toolbarIcons/toolbarIcons/draw-curve.png"));
    ui->actionCurveTool->setData("curve");

    toolbarActions = new QActionGroup(this);
    toolbarActions->addAction(ui->actionPointerTool);
    toolbarActions->addAction(ui->actionLineTool);
    toolbarActions->addAction(ui->actionRectangleTool);
    toolbarActions->addAction(ui->actionPolygonTool);
    toolbarActions->addAction(ui->actionCircleTool);
    toolbarActions->addAction(ui->actionCurveTool);
    ui->actionPointerTool->setChecked(true);

    // set up dialogs

    serialDialog = new SerialConfigDialog(this);
    // check if serial is already configured
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "vxmcontroller");
    if (!settings.value("serialDevice/portName").isNull())
        serialDialog->setSerialDevice(settings.value("serialDevice/portName").toString());

    cameraDialog = new CameraConfigDialog(this);
    labelConnectionStatus = new QLabel("Status: Not Connected");
    labelConnectionStatus->setMinimumWidth(200);
    ui->statusbar->addPermanentWidget(labelConnectionStatus);

    crosshairDialog = new CrosshairConfigDialog(this);

    // setup radio button Id numbers
    ui->btnGrpDirection->setId(ui->radioDown, VXMController::MOVE_DOWN);
    ui->btnGrpDirection->setId(ui->radioUp, VXMController::MOVE_UP);
    ui->btnGrpDirection->setId(ui->radioLeft, VXMController::MOVE_LEFT);
    ui->btnGrpDirection->setId(ui->radioRight, VXMController::MOVE_RIGHT);
    ui->btnGrpDrawType->setId(ui->radioDrawManual, MANUAL_MOVE);
    ui->btnGrpDrawType->setId(ui->radioDrawShapes, DRAW_ENTITIES);


    QList<QByteArray> cameraDevices = QCamera::availableDevices();
    camera = new QCamera(cameraDevices.first());
    shapeDrawer = new DrawableViewfinder();
    videoSurface = new CustomVideoSurface(shapeDrawer, this);
    camera->setViewfinder(videoSurface);

    QVBoxLayout *l = new QVBoxLayout();
    l->addWidget(shapeDrawer);
    ui->displayFrame->setLayout(l);

    m_calibrationStep = 0;
    m_tmpXStepsPerFOV = 1;
    m_tmpYStepsPerFOV = 1;

    m_entitiesQueuedForDrawing = false;
    m_currentDrawStep = 0;

    if (!settings.value("crosshairs/xpos").isNull() && !settings.value("crosshairs/ypos").isNull()) {
        int x = shapeDrawer->width() * settings.value("crosshairs/xpos").toInt() / 100;
        int y = shapeDrawer->height() * settings.value("crosshairs/ypos").toInt() / 100;
        m_crosshairs = QPoint(x,y);
    }
    else {
        int x = shapeDrawer->width() / 2;
        int y = shapeDrawer->height() / 2;
        settings.setValue("crosshairs/xpos", x);
        settings.setValue("crosshairs/ypos", y);
        m_crosshairs = QPoint(x,y);
    }

    camera->start();

    controller = new VXMController();
    connect(controller, SIGNAL(serialConnected()), this, SLOT(controller_connected()));
    connect(controller, SIGNAL(serialDisconnected()), this, SLOT(controller_disconnected()));
    connect(controller, SIGNAL(serialReady()), this, SLOT(controller_ready()));
    connect(controller, SIGNAL(serialBusy()), this, SLOT(controller_busy()));

    connect(camera, SIGNAL(error(QCamera::Error)), this, SLOT(camera_error(QCamera::Error)));

    connect(shapeDrawer, SIGNAL(entityChanged()), this, SLOT(drawing_updated()));
    connect(shapeDrawer, SIGNAL(entityAdded()), this, SLOT(entity_added()));
    connect(shapeDrawer, SIGNAL(pointClicked()), this, SLOT(viewfinder_point_clicked()));

    connect(toolbarActions, SIGNAL(triggered(QAction*)), this, SLOT(toolbar_action_triggered(QAction*)));

    connect(crosshairDialog, SIGNAL(crosshairConfigChanged(int, int)), this, SLOT(crosshair_position_updated(int,int)));

    connect(this, SIGNAL(updateCalibrationStep()), this, SLOT(calibration_step_updated()));
    connect(this, SIGNAL(updateDrawStep()), this, SLOT(draw_step_updated()));
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
        QSettings settings(QSettings::IniFormat, QSettings::UserScope, "vxmcontroller");
        settings.setValue("serialDevice/portName", serialDialog->getSerialSettings().portName);
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
void MainWindow::on_actionCrosshairConfig_triggered() {
    if (crosshairDialog->exec()) {
        ui->statusbar->showMessage("accepted");
        QSettings settings(QSettings::IniFormat, QSettings::UserScope, "vxmcontroller");
        settings.setValue("crosshairs/xpos", crosshairDialog->getXSlideValue());
        settings.setValue("crosshairs/ypos", crosshairDialog->getYSlideValue());
    }
    else {
        ui->statusbar->showMessage("canceled");
        QSettings settings(QSettings::IniFormat, QSettings::UserScope, "vxmcontroller");
        int x = settings.value("crosshairs/xpos").toInt();
        int y = settings.value("crosshairs/ypos").toInt();
        m_crosshairs = QPoint(shapeDrawer->width() * x / 100, shapeDrawer->height() * y / 100);
        shapeDrawer->setCrosshairs(m_crosshairs);
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
        shapeDrawer->setEntity(DrawableViewfinder::Pointer);
        m_calibrationStep = 0;
        ui->btnGrpDrawType->buttonClicked(ui->btnGrpDrawType->checkedId());
    }
    else {
        ui->btnCalibrate->setText("Cancel");
        ui->actionPointerTool->trigger();
        m_calibrationStep = 0;
        emit updateCalibrationStep();
    }
}

void MainWindow::on_btnGrpDrawType_buttonClicked(int id) {
    refreshMoveBtnState();

    if (id == MANUAL_MOVE) {
        toggleManualControls(true);
    }
    else if (id == DRAW_ENTITIES) {
        toggleManualControls(false);
    }
}

void MainWindow::on_btnMove_clicked() {
    if (ui->btnGrpDrawType->checkedId() == MANUAL_MOVE) {
        QPoint p;
        if (ui->radioUp->isChecked())
            p = QPoint(0,-1);
        else if (ui->radioDown->isChecked())
            p = QPoint(0,1);
        else if (ui->radioRight->isChecked())
            p = QPoint(1,0);
        else
            p = QPoint(-1,0);
        int s = ui->spinBoxSteps->value();
        p *= s;
        controller->move(p);
    }
    else if (ui->btnGrpDrawType->checkedId() == DRAW_ENTITIES) {

        QSettings settings(QSettings::IniFormat, QSettings::UserScope, "vxmcontroller");
        double xScale = settings.value("calibration/widthInSteps", 1).toDouble() / shapeDrawer->width();
        double yScale = settings.value("calibration/heightInSteps", 1).toDouble() / shapeDrawer->height();
        double skew = 0; // TODO: implement axis skew calibration
        m_translator = PointTranslator(xScale, yScale, skew, m_crosshairs);

        m_entitiesToDraw = shapeDrawer->getListOfEntities();
        if (m_entitiesToDraw->size() == 0)
            return;

        m_entitiesQueuedForDrawing = true;

        for (std::list<DrawableEntity*>::iterator it = m_entitiesToDraw->begin(); it != m_entitiesToDraw->end(); it++) {
            (*it)->setOutlined(false);
        }

        m_currentEntity = m_entitiesToDraw->begin();
        shapeDrawer->freezeFrame(true);
        emit updateDrawStep();
    }
}

void MainWindow::on_btnCalMoveX_clicked() {
    emit updateCalibrationStep();
}

void MainWindow::on_btnCalMoveY_clicked() {
    emit updateCalibrationStep();
}

void MainWindow::on_btnCalSave_clicked() {
    emit updateCalibrationStep();
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
    if (m_calibrationStep > 0) {
        emit updateCalibrationStep();
    }
    else if (m_currentDrawStep > 0) {
        emit updateDrawStep();
    }
    // TODO: remove this
    /*
    else if (m_entitiesQueuedForDrawing) {
        (*m_currentEntity)->setOutlined(true);
        m_currentEntity++;
        if (m_currentEntity != m_entitiesToDraw->end()) {
            queueEntityForDrawing(*m_currentEntity, m_translator);
            m_controllerProgramLoaded = true;
        }
        else {
            m_entitiesQueuedForDrawing = false;
            shapeDrawer->freezeFrame(false);
            refreshMoveBtnState();
        }

    }
    */
    else {
        refreshMoveBtnState();
    }
}

void MainWindow::controller_busy() {
    this->ui->btnMove->setEnabled(false);
    if (this->ui->btnCalibrate->text() != "Cancel")
        this->ui->btnCalibrate->setEnabled(false);
}

void MainWindow::camera_error(QCamera::Error /* e */) {
    this->ui->statusbar->showMessage("camera error");
}

void MainWindow::crosshair_position_updated(int x, int y) {
    m_crosshairs = QPoint(shapeDrawer->width() * (double) x / 100, shapeDrawer->height() * (double) y / 100);
    shapeDrawer->setCrosshairs(m_crosshairs);
}

void MainWindow::drawing_updated() {
    refreshMoveBtnState();
}

void MainWindow::entity_added() {
    ui->actionPointerTool->trigger();
}

void MainWindow::draw_step_updated() {
    m_currentDrawStep += 1;
    if (m_currentDrawStep == 1) {
        controller->savePosition();
    }
    else if (m_currentDrawStep == 2) {
        if (m_currentEntity != m_entitiesToDraw->end()) {
            controller->move(m_translator.translatePoint((*m_currentEntity)->getStartPoint()));
        }
        else {
            m_currentDrawStep = 0;
            refreshMoveBtnState();
            shapeDrawer->freezeFrame(false);
        }
    }
    else if (m_currentDrawStep == 3) {
        queueEntityForDrawing(*m_currentEntity, m_translator);
    }
    else if (m_currentDrawStep == 4) {
        controller->execQueue();
        (*m_currentEntity)->startOutlining(controller->getEstimatedExecTime() / 10);
    }
    else if (m_currentDrawStep == 5) {
        (*m_currentEntity)->setOutlined(true);
        controller->returnToSavedPosition();
    }
    else if (m_currentDrawStep == 6) {
        m_currentEntity++;
        m_currentDrawStep = 0;
        emit updateDrawStep();
    }
}

void MainWindow::viewfinder_point_clicked() {
    emit updateCalibrationStep();
}

void MainWindow::toolbar_action_triggered(QAction *qa) {
    QString data = qa->data().toString();

    if (data == "pointer") {
        shapeDrawer->setEntity(DrawableViewfinder::Pointer);
    }
    else if (data == "line") {
        shapeDrawer->deselectEntity();
        shapeDrawer->setEntity(DrawableViewfinder::Line);
    }
    else if (data == "rectangle") {
        shapeDrawer->deselectEntity();
        shapeDrawer->setEntity(DrawableViewfinder::Rectangle);
    }
    else if (data == "polygon") {
        shapeDrawer->deselectEntity();
        shapeDrawer->setEntity(DrawableViewfinder::Polygon);
    }
    else if (data == "circle") {
        shapeDrawer->deselectEntity();
        shapeDrawer->setEntity(DrawableViewfinder::Circle);
    }
    else if (data == "curve") {
        shapeDrawer->deselectEntity();
        shapeDrawer->setEntity(DrawableViewfinder::Curve);
    }
}

void MainWindow::calibration_step_updated() {
    m_calibrationStep++;
    if (m_calibrationStep == 1) {
        // Instruction1: Set point
        // disable all but the first step and set drawer's shape
        ui->stackedWidget->setCurrentWidget(ui->pageCalibrate);
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
        shapeDrawer->setEntity(DrawableViewfinder::CalibrationMode);
    }
    else if (m_calibrationStep == 2) {
        m_tmpPoint = shapeDrawer->getClickPosition();
        // Instruction2: Move in X direction
        ui->labelInstruction1->setEnabled(false);
        ui->labelInstruction2->setEnabled(true);
        ui->btnCalMoveX->setEnabled(true);
        ui->spinBoxCalXSteps->setEnabled(true);
    }
    else if (m_calibrationStep == 3) {
        // Instruction2, part 2: Tell controller to move
        controller->move(QPoint(ui->spinBoxCalXSteps->value(),0)); // use a 1:1 move ratio
    }
    else if (m_calibrationStep == 4) {
        // controller is ready, ask for new point
        // Instruction3: Mark new Point
        ui->labelInstruction2->setEnabled(false);
        ui->btnCalMoveX->setEnabled(false);
        ui->spinBoxCalXSteps->setEnabled(false);
        ui->labelInstruction3->setEnabled(true);
    }
    else if (m_calibrationStep == 5) {
        // TODO: calculate axis skew
        QPoint newPoint = shapeDrawer->getClickPosition();
        double numSections = (double) shapeDrawer->width() / (newPoint.x() - m_tmpPoint.x());
        m_tmpXStepsPerFOV = (double) ui->spinBoxCalXSteps->value() * numSections;
        m_tmpPoint = newPoint;

        // Instruction4: Move in Y direction
        ui->labelInstruction3->setEnabled(false);
        ui->labelInstruction4->setEnabled(true);
        ui->btnCalMoveY->setEnabled(true);
        ui->spinBoxCalYSteps->setEnabled(true);
    }
    else if (m_calibrationStep == 6) {
        // Instruction4, part2: tell controller to move and wait for ready
        // signal
        controller->move(QPoint(0,ui->spinBoxCalYSteps->value()));
    }
    else if (m_calibrationStep == 7) {
        // controller is ready, ask for new point
        // Instruction5: Mark new location
        ui->labelInstruction4->setEnabled(false);
        ui->btnCalMoveY->setEnabled(false);
        ui->spinBoxCalYSteps->setEnabled(false);
        ui->labelInstruction5->setEnabled(true);
    }
    else if (m_calibrationStep == 8) {
        // new point clicked, calculate difference and number of steps per
        // field of view, then ask user to click crosshairs
        // TODO: calculate axis skew
        QPoint newPoint = shapeDrawer->getClickPosition();
        double numSections = (double) shapeDrawer->height() / (newPoint.y() - m_tmpPoint.y());
        m_tmpYStepsPerFOV = (double) ui->spinBoxCalYSteps->value() * numSections;
        // Instruction6: Mark crosshairs
        ui->labelInstruction5->setEnabled(false);
        ui->labelInstruction6->setEnabled(true);
    }
    else if (m_calibrationStep == 9) {
        // crosshairs clicked, ask user to save
        // TODO: crosshairs is just assumed to be in middle of screen, fix this
        // if necessary
        ui->labelInstruction6->setEnabled(false);
        ui->btnCalSave->setEnabled(true);
    }
    else if (m_calibrationStep == 10) {
        // save button clicked. save correct values to file and reset
        // calibration mode
        if (m_tmpXStepsPerFOV < 0)
            m_tmpXStepsPerFOV *= -1;
        if (m_tmpYStepsPerFOV < 0)
            m_tmpYStepsPerFOV *= -1;

        QSettings settings(QSettings::IniFormat, QSettings::UserScope, "vxmcontroller");
        settings.setValue("calibration/widthInSteps", qFloor(m_tmpXStepsPerFOV + 0.5));
        settings.setValue("calibration/heightInSteps", qFloor(m_tmpYStepsPerFOV + 0.5));

        ui->btnCalibrate->click();
    }
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    QRect geo(0,0,0,0);
    geo.setWidth(ui->displayFrame->width());
    geo.setHeight(ui->displayFrame->height());
    shapeDrawer->setGeometry(geo);

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "vxmcontroller");
    double xchPos;
    double ychPos;
    if (settings.value("crosshairs/xpos").isNull())
        xchPos = 0.5;
    else
        xchPos = settings.value("crosshairs/xpos").toDouble() / 100;

    if (settings.value("crosshairs/ypos").isNull())
        ychPos = 0.5;
    else
        ychPos = settings.value("crosshairs/ypos").toDouble() / 100;


    m_crosshairs = QPoint(shapeDrawer->width() * xchPos, shapeDrawer->height() * ychPos);
    shapeDrawer->setCrosshairs(m_crosshairs);

    if (m_calibrationStep > 0) {
        // if resizing the window while in calibration mode, cancel calibration
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
    this->ui->btnCalibrate->setEnabled(false);

    // do nothing if controller isn't connected
    if (!this->controller->isSerialOpen())
        return;

    this->ui->btnCalibrate->setEnabled(true);

    // do nothing if controller hasn't been calibrated
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "vxmcontroller");
    if (settings.value("calibration/widthInSteps").isNull() || settings.value("calibration/heightInSteps").isNull())
        return;

    // make sure that either user is moving manually, or user has drawn a shape
    if (shapeDrawer->getListOfEntities()->size() > 0 || this->ui->radioDrawManual->isChecked())
        this->ui->btnMove->setEnabled(true);
}

void MainWindow::queueEntityForDrawing(DrawableEntity* ent, PointTranslator pt) {
    controller->newQueue();
    controller->addSavePositionToQueue();
    controller->addMoveToQueue(pt.translatePoint(ent->getStartPoint()));
    controller->addOutputHighToQueue();

    std::list<std::list<QPoint> > curves =  ent->getListOfCurves();
    for (std::list<std::list<QPoint> >::iterator it = curves.begin(); it != curves.end(); it++) {
        std::list<QPoint> translatedControlPoints = pt.translatePoints(*it);
        std::list<QPoint> vects = controlPointsToVectors(translatedControlPoints);
        if (vects.size() == 1) { // first degree bezier curve (ie straight line)
            controller->addMoveToQueue(vects.front());
        }
        else { // cubic bezier curve
            controller->addCurveToQueue(vects);
        }
    }
    controller->addOutputLowToQueue();
    controller->addReturnToQueue();
    controller->loadQueue();
}

std::list<QPoint> MainWindow::controlPointsToVectors(std::list<QPoint> controlPoints) {
    std::list<QPoint> vects;
    if (controlPoints.size() == 2) { // straight line
        QPoint p = controlPoints.back() - controlPoints.front();
        vects.push_back(p);
    }
    else if (controlPoints.size() == 4) { // cubic bezier curve
        QPoint ps[4];
        int i = 0;
        QPoint first = controlPoints.front();
        // shift all points to the origin
        for (std::list<QPoint>::iterator it = controlPoints.begin(); it != controlPoints.end(); it++) {
            ps[i] = (*it) - first;
            i++;
        }

        // convert from graph space to equation space
        //  coefficients:
        double a = ps[3].x() - 3 * ps[2].x() + 3 * ps[1].x() - ps[0].x();
        double b = 3 * ps[2].x() - 6 * ps[1].x() + 3 * ps[0].x();
        double c = 3 * ps[1].x() - 3 * ps[0].x();
        double d = ps[0].x();

        double e = ps[3].y() - 3 * ps[2].y() + 3 * ps[1].y() - ps[0].y();
        double f = 3 * ps[2].y() - 6 * ps[1].y() + 3 * ps[0].y();
        double g = 3 * ps[1].y() - e * ps[0].y();
        double h = ps[0].y();

        //  calculate set of points along the curve
        double x = 0;
        double y = 0;
        double t = 0;
        std::list<QPoint> points;
        for (int i=0; i < 26; i++) {
            t = (double) i * 1/25;
            x = (((a*t + b)*t) + c)*t + d;
            y = (((e*t + f)*t) + g)*t + h;

            points.push_back(QPoint(qFloor(x + 0.5), qFloor(y + 0.5)));
        }

        //  convert series of points to series of vectors
        std::list<QPoint>::iterator fst = points.begin();
        std::list<QPoint>::iterator snd = points.begin();
        snd++;
        while (snd != points.end()) {
            vects.push_back((*snd) - (*fst));
            fst++;
            snd++;
        }
    }
    return vects;
}


