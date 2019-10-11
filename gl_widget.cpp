#include "gl_widget.h"
#include "global.h"
// ------------------------------ public -------------------------------------

GL_WIDGET::GL_WIDGET(QWidget *parent) : QWidget(parent),
	size_weight(SIZE_WEIGHT) {

	gl_window = new GL_WINDOW(nullptr);
	gl_window_container = createWindowContainer(gl_window, this);
	setFocus();
	gl_window_container->setGeometry(0, 0, INIT_WIDTH, INIT_HEIGHT);
	width = INIT_WIDTH;
	height = INIT_HEIGHT;
	gl_window->installEventFilter(this);
	setMouseTracking(true);		// this is resource consuming !!!

	AUDIO::init();
	AUDIO::start_bgm();

	init();
}

int GL_WIDGET::get_width() {
	return width;
}
int GL_WIDGET::get_height() {
	return height;
}





// ------------------------------ private -------------------------------------

void GL_WIDGET::init() {
	set_mouse_focus = true;
	pressing_key = KEY_NONE;

	// 演示用 触发定时渲染 100ms一次
	QTimer *timer = new QTimer();
	connect(timer, SIGNAL(timeout()), this, SLOT(do_render()));
	connect(timer, SIGNAL(timeout()), this, SLOT(key_pressing()));
	timer->start(10);	// fps = 1000 / 20
}

void GL_WIDGET::set_mouse_center() {
	QCursor c = cursor();
	c.setPos(mapToGlobal(QPoint(get_width() / 2, get_height() / 2)));
	c.setShape(Qt::BlankCursor);
	setCursor(c);
}

void GL_WIDGET::do_render() {
	if (set_mouse_focus)
		set_mouse_center();
	GLfloat unused[] = { 0 };
	gl_window->render(unused, 0);
}

void GL_WIDGET::on_game_profile_change(QString filename) {
	UTILS::load(filename.toStdString().c_str());
	do_render();
}

void GL_WIDGET::on_game_profile_save(QString filename) {
	UTILS::save(filename.toStdString().c_str());
}

void GL_WIDGET::on_resize(QSize new_size) {
	// 按比例缩放
	width = size_weight * new_size.width();
	height = size_weight * new_size.height();
	// 改变自身控件大小
	resize(width, height);
	// 改变子窗口大小
	gl_window_container->setGeometry(0, 0, width, height);
	// 重新设置窗口中心位置
	Camera::cameraResize(width, height);
	// 重渲染
	do_render();
}

void GL_WIDGET::key_pressing() {
	bool is_playing = false;
	if (pressing_key & KEY_ESC)
		set_mouse_focus = false;
	if (pressing_key & KEY_W) {
		Camera::cameraMove(KEY_W);
		if (Camera::cameraPlat() && !is_playing) {
			AUDIO::start_walking();
			is_playing = true;
		}
	}
	if (pressing_key & KEY_A) {
		Camera::cameraMove(KEY_A);
		if (Camera::cameraPlat() && !is_playing) {
			AUDIO::start_walking();
			is_playing = true;
		}
	}
	if (pressing_key & KEY_S) {
		Camera::cameraMove(KEY_S);
		if (Camera::cameraPlat() && !is_playing) {
			AUDIO::start_walking();
			is_playing = true;
		}
	}
	if (pressing_key & KEY_D) {
		Camera::cameraMove(KEY_D);
		if (Camera::cameraPlat() && !is_playing) {
			AUDIO::start_walking();
			is_playing = true;
		}
	}
	if (pressing_key & KEY_SPACE)
		Camera::cameraMove(KEY_SPACE);
	if (pressing_key & KEY_X)
		Camera::cameraMove(KEY_X);
}




////////////////////////////////////////////////////////////////////////////////
//////////////////////////// Event Override ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool GL_WIDGET::eventFilter(QObject *target, QEvent *event) {
	if (target == gl_window) {
		switch (event->type()) {
		case QEvent::MouseMove:
			mouseMoveEvent((QMouseEvent*)event); return true;
		case QEvent::MouseButtonPress:
			mousePressEvent((QMouseEvent*)event); return true;
		case QEvent::MouseButtonRelease:
			mouseReleaseEvent((QMouseEvent*)event); return true;
		case QEvent::Wheel:
			wheelEvent((QWheelEvent*)event); return true;
		case QEvent::KeyPress:
			keyPressEvent((QKeyEvent*)event); return true;
		case QEvent::KeyRelease:
			keyReleaseEvent((QKeyEvent*)event); return true;
		default: break;
		}
	}
	return QWidget::eventFilter(target, event);
}

void GL_WIDGET::mouseMoveEvent(QMouseEvent *event) {
	QPoint cursor = event->pos();	// 相对控件的坐标
	// 如果需要锁住鼠标位置，在每次移动鼠标时将其移回中心
	if (set_mouse_focus) {
		//set_mouse_center();
		// 旋转摄像机
		Camera::cameraRotate(cursor.x(), cursor.y());
	}
	/*
	string tip = "Mouse pos: " + UTILS::int_to_string(cursor.x())
		+ " " + UTILS::int_to_string(cursor.y())
		+ "Camera pos: " + UTILS::int_to_string(cameraPos.x*1000)
		+ "," + UTILS::int_to_string(cameraPos.y*1000) + "," + UTILS::int_to_string(cameraPos.z*1000)
		+ " Press ESC to escape" + UTILS::int_to_string((set_mouse_focus?1:0));
	UTILS::logd(tip.c_str());
	*/
}

void GL_WIDGET::mousePressEvent(QMouseEvent *event) {
	// 每次点击控件，说明接下来需要锁住鼠标
	set_mouse_focus = true;
	if (event->button() == Qt::LeftButton) {
		if (GL_PAINTER::canUseDestroyCube) {
			setMapData(GL_PAINTER::destroyCube[0], GL_PAINTER::destroyCube[1], GL_PAINTER::destroyCube[2], CUBE_AIR);
			AUDIO::start_break_cube();
		}
	}
	else {
		if (GL_PAINTER::canUsePutCube)
			setMapData(GL_PAINTER::putCube[0], GL_PAINTER::putCube[1], GL_PAINTER::putCube[2], cubeTypeHolding);
	}
}

void GL_WIDGET::mouseReleaseEvent(QMouseEvent *event) {
	
}

void GL_WIDGET::wheelEvent(QWheelEvent *event) {
	setCurHolding(event->delta() > 0);
}

void GL_WIDGET::keyPressEvent(QKeyEvent *event) {
	switch (event->key()) {
		// 按下ESC取消对鼠标的锁定
	case Qt::Key_Escape:
		set_mouse_focus = false; break;
	case Qt::Key_W:
		if (!event->isAutoRepeat())
			pressing_key |= KEY_W;
		break;
	case Qt::Key_A:
		if (!event->isAutoRepeat())
			pressing_key |= KEY_A;
		break;
	case Qt::Key_S:
		if (!event->isAutoRepeat())
			pressing_key |= KEY_S;
		break;
	case Qt::Key_D:
		if (!event->isAutoRepeat())
			pressing_key |= KEY_D;
		break;
	case Qt::Key_Space:
		if (!event->isAutoRepeat())
			pressing_key |= KEY_SPACE;
		break;
	case Qt::Key_X:
		if (!event->isAutoRepeat())
			pressing_key |= KEY_X;
		break;
	default: break;
	}
}

void GL_WIDGET::keyReleaseEvent(QKeyEvent *event) {
	//AUDIO::stop_walking();
	if (!event->isAutoRepeat()) {
		switch (event->key()) {
			// 按下ESC取消对鼠标的锁定
		case Qt::Key_Escape:
			set_mouse_focus = false; break;
		case Qt::Key_W:
			if (!event->isAutoRepeat())
				pressing_key &= (~KEY_W);
			break;
		case Qt::Key_A:
			if (!event->isAutoRepeat())
				pressing_key &= (~KEY_A);
			break;
		case Qt::Key_S:
			if (!event->isAutoRepeat())
				pressing_key &= (~KEY_S);
			break;
		case Qt::Key_D:
			if (!event->isAutoRepeat())
				pressing_key &= (~KEY_D);
			break;
		case Qt::Key_Space:
			if (!event->isAutoRepeat())
				pressing_key &= (~KEY_SPACE);
			break;
		case Qt::Key_X:
			if (!event->isAutoRepeat())
				pressing_key &= (~KEY_X);
			break;
		default: break;
		}
	}
}