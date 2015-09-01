/*     _________  ___
 _____ \_   /\  \/  / Qt/AboutDialog.cpp
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#include "AboutDialog.hpp"
#include "ui_AboutDialog.h"
#include <Z/inspection/compiler.h>



AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AboutDialog)
	{
	ui->setupUi(this);
	setWindowFlags(Qt::Dialog);
	setFixedSize(width(), height());
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	this->setStyleSheet("QToolButton {border:none; color:#0000ff;} QToolButton:hover {text-decoration:underline;}");
	ui->compilationTimeLabel->setText("Compiled on " __DATE__ " " __TIME__ " UTC with " Z_COMPILER_STRING " v" Z_COMPILER_VERSION_STRING);
	}


AboutDialog::~AboutDialog()
	{
	delete ui;
	}


// Qt/AboutDialog.cpp EOF
