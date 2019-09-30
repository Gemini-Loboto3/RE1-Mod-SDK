#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_FileEditor.h"
#include "Encoding.h"

class FileEditor : public QMainWindow
{
	Q_OBJECT

public:
	FileEditor(QWidget *parent = Q_NULLPTR);

	void UpdatePreview();

	//void DrawString(QString &str, QImage &dst);
	//void DrawChar(ucs_char_t ch, QImage &dst);
	//int GetLineWidth(QString &str, int pos);
	void Swap(int it1, int it2);

	void Enable(bool enable);

private:
	Ui::FileEditorClass ui;

	QImage img/*, font*/;
	//int px, py;

	QPixmap pix;
	QString file_name;
	QStringList text;

private slots:
	void onNew();
	void onLoad();
	void onSave();
	void onSaveAs();
	void onExport();

	void onBatch();
	void onAbout();
	void onSelection();

	void onTextChange();
	void onClicked_add();
	void onClicked_delete();
	void onClicked_back();
	void onClicked_next();
	void onClicked_moveup();
	void onClicked_movedn();
};
