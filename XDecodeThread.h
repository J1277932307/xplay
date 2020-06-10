#pragma once


#include <QThread>
#include <list>
#include <mutex>
#include "libavcodec/avcodec.h"
#include "XDecode.h"


class XDecodeThread :public QThread
{
public:
	XDecodeThread();
	virtual ~XDecodeThread();

	virtual void push(AVPacket* pkt);

	//清理队列
	virtual void clear();
	virtual void close();

	//取出一帧数据
	virtual AVPacket* pop();

	//最大队列
	int maxList = 100;
	bool isExit = false;
	

protected:
	XDecode* decode = nullptr;
	std::list<AVPacket*> packs;
	std::mutex mux;
	
};

