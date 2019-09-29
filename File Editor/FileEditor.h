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

	void DrawString(QString &str);
	void DrawChar(ucs_char_t ch);
	int GetLineWidth(QString &str, int pos);

private:
	Ui::FileEditorClass ui;

	QImage img, font;
	int px, py;

	QPixmap pix;
	QStringList text;

private slots:
	void onLoad();
	void onSave();

	void onSelection();

	void onTextChange();
	void onClicked_add();
	void onClicked_delete();
	void onClicked_back();
	void onClicked_next();
};
