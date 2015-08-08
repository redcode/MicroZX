/*	_________  ___
  _____ \_   /\  \/  /	Qt/MainWindow.cpp
 |  |  |_/  /__>    <	Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
 |   ____________/\__\	Released under the terms of the GNU General Public License v3.
 |_*/


#include "MainWindow.hpp"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
	{
	ui->setupUi(this);
	}

MainWindow::~MainWindow()
	{
	delete ui;
	}

