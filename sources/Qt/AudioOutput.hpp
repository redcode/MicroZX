/*     _________  ___
 _____ \_   /\  \/  / Qt/AudioOutputPlayer.hpp
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#ifndef __mZX_Qt_AudioOutput_HPP
#define __mZX_Qt_AudioOutput_HPP

#include <QIODevice>
#include <QAudioOutput>
#include <QTimer>
#include <Z/types/buffering.h>

class AudioOutput : QIODevice {
	public:
	ZRingBuffer buffer;

	private:
	znatural      bufferFrameCount;
	znatural      sampleRate;
	QTimer*	      timer;
	QAudioOutput* output;

	public:
	AudioOutput(QObject *parent);
	~AudioOutput();
	qint64 readData(char *data, qint64 maxlen);
	qint64 writeData(const char *data, qint64 len);
	bool start();
	void stop();
};

#endif // __mZX_Qt_AudioOutput_HPP
