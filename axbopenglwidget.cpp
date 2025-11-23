#include "axbopenglwidget.h"

unsigned int VBO,VAO;
float vertices[] = {
    -0.5f,-0.5f,0.0f,
    0.5f,-0.5f,0.0f,
    0.0f,0.5f,0.0f
};

AXBOpenGLWidget::AXBOpenGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{

}

void AXBOpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    //创建VBO/VAO对象，并赋予ID

    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);

    //绑定VAO VBO 的对象
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);

    //为当前绑定到target的缓冲区对象创建一个新的数据存储区域
    //如果data不是NULL 则使次指针的数据初始化数据存储
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

    //告知显卡如何解析缓冲区的数据
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    //开启VAO管理的第一个属性值
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);


}

void AXBOpenGLWidget::resizeGL(int w, int h)
{

}

void AXBOpenGLWidget::paintGL()
{
    glClearColor(0.2f,0.3f,0.3f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES,0,3);
}
