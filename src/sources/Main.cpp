#include "MainWindow.h"
#include <QApplication>

///
/// Entry point for the Qt Application.
///
/// \param argc Command line arguments
/// \param argv Number of command line arguments
/// \return Standard codes
///
int main(int argc, char** argv) {
	// Set application values
	QCoreApplication::setApplicationName("MADP-TreeVis");
	QCoreApplication::setOrganizationName("UoL");
	QCoreApplication::setApplicationVersion("1.0");

	// Create application, show it
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
