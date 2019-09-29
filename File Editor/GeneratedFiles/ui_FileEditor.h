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
    QAction *actionAbout_FILE_Editor;
    QAction *actionNew;
    QAction *actionLoad;
    QAction *actionSave;
    QAction *actionExport;
    QAction *actionQuit;
    QAction *actionGo_back;
    QAction *actionGo_forward;
    QAction *actionAdd;
    QAction *actionDelete;
    QAction *actionUp;
    QAction *actionDown;
    QWidget *centralWidget;
    QTableWidget *tableWidget;
    QLabel *gfxPreview;
    QTextEdit *textEdit;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QToolBar *mainToolBar;

    void setupUi(QMainWindow *FileEditorClass)
    {
        if (FileEditorClass->objectName().isEmpty())
            FileEditorClass->setObjectName(QString::fromUtf8("FileEditorClass"));
        FileEditorClass->resize(512, 498);
        actionAbout_FILE_Editor = new QAction(FileEditorClass);
        actionAbout_FILE_Editor->setObjectName(QString::fromUtf8("actionAbout_FILE_Editor"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/FileEditor/Resources/About.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionAbout_FILE_Editor->setIcon(icon);
        actionNew = new QAction(FileEditorClass);
        actionNew->setObjectName(QString::fromUtf8("actionNew"));
        actionLoad = new QAction(FileEditorClass);
        actionLoad->setObjectName(QString::fromUtf8("actionLoad"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/FileEditor/Resources/Load.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionLoad->setIcon(icon1);
        actionSave = new QAction(FileEditorClass);
        actionSave->setObjectName(QString::fromUtf8("actionSave"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/FileEditor/Resources/Save.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave->setIcon(icon2);
        actionExport = new QAction(FileEditorClass);
        actionExport->setObjectName(QString::fromUtf8("actionExport"));
        actionQuit = new QAction(FileEditorClass);
        actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/FileEditor/Resources/Exit.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionQuit->setIcon(icon3);
        actionGo_back = new QAction(FileEditorClass);
        actionGo_back->setObjectName(QString::fromUtf8("actionGo_back"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/FileEditor/Resources/Back.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionGo_back->setIcon(icon4);
        actionGo_forward = new QAction(FileEditorClass);
        actionGo_forward->setObjectName(QString::fromUtf8("actionGo_forward"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/FileEditor/Resources/Forward.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionGo_forward->setIcon(icon5);
        actionAdd = new QAction(FileEditorClass);
        actionAdd->setObjectName(QString::fromUtf8("actionAdd"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/FileEditor/Resources/Thumbs up.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionAdd->setIcon(icon6);
        actionDelete = new QAction(FileEditorClass);
        actionDelete->setObjectName(QString::fromUtf8("actionDelete"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/FileEditor/Resources/Thumbs down.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDelete->setIcon(icon7);
        actionUp = new QAction(FileEditorClass);
        actionUp->setObjectName(QString::fromUtf8("actionUp"));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/FileEditor/Resources/Up.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionUp->setIcon(icon8);
        actionDown = new QAction(FileEditorClass);
        actionDown->setObjectName(QString::fromUtf8("actionDown"));
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/FileEditor/Resources/Down.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDown->setIcon(icon9);
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
        tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
        tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableWidget->setWordWrap(false);
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
        FileEditorClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(FileEditorClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        mainToolBar->setIconSize(QSize(16, 16));
        FileEditorClass->addToolBar(Qt::TopToolBarArea, mainToolBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionNew);
        menuFile->addSeparator();
        menuFile->addAction(actionLoad);
        menuFile->addSeparator();
        menuFile->addAction(actionSave);
        menuFile->addAction(actionExport);
        menuFile->addSeparator();
        menuFile->addAction(actionQuit);
        menuHelp->addAction(actionAbout_FILE_Editor);
        mainToolBar->addAction(actionLoad);
        mainToolBar->addAction(actionSave);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionAdd);
        mainToolBar->addAction(actionDelete);
        mainToolBar->addAction(actionUp);
        mainToolBar->addAction(actionDown);
        mainToolBar->addAction(actionGo_back);
        mainToolBar->addAction(actionGo_forward);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionAbout_FILE_Editor);

        retranslateUi(FileEditorClass);

        QMetaObject::connectSlotsByName(FileEditorClass);
    } // setupUi

    void retranslateUi(QMainWindow *FileEditorClass)
    {
        FileEditorClass->setWindowTitle(QApplication::translate("FileEditorClass", "FileEditor", nullptr));
        actionAbout_FILE_Editor->setText(QApplication::translate("FileEditorClass", "About FILE Editor", nullptr));
        actionNew->setText(QApplication::translate("FileEditorClass", "New", nullptr));
        actionLoad->setText(QApplication::translate("FileEditorClass", "Load", nullptr));
        actionSave->setText(QApplication::translate("FileEditorClass", "Save", nullptr));
        actionExport->setText(QApplication::translate("FileEditorClass", "Export", nullptr));
        actionQuit->setText(QApplication::translate("FileEditorClass", "Quit", nullptr));
        actionGo_back->setText(QApplication::translate("FileEditorClass", "Go back", nullptr));
        actionGo_forward->setText(QApplication::translate("FileEditorClass", "Go forward", nullptr));
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
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("FileEditorClass", "Text", nullptr));
        menuFile->setTitle(QApplication::translate("FileEditorClass", "File", nullptr));
        menuHelp->setTitle(QApplication::translate("FileEditorClass", "Help", nullptr));
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
