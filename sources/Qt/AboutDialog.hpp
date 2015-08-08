/*	_________  ___
  _____ \_   /\  \/  /	Qt/AboutDialog.hpp
 |  |  |_/  /__>    <	Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
 |   ____________/\__\	Released under the terms of the GNU General Public License v3.
 |_*/

#ifndef __mZX_Qt_AboutDialog_HPP
#define __mZX_Qt_AboutDialog_HPP

#include <QDialog>

namespace Ui {
	class AboutDialog;
}

class AboutDialog : public QDialog {Q_OBJECT
	public:
	explicit AboutDialog(QWidget *parent = 0);
	~AboutDialog();

	private:
	Ui::AboutDialog *ui;
};

#endif // __mZX_Qt_AboutDialog_HPP
