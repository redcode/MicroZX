/*     _________  ___
 _____ \_   /\  \/  / Qt/MainWindow.hpp
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

namespace Ui {
	class MainWindow;
	}

class MainWindow : public QMainWindow
	{
	Q_OBJECT

	public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	private:
	Ui::MainWindow *ui;
	};

#endif // MAINWINDOW_HPP
