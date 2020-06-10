#pragma once
#include <QThread>
#include <list>
#include <mutex>
#include "XDecode.h"
#include "IVideoCall.h"
#include "XDecodeThread.h"

extern "C"
{
#include "libavcodec/avcodec.h"
}
class XVideoThread :public XDecodeThread
{
public:
	virtual bool open(AVCodecParameters* para, IVideoCall* call, int width, int height);
	
	void run();
	XVideoThread();
	virtual ~XVideoThread();

	//解码pts，如果接收到的解码数据pts>= seekPts,返回真，并且显示画面
	virtual bool rePaintPts(AVPacket* pkt, long long seekPts);

	//暂停
	void setPause(bool isPause);


	//同步时间
	long long syn_pts = 0;
	bool isPause = false;
	

protected:
	IVideoCall* call = nullptr;
	std::mutex vmux;
	
	
};

