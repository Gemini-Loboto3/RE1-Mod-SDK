#include "BatchProcess.h"
#include <QFileDialog>
#include "tinyxml2.h"
#include "render.h"

extern QRender qrender;

BatchProcess::BatchProcess(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	connect(ui.pushBrowse_in,  SIGNAL(released()), this, SLOT(onBrowse_in()));
	connect(ui.pushBrowse_out, SIGNAL(released()), this, SLOT(onBrowse_out()));
	connect(ui.pushConvert,    SIGNAL(released()), this, SLOT(onConvert()));
}

BatchProcess::~BatchProcess()
{
}

void BatchProcess::onBrowse_in()
{
	ui.lineEdit_in->setText(QFileDialog::getExistingDirectory(this, "Select input folder", ""));
}

void BatchProcess::onBrowse_out()
{
	ui.lineEdit_out->setText(QFileDialog::getExistingDirectory(this, "Select output folder", ""));
}

void BatchProcess::onConvert()
{
	QDir directory(ui.lineEdit_in->text());
	QStringList list = directory.entryList(QStringList() << "*.xml" << "*.XML", QDir::Files);

	ui.progressBar->setValue(0);
	ui.progressBar->setMaximum(list.size());

	QString in_folder = ui.lineEdit_in->text();
	QString out_folder = ui.lineEdit_out->text();
	for (int i = 0, si = list.size(); i < si; i++)
	{
		QString filename = list[i];

		QString in = in_folder + '/' + filename;
		filename.replace(".xml", "", Qt::CaseInsensitive);
		QString tname;
		tname.sprintf("%s/%s", out_folder.toStdString().c_str(), filename.toStdString().c_str());

		tinyxml2::XMLDocument xml;
		xml.LoadFile(in.toUtf8());

		QStringList text;

		auto root = xml.FirstChild();
		if (strcmp("Strings", root->Value()) != 0)
			continue;

		auto s = root->FirstChildElement("Text");
		while (s)
		{
			const char *ss = s->GetText() ? s->GetText() : "";
			QString st = QString::fromUtf8(ss);
			st = st.replace("\\n", "\n");
			text.push_back(st);
			s = s->NextSiblingElement("Text");
		}

		// process all strings
		//QRender render;
		QString out;
		QImage img(256, 192, QImage::Format_RGBA8888);
		for (int j = 0, sj = text.size(); j < sj; j++)
		{
			img.fill(QColor(0, 0, 0, 0));
			qrender.DrawString(text[j], img);
			out.sprintf("%s%02d.png", tname.toStdString().c_str(), j);
			img.save(out, "png");
		}

		ui.progressBar->setValue(i + 1);
	}
}
