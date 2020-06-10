#pragma once
#include <mutex>
extern "C"
{
#include "libavcodec/avcodec.h"
}
extern void XFreePacket(AVPacket** pkt);
class XDecode
{
public:
	XDecode();

	virtual ~XDecode();

	//打开解码器,不管成功与否都要释放para空间
	virtual bool open(AVCodecParameters* para);

	virtual void close();
	virtual void clear();

	//发送压缩的视频包到解码线程,注意要释放pkt空间
	virtual bool sendPacket(AVPacket* pkt);

	//从解码线程中接收已解码的视频帧,一次send可能需要多次recv
	//由调用者释放avFrame
	virtual AVFrame* recvFrame();

	bool isAudio = false;
	//当前解码的pts
	long long pts = 0;

protected:
	AVCodecContext* codec = nullptr;
	std::mutex mux;
};

