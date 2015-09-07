/*     _________  ___
 _____ \_   /\  \/  / Qt/MachineWindow.cpp
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#include "MachineWindow.hpp"
#include "ui_MachineWindow.h"
#include "AboutDialog.hpp"

#include <QFile>
#include <QMessageBox>
#include <QKeyEvent>
#include <QFileDialog>
#include <QInputDialog>
#include <math.h>
#include <Z/keys/layout.h>
#include <Z/functions/buffering/ZTripleBuffer.h>
#include <Z/functions/buffering/ZRingBuffer.h>
#include <Z/functions/geometry/ZRectangle.h>
#include <Z/hardware/machine/model/computer/ZX Spectrum/ZX Spectrum.h>
#include "system.h"
#include <Z/types/time.h>
#include "MachineABI.h"

#define MACHINE_SCREEN_SIZE z_2d(Z_ZX_SPECTRUM_SCREEN_WIDTH, Z_ZX_SPECTRUM_SCREEN_HEIGHT)


static AboutDialog* aboutDialog = NULL;


double MachineWindow::currentZoom()
	{
	return isFullScreen()
		? ui->videoOutputView->contentSize().x / double(Z_ZX_SPECTRUM_SCREEN_WIDTH)
		: double(ui->videoOutputView->width()) / double(Z_ZX_SPECTRUM_SCREEN_WIDTH);
	}


void MachineWindow::setZoom(zreal zoom)
	{
	if (isFullScreen())
		{
		Z2D boundsSize = z_2d(ui->videoOutputView->width(), ui->videoOutputView->height());
		Z2D zoomedSize = z_2d(zreal(Z_ZX_SPECTRUM_SCREEN_WIDTH) * zoom, zreal(Z_ZX_SPECTRUM_SCREEN_HEIGHT) * zoom);

		ui->videoOutputView->setContentSize(z_2d_contains(boundsSize, zoomedSize)
			? zoomedSize
			: z_2d_fit(MACHINE_SCREEN_SIZE, boundsSize));
		}

	else	{
		QRect frame = geometry();

		frame.setHeight(frame.height() - ui->videoOutputView->height() + int(zreal(Z_ZX_SPECTRUM_SCREEN_HEIGHT) * zoom));
		frame.setWidth(zreal(Z_ZX_SPECTRUM_SCREEN_WIDTH) * zoom);

		setGeometry(frame);
		}
	}


MachineWindow::MachineWindow(QWidget *parent) :	QMainWindow(parent), ui(new Ui::MachineWindow)
	{
	ui->setupUi(this);
	addActions(ui->menuBar->actions());
	ui->videoOutputView->setFocus(Qt::OtherFocusReason);

	qDebug("%d", hasMouseTracking());
	//ui->centralwidget->layout()->setMenuBar(ui->menubar);
	//setMenuBar(NULL);

	flags.running = false;

	//---------------------------------------.
	// Create the machine and its components |
	//---------------------------------------'

	ui->videoOutputView->setResolutionAndFormat
		(z_2d_type(SIZE)(Z_ZX_SPECTRUM_SCREEN_WIDTH, Z_ZX_SPECTRUM_SCREEN_HEIGHT), 0);

	alsa_output_initialize(&audioOutput);


	keyboardBuffer = (ZTripleBuffer *)malloc(sizeof(ZTripleBuffer));
	z_triple_buffer_initialize(keyboardBuffer, malloc(sizeof(zuint64) * 3), sizeof(zuint64));
	keyboard = (Z64Bit *)z_triple_buffer_production_buffer(keyboardBuffer);
	memset(keyboardBuffer->buffers[0], 0xFF, sizeof(zuint64) * 3);

	z_ring_buffer_initialize(&audioOutputBuffer, malloc(sizeof(zint16) * 882 * 4), sizeof(zint16) * 882, 4);

	MachineABI *abi = &machine_abi_table[4];

	machine_initialize(&machine, abi, ui->videoOutputView->buffer(), &audioOutput.buffer);
	machine.keyboard_input = keyboardBuffer;

	zsize index = abi->rom_count;
	ROM *rom;

	while (index)
		{
		rom = &abi->roms[--index];

		QString fileName;
		fileName.sprintf("/home/ciro/ROMs/NEStalin/%s.rom", rom->file_name);

		QFile file(fileName);

		if (!file.open(QIODevice::ReadOnly))
			{
			QMessageBox::information(this, tr("Unable to open file"),
			file.errorString());
			}

		else	{
			file.read((char *)(machine.context->memory + rom->base_address), rom->size);
			file.close();
			}
		}

	keyboardState.value_uint64 = Z_UINT64(0xFFFFFFFFFFFFFFFF);
	setWindowTitle(QString(abi->model_name));
	ui->videoOutputView->start();
	machine_power(&machine, ON);
	alsa_output_start(&audioOutput);
	}


MachineWindow::~MachineWindow()
	{
	ui->videoOutputView->stop();
	alsa_output_stop(&audioOutput);
	alsa_output_finalize(&audioOutput);
	//delete audioOutput;
	machine_power(&machine, OFF);
	delete ui;
	}


#define KEY_DOWN(line, bit) keyboardState.array_uint8[line] &= ~(1 << bit)
#define KEY_UP(  line, bit) keyboardState.array_uint8[line] |=  (1 << bit)


void MachineWindow::keyPressEvent(QKeyEvent* event)
	{
	//int key = event->key();
	//qDebug("%c => %d\n", (char)key, key);
	//Qt::Key
	switch (event->key())
		{
		case Qt::Key_Escape:
		if (isFullScreen())
			{
			if (fullScreenMenuFrame->isVisible())
				{
				fullScreenMenuFrame->hide();
				QApplication::setOverrideCursor(Qt::BlankCursor);
				}

			else	{
				fullScreenMenuFrame->show();
				QApplication::restoreOverrideCursor();
				}
			}
		break;

		// Line 0
		case Qt::Key_Shift: KEY_DOWN(0, 0); break;
		case Qt::Key_Z: KEY_DOWN(0, 1); break;
		case Qt::Key_X: KEY_DOWN(0, 2); break;
		case Qt::Key_C: KEY_DOWN(0, 3); break;
		case Qt::Key_V: KEY_DOWN(0, 4); break;

		// Line 1
		case Qt::Key_A: KEY_DOWN(1, 0); break;
		case Qt::Key_S: KEY_DOWN(1, 1); break;
		case Qt::Key_D: KEY_DOWN(1, 2); break;
		case Qt::Key_F: KEY_DOWN(1, 3); break;
		case Qt::Key_G: KEY_DOWN(1, 4); break;

		//Line 2
		case Qt::Key_Q: KEY_DOWN(2, 0); break;
		case Qt::Key_W: KEY_DOWN(2, 1); break;
		case Qt::Key_E: KEY_DOWN(2, 2); break;
		case Qt::Key_R: KEY_DOWN(2, 3); break;
		case Qt::Key_T: KEY_DOWN(2, 4); break;

		// Line 3
		case Qt::Key_1: KEY_DOWN(3, 0); break;
		case Qt::Key_2: KEY_DOWN(3, 1); break;
		case Qt::Key_3: KEY_DOWN(3, 2); break;
		case Qt::Key_4: KEY_DOWN(3, 3); break;
		case Qt::Key_5: KEY_DOWN(3, 4); break;

		// Line 4
		case Qt::Key_0: KEY_DOWN(4, 0); break;
		case Qt::Key_9: KEY_DOWN(4, 1); break;
		case Qt::Key_8: KEY_DOWN(4, 2); break;
		case Qt::Key_7: KEY_DOWN(4, 3); break;
		case Qt::Key_6: KEY_DOWN(4, 4); break;

		// Line 5
		case Qt::Key_P: KEY_DOWN(5, 0); break;
		case Qt::Key_O: KEY_DOWN(5, 1); break;
		case Qt::Key_I: KEY_DOWN(5, 2); break;
		case Qt::Key_U: KEY_DOWN(5, 3); break;
		case Qt::Key_Y: KEY_DOWN(5, 4); break;

		// Line 6
		case Qt::Key_Return: KEY_DOWN(6, 0); break;
		case Qt::Key_L: KEY_DOWN(6, 1); break;
		case Qt::Key_K: KEY_DOWN(6, 2); break;
		case Qt::Key_J: KEY_DOWN(6, 3); break;
		case Qt::Key_H: KEY_DOWN(6, 4); break;

		// Line 7
		case Qt::Key_Space: KEY_DOWN(7, 0); break;
		case Qt::Key_Alt: KEY_DOWN(7, 1); break;
		case Qt::Key_M: KEY_DOWN(7, 2); break;
		case Qt::Key_N: KEY_DOWN(7, 3); break;
		case Qt::Key_B: KEY_DOWN(7, 4); break;

		// Composed
		case Qt::Key_Backspace:	KEY_DOWN(4, 0); KEY_DOWN(0, 0); break;
		case Qt::Key_Comma:	KEY_DOWN(7, 1); KEY_DOWN(7, 3); break;
		case Qt::Key_Down:	KEY_DOWN(0, 0); KEY_DOWN(4, 4); break;
		case Qt::Key_Up:	KEY_DOWN(0, 0); KEY_DOWN(4, 3); break;
		case Qt::Key_Left:	KEY_DOWN(0, 0); KEY_DOWN(3, 4); break;
		case Qt::Key_Right:	KEY_DOWN(0, 0); KEY_DOWN(4, 2); break;

		default: break;
		}

	keyboard->value_uint64 = keyboardState.value_uint64;
	keyboard = (Z64Bit *)z_triple_buffer_produce(keyboardBuffer);
	}


void MachineWindow::keyReleaseEvent(QKeyEvent* event)
	{
	switch (event->key())
		{
		// Line 0
		case Qt::Key_Shift: KEY_UP(0, 0); break;
		case Qt::Key_Z: KEY_UP(0, 1); break;
		case Qt::Key_X: KEY_UP(0, 2); break;
		case Qt::Key_C: KEY_UP(0, 3); break;
		case Qt::Key_V: KEY_UP(0, 4); break;
		// Line 1
		case Qt::Key_A: KEY_UP(1, 0); break;
		case Qt::Key_S: KEY_UP(1, 1); break;
		case Qt::Key_D: KEY_UP(1, 2); break;
		case Qt::Key_F: KEY_UP(1, 3); break;
		case Qt::Key_G: KEY_UP(1, 4); break;
		//Line 2
		case Qt::Key_Q: KEY_UP(2, 0); break;
		case Qt::Key_W: KEY_UP(2, 1); break;
		case Qt::Key_E: KEY_UP(2, 2); break;
		case Qt::Key_R: KEY_UP(2, 3); break;
		case Qt::Key_T: KEY_UP(2, 4); break;
		// Line 3
		case Qt::Key_1: KEY_UP(3, 0); break;
		case Qt::Key_2: KEY_UP(3, 1); break;
		case Qt::Key_3: KEY_UP(3, 2); break;
		case Qt::Key_4: KEY_UP(3, 3); break;
		case Qt::Key_5: KEY_UP(3, 4); break;
		// Line 4
		case Qt::Key_0: KEY_UP(4, 0); break;
		case Qt::Key_9: KEY_UP(4, 1); break;
		case Qt::Key_8: KEY_UP(4, 2); break;
		case Qt::Key_7: KEY_UP(4, 3); break;
		case Qt::Key_6: KEY_UP(4, 4); break;
		// Line 5
		case Qt::Key_P: KEY_UP(5, 0); break;
		case Qt::Key_O: KEY_UP(5, 1); break;
		case Qt::Key_I: KEY_UP(5, 2); break;
		case Qt::Key_U: KEY_UP(5, 3); break;
		case Qt::Key_Y: KEY_UP(5, 4); break;
		// Line 6
		case Qt::Key_Return: KEY_UP(6, 0); break;
		case Qt::Key_L: KEY_UP(6, 1); break;
		case Qt::Key_K: KEY_UP(6, 2); break;
		case Qt::Key_J: KEY_UP(6, 3); break;
		case Qt::Key_H: KEY_UP(6, 4); break;
		// Line 7
		case Qt::Key_Space:	 KEY_UP(7, 0); break;
		case Qt::Key_Alt: KEY_UP(7, 1); break;
		case Qt::Key_M: KEY_UP(7, 2); break;
		case Qt::Key_N: KEY_UP(7, 3); break;
		case Qt::Key_B: KEY_UP(7, 4); break;
		// Composed
		case Qt::Key_Backspace:	KEY_UP(4, 0); KEY_UP(0, 0); break;
		case Qt::Key_Comma:	KEY_UP(7, 1); KEY_UP(7, 3); break;
		case Qt::Key_Down:	KEY_UP(0, 0); KEY_UP(4, 4); break;
		case Qt::Key_Up:	KEY_UP(0, 0); KEY_UP(4, 3); break;
		case Qt::Key_Left:	KEY_UP(0, 0); KEY_UP(3, 4); break;
		case Qt::Key_Right:	KEY_UP(0, 0); KEY_UP(4, 2); break;

		default: break;
		}

	keyboard->value_uint64 = keyboardState.value_uint64;
	keyboard = (Z64Bit *)z_triple_buffer_produce(keyboardBuffer);
	}


void MachineWindow::mouseDoubleClickEvent(QMouseEvent *)
	{
	bool enabled = !ui->actionViewFullScreen->isChecked();

	ui->actionViewFullScreen->setChecked(enabled);
	}


void MachineWindow::resizeEvent(QResizeEvent *)
	{
	if (isFullScreen())
		{
		ui->videoOutputView->setScaling(Z_SCALING_NONE);

		ui->videoOutputView->setContentSize(z_2d_fit
			(z_2d(Z_ZX_SPECTRUM_SCREEN_WIDTH, Z_ZX_SPECTRUM_SCREEN_HEIGHT),
			 z_2d(ui->videoOutputView->width(), ui->videoOutputView->height())));

		fullScreenMenuFrame->setGeometry(0.0, 0.0, ui->videoOutputView->width(), ui->menuBar->height());
		fullScreenMenuFrame->show();
		}
	}


void MachineWindow::aboutDialogClosed()
	{aboutDialog = NULL;}


void MachineWindow::on_actionFileNewWindow_triggered()
	{
	}


void MachineWindow::on_actionFileOpen_triggered()
	{
	QString fileName = QFileDialog::getOpenFileName
		(this, tr("Open ROM Image"), "", tr("NES ROM Images (*.nes)"));
	}



void MachineWindow::on_actionFileQuit_triggered()
	{
	exit(0);
	}


void MachineWindow::on_actionMachinePower_toggled(bool enabled)
	{
	ui->actionMachinePause->setChecked(false);
	ui->actionMachinePause->setEnabled(enabled);
	ui->actionMachineReset->setEnabled(enabled);

	bool state = !machine.flags.power;

	machine_power(&machine, state);

	if (state) ui->videoOutputView->start();

	else	{
		ui->videoOutputView->stop();
		ui->videoOutputView->blank();
		}
	}


void MachineWindow::on_actionMachinePause_toggled(bool enabled)
	{
	Q_UNUSED(enabled);
	bool state = !machine.flags.pause;

	machine_pause(&machine, state);

	if (state) ui->videoOutputView->stop();
	else ui->videoOutputView->start();
	}


void MachineWindow::on_actionMachineReset_triggered()
	{
	ui->actionMachinePause->setChecked(false);

	bool pause = machine.flags.pause;
	machine_reset(&machine);
	if (pause) ui->videoOutputView->start();
	}


void MachineWindow::on_actionViewFullScreen_toggled(bool enabled)
	{
	if (enabled)
		{
		qDebug("FULL");
		showFullScreen();
		fullScreenMenuFrame = new QFrame(this);
		fullScreenMenuFrame->setAutoFillBackground(true);
		QHBoxLayout layout;
		fullScreenMenuFrame->setLayout(&layout);
		layout.setMenuBar(ui->menuBar);
		setMenuBar(NULL);
		}

	else	{
		//setMenuBar(ui->menubar);
		ui->menuBar->show();
		ui->videoOutputView->setScaling(Z_SCALING_FIT);
		if (fullScreenMenuFrame->isHidden()) QApplication::restoreOverrideCursor();
		setMenuBar(ui->menuBar);
		//fullScreenMenuFrame->layout()->setMenuBar(NULL);
		fullScreenMenuFrame->setParent(NULL);
		delete fullScreenMenuFrame;
		fullScreenMenuFrame = NULL;
		showNormal();
		}
	}


void MachineWindow::on_actionViewZoomIn_triggered()
	{setZoom(floor(currentZoom() / 0.5) * 0.5 + 0.5);}


void MachineWindow::on_actionViewZoomOut_triggered()
	{
	zreal zoom = ceil(currentZoom() / 0.5) * 0.5 - 0.5;

	setZoom(zoom <= 1.0 ? 1.0 : zoom);
	}


void MachineWindow::on_actionView1x_triggered() {setZoom(1.0);}
void MachineWindow::on_actionView2x_triggered() {setZoom(2.0);}
void MachineWindow::on_actionView3x_triggered() {setZoom(3.0);}


void MachineWindow::on_actionViewSmooth_toggled(bool enabled)
	{
	ui->videoOutputView->setLinearInterpolation(enabled);
	}


void MachineWindow::on_actionWindowEditTitle_triggered()
	{
	bool ok;
	QInputDialog dialog(this);

	dialog.setWindowFlags((dialog.windowFlags() & ~Qt::WindowMaximizeButtonHint));

	QString text = dialog.getText
		(this, QString(" "),
		 tr("Window title:"), QLineEdit::Normal,
		 windowTitle(), &ok);

	if (ok) setWindowTitle(text.isEmpty() ? QString(machine.abi->model_name) : text);
	}


void MachineWindow::on_actionHelpAbout_triggered()
	{
	if (!aboutDialog)
		{
		(aboutDialog = new AboutDialog(this))->setAttribute(Qt::WA_DeleteOnClose);
		connect(aboutDialog, SIGNAL(finished(int)), this, SLOT(aboutDialogClosed()));
		}

	aboutDialog->show();
	aboutDialog->raise();
	}


// Qt/MachineWindow.cpp EOF
