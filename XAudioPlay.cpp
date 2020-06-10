#include "XAudioPlay.h"
#include <mutex>

class CXAudioPlay : public XAudioPlay
{
public:

	bool open()
	{
		close();
		QAudioFormat fmt;
		fmt.setSampleRate(sampleRate);
		fmt.setSampleSize(sampleSize);
		fmt.setChannelCount(channels);
		fmt.setCodec("audio/pcm");
		fmt.setByteOrder(QAudioFormat::LittleEndian);
		fmt.setSampleType(QAudioFormat::UnSignedInt);
		mux.lock();
		output = new QAudioOutput(fmt);
		io = output->start();
		mux.unlock();
		if (!io)
		{
			return false;

		}
		return true;
	}
	void close()
	{
		mux.lock();
		if (io)
		{
			io->close();
			io = nullptr;
		}
		if (output)
		{
			output->stop();
			delete output;
			output = nullptr;
		}
		mux.unlock();
	}

	void clear()
	{
		mux.lock();
		if (io)
		{
			io->reset();
		}

		mux.unlock();
	}

	bool write(const unsigned char* data, int dataSize)
	{
		if (data == nullptr || dataSize <= 0)
		{
			return false;
		}
		mux.lock();
		if (output == nullptr || io == nullptr)
		{
			mux.unlock();
			return false;
		}
		int size = io->write((char*)data, dataSize);
		mux.unlock();
		if (dataSize != size)
		{
			return false;
		}
		return true;
	}

	int getFree()
	{
		mux.lock();
		if (!output)       //如果output还没初始化
		{
			mux.unlock();
			return 0;
		}
		
		int free = output->bytesFree();
		mux.unlock();
		return free;



		mux.unlock();
	}
	long long getNoPlayPts()
	{

		mux.lock();
		if (!output)
		{
			mux.unlock();
			return 0;
		}
		long long pts = 0;
		//还未播放的字节
		double size = double(output->bufferSize() - output->bytesFree());
		//计算一秒音频字节大小
		double secSize = double(sampleRate* (sampleSize / 8)* channels);

		if (secSize <= 0)
		{
			pts = 0;
		}
		else

		{
			pts = size /secSize * 1000;
		}


		mux.unlock();

		return pts;

	}

	void setPause(bool isPause)
	{
		mux.lock();
		if (!output)
		{
			mux.unlock();
			return;
		}
		if (isPause)
		{
			output->suspend();
		}
		else
		{
			output->resume();
		}
		mux.unlock();
	}

public:
	QAudioOutput* output = nullptr;
	QIODevice* io = nullptr;
	std::mutex mux;
};
XAudioPlay::XAudioPlay()
{
}

XAudioPlay::~XAudioPlay()
{
}

XAudioPlay* XAudioPlay::get()
{
	static CXAudioPlay play;
	return &play;
}
