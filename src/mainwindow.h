#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "serialconfigdialog.h"
#include <QMainWindow>
#include <QLabel>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionSerialConfig_triggered();
    void on_btnConnect_clicked();

private:
    Ui::MainWindow *ui;
    SerialConfigDialog *serialdialog;
    QLabel *labelConnectionStatus;
};

#endif
