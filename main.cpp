#include "preheatstation.h"
#include <QtWidgets/QApplication>
#include "cmdDevice.h"
#include "database.h"
#include "preheatstation.h"
#include "deviceInquiry.h"
#include "cmdDevice.h"
#include "OpenNI.h"


using namespace openni;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	PreheatStation w;


	w.show();
	return a.exec();
}
