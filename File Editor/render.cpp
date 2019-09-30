#include "render.h"

#define BASE_X	16
#define BASE_Y	0

void QRender::DrawString(QString &str, QImage &dst)
{
	//img.fill(QColor(0, 0, 0));

	// count the number of lines
	int lines = 1;
	for (int i = 0, si = str.size(); i < si; i++)
		if (str[i].toLatin1() == '\n')
			lines++;

	px = BASE_X, py = (192 - (lines * 16)) / 2;
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
			else if (tag.compare("right", Qt::CaseInsensitive) == 0)
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
			DrawChar(ch.unicode(), dst);
		}
	}
}

void QRender::DrawChar(ucs_char_t ch, QImage &dst)
{
	// skip spaces entirely
	if (ch == ' ')
	{
		px += encoding.GetWidth((u8)encoding.Find(ch));
		return;
	}

	u8 c = encoding.Find(ch) & 0xff;
	int cx = (c % 16) * 16,
		cy = (c / 16) * 16;

	for (int y = 0; y < 16; y++)
	{
		for (int x = 0; x < 16; x++)
		{
			QColor col = font.pixelColor(x + cx, y + cy);
			if (col == QColor(0, 0, 0)) continue;
			dst.setPixelColor(x + px, y + py, col);
		}
	}

	px += encoding.GetWidth(c);
}

int QRender::GetLineWidth(QString &str, int pos)
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

void QRender::LoadFont(QString filename)
{
	font.load(filename);
}
