#include "XPlay.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include "XDemuxThread.h"

using std::cout;
using std::endl;


int main(int argc, char *argv[])
{
	

	
	QApplication a(argc, argv);
	XPlay w;
	w.show();
	
	


	/*XDemuxThread dt;
	char* url = "rtmp://58.200.131.2:1935/livetv/hunantv";
	url = "z:/1.mp4";
	dt.open(url, w.ui.openGLWidget);
	dt.Start();*/
	return a.exec();
}
