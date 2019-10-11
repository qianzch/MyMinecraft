#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <QWindow>
#include <QOpenGLContext>
#include <QTimer>
#include "gl_painter.h"


/*
*	使用原生OpenGL而不是Qt封装好的OpenGL时，需要将画面渲染在一个Window中
*	然后将Window再嵌入到Widget里，这个类实现了上述Window
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