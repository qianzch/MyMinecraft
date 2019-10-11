#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <QWindow>
#include <QOpenGLContext>
#include <QTimer>
#include "gl_painter.h"


/*
*	ʹ��ԭ��OpenGL������Qt��װ�õ�OpenGLʱ����Ҫ��������Ⱦ��һ��Window��
*	Ȼ��Window��Ƕ�뵽Widget������ʵ��������Window
*/
class GL_WINDOW : public QWindow {
	Q_OBJECT
	
public:
	GL_WINDOW(QWindow* parent);

public slots:
	void render(GLfloat *attr, int mode);

protected:
	void exposeEvent(QExposeEvent *event) override;

private:
	QOpenGLContext* context;
};