#include "XVideoThread.h"
#include <iostream>

using std::endl;
using std::cout;

bool XVideoThread::open(AVCodecParameters* para, IVideoCall* call, int width, int height)
{
	if (para == nullptr)
	{
		return false;
	}
	clear();

	vmux.lock();
	syn_pts = 0;
	this->call = call;
	if (call)
	{
		call->init(width, height);
	}

	vmux.unlock();
	int re = true;
	if (!decode->open(para))
	{
		cout << "audio XDecode open failed!" << endl;
		re = false;

	}

	//cout << "XAudioThread::Open :" << re << endl;
	return re;
}



void XVideoThread::run()
{
	while (!isExit)
	{
		vmux.lock();
		if (this->isPause)
		{
			vmux.unlock();
			msleep(5);
			continue;
		}
		
		//音视频同步
		if (syn_pts >0 && syn_pts < decode->pts)
		{
			vmux.unlock();
			msleep(1);
			continue;
		}

		AVPacket* pkt = pop();
		
		bool re = decode->sendPacket(pkt);
		if (!re)
		{
			vmux.unlock();
			msleep(1);
			continue;
		}

		while (!isExit)
		{
			AVFrame* frame = decode->recvFrame();
			if (frame == nullptr)
			{
				break;
			}
			if (call)
			{
				call->rePaint(frame);
			}
		}
		vmux.unlock();
	}
}



XVideoThread::XVideoThread()
{
}

XVideoThread::~XVideoThread()
{
	//isExit = true;
	//wait();
}

bool XVideoThread::rePaintPts(AVPacket* pkt, long long seekPts)
{
	vmux.lock();

	bool re = decode->sendPacket(pkt);
	if (!re)
	{
		vmux.unlock();
		return true;
	}
		

	AVFrame* frame = decode->recvFrame();
	if (!frame)
	{
		vmux.unlock();
		return false;
	}
		

	if (decode->pts >= seekPts)
	{
		if (call)
			call->rePaint(frame);
		vmux.unlock();
		return true;
	}

	av_frame_free(&frame);
	vmux.unlock();

	return false;
}

void XVideoThread::setPause(bool isPause)
{
	vmux.lock();
	this->isPause = isPause;
	vmux.unlock();
}
