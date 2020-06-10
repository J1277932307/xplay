#pragma once
#include <mutex>
extern "C"
{
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavcodec/avcodec.h"
}
class XDemux
{
public:
	XDemux();
	virtual ~XDemux();

	//打开媒体文件或是流媒体文件(rtmp http rstp)
	virtual bool Open(const char* url);

	//读取未解码的包
	virtual AVPacket* readPacket();

	//获取视频参数,返回空间需要清理
	AVCodecParameters* copyVideoPara();

	//获取音频参数，返回空间需要清理
	AVCodecParameters* copyAudioPara();

	virtual bool isAudio(AVPacket* pkt);

	//seek位置,pos范围为0.0~1.0
	virtual bool seek(double pos);

	//只读视频，音频丢弃
	virtual AVPacket* readVideo();

	//清空读取缓存
	virtual void clear();

	//关闭
	virtual void close();

public:


	int totalMs = 0;      //媒体总时长
	int width = 0;    //视频宽
	int height = 0;   //视频高
	int sampleRate = 0; //采样率
	int channels = 0;   //音频通道数
	

protected:
	std::mutex mux;
	AVFormatContext* ic = nullptr;    //解封装上下文
	int videoStream = -1;        //视频索引
	int audioStream = -1;        //音频索引
};

