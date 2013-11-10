#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serialdialog = new SerialConfigDialog(this);
    labelConnectionStatus = new QLabel("Status: Not Connected");
    labelConnectionStatus->setMinimumWidth(200);
    ui->statusbar->addPermanentWidget(labelConnectionStatus);
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
    this->labelConnectionStatus->setText("Status: Connected");
}
