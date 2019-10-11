#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <QWidget>
#include <QWindow>
#include <QMouseEvent>
#include <QWheelEvent>
#include "gl_window.h"
#include "utils.h"
#include "audio.h"
using namespace std;



/*
*	这个类主要做一些交互工作
*	* 相应键盘鼠标事件
*	* 触发定时渲染
*	* 开新线程
*/
class GL_WIDGET : public QWidget {
	Q_OBJECT

public:
	GL_WIDGET(QWidget *parent);
	int get_width();
	int get_height();

protected:
	// eventFilter作用见文档，简单讲就是不用管它
	bool eventFilter(QObject *target, QEvent *event) override;
	// 当鼠标处于GL_WIDGET控件内且移动时会不断触发mouseMoveEvent
	void mouseMoveEvent(QMouseEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;
	void keyPressEvent(QKeyEvent *event) override;
	void keyReleaseEvent(QKeyEvent *event) override;

private slots:
	// 触发渲染的槽函数，作业中应该是鼠标或键盘事件来触发渲染
	void do_render();
	// 改变游戏存档时，会触发此函数
	void on_game_profile_change(QString filename);
	void on_game_profile_save(QString filename);
	// 改变窗口大小时，会触发次函数
	void on_resize(QSize new_size);
	// 优化键盘事件
	void key_pressing();
	
private:
	// 将window嵌入本Widget中
	QWidget *gl_window_container;
	GL_WINDOW *gl_window;
	int width, height;
	// 大小占整个窗口的比值
	const double size_weight;
	// 标志位，是否将鼠标强制定位到控件中心
	bool set_mouse_focus;
	// 优化键盘事件
	int pressing_key;

	void init();
	// 将鼠标锁定在中心
	void set_mouse_center();
};