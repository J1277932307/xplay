#include "XDecode.h"
#include <iostream>
using std::cout;
using std::endl;

void XFreePacket(AVPacket** pkt)
{
	if (!pkt || !(*pkt))return;
	av_packet_free(pkt);
}


XDecode::XDecode()
{
}

XDecode::~XDecode()
{

}

bool XDecode::open(AVCodecParameters* para)
{

	if (!para)
	{
		return false;
	}
	close();

	//找到解码器
	AVCodec* vcodec = avcodec_find_decoder(para->codec_id);
	if (!vcodec)
	{
		//如果查找解码器失败，则释放para空间
		avcodec_parameters_free(&para);
		cout << "can't find the codec id " << para->codec_id << endl;
		return false;
	}

	//为解码器上下文分配空间
	mux.lock();
	codec = avcodec_alloc_context3(vcodec);

	//将解码器参数复制到新分配的解码器上下文中
	avcodec_parameters_to_context(codec, para);

	//para使命完成，释放para空间
	avcodec_parameters_free(&para);

	//设置八线程解码
	codec->thread_count = 8;

	//打开解码器
	int re = avcodec_open2(codec,0,0);
	if (re != 0)
	{
		//如果打开解码器失败
		//释放资源
		
		avcodec_free_context(&codec);
		
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << "avcodec_open2 failed : " << buf << endl;
		mux.unlock();
		return false;	
	}
	mux.unlock();
	cout << "avcodec_open2 success!" << endl;
	return true;
}

void XDecode::close()
{
	mux.lock();
	if (codec)
	{
		avcodec_close(codec);
		avcodec_free_context(&codec);
	}
	pts = 0;
	mux.unlock();
}

void XDecode::clear()
{
	mux.lock();
	if (codec)
	{
		avcodec_flush_buffers(codec);
	}
	mux.unlock();
}

bool XDecode::sendPacket(AVPacket* pkt)
{

	if (pkt == nullptr || pkt->size <= 0 || pkt->data == nullptr)
	{
		return false;
	}
	
	mux.lock();
	if (!codec)
	{
		mux.unlock();
		return false;
	}
	//发送到解码线程
	int ret = avcodec_send_packet(codec,pkt);
	mux.unlock();
	av_packet_free(&pkt);
	if (ret != 0)
	{
		return false;
	}
	return true;
}

AVFrame* XDecode::recvFrame()
{
	mux.lock();
	if (!codec)
	{
		mux.unlock();
		return nullptr;
	}
	AVFrame* frame = av_frame_alloc();
	int ret = avcodec_receive_frame(codec, frame);
	mux.unlock();
	if (ret != 0)
	{
		av_frame_free(&frame);
		return nullptr;
	}
	pts = frame->pts;
	return frame;
}
