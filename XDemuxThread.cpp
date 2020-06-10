#include "XDemuxThread.h"
#include <iostream>

using std::endl;
using std::cout;

bool XDemuxThread::open(const char* url, IVideoCall* call)
{
	if (url == nullptr || url == "")
	{
		return false;
	}
	

	mux.lock();
	if (!demux) demux = new XDemux();
	if (!vt) vt = new XVideoThread();
	if (!at) at = new XAudioThread();
	

	

	bool re = demux->Open(url);
	if (!re)
	{
		mux.unlock();
		cout << "demux.open(url) failed!" << endl;
		return false;
	}

	//打开视频解码器和处理线程
	if (!vt->open(demux->copyVideoPara(), call, demux->width, demux->height))
	{
		re = false;
	}

	//打开音频解码器和处理线程
	if (!at->open(demux->copyAudioPara(), demux->sampleRate, demux->channels))
	{
		re = false;
	}
	totalMs = demux->totalMs;
	mux.unlock();
	return re;
	
	
}

void XDemuxThread::Start()
{
	mux.lock();

	if (demux == nullptr)
	{
		demux = new XDemux();

	}

	if (vt == nullptr)
	{
		vt = new XVideoThread();

	}
	if (at == nullptr)
	{
		at = new XAudioThread();
	}

	QThread::start();
	if (vt)
	{
		vt->start();
	}
	if (at)
	{
		at->start();
	}
	mux.unlock();
}

void XDemuxThread::close()
{
	isExit = true;
	wait();
	mux.lock();
	if (vt)
	{
		vt->close();
		delete vt;
		vt = nullptr;
	}
	if (at)
	{
		at->close();
		delete at;
		at = nullptr;
	}
	mux.unlock();
}

void XDemuxThread::seek(double pos)
{
	clear();
	mux.lock();
	bool status = this->isPause;
	mux.unlock();
	setPause(true);
	mux.lock();
	if (demux)
	{
		demux->seek(pos);
	}
	long long seekPts = pos * demux->totalMs;

	
	//如果原来是暂停状态，则保持，否则恢复
	/*if (!status)
	{
		setPause(false);
	}*/

	
	while (!isExit)
	{
		AVPacket* pkt = demux->readVideo();
		if (!pkt)
		{
			break;
		}

		if (vt->rePaintPts(pkt, seekPts))
		{
			this->pts = seekPts;
			break;
		}
		

		

	}
	mux.unlock();
	if (!status)
	{
		setPause(false);
	}
}

void XDemuxThread::clear()
{
	mux.lock();
	if (demux)
	{
		demux->clear();
	}
	if (vt)
	{
		vt->clear();
	}
	if (at)
	{
		at->clear();
	}

	mux.unlock();
}

void XDemuxThread::run()
{
	while (!isExit)
	{
		mux.lock();
		if (isPause)
		{
			mux.unlock();
			msleep(5);
			continue;
		}
		
		if (!demux)
		{
			mux.unlock();
			msleep(5);
			continue;
		}

		

		//音视频同步
		if (vt && at)
		{
			pts = at->pts;
			vt->syn_pts = at->pts;
		}
		AVPacket* pkt = demux->readPacket();
		if (pkt == nullptr)
		{
			mux.unlock();
			msleep(5);
			continue;
		}
		//如果是音频包
		if (demux->isAudio(pkt))
		{
			if (at)
			{
				at->push(pkt);
			}
			
		}
		else
		{
			if (vt)
			{
				vt->push(pkt);
			}
		}


		mux.unlock();
		msleep(1);
	}
}

XDemuxThread::XDemuxThread()
{
}

XDemuxThread::~XDemuxThread()
{
	isExit = true;
	wait();
}

void XDemuxThread::setPause(bool isPause)
{
	mux.lock();
	this->isPause = isPause;
	if (at)
		at->setPause(isPause);
	if (vt)
		vt->setPause(isPause);
	mux.unlock();

}
