#include "FileEditor.h"
#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>
#include "tinyxml2.h"
#include "BatchProcess.h"
#include "AboutDialog.h"
#include "render.h"
//#include "lodepng.h"

extern QRender qrender;

#define BASE_X	16
#define BASE_Y	0

#define LEFT	(16 + 32)
#define RIGHT	(320 - LEFT)
#define TOP		(24)
#define BOTTOM	(240 - 24)

FileEditor::FileEditor(QWidget *parent)
	: QMainWindow(parent),
	img(256, 192, QImage::Format_ARGB32),
	//font("font.png"),
	pix(320, 240)
{
	pix.fill(QColor(0, 0, 0));
	encoding.Set_encoding("encoding.xml");

	ui.setupUi(this);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

	ui.gfxPreview->resize(320 + 2, 240 + 2);

	img.fill(QColor(0, 0, 0));
	qrender.DrawString(QString(""), img);

	UpdatePreview();

	connect(ui.textEdit,			SIGNAL(textChanged()),	this,	SLOT(onTextChange()));
	connect(ui.actionNew,			SIGNAL(triggered()),	this,	SLOT(onNew()));
	connect(ui.actionLoad,			SIGNAL(triggered()),	this,	SLOT(onLoad()));
	connect(ui.actionSave,			SIGNAL(triggered()),	this,	SLOT(onSave()));
	connect(ui.actionSaveAs,		SIGNAL(triggered()),	this,	SLOT(onSaveAs()));
	connect(ui.actionExport,		SIGNAL(triggered()),	this,	SLOT(onExport()));
	connect(ui.actionAdd,			SIGNAL(triggered()),	this,	SLOT(onClicked_add()));
	connect(ui.actionDelete,		SIGNAL(triggered()),	this,	SLOT(onClicked_delete()));
	connect(ui.actionUp,			SIGNAL(triggered()),	this,	SLOT(onClicked_moveup()));
	connect(ui.actionDown,			SIGNAL(triggered()),	this,	SLOT(onClicked_movedn()));
	connect(ui.actionBatch,			SIGNAL(triggered()),	this,	SLOT(onBatch()));
	connect(ui.actionAbout,			SIGNAL(triggered()),	this,	SLOT(onAbout()));

	connect(ui.tableWidget,			SIGNAL(itemSelectionChanged()), this, SLOT(onSelection()));

	Enable(false);
}

void FileEditor::UpdatePreview()
{
	pix.fromImage(img);
	QPainter p(&pix);

	p.drawImage(32, 24, img);
	p.setPen(Qt::red);
	p.drawLine(QPoint(LEFT - 1, 0 ), QPoint(LEFT - 1, 240));
	p.drawLine(QPoint(RIGHT + 1, 0 ), QPoint(RIGHT + 1, 240));
	p.drawLine(QPoint(0, TOP - 1), QPoint(320, TOP - 1));
	p.drawLine(QPoint(0, BOTTOM + 1), QPoint(320, BOTTOM + 1));
	p.end();

	ui.gfxPreview->setPixmap(pix);
}

//void FileEditor::DrawString(QString &str, QImage &dst)
//{
//	img.fill(QColor(0, 0, 0));
//
//	// count the number of lines
//	int lines = 1;
//	for (int i = 0, si = str.size(); i < si; i++)
//		if (str[i].toLatin1() == '\n')
//			lines++;
//
//	px = BASE_X, py = (192 - (lines * 16)) / 2;
//	for (int i = 0, si = str.size(); i < si; i++)
//	{
//		const QChar ch = str[i];
//		switch (ch.unicode())
//		{
//		case '\0':
//			return;
//		case '\n':
//			py += 16;
//			px = BASE_X;
//			break;
//		case '{':
//			{
//				bool is_tag = false;
//				QString tag;
//				for (int j = i + 1; j < si; j++)
//				{
//					if (str[j].unicode() == '}')
//					{
//						i = j;
//						j = si;
//						is_tag = true;
//						break;
//					}
//					tag += str[j].unicode();
//				}
//				// do checks
//				if (tag.compare("center", Qt::CaseInsensitive) == 0)
//				{
//					px = 128 - GetLineWidth(str, i + 1) / 2;
//				}
//				else if(tag.compare("right", Qt::CaseInsensitive) == 0)
//				{
//					px = 256 - BASE_X - GetLineWidth(str, i + 1);
//				}
//				else if (tag.compare("list", Qt::CaseInsensitive) == 0)
//				{
//					px = 30;
//				}
//			}
//			break;
//		default:
//			DrawChar(ch.unicode(), dst);
//		}
//	}
//}
//
//void FileEditor::DrawChar(ucs_char_t ch, QImage &dst)
//{
//	u8 c = encoding.Find(ch) & 0xff;
//	int cx = (c % 16) * 16,
//		cy = (c / 16) * 16;
//
//	for (int y = 0; y < 16; y++)
//	{
//		for (int x = 0; x < 16; x++)
//		{
//			QColor col = font.pixelColor(x + cx, y + cy);
//			if (col == QColor(0, 0, 0)) continue;
//			dst.setPixelColor(x + px, y + py, col);
//		}
//	}
//
//	px += encoding.GetWidth(c);
//}
//
//int FileEditor::GetLineWidth(QString &str, int pos)
//{
//	int w = 0;
//	for (int i = pos, si = str.size(); i < si; i++)
//	{
//		ushort ch = str[i].unicode();
//		if (ch == '\0' || ch == '{' || ch == '\n')
//			break;
//		u8 c = encoding.Find(ch) & 0xff;
//		w += encoding.GetWidth(c);
//	}
//
//	return w;
//}

void FileEditor::Swap(int it1, int it2)
{
	QString s1 = text[it1],
		s2 = text[it2];

	text[it2] = s1;
	text[it1] = s2;

	ui.tableWidget->item(it1, 0)->setText(s2);
	ui.tableWidget->item(it2, 0)->setText(s1);
	ui.tableWidget->selectRow(it2);
}

void FileEditor::Enable(bool enable)
{
	ui.tableWidget->setEnabled(enable);
	ui.gfxPreview->setEnabled(enable);
	ui.textEdit->setEnabled(enable);

	ui.actionSave->setEnabled(enable);
	ui.actionSaveAs->setEnabled(enable);
	ui.actionExport->setEnabled(enable);
	ui.actionAdd->setEnabled(enable);
	ui.actionDelete->setEnabled(enable);
	ui.actionUp->setEnabled(enable);
	ui.actionDown->setEnabled(enable);
}

///////////////////////////////////////////
void FileEditor::onNew()
{
	text.clear();
	text.push_back("{center}New FILE");
	file_name.clear();

	// populate list
	ui.tableWidget->reset();
	ui.tableWidget->setRowCount(text.size());
	for (int i = 0, si = text.size(); i < si; i++)
		ui.tableWidget->setItem(i, 0, new QTableWidgetItem(text[i]));
	ui.tableWidget->selectRow(0);

	Enable(true);
	this->setWindowTitle(QString("FILE Editor [New project - unnamed]"));
}

void FileEditor::onLoad()
{
	QString filename = QFileDialog::getOpenFileName(this, "Open XML project", "", "XML project (*.xml);;All files (*)");
	if (filename.isEmpty()) return;

	tinyxml2::XMLDocument xml;
	xml.LoadFile(filename.toUtf8());

	text.clear();

	auto root = xml.FirstChild();
	if (strcmp("Strings", root->Value()) != 0)
	{
		//count = 0;
		return;
	}
	auto s = root->FirstChildElement("Text");
	while (s)
	{
		const char *ss = s->GetText() ? s->GetText() : "";
		QString st = QString::fromUtf8(ss);
		st = st.replace("\\n", "\n");
		text.push_back(st);
		s = s->NextSiblingElement("Text");
	}

	if (text.size() == 0)
		return;

	// populate list
	ui.tableWidget->reset();
	ui.tableWidget->setRowCount(text.size());
	for (int i = 0, si = text.size(); i < si; i++)
		ui.tableWidget->setItem(i, 0, new QTableWidgetItem(text[i]));
	ui.tableWidget->selectRow(0);

	file_name = filename;
	Enable(true);

	this->setWindowTitle(QString("FILE Editor ["+ file_name +"]"));
	//count = text.size();
	//return count > 0;
}

void FileEditor::onSave()
{
	if (file_name.isEmpty())
	{
		file_name = QFileDialog::getSaveFileName(this, "Save XML project", "", "XML project (*.xml);;All files (*)");
		if (file_name.isEmpty())
			return;
	}

	tinyxml2::XMLDocument xml;
	if (text.size() == 0) return;
	xml.SetBOM(true);

	tinyxml2::XMLElement *items = xml.NewElement("Strings");
	// extract text
	for (size_t i = 0, si = text.size(); i < si; i++)
	{
		tinyxml2::XMLElement *sub = xml.NewElement("Text");

		QString temp = text[i];
		temp.replace("\n", "\\n");

		sub->SetText(temp.toUtf8().constData());
		items->InsertEndChild(sub);
	}

	xml.InsertEndChild(items);
	xml.SaveFile(file_name.toStdString().c_str());

	this->setWindowTitle(QString("FILE Editor ["+file_name+"]"));
}

void FileEditor::onSaveAs()
{
	file_name = QFileDialog::getSaveFileName(this, "Save XML project", "", "XML project (*.xml);;All files (*)");
	if (file_name.isEmpty())
		return;

	onSave();
}

void FileEditor::onExport()
{
	QString filename = QFileDialog::getSaveFileName(this, "Export FILE", "", "FILE image sequence (*)");
	if (filename.isEmpty())
		return;

	QImage out(256, 192, QImage::Format::Format_RGBA8888);
	for (int i = 0, si = text.size(); i < si; i++)
	{
		QString name;
		name.sprintf("%s%02d.png", filename.toStdString().c_str(), i);
		out.fill(QColor(0, 0, 0, 0));
		qrender.DrawString(text[i], out);
		out.save(name, "png");
	}
}

void FileEditor::onBatch()
{
	BatchProcess b(this);
	b.exec();
}

void FileEditor::onAbout()
{
	AboutDialog b(this);
	b.exec();
}

void FileEditor::onSelection()
{
	int sel = ui.tableWidget->currentRow();

	ui.textEdit->setText(text[sel]);
}

void FileEditor::onTextChange()
{
	QString str = ui.textEdit->toPlainText();
	img.fill(QColor(0, 0, 0));
	qrender.DrawString(str, img);
	UpdatePreview();
	// update list
	int sel = ui.tableWidget->currentRow();
	ui.tableWidget->item(sel, 0)->setText(str);
	text[sel] = str;
}

void FileEditor::onClicked_add()
{
	text.push_back("{center}New page");
	int ncount = text.size();

	ui.tableWidget->setRowCount(ncount);
	ui.tableWidget->setItem(ncount - 1, 0, new QTableWidgetItem(text[ncount - 1]));
	ui.tableWidget->selectRow(ncount - 1);
	//ui.textEdit->setText("");
}

void FileEditor::onClicked_delete()
{
	int cnt = text.size();
	if (cnt <= 1)
	{
		QMessageBox(QMessageBox::Information, QString("Warning"), QString("You can't have a FILE with\nno text entries.")).exec();
		return;
	}
	int row = ui.tableWidget->currentRow();

	ui.tableWidget->removeRow(row);
	text.erase(text.begin() + row);
}

void FileEditor::onClicked_back()
{

}

void FileEditor::onClicked_next()
{

}

void FileEditor::onClicked_moveup()
{
	int sel = ui.tableWidget->currentRow();
	// can't move anymore
	if (sel == 0) return;

	int cnt = text.size();
	// can't move one entry
	if (cnt <= 1) return;

	Swap(sel, sel - 1);
}

void FileEditor::onClicked_movedn()
{
	int cnt = text.size();
	int sel = ui.tableWidget->currentRow();
	// can't move anymore or one entry
	if (sel + 1 >= cnt || cnt <= 1) return;

	Swap(sel, sel + 1);
}
