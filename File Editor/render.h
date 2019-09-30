#include <QImage>
#include <QString>
#include "Encoding.h"

class QRender
{
public:
	void LoadFont(QString filename);

	void DrawString(QString &str, QImage &dst);
	void DrawChar(ucs_char_t ch, QImage &dst);
	int GetLineWidth(QString &str, int pos);

private:
	QImage font;
	int px, py;
};
