#include "XPlay.h"
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include "XDemuxThread.h"


static XDemuxThread dt;


XPlay::XPlay(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	dt.Start();
	startTimer(40);
	
}

void XPlay::openFile()
{
	videoPath = QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("选择视频文件"));
	if (videoPath.isEmpty())
	{
		return;
	}
	this->setWindowTitle(videoPath);
	if (!dt.open(videoPath.toLocal8Bit(), ui.openGLWidget))
	{
		
		QMessageBox::information(0, "error", "open file failed!");
		return;
	}
	setPause(dt.isPause);

	
}

XPlay::~XPlay()
{
	dt.close();
}

void XPlay::timerEvent(QTimerEvent* e)
{
	if (isSliderPress)
	{
		return;
	}
	long long total = dt.totalMs;
	if (total > 0)
	{
		double pos = (double)dt.pts / (double)total;
		
		int v = ui.playPos->maximum()* pos;
		ui.playPos->setValue(v);
	}
}

void XPlay::resizeEvent(QResizeEvent* e)
{
	ui.playPos->move(50, this->height() - 100);
	ui.playPos->resize(this->width() - 100, ui.playPos->height());
	ui.pushButton->move(100, this->height() - 150);
	ui.playPos->move(ui.pushButton->x() + ui.pushButton->width() + 20, ui.pushButton->y());
	ui.openGLWidget->resize(this->size());
}

void XPlay::mouseDoubleClickEvent(QMouseEvent* e)
{
	if (isFullScreen())
	{
		this->showNormal();
	}
	else
	{
		this->showFullScreen();
	}
}

void XPlay::setPause(bool isPause)
{
	if (isPause)
	{
		ui.play->setText(QString::fromLocal8Bit("播放"));
	}
	else
	{
		ui.play->setText(QString::fromLocal8Bit("暂停"));
	}
}

void XPlay::playOrpause()
{
	bool isPause = !dt.isPause;
	setPause(isPause);
	dt.setPause(isPause);

}

void XPlay::sliderPress()
{
	isSliderPress = true;
}

void XPlay::sliderRelease()
{
	isSliderPress = false;

	double pos = 0.0;
	//计算位置
	pos = (double)ui.playPos->value() / (double)ui.playPos->maximum();
	dt.seek(pos);
}
