#include "FileEditor.h"
#include <QPainter>
#include <QFileDialog>
#include "tinyxml2.h"

#define BASE_X	16
#define BASE_Y	0

#define LEFT	(16 + 32)
#define RIGHT	(320 - LEFT)
#define TOP		(24)
#define BOTTOM	(240 - 24)

FileEditor::FileEditor(QWidget *parent)
	: QMainWindow(parent),
	img(256, 192, QImage::Format_ARGB32),
	font("font.png"),
	pix(320, 240)
{
	encoding.Set_encoding("encoding.xml");

	ui.setupUi(this);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

	ui.gfxPreview->resize(320 + 2, 240 + 2);
	img.fill(QColor(0, 0, 0));

	DrawString(QString(""));

	UpdatePreview();

	connect(ui.textEdit,         SIGNAL(textChanged()), this, SLOT(onTextChange()));
	connect(ui.actionLoad,       SIGNAL(triggered()),   this, SLOT(onLoad()));
	connect(ui.actionAdd,        SIGNAL(triggered()),   this, SLOT(onClicked_add()));
	connect(ui.actionDelete,     SIGNAL(triggered()),   this, SLOT(onClicked_delete()));
	connect(ui.actionGo_back,    SIGNAL(triggered()),   this, SLOT(onClicked_back()));
	connect(ui.actionGo_forward, SIGNAL(triggered()),   this, SLOT(onClicked_next()));

	connect(ui.tableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(onSelection()));
}

void FileEditor::UpdatePreview()
{
	pix.fromImage(img);
	QPainter p(&pix);

	p.drawImage(32, 24, img);
	p.setPen(Qt::red);
	p.drawLine(QPoint(LEFT - 1, 0 ), QPoint(LEFT - 1, 240));
	p.drawLine(QPoint(RIGHT + 1, 0 ), QPoint(RIGHT + 1, 240));
	p.end();

	ui.gfxPreview->setPixmap(pix);
}

void FileEditor::DrawString(QString &str)
{
	img.fill(QColor(0, 0, 0));
	px = BASE_X, py = 0;
	for (int i = 0, si = str.size(); i < si; i++)
	{
		const QChar ch = str[i];
		switch (ch.unicode())
		{
		case '\0':
			return;
		case '\n':
			py += 16;
			px = BASE_X;
			break;
		case '{':
			{
				bool is_tag = false;
				QString tag;
				for (int j = i + 1; j < si; j++)
				{
					if (str[j].unicode() == '}')
					{
						i = j;
						j = si;
						is_tag = true;
						break;
					}
					tag += str[j].unicode();
				}
				// do checks
				if (tag.compare("center", Qt::CaseInsensitive) == 0)
				{
					px = 128 - GetLineWidth(str, i + 1) / 2;
				}
				else if(tag.compare("right", Qt::CaseInsensitive) == 0)
				{
					px = 256 - BASE_X - GetLineWidth(str, i + 1);
				}
				else if (tag.compare("list", Qt::CaseInsensitive) == 0)
				{
					px = 30;
				}
			}
			break;
		default:
			DrawChar(ch.unicode());
		}
	}
}

void FileEditor::DrawChar(ucs_char_t ch)
{
	u8 c = encoding.Find(ch) & 0xff;
	int cx = (c % 16) * 16,
		cy = (c / 16) * 16;

	for (int y = 0; y < 16; y++)
	{
		for (int x = 0; x < 16; x++)
		{
			QColor col = font.pixelColor(x + cx, y + cy);
			if (col == QColor(0, 0, 0)) continue;
			img.setPixelColor(x + px, y + py, col);
		}
	}

	px += encoding.GetWidth(c);
}

int FileEditor::GetLineWidth(QString &str, int pos)
{
	int w = 0;
	for (int i = pos, si = str.size(); i < si; i++)
	{
		ushort ch = str[i].unicode();
		if (ch == '\0' || ch == '{' || ch == '\n')
			break;
		u8 c = encoding.Find(ch) & 0xff;
		w += encoding.GetWidth(c);
	}

	return w;
}

///////////////////////////////////////////
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

	// populate list
	ui.tableWidget->setRowCount(text.size());
	for (int i = 0, si = text.size(); i < si; i++)
	{
		ui.tableWidget->setItem(i, 0, new QTableWidgetItem(text[i]));
	}

	//count = text.size();
	//return count > 0;
}

void FileEditor::onSave()
{

}

void FileEditor::onSelection()
{
	int sel = ui.tableWidget->currentRow();

	ui.textEdit->setText(text[sel]);
}

void FileEditor::onTextChange()
{
	QString str = ui.textEdit->toPlainText();
	DrawString(str);
	UpdatePreview();
}

void FileEditor::onClicked_add()
{
	text.push_back("{center}New page");
	ui.textEdit->setText("");
}

void FileEditor::onClicked_delete()
{
	text.erase(text.begin());
}

void FileEditor::onClicked_back()
{

}

void FileEditor::onClicked_next()
{

}
