#ifndef CAMERACONFIGDIALOG_H
#define CAMERACONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class CameraConfigDialog;
}

class CameraConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CameraConfigDialog(QWidget *parent = 0);
    ~CameraConfigDialog();

    int exec();

    QByteArray getDevice();
    bool getMirror();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::CameraConfigDialog *ui;
    QByteArray device;
    bool mirror;
};

#endif // CAMERACONFIGDIALOG_H
