#include "XResample.h"
#include <iostream>
#pragma comment(lib,"swresample.lib")

XResample::XResample()
{
}

XResample::~XResample()
{
}

bool XResample::open(AVCodecParameters* para, bool isClearPara)
{
	if (!para)
	{
		return false;
	}
	mux.lock();
	actx = swr_alloc_set_opts(
		actx,
		av_get_default_channel_layout(2),  //输出声道数
		sampleFormat,                      //输出采样格式
		para->sample_rate,                             //输出采样率
		av_get_default_channel_layout(para->channels),  //输入声道数
		(AVSampleFormat)para->format,       //输入采样格式
		para->sample_rate,                 //输入采样率
		0,0
		);
	if (isClearPara)
	{
		avcodec_parameters_free(&para);
	}
	
	int ret = swr_init(actx);
	mux.unlock();
	if (ret != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(ret, buf, sizeof(buf) - 1);
		std::cout << "swr_init  failed! :" << buf << std::endl;
		return false;
	}


	return true;
}

//返回输出数据大小,不管成功与否都释放inData空间
int XResample::resample(AVFrame* inData, unsigned char* outData)
{
	if (inData == nullptr)
	{
		return  0;
	}
	if (outData == nullptr)
	{
		av_frame_free(&inData);
		return 0;
	}
	uint8_t* data[2] = { 0 };
	data[0] = outData;
	int ret = swr_convert(actx, data, inData->nb_samples, (const uint8_t**)inData->data, inData->nb_samples);
	
	int outSize = ret * inData->channels * av_get_bytes_per_sample(sampleFormat);

	av_frame_free(&inData);

	if (ret <= 0)
		return ret;
	return outSize;


}

void XResample::close()
{
	mux.lock();
	if (actx)
	{
		swr_free(&actx);

	}
	mux.unlock();
}
