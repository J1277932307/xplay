#include "XAudioThread.h"
#include <iostream>

using std::cout;
using std::endl;

bool XAudioThread::open(AVCodecParameters* para, int sampleRate, int channels)
{
	if (para == nullptr)
		return false;
	clear();
	amux.lock();
	pts = 0;
	bool re = true;

	
	
	if (!res->open(para,false))
	{
		cout << "XResample open failed " << endl;
		re = false;
	}

	ap->channels = channels;
	ap->sampleRate = sampleRate;

	if (!ap->open())
	{
		re = false;
		cout << "XAudioPlay open failed!" << endl;
	}

	if (!decode->open(para))
	{
		cout << "audio XDecode open failed!" << endl;
		re = false;
	}

	amux.unlock();
	//cout << "XAudioThread::Open :" << re << endl;
	return re;
}

void XAudioThread::run()
{
	unsigned char* pcm = new unsigned char[1024 * 1024 * 10];
	while (!isExit)
	{
		amux.lock();
		
		if (isPause)
		{
			amux.unlock();
			msleep(5);
			continue;
		}
		AVPacket* pkt = pop();
		
		
		bool re = decode->sendPacket(pkt);
		if (!re)
		{
			amux.unlock();
			msleep(1);
			continue;
		}

		//一次发送，多次接收
		while (!isExit)
		{
			AVFrame* frame = decode->recvFrame();
			if(frame == nullptr)
			{
				break;
			}
			pts = decode->pts - ap->getNoPlayPts();

			//cout << "audio pst = " << pts << endl;

			//重采样
			int size = res->resample(frame, pcm);

			//播放音频
			while (!isExit)
			{
				if (size <= 0)
				{
					break;
				}
				//缓冲未播完，空间不够
				if (ap->getFree() < size || isPause)
				{
					msleep(1);
					continue;
				}
				ap->write(pcm, size);
				break;
			}
		}
		

		amux.unlock();
	}
	delete [] pcm;
}

void XAudioThread::close()
{
	XDecodeThread::close();
	if (res)
	{
		res->close();
		amux.lock();
		delete res;
		res = nullptr;
		amux.unlock();
	}
	if (ap)
	{
		ap->close();
		
		amux.lock();
		delete ap;
		ap = nullptr;
		amux.unlock();
	}
}

XAudioThread::XAudioThread()
{
	if (!res) res = new XResample();
	if (!ap) ap = XAudioPlay::get();
}
XAudioThread::~XAudioThread()
{
	isExit = true;
	wait();
}

void XAudioThread::setPause(bool isPause)
{
	
	this->isPause = isPause;
	if (ap)
	{
		ap->setPause(isPause);
	}
	
}

void XAudioThread::clear()
{

	XDecodeThread::clear();
	mux.lock();

	if (ap)
	{
		ap->clear();
	}
	mux.unlock();
}
