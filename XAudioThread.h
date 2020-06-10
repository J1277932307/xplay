#pragma once
#include <QThread>
#include <list>
#include "XDecode.h"
#include "XAudioPlay.h"
#include "XResample.h"
#include "XDecodeThread.h"

class XAudioThread :public XDecodeThread
{
public:
	virtual bool open(AVCodecParameters* para,int sampleRate,int channels);
	void run();
	//停止线程，清理资源
	virtual void close();
	XAudioThread();
	virtual ~XAudioThread();

	//暂停
	void setPause(bool isPause);

	void clear();

public:
	//当前音频播放的pts
	long long pts = 0;
	bool isPause = false;



	

protected:
	
	std::mutex amux;
	XAudioPlay* ap = nullptr;
	XResample* res = nullptr;
	
};

