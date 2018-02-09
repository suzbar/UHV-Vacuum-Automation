/********************************************************************************
** Form generated from reading UI file 'vcinputwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VCINPUTWINDOW_H
#define UI_VCINPUTWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_VCinputwindow
{
public:
    QGroupBox *groupBox;
    QLabel *label;
    QLabel *label_2;
    QLabel *XTlabel;
    QLabel *XTValLabel;
    QLabel *YTValLabel;
    QLabel *YTlabel;
    QLabel *ZTValLabel;
    QLabel *ZTlabel;
    QLabel *XRValLabel;
    QLabel *XRlabel;
    QLabel *YRValLabel;
    QLabel *YRlabel;
    QLabel *ZRValLabel;
    QLabel *ZR_label;
    QFrame *line;
    QGroupBox *groupBox_2;
    QLabel *label_15;
    QLabel *label_16;
    QLabel *label_19;
    QLabel *label_20;
    QLabel *label_22;
    QDoubleSpinBox *xtrans_in;
    QDoubleSpinBox *ytrans_in;
    QDoubleSpinBox *ztrans_in;
    QDoubleSpinBox *yrot_in;
    QDoubleSpinBox *zrot_in;
    QDoubleSpinBox *xrot_in;
    QLabel *label_17;
    QLabel *label_23;
    QLabel *label_21;
    QPushButton *goto_button;
    QPushButton *stop_button;

    void setupUi(QWidget *VCinputwindow)
    {
        if (VCinputwindow->objectName().isEmpty())
            VCinputwindow->setObjectName(QStringLiteral("VCinputwindow"));
        VCinputwindow->resize(737, 404);
        groupBox = new QGroupBox(VCinputwindow);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(40, 30, 491, 141));
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 40, 91, 31));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 80, 91, 31));
        XTlabel = new QLabel(groupBox);
        XTlabel->setObjectName(QStringLiteral("XTlabel"));
        XTlabel->setGeometry(QRect(160, 50, 21, 21));
        XTValLabel = new QLabel(groupBox);
        XTValLabel->setObjectName(QStringLiteral("XTValLabel"));
        XTValLabel->setGeometry(QRect(180, 50, 67, 21));
        YTValLabel = new QLabel(groupBox);
        YTValLabel->setObjectName(QStringLiteral("YTValLabel"));
        YTValLabel->setGeometry(QRect(270, 50, 67, 21));
        YTlabel = new QLabel(groupBox);
        YTlabel->setObjectName(QStringLiteral("YTlabel"));
        YTlabel->setGeometry(QRect(250, 50, 16, 21));
        ZTValLabel = new QLabel(groupBox);
        ZTValLabel->setObjectName(QStringLiteral("ZTValLabel"));
        ZTValLabel->setGeometry(QRect(360, 50, 67, 21));
        ZTlabel = new QLabel(groupBox);
        ZTlabel->setObjectName(QStringLiteral("ZTlabel"));
        ZTlabel->setGeometry(QRect(340, 50, 21, 21));
        XRValLabel = new QLabel(groupBox);
        XRValLabel->setObjectName(QStringLiteral("XRValLabel"));
        XRValLabel->setGeometry(QRect(180, 90, 67, 21));
        XRlabel = new QLabel(groupBox);
        XRlabel->setObjectName(QStringLiteral("XRlabel"));
        XRlabel->setGeometry(QRect(160, 90, 21, 21));
        YRValLabel = new QLabel(groupBox);
        YRValLabel->setObjectName(QStringLiteral("YRValLabel"));
        YRValLabel->setGeometry(QRect(270, 90, 67, 21));
        YRlabel = new QLabel(groupBox);
        YRlabel->setObjectName(QStringLiteral("YRlabel"));
        YRlabel->setGeometry(QRect(250, 90, 21, 21));
        ZRValLabel = new QLabel(groupBox);
        ZRValLabel->setObjectName(QStringLiteral("ZRValLabel"));
        ZRValLabel->setGeometry(QRect(360, 90, 67, 21));
        ZR_label = new QLabel(groupBox);
        ZR_label->setObjectName(QStringLiteral("ZR_label"));
        ZR_label->setGeometry(QRect(340, 90, 21, 21));
        line = new QFrame(VCinputwindow);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(50, 170, 391, 16));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        groupBox_2 = new QGroupBox(VCinputwindow);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(40, 200, 491, 141));
        label_15 = new QLabel(groupBox_2);
        label_15->setObjectName(QStringLiteral("label_15"));
        label_15->setGeometry(QRect(270, 50, 16, 21));
        label_16 = new QLabel(groupBox_2);
        label_16->setObjectName(QStringLiteral("label_16"));
        label_16->setGeometry(QRect(20, 80, 91, 31));
        label_19 = new QLabel(groupBox_2);
        label_19->setObjectName(QStringLiteral("label_19"));
        label_19->setGeometry(QRect(380, 50, 21, 21));
        label_20 = new QLabel(groupBox_2);
        label_20->setObjectName(QStringLiteral("label_20"));
        label_20->setGeometry(QRect(20, 40, 91, 31));
        label_22 = new QLabel(groupBox_2);
        label_22->setObjectName(QStringLiteral("label_22"));
        label_22->setGeometry(QRect(160, 50, 21, 21));
        xtrans_in = new QDoubleSpinBox(groupBox_2);
        xtrans_in->setObjectName(QStringLiteral("xtrans_in"));
        xtrans_in->setGeometry(QRect(180, 40, 69, 27));
        xtrans_in->setMinimum(-17);
        xtrans_in->setMaximum(0);
        xtrans_in->setSingleStep(0.1);
        ytrans_in = new QDoubleSpinBox(groupBox_2);
        ytrans_in->setObjectName(QStringLiteral("ytrans_in"));
        ytrans_in->setGeometry(QRect(290, 40, 69, 27));
        ytrans_in->setMinimum(-17);
        ytrans_in->setMaximum(0);
        ytrans_in->setSingleStep(0.1);
        ztrans_in = new QDoubleSpinBox(groupBox_2);
        ztrans_in->setObjectName(QStringLiteral("ztrans_in"));
        ztrans_in->setEnabled(false);
        ztrans_in->setGeometry(QRect(400, 40, 69, 27));
        ztrans_in->setMinimum(0);
        ztrans_in->setValue(0);
        yrot_in = new QDoubleSpinBox(groupBox_2);
        yrot_in->setObjectName(QStringLiteral("yrot_in"));
        yrot_in->setEnabled(false);
        yrot_in->setGeometry(QRect(290, 80, 69, 27));
        yrot_in->setMinimum(-180);
        yrot_in->setMaximum(180);
        yrot_in->setSingleStep(0.1);
        zrot_in = new QDoubleSpinBox(groupBox_2);
        zrot_in->setObjectName(QStringLiteral("zrot_in"));
        zrot_in->setGeometry(QRect(400, 80, 69, 27));
        zrot_in->setMaximum(360);
        zrot_in->setSingleStep(0.1);
        xrot_in = new QDoubleSpinBox(groupBox_2);
        xrot_in->setObjectName(QStringLiteral("xrot_in"));
        xrot_in->setEnabled(false);
        xrot_in->setGeometry(QRect(180, 80, 69, 27));
        xrot_in->setMinimum(-180);
        xrot_in->setMaximum(180);
        xrot_in->setSingleStep(0.1);
        xrot_in->setValue(-90);
        label_17 = new QLabel(groupBox_2);
        label_17->setObjectName(QStringLiteral("label_17"));
        label_17->setGeometry(QRect(270, 90, 16, 21));
        label_23 = new QLabel(groupBox_2);
        label_23->setObjectName(QStringLiteral("label_23"));
        label_23->setGeometry(QRect(160, 90, 21, 21));
        label_21 = new QLabel(groupBox_2);
        label_21->setObjectName(QStringLiteral("label_21"));
        label_21->setGeometry(QRect(380, 90, 21, 21));
        goto_button = new QPushButton(VCinputwindow);
        goto_button->setObjectName(QStringLiteral("goto_button"));
        goto_button->setGeometry(QRect(590, 300, 111, 27));
        goto_button->setMouseTracking(true);
        stop_button = new QPushButton(VCinputwindow);
        stop_button->setObjectName(QStringLiteral("stop_button"));
        stop_button->setGeometry(QRect(590, 150, 111, 27));

        retranslateUi(VCinputwindow);

        QMetaObject::connectSlotsByName(VCinputwindow);
    } // setupUi

    void retranslateUi(QWidget *VCinputwindow)
    {
        VCinputwindow->setWindowTitle(QApplication::translate("VCinputwindow", "Form", 0));
        groupBox->setTitle(QApplication::translate("VCinputwindow", "Current Sample Position", 0));
        label->setText(QApplication::translate("VCinputwindow", "Translation:", 0));
        label_2->setText(QApplication::translate("VCinputwindow", "Rotation:", 0));
        XTlabel->setText(QApplication::translate("VCinputwindow", "X", 0));
        XTValLabel->setText(QApplication::translate("VCinputwindow", "0.001", 0));
        YTValLabel->setText(QApplication::translate("VCinputwindow", "0.001", 0));
        YTlabel->setText(QApplication::translate("VCinputwindow", "Y", 0));
        ZTValLabel->setText(QApplication::translate("VCinputwindow", "0.001", 0));
        ZTlabel->setText(QApplication::translate("VCinputwindow", "Z", 0));
        XRValLabel->setText(QApplication::translate("VCinputwindow", "0.001", 0));
        XRlabel->setText(QApplication::translate("VCinputwindow", "X", 0));
        YRValLabel->setText(QApplication::translate("VCinputwindow", "0.001", 0));
        YRlabel->setText(QApplication::translate("VCinputwindow", "Y", 0));
        ZRValLabel->setText(QApplication::translate("VCinputwindow", "0.001", 0));
        ZR_label->setText(QApplication::translate("VCinputwindow", "Z", 0));
        groupBox_2->setTitle(QApplication::translate("VCinputwindow", "Position Sample at:", 0));
        label_15->setText(QApplication::translate("VCinputwindow", "Y", 0));
        label_16->setText(QApplication::translate("VCinputwindow", "Rotation:", 0));
        label_19->setText(QApplication::translate("VCinputwindow", "Z", 0));
        label_20->setText(QApplication::translate("VCinputwindow", "Translation:", 0));
        label_22->setText(QApplication::translate("VCinputwindow", "X", 0));
        label_17->setText(QApplication::translate("VCinputwindow", "Y", 0));
        label_23->setText(QApplication::translate("VCinputwindow", "X", 0));
        label_21->setText(QApplication::translate("VCinputwindow", "Z", 0));
        goto_button->setText(QApplication::translate("VCinputwindow", "Goto Position", 0));
        stop_button->setText(QApplication::translate("VCinputwindow", "Stop", 0));
    } // retranslateUi

};

namespace Ui {
    class VCinputwindow: public Ui_VCinputwindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VCINPUTWINDOW_H
