/*     _________  ___
 _____ \_   /\  \/  / Qt/main.cpp
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#include "MainWindow.hpp"
#include "MachineWindow.hpp"
#include <QApplication>
#include <X11/Xlib.h>


int main(int argc, char **argv)
	{
	XInitThreads();
	QApplication application(argc, argv);
	MachineWindow window;
	window.show();

	return application.exec();
	}
