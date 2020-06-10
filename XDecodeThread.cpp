#include "XDecodeThread.h"

XDecodeThread::XDecodeThread()
{
	if (!decode) decode = new XDecode();
}

XDecodeThread::~XDecodeThread()
{
	//等待线程退出
	isExit = true;
	wait();
}

void XDecodeThread::push(AVPacket* pkt)
{
	if (!pkt)return;
	//阻塞
	while (!isExit)
	{
		mux.lock();
		if (packs.size() < maxList)
		{
			packs.push_back(pkt);
			mux.unlock();
			break;
		}
		mux.unlock();
		msleep(1);
	}
}

void XDecodeThread::clear()
{
	mux.lock();
	decode->clear();
	while (!packs.empty())
	{
		AVPacket* pkt = packs.front();
		XFreePacket(&pkt);
		packs.pop_front();
	}
	mux.unlock();
}

void XDecodeThread::close()
{
	clear();
	
	isExit = true;
	wait();
	
	decode->close();
	mux.lock();
	delete decode;
	decode = nullptr;
	mux.unlock();

}

AVPacket* XDecodeThread::pop()
{
	mux.lock();
	if (packs.empty())
	{
		mux.unlock();
		return nullptr;
	}
	AVPacket* pkt = packs.front();
	packs.pop_front();
	mux.unlock();
	return pkt;
}
