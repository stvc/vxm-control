#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serialdialog = new SerialConfigDialog(this);
    labelConnectionStatus = new QLabel("Status: Not Connected");
    labelConnectionStatus->setMinimumWidth(200);
    ui->statusbar->addPermanentWidget(labelConnectionStatus);

    controller = new VXMController();
    connect(controller, SIGNAL(serialConnected()), this, SLOT(controller_connected()));
    connect(controller, SIGNAL(serialDisconnected()), this, SLOT(controller_disconnected()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionSerialConfig_triggered() {
    if (serialdialog->exec()) {
    }
}

void MainWindow::on_btnConnect_clicked() {
    if (!serialdialog->getHasBeenConfigured()) {
        QMessageBox::critical(this, "Warning", "The serial connection has not been configured");
        return;
    }
    if (controller->isSerialOpen()) {
        controller->closeSerialConnection();
    }
    else {
        controller->openSerialConnection(serialdialog->getSerialSettings());
    }
}

void MainWindow::controller_connected() {
    this->ui->btnConnect->setText("Disconnect");
    this->ui->btnDoMove->setEnabled(true);
    this->labelConnectionStatus->setText("Status: Connected");
}

void MainWindow::controller_disconnected() {
    this->ui->btnConnect->setText("Connect");
    this->ui->btnDoMove->setEnabled(false);
    this->labelConnectionStatus->setText("Status: Not Connected");
}
