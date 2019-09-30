#pragma once

#include <QDialog>
#include "ui_AboutDialog.h"

class AboutDialog : public QDialog
{
	Q_OBJECT

public:
	AboutDialog(QWidget *parent = Q_NULLPTR);
	~AboutDialog();

private:
	Ui::AboutDialog ui;
};
