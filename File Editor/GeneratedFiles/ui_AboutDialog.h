/********************************************************************************
** Form generated from reading UI file 'AboutDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUTDIALOG_H
#define UI_ABOUTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>

QT_BEGIN_NAMESPACE

class Ui_AboutDialog
{
public:
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;

    void setupUi(QDialog *AboutDialog)
    {
        if (AboutDialog->objectName().isEmpty())
            AboutDialog->setObjectName(QString::fromUtf8("AboutDialog"));
        AboutDialog->resize(221, 174);
        label = new QLabel(AboutDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 10, 201, 101));
        label->setPixmap(QPixmap(QString::fromUtf8(":/About/Resources/logo.png")));
        label->setTextInteractionFlags(Qt::NoTextInteraction);
        label_2 = new QLabel(AboutDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 120, 201, 16));
        label_2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        label_3 = new QLabel(AboutDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 140, 201, 31));
        label_3->setTextFormat(Qt::PlainText);
        label_3->setScaledContents(false);
        label_3->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        label_3->setTextInteractionFlags(Qt::NoTextInteraction);

        retranslateUi(AboutDialog);

        QMetaObject::connectSlotsByName(AboutDialog);
    } // setupUi

    void retranslateUi(QDialog *AboutDialog)
    {
        AboutDialog->setWindowTitle(QApplication::translate("AboutDialog", "AboutDialog", nullptr));
        label->setText(QString());
        label_2->setText(QApplication::translate("AboutDialog", "FILE Editor - Gemini 2019", nullptr));
        label_3->setText(QApplication::translate("AboutDialog", "Get ready for some sexy FILE action\n"
"with preview, pregunta, and prefetching.", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AboutDialog: public Ui_AboutDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUTDIALOG_H
