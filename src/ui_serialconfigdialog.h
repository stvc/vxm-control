/********************************************************************************
** Form generated from reading UI file 'serialconfigdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.1.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SERIALCONFIGDIALOG_H
#define UI_SERIALCONFIGDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>

QT_BEGIN_NAMESPACE

class Ui_SerialConfigDialog
{
public:
    QHBoxLayout *horizontalLayout;
    QFormLayout *formLayout;
    QLabel *label;
    QComboBox *cbPort;
    QLabel *label_2;
    QComboBox *cbBaudRate;
    QLabel *label_3;
    QComboBox *cbData;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QComboBox *cbParity;
    QComboBox *cbStop;
    QComboBox *cbFlowControl;
    QLabel *labelPortInfo;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SerialConfigDialog)
    {
        if (SerialConfigDialog->objectName().isEmpty())
            SerialConfigDialog->setObjectName(QStringLiteral("SerialConfigDialog"));
        SerialConfigDialog->resize(400, 230);
        horizontalLayout = new QHBoxLayout(SerialConfigDialog);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QStringLiteral("formLayout"));
        label = new QLabel(SerialConfigDialog);
        label->setObjectName(QStringLiteral("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        cbPort = new QComboBox(SerialConfigDialog);
        cbPort->setObjectName(QStringLiteral("cbPort"));

        formLayout->setWidget(0, QFormLayout::FieldRole, cbPort);

        label_2 = new QLabel(SerialConfigDialog);
        label_2->setObjectName(QStringLiteral("label_2"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_2);

        cbBaudRate = new QComboBox(SerialConfigDialog);
        cbBaudRate->setObjectName(QStringLiteral("cbBaudRate"));

        formLayout->setWidget(2, QFormLayout::FieldRole, cbBaudRate);

        label_3 = new QLabel(SerialConfigDialog);
        label_3->setObjectName(QStringLiteral("label_3"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_3);

        cbData = new QComboBox(SerialConfigDialog);
        cbData->setObjectName(QStringLiteral("cbData"));

        formLayout->setWidget(3, QFormLayout::FieldRole, cbData);

        label_4 = new QLabel(SerialConfigDialog);
        label_4->setObjectName(QStringLiteral("label_4"));

        formLayout->setWidget(4, QFormLayout::LabelRole, label_4);

        label_5 = new QLabel(SerialConfigDialog);
        label_5->setObjectName(QStringLiteral("label_5"));

        formLayout->setWidget(5, QFormLayout::LabelRole, label_5);

        label_6 = new QLabel(SerialConfigDialog);
        label_6->setObjectName(QStringLiteral("label_6"));

        formLayout->setWidget(6, QFormLayout::LabelRole, label_6);

        cbParity = new QComboBox(SerialConfigDialog);
        cbParity->setObjectName(QStringLiteral("cbParity"));

        formLayout->setWidget(4, QFormLayout::FieldRole, cbParity);

        cbStop = new QComboBox(SerialConfigDialog);
        cbStop->setObjectName(QStringLiteral("cbStop"));

        formLayout->setWidget(5, QFormLayout::FieldRole, cbStop);

        cbFlowControl = new QComboBox(SerialConfigDialog);
        cbFlowControl->setObjectName(QStringLiteral("cbFlowControl"));

        formLayout->setWidget(6, QFormLayout::FieldRole, cbFlowControl);

        labelPortInfo = new QLabel(SerialConfigDialog);
        labelPortInfo->setObjectName(QStringLiteral("labelPortInfo"));

        formLayout->setWidget(1, QFormLayout::FieldRole, labelPortInfo);


        horizontalLayout->addLayout(formLayout);

        buttonBox = new QDialogButtonBox(SerialConfigDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Vertical);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        horizontalLayout->addWidget(buttonBox);


        retranslateUi(SerialConfigDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), SerialConfigDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SerialConfigDialog, SLOT(reject()));

        cbBaudRate->setCurrentIndex(2);
        cbData->setCurrentIndex(3);
        cbParity->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(SerialConfigDialog);
    } // setupUi

    void retranslateUi(QDialog *SerialConfigDialog)
    {
        SerialConfigDialog->setWindowTitle(QApplication::translate("SerialConfigDialog", "Dialog", 0));
        label->setText(QApplication::translate("SerialConfigDialog", "Port:", 0));
        label_2->setText(QApplication::translate("SerialConfigDialog", "Baud Rate:", 0));
        cbBaudRate->clear();
        cbBaudRate->insertItems(0, QStringList()
         << QApplication::translate("SerialConfigDialog", "2400", 0)
         << QApplication::translate("SerialConfigDialog", "4800", 0)
         << QApplication::translate("SerialConfigDialog", "9600", 0)
         << QApplication::translate("SerialConfigDialog", "19200", 0)
         << QApplication::translate("SerialConfigDialog", "38400", 0)
        );
        label_3->setText(QApplication::translate("SerialConfigDialog", "Data:", 0));
        cbData->clear();
        cbData->insertItems(0, QStringList()
         << QApplication::translate("SerialConfigDialog", "5 bits", 0)
         << QApplication::translate("SerialConfigDialog", "6 bits", 0)
         << QApplication::translate("SerialConfigDialog", "7 bits", 0)
         << QApplication::translate("SerialConfigDialog", "8 bits", 0)
        );
        label_4->setText(QApplication::translate("SerialConfigDialog", "Parity:", 0));
        label_5->setText(QApplication::translate("SerialConfigDialog", "Stop:", 0));
        label_6->setText(QApplication::translate("SerialConfigDialog", "Flow Control:", 0));
        cbParity->clear();
        cbParity->insertItems(0, QStringList()
         << QApplication::translate("SerialConfigDialog", "None", 0)
         << QApplication::translate("SerialConfigDialog", "Odd", 0)
         << QApplication::translate("SerialConfigDialog", "Even", 0)
         << QApplication::translate("SerialConfigDialog", "Mark", 0)
         << QApplication::translate("SerialConfigDialog", "Space", 0)
        );
        cbStop->clear();
        cbStop->insertItems(0, QStringList()
         << QApplication::translate("SerialConfigDialog", "1 bit", 0)
         << QApplication::translate("SerialConfigDialog", "1.5 bits", 0)
         << QApplication::translate("SerialConfigDialog", "2 bits", 0)
        );
        cbFlowControl->clear();
        cbFlowControl->insertItems(0, QStringList()
         << QApplication::translate("SerialConfigDialog", "None", 0)
         << QApplication::translate("SerialConfigDialog", "RTS/CTS", 0)
         << QApplication::translate("SerialConfigDialog", "XON/XOFF", 0)
        );
        labelPortInfo->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class SerialConfigDialog: public Ui_SerialConfigDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERIALCONFIGDIALOG_H
