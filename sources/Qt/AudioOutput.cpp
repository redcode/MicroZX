/*     _________  ___
 _____ \_   /\  \/  / Qt/AudioOutputPlayer.cpp
|  |  |_/  /__>    <  Copyright © 2014-2015 Manuel Sainz de Baranda y Goñi.
|   ____________/\__\ Released under the GNU General Public License v3.
|_*/

#include "AudioOutput.hpp"
#include <QAudioOutput>
#include <Z/functions/buffering/ZRingBuffer.h>
#include <stdlib.h>

#define FRAME_SIZE Z_INT16_SIZE * 882


AudioOutput::AudioOutput(QObject *parent)
: QIODevice(parent), sampleRate(44100), bufferFrameCount(2)
	{
	void *frames = calloc(1, FRAME_SIZE * 4);
	z_ring_buffer_initialize(&buffer, frames, Z_INT16_SIZE * 882, 4);
	}

AudioOutput::~AudioOutput()
	{
	}


qint64 AudioOutput::readData(char *data, qint64 maxlen)
	{
	void *frame;

	if (buffer.fill_count < 2)
		{
		frame = z_ring_buffer_consumption_buffer(&buffer);
		memcpy(data, frame, FRAME_SIZE);
		return FRAME_SIZE;
		}

	while (buffer.fill_count > 3) z_ring_buffer_try_consume(&buffer);

	frame = z_ring_buffer_consumption_buffer(&buffer);
	z_ring_buffer_try_consume(&buffer);
	memcpy(data, frame, FRAME_SIZE);
	return FRAME_SIZE;
	}


qint64 AudioOutput::writeData(const char *data, qint64 len)
	{
	Q_UNUSED(data);
	Q_UNUSED(len);
	return 0;
	}


bool AudioOutput::start()
	{
	timer = new QTimer(this);

	QAudioFormat format;
	format.setFrequency(sampleRate);
	format.setChannels(1);
	format.setSampleSize(16);
	format.setCodec("audio/pcm");
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setSampleType(QAudioFormat::SignedInt);

	QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());

	if (!info.isFormatSupported(format))
		return false;

	output = new QAudioOutput(QAudioDeviceInfo::defaultOutputDevice(), format, this);

	open(QIODevice::ReadOnly);
	output->start(this);
	return true;
	}


void AudioOutput::stop()
	{
	}
