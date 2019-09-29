#include "FileEditor.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	FileEditor w;
	w.show();
	return a.exec();
}
