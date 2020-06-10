#include "XDemux.h"
#include <iostream>



#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")

using std::cout;
using std::endl;
XDemux::XDemux()
{
	static bool isFirst = true;  //判断是否是第一次
	static std::mutex dmux;      //加锁，为了多线程安全
	dmux.lock();
	if (isFirst)                 //如果是第一次，则进行某些初始化工作
	{
		avformat_network_init();  
		isFirst = false;        

	}
	dmux.unlock();
}
XDemux::~XDemux(){}

//打开媒体文件或是流媒体文件(rtmp http rstp)
bool XDemux::Open(const char* url) {
	

	//参数设置
	close();
	AVDictionary* opts = nullptr;
	//设置rtsp流以tcp协议打开
	av_dict_set(&opts,"rtsp_transport","tcp",0);
	//网络延时时间
	av_dict_set(&opts,"max_delay","500",0);

	mux.lock();

	int ret = avformat_open_input(&ic, url, 0, &opts);
	if (ret != 0)
	{
		mux.unlock();
		char buf[1024] = { 0 };
		av_strerror(ret, buf, sizeof(buf) - 1);
		cout << "open " << url << " failed! :" << buf << endl;
		return false;
	}

	cout << "open " << url << " sucess!" << endl;

	//获取流信息
	ret = avformat_find_stream_info(ic,0);
	if (ret != 0)
	{
		avformat_free_context(ic);
		mux.unlock();
		char buf[1024] = { 0 };
		av_strerror(ret, buf, sizeof(buf) - 1);
		cout << "get stream info  failed! :" << buf << endl;
		return false;
	}

	//获取总时长(毫秒)
	totalMs = ic->duration / (AV_TIME_BASE / 1000);
	cout << "totalMS = " << totalMs << endl;

	//打印视频流详细信息
	av_dump_format(ic,0,url,0);


	//获取视频流索引
	videoStream = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);

	//获取视频宽和高
	width = ic->streams[videoStream]->codecpar->width;
	height = ic->streams[videoStream]->codecpar->height;
	


	//获取音频流索引
	audioStream = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
	//获取采样率
	sampleRate = ic->streams[audioStream]->codecpar->sample_rate;
	//获取音频通道数
	channels = ic->streams[audioStream]->codecpar->channels;
	mux.unlock();

	return true;



}

AVPacket* XDemux::readPacket()
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
		return nullptr;
	}


	AVPacket* pkt = av_packet_alloc();

	//读取一帧，并分配空间
	int re = av_read_frame(ic,pkt);
	if (re != 0)
	{
		av_packet_free(&pkt);
		mux.unlock();
		return nullptr;
	}

	//pts转换为毫秒
	pkt->pts = pkt->pts * (1000 * (av_q2d(ic->streams[pkt->stream_index]->time_base)));
	//pts转换为毫秒
	pkt->dts = pkt->dts * (1000 * (av_q2d(ic->streams[pkt->stream_index]->time_base)));

	mux.unlock();
	cout << pkt->pts << " " << std::flush;
	return pkt;


}


AVCodecParameters* XDemux::copyVideoPara()
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
		return nullptr;
	}
	AVCodecParameters* pa = avcodec_parameters_alloc(); 
	avcodec_parameters_copy(pa, ic->streams[videoStream]->codecpar);

	mux.unlock();
	return pa;
}

AVCodecParameters* XDemux::copyAudioPara()
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
		return nullptr;
	}
	AVCodecParameters* pa = avcodec_parameters_alloc();
	avcodec_parameters_copy(pa, ic->streams[audioStream]->codecpar);

	mux.unlock();
	return pa;
}

bool XDemux::isAudio(AVPacket* pkt)
{
	if (!pkt)
	{
		return false;
	}
	if (pkt->stream_index == videoStream)
	{
		return false;
	}
	return true;
}

bool XDemux::seek(double pos)
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
		return false;
	}
	//清理读取缓冲
	avformat_flush(ic);
	long long seekPos = 0;
	seekPos = ic->streams[videoStream]->duration* pos;  //换算seek的点

	//跳到指定的位置的关键帧
	int ret = av_seek_frame(ic, videoStream, seekPos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
	mux.unlock();
	if (ret < 0)
	{
		return false;
	}

	return true;
}

AVPacket* XDemux::readVideo()
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
		return 0;
	}
	mux.unlock();
	AVPacket* pkt = nullptr;

	
	//防止阻塞
	for (int i = 0; i < 20; ++i)
	{
		pkt = readPacket();
		if (pkt == nullptr)
		{
			break;
		}
		if (pkt->stream_index == videoStream)
		{
			break;
		}
		av_packet_free(&pkt);
	}

	return pkt;
}

void XDemux::clear()
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
		return;
	}

	avformat_flush(ic);
	mux.unlock();
}

void XDemux::close()
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
		return;
	}
	avformat_close_input(&ic);
	totalMs = 0;
	mux.unlock();
}
