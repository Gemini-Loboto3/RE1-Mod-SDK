/********************************************************************************
** Form generated from reading UI file 'BatchProcess.ui'
**
** Created by: Qt User Interface Compiler version 5.12.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BATCHPROCESS_H
#define UI_BATCHPROCESS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_BatchProcess
{
public:
    QLineEdit *lineEdit_in;
    QLabel *label;
    QPushButton *pushBrowse_in;
    QPushButton *pushBrowse_out;
    QLineEdit *lineEdit_out;
    QLabel *label_2;
    QPushButton *pushConvert;
    QProgressBar *progressBar;

    void setupUi(QDialog *BatchProcess)
    {
        if (BatchProcess->objectName().isEmpty())
            BatchProcess->setObjectName(QString::fromUtf8("BatchProcess"));
        BatchProcess->resize(411, 140);
        lineEdit_in = new QLineEdit(BatchProcess);
        lineEdit_in->setObjectName(QString::fromUtf8("lineEdit_in"));
        lineEdit_in->setGeometry(QRect(10, 30, 311, 20));
        label = new QLabel(BatchProcess);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 10, 81, 16));
        pushBrowse_in = new QPushButton(BatchProcess);
        pushBrowse_in->setObjectName(QString::fromUtf8("pushBrowse_in"));
        pushBrowse_in->setGeometry(QRect(330, 30, 75, 23));
        pushBrowse_out = new QPushButton(BatchProcess);
        pushBrowse_out->setObjectName(QString::fromUtf8("pushBrowse_out"));
        pushBrowse_out->setGeometry(QRect(330, 70, 75, 23));
        lineEdit_out = new QLineEdit(BatchProcess);
        lineEdit_out->setObjectName(QString::fromUtf8("lineEdit_out"));
        lineEdit_out->setGeometry(QRect(10, 70, 311, 20));
        label_2 = new QLabel(BatchProcess);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 50, 81, 16));
        pushConvert = new QPushButton(BatchProcess);
        pushConvert->setObjectName(QString::fromUtf8("pushConvert"));
        pushConvert->setGeometry(QRect(330, 110, 75, 23));
        progressBar = new QProgressBar(BatchProcess);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setEnabled(true);
        progressBar->setGeometry(QRect(10, 110, 311, 23));
        progressBar->setValue(0);
        progressBar->setTextVisible(false);
        progressBar->setOrientation(Qt::Horizontal);

        retranslateUi(BatchProcess);

        QMetaObject::connectSlotsByName(BatchProcess);
    } // setupUi

    void retranslateUi(QDialog *BatchProcess)
    {
        BatchProcess->setWindowTitle(QApplication::translate("BatchProcess", "BatchProcess", nullptr));
        label->setText(QApplication::translate("BatchProcess", "Input folder", nullptr));
        pushBrowse_in->setText(QApplication::translate("BatchProcess", "Browse...", nullptr));
        pushBrowse_out->setText(QApplication::translate("BatchProcess", "Browse...", nullptr));
        label_2->setText(QApplication::translate("BatchProcess", "Output folder", nullptr));
        pushConvert->setText(QApplication::translate("BatchProcess", "Convert!", nullptr));
    } // retranslateUi

};

namespace Ui {
    class BatchProcess: public Ui_BatchProcess {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BATCHPROCESS_H
