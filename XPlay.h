#pragma once

#include <QtWidgets/QWidget>
#include "ui_XPlay.h"

class XPlay : public QWidget
{
	Q_OBJECT

public:
	XPlay(QWidget *parent = Q_NULLPTR);
	~XPlay();

	//定时器 滑动条显示
	void timerEvent(QTimerEvent* e);

	//窗口尺寸变化事件
	void resizeEvent(QResizeEvent* e);

	//双击全屏
	void mouseDoubleClickEvent(QMouseEvent* e);

	void setPause(bool isPause);

	


public slots:
	void openFile();
	void playOrpause();
	void sliderPress();
	void sliderRelease();

private:
	QString videoPath = "";
	Ui::XPlayClass ui;
	bool isSliderPress = false;
};
