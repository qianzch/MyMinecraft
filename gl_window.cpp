#include "gl_window.h"

GL_WINDOW::GL_WINDOW(QWindow* parent) : QWindow(parent) {
	setSurfaceType(QWindow::OpenGLSurface);
	context = nullptr;
}

void GL_WINDOW::render(GLfloat *attr/*unused*/, int mode/*unused*/) {
	if (!isExposed())
		return;
	if (!context) {
		context = new QOpenGLContext(this);
		context->setFormat(requestedFormat());
		context->create();
	}
	const qreal retinaScale = devicePixelRatio();
	viewport_width = width() * retinaScale;
	viewport_height = height() * retinaScale;
	glViewport(0, 0, viewport_width, viewport_height);
	context->makeCurrent(this);

	if (!GL_PAINTER::if_init()) GL_PAINTER::init();
	
	// 这个函数里只有下面这一行可以修改...其他地方请不要动
	GL_PAINTER::paint(attr, mode);
	
	context->swapBuffers(this);
}

void GL_WINDOW::exposeEvent(QExposeEvent *event) {
	Q_UNUSED(event);

	if (isExposed()) {
		render(nullptr, 0);
	}
}