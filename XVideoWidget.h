#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QGLShaderProgram>
#include <mutex>
#include "IVideoCall.h"
extern "C"
{

#include "libavcodec/avcodec.h"
}
class XVideoWidget : public QOpenGLWidget, protected QOpenGLFunctions,public IVideoCall
{
    Q_OBJECT

public:
    XVideoWidget(QWidget* parent);
    ~XVideoWidget();
    void init(int width,int height);
    virtual void rePaint(AVFrame* frame);

protected:
    //刷新显示
    void paintGL() override;
    //初始化
    void initializeGL() override;
    //窗口尺寸变化
    void resizeGL(int width, int height) override;

private:
    std::mutex mux;

    //shader程序
    QGLShaderProgram program;

    //shader中yuv变量的地址
    GLuint unis[3] = { 0 };

    //opengl的texture地址
    GLuint texs[3] = { 0 };
    
    //材质内存空间
    unsigned char* datas[3] = { 0 };

    int width = 0;
    int height = 0;
};
