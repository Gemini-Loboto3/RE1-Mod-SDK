#pragma once

#include <QDialog>
#include "ui_BatchProcess.h"

class BatchProcess : public QDialog
{
	Q_OBJECT

public:
	BatchProcess(QWidget *parent = Q_NULLPTR);
	~BatchProcess();

private:
	Ui::BatchProcess ui;

private slots:
	void onBrowse_in();
	void onBrowse_out();
	void onConvert();
};
