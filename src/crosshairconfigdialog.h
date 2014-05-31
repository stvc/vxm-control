#ifndef CROSSHAIRCONFIGDIALOG_H
#define CROSSHAIRCONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class CrosshairConfigDialog;
}

class CrosshairConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CrosshairConfigDialog(QWidget *parent = 0);
    ~CrosshairConfigDialog();

    void setCurrentSliderValues(int, int);
    int getXSlideValue();
    int getYSlideValue();

private slots:
    void on_hSlideX_valueChanged(int);
    void on_hSlideY_valueChanged(int);

signals:
    void crosshairConfigChanged(int, int);

private:
    Ui::CrosshairConfigDialog *ui;
};

#endif // CROSSHAIRCONFIGDIALOG_H
