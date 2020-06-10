#pragma once
extern "C"
{
#include "libavcodec/avcodec.h"
}
class IVideoCall
{
public:
	virtual void init(int width, int height) = 0;
	virtual void rePaint(AVFrame* frame) = 0;
};

