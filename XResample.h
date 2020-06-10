#pragma once
#include <mutex>
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
}
class XResample
{

public:
	XResample();
	virtual ~XResample();
	//打开重采样器
	virtual bool open(AVCodecParameters* para,bool isClearPara = false);
	virtual int resample(AVFrame* inData,unsigned char* outData);
	virtual void close();

protected:
	std::mutex mux;
	SwrContext* actx = nullptr;
	AVSampleFormat sampleFormat = AV_SAMPLE_FMT_S16;
};

