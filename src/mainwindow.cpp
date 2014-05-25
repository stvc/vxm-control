#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    appSettings(QSettings::IniFormat, QSettings::UserScope, "vxmcontroller")
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

    // TODO: reenable these after implementation
    ui->actionPolygonTool->setEnabled(false);
    ui->actionCurveTool->setEnabled(false);


    // set up dialogs

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

    inCalibrationMode = false;
    calibrationStep = 0;

    m_entitiesQueuedForDrawing = false;
    crossHairs = QPoint(shapeDrawer->width()/2, shapeDrawer->height()/2);

    camera->start();

    controller = new VXMController();
    connect(controller, SIGNAL(serialConnected()), this, SLOT(controller_connected()));
    connect(controller, SIGNAL(serialDisconnected()), this, SLOT(controller_disconnected()));
    connect(controller, SIGNAL(serialReady()), this, SLOT(controller_ready()));
    connect(controller, SIGNAL(serialBusy()), this, SLOT(controller_busy()));

    connect(camera, SIGNAL(error(QCamera::Error)), this, SLOT(camera_error(QCamera::Error)));

    connect(shapeDrawer, SIGNAL(entityChanged()), this, SLOT(drawing_updated()));
    connect(shapeDrawer, SIGNAL(entityAdded()), this, SLOT(entity_added()));

    connect(toolbarActions, SIGNAL(triggered(QAction*)), this, SLOT(toolbar_action_triggered(QAction*)));

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
    // TODO: rework calibration routine for new Shape system
    /*
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
    */
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
        m_translator = PointTranslator(xScale, yScale, skew, crossHairs);

        m_entitiesToDraw = shapeDrawer->getListOfEntities();
        if (m_entitiesToDraw->size() == 0)
            return;

        m_entitiesQueuedForDrawing = true;

        for (std::list<DrawableEntity*>::iterator it = m_entitiesToDraw->begin(); it != m_entitiesToDraw->end(); it++) {
            (*it)->setOutlined(false);
        }

        m_currentEntity = m_entitiesToDraw->begin();
        shapeDrawer->freezeFrame(true);
        drawEntity(*m_currentEntity, m_translator);
    }
}

void MainWindow::on_btnCalMoveX_clicked() {
    ui->btnCalMoveX->setEnabled(false);
    ui->spinBoxCalXSteps->setEnabled(false);
    controller->move(QPoint(ui->spinBoxCalXSteps->value(),0));
}

void MainWindow::on_btnCalMoveY_clicked() {
    ui->btnCalMoveY->setEnabled(false);
    ui->spinBoxCalYSteps->setEnabled(false);
    controller->move(QPoint(0,ui->spinBoxCalYSteps->value()));
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
    if (m_entitiesQueuedForDrawing) {
        (*m_currentEntity)->setOutlined(true);
        m_currentEntity++;
        if (m_currentEntity != m_entitiesToDraw->end()) {
            drawEntity(*m_currentEntity, m_translator);
        }
        else {
            m_entitiesQueuedForDrawing = false;
            shapeDrawer->freezeFrame(false);
            refreshMoveBtnState();
        }

    }
    else {
    }
}

void MainWindow::controller_busy() {
    this->ui->btnMove->setEnabled(false);
    this->ui->btnCalibrate->setEnabled(false);
}

void MainWindow::camera_error(QCamera::Error /* e */) {
    this->ui->statusbar->showMessage("camera error");
}

void MainWindow::drawing_updated() {
    refreshMoveBtnState();
}

void MainWindow::entity_added() {
    ui->actionPointerTool->trigger();
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
    // TODO: rework calibration routine for new Shape system
    /*
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

    */
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    QRect geo(0,0,0,0);
    geo.setWidth(ui->displayFrame->width());
    geo.setHeight(ui->displayFrame->height());
    shapeDrawer->setGeometry(geo);

    crossHairs = QPoint(shapeDrawer->width()/2, shapeDrawer->height()/2);

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
    if (shapeDrawer->getListOfEntities()->size() > 0 || this->ui->radioDrawManual->isChecked())
        this->ui->btnMove->setEnabled(true);
}

void MainWindow::drawEntity(DrawableEntity* ent, PointTranslator pt) {
    controller->newQueue();
    controller->addMoveToQueue(pt.translatePoint(ent->getStartPoint()));
    controller->addOutputHighToQueue();

    std::list<std::list<QPoint> > curves =  ent->getListOfCurves();
    for (std::list<std::list<QPoint> >::iterator it = curves.begin(); it != curves.end(); it++) {
        std::list<QPoint> translatedControlPoints = pt.translatePoints(*it);
        std::list<QPoint> vects = controlPointsToVectors(translatedControlPoints);
        if (it->size() == 1) { // first degree bezier curve (ie straight line)
            controller->addMoveToQueue(vects.front());
        }
        else { // cubic bezier curve
            controller->addCurveToQueue(vects);
        }
    }
    controller->addOutputLowToQueue();
    controller->addReturnToQueue();
    ent->startOutlining(controller->getEstimatedExecTime() / 10);
    controller->execQueue();
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
        int a = ps[3].x() - 3 * ps[2].x() + 3 * ps[1].x() - ps[0].x();
        int b = 3 * ps[2].x() - 6 * ps[1].x() + 3 * ps[0].x();
        int c = 3 * ps[1].x() - 3 * ps[0].x();
        int d = ps[0].x();

        int e = ps[3].y() - 3 * ps[2].y() + 3 * ps[1].y() - ps[0].y();
        int f = 3 * ps[2].y() - 6 * ps[1].y() + 3 * ps[0].y();
        int g = 3 * ps[1].y() - e * ps[0].y();
        int h = ps[0].y();

        //  calculate set of points along the curve
        double x = 0;
        double y = 0;
        double t = 0;
        std::list<QPoint> points;
        for (int i=0; i < 26; i++) {
            t = i * 1/25;
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


