#pragma once

#include <QAudioFormat.h>
#include <QAudioOutput.h>
class XAudioPlay
{
public:

	int sampleRate = 44100;
	int sampleSize = 16;
	int channels = 2;

	XAudioPlay();
	virtual ~XAudioPlay();

	static XAudioPlay* get();

	//打开音频播放
	virtual bool open() = 0;
	virtual void close() = 0;
	virtual bool write(const unsigned char* data, int dataSize) = 0;;
	virtual int getFree() = 0;
	//返回缓冲上还没有播放的时间(ms)
	virtual long long getNoPlayPts() = 0;
	virtual void setPause(bool isPause) = 0;
	virtual void clear() = 0;
public:
	
};

