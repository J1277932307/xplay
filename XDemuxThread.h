#pragma once
#include <QThread>
#include <mutex>
#include "IVideoCall.h"
#include "XDemux.H"
#include "XVideoThread.h"
#include "XAudioThread.h"
class XDemuxThread : public QThread
{
public:
	virtual bool open(const char* url, IVideoCall* call);
	virtual void Start();
	virtual void close();
	virtual void seek(double pos);
	virtual void clear();

	void run();
	XDemuxThread();
	virtual ~XDemuxThread();
	void setPause(bool isPause);

	bool isExit = false;
	long long pts = 0;
	long long totalMs = 0;
	bool isPause = false;
protected:
	std::mutex mux;
	XDemux* demux = nullptr;
	XVideoThread* vt = nullptr;
	XAudioThread* at = nullptr;
};

