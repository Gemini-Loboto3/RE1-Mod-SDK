/********************************************************************************
** Form generated from reading UI file 'FileEditor.ui'
**
** Created by: Qt User Interface Compiler version 5.12.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FILEEDITOR_H
#define UI_FILEEDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FileEditorClass
{
public:
    QAction *actionAbout;
    QAction *actionNew;
    QAction *actionLoad;
    QAction *actionSave;
    QAction *actionExport;
    QAction *actionQuit;
    QAction *actionAdd;
    QAction *actionDelete;
    QAction *actionUp;
    QAction *actionDown;
    QAction *actionBatch;
    QAction *actionSaveAs;
    QWidget *centralWidget;
    QTableWidget *tableWidget;
    QLabel *gfxPreview;
    QTextEdit *textEdit;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QMenu *menuTools;
    QToolBar *mainToolBar;

    void setupUi(QMainWindow *FileEditorClass)
    {
        if (FileEditorClass->objectName().isEmpty())
            FileEditorClass->setObjectName(QString::fromUtf8("FileEditorClass"));
        FileEditorClass->resize(512, 515);
        FileEditorClass->setTabShape(QTabWidget::Rounded);
        actionAbout = new QAction(FileEditorClass);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/png/StatusAnnotations_Information_16xMD_color.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionAbout->setIcon(icon);
        actionAbout->setAutoRepeat(false);
        actionNew = new QAction(FileEditorClass);
        actionNew->setObjectName(QString::fromUtf8("actionNew"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/icons/png/NewScript_16x.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionNew->setIcon(icon1);
        actionLoad = new QAction(FileEditorClass);
        actionLoad->setObjectName(QString::fromUtf8("actionLoad"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/icons/png/OpenFile_16x.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionLoad->setIcon(icon2);
        actionLoad->setAutoRepeat(false);
        actionSave = new QAction(FileEditorClass);
        actionSave->setObjectName(QString::fromUtf8("actionSave"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/icons/png/Save_16x.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave->setIcon(icon3);
        actionSave->setAutoRepeat(false);
        actionExport = new QAction(FileEditorClass);
        actionExport->setObjectName(QString::fromUtf8("actionExport"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/icons/png/SaveAll_16x.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionExport->setIcon(icon4);
        actionQuit = new QAction(FileEditorClass);
        actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/icons/png/Exit_16x.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionQuit->setIcon(icon5);
        actionAdd = new QAction(FileEditorClass);
        actionAdd->setObjectName(QString::fromUtf8("actionAdd"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/icons/png/action_add_16xLG.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionAdd->setIcon(icon6);
        actionAdd->setAutoRepeat(false);
        actionDelete = new QAction(FileEditorClass);
        actionDelete->setObjectName(QString::fromUtf8("actionDelete"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/icons/png/action_Cancel_16xLG.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDelete->setIcon(icon7);
        actionDelete->setAutoRepeat(false);
        actionUp = new QAction(FileEditorClass);
        actionUp->setObjectName(QString::fromUtf8("actionUp"));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/icons/png/CheckInItem_16x.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionUp->setIcon(icon8);
        actionUp->setAutoRepeat(false);
        actionDown = new QAction(FileEditorClass);
        actionDown->setObjectName(QString::fromUtf8("actionDown"));
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/icons/png/CheckOutItem_16x.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDown->setIcon(icon9);
        actionDown->setAutoRepeat(false);
        actionBatch = new QAction(FileEditorClass);
        actionBatch->setObjectName(QString::fromUtf8("actionBatch"));
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/FileEditor/Resources/Disaster.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionBatch->setIcon(icon10);
        actionSaveAs = new QAction(FileEditorClass);
        actionSaveAs->setObjectName(QString::fromUtf8("actionSaveAs"));
        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/icons/png/SaveAs_16x.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSaveAs->setIcon(icon11);
        centralWidget = new QWidget(FileEditorClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        tableWidget = new QTableWidget(centralWidget);
        if (tableWidget->columnCount() < 1)
            tableWidget->setColumnCount(1);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        tableWidget->setGeometry(QRect(10, 10, 151, 431));
        tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableWidget->setTabKeyNavigation(false);
        tableWidget->setProperty("showDropIndicator", QVariant(false));
        tableWidget->setDragDropOverwriteMode(false);
        tableWidget->setAlternatingRowColors(true);
        tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
        tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableWidget->setWordWrap(false);
        tableWidget->setCornerButtonEnabled(false);
        gfxPreview = new QLabel(centralWidget);
        gfxPreview->setObjectName(QString::fromUtf8("gfxPreview"));
        gfxPreview->setGeometry(QRect(170, 200, 331, 241));
        gfxPreview->setFrameShape(QFrame::StyledPanel);
        gfxPreview->setText(QString::fromUtf8(""));
        gfxPreview->setTextFormat(Qt::PlainText);
        gfxPreview->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        gfxPreview->setTextInteractionFlags(Qt::NoTextInteraction);
        textEdit = new QTextEdit(centralWidget);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setGeometry(QRect(170, 10, 331, 181));
        FileEditorClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(FileEditorClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 512, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        menuTools = new QMenu(menuBar);
        menuTools->setObjectName(QString::fromUtf8("menuTools"));
        FileEditorClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(FileEditorClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        mainToolBar->setMovable(false);
        mainToolBar->setIconSize(QSize(16, 16));
        mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        mainToolBar->setFloatable(false);
        FileEditorClass->addToolBar(Qt::TopToolBarArea, mainToolBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuTools->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionNew);
        menuFile->addSeparator();
        menuFile->addAction(actionLoad);
        menuFile->addSeparator();
        menuFile->addAction(actionSave);
        menuFile->addAction(actionSaveAs);
        menuFile->addSeparator();
        menuFile->addAction(actionExport);
        menuFile->addSeparator();
        menuFile->addAction(actionQuit);
        menuHelp->addAction(actionAbout);
        menuTools->addAction(actionAdd);
        menuTools->addAction(actionDelete);
        menuTools->addAction(actionUp);
        menuTools->addAction(actionDown);
        menuTools->addSeparator();
        menuTools->addAction(actionBatch);
        mainToolBar->addAction(actionLoad);
        mainToolBar->addAction(actionSave);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionAdd);
        mainToolBar->addAction(actionDelete);
        mainToolBar->addAction(actionUp);
        mainToolBar->addAction(actionDown);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionAbout);

        retranslateUi(FileEditorClass);

        QMetaObject::connectSlotsByName(FileEditorClass);
    } // setupUi

    void retranslateUi(QMainWindow *FileEditorClass)
    {
        FileEditorClass->setWindowTitle(QApplication::translate("FileEditorClass", "FILE Editor", nullptr));
        actionAbout->setText(QApplication::translate("FileEditorClass", "About", nullptr));
#ifndef QT_NO_TOOLTIP
        actionAbout->setToolTip(QApplication::translate("FileEditorClass", "About", nullptr));
#endif // QT_NO_TOOLTIP
        actionNew->setText(QApplication::translate("FileEditorClass", "New", nullptr));
        actionLoad->setText(QApplication::translate("FileEditorClass", "Load", nullptr));
        actionSave->setText(QApplication::translate("FileEditorClass", "Save", nullptr));
        actionExport->setText(QApplication::translate("FileEditorClass", "Export", nullptr));
        actionQuit->setText(QApplication::translate("FileEditorClass", "Quit", nullptr));
        actionAdd->setText(QApplication::translate("FileEditorClass", "Add", nullptr));
        actionDelete->setText(QApplication::translate("FileEditorClass", "Delete", nullptr));
        actionUp->setText(QApplication::translate("FileEditorClass", "Up", nullptr));
#ifndef QT_NO_TOOLTIP
        actionUp->setToolTip(QApplication::translate("FileEditorClass", "Move before", nullptr));
#endif // QT_NO_TOOLTIP
        actionDown->setText(QApplication::translate("FileEditorClass", "Down", nullptr));
#ifndef QT_NO_TOOLTIP
        actionDown->setToolTip(QApplication::translate("FileEditorClass", "Move after", nullptr));
#endif // QT_NO_TOOLTIP
        actionBatch->setText(QApplication::translate("FileEditorClass", "Batch converter...", nullptr));
        actionSaveAs->setText(QApplication::translate("FileEditorClass", "Save as...", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("FileEditorClass", "Text", nullptr));
        menuFile->setTitle(QApplication::translate("FileEditorClass", "File", nullptr));
        menuHelp->setTitle(QApplication::translate("FileEditorClass", "Help", nullptr));
        menuTools->setTitle(QApplication::translate("FileEditorClass", "Tools", nullptr));
#ifndef QT_NO_TOOLTIP
        mainToolBar->setToolTip(QApplication::translate("FileEditorClass", "Move forward", nullptr));
#endif // QT_NO_TOOLTIP
    } // retranslateUi

};

namespace Ui {
    class FileEditorClass: public Ui_FileEditorClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FILEEDITOR_H
