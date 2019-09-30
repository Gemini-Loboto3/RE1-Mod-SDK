#include "FileEditor.h"
#include <QtWidgets/QApplication>
#include "render.h"

QRender qrender;

int main(int argc, char *argv[])
{
	qrender.LoadFont("font.png");

	QApplication a(argc, argv);
	FileEditor w;
	w.show();
	return a.exec();
}
