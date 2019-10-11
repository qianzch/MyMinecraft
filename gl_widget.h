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
*	�������Ҫ��һЩ��������
*	* ��Ӧ��������¼�
*	* ������ʱ��Ⱦ
*	* �����߳�
*/
class GL_WIDGET : public QWidget {
	Q_OBJECT

public:
	GL_WIDGET(QWidget *parent);
	int get_width();
	int get_height();

protected:
	// eventFilter���ü��ĵ����򵥽����ǲ��ù���
	bool eventFilter(QObject *target, QEvent *event) override;
	// ����괦��GL_WIDGET�ؼ������ƶ�ʱ�᲻�ϴ���mouseMoveEvent
	void mouseMoveEvent(QMouseEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;
	void keyPressEvent(QKeyEvent *event) override;
	void keyReleaseEvent(QKeyEvent *event) override;

private slots:
	// ������Ⱦ�Ĳۺ�������ҵ��Ӧ������������¼���������Ⱦ
	void do_render();
	// �ı���Ϸ�浵ʱ���ᴥ���˺���
	void on_game_profile_change(QString filename);
	void on_game_profile_save(QString filename);
	// �ı䴰�ڴ�Сʱ���ᴥ���κ���
	void on_resize(QSize new_size);
	// �Ż������¼�
	void key_pressing();
	
private:
	// ��windowǶ�뱾Widget��
	QWidget *gl_window_container;
	GL_WINDOW *gl_window;
	int width, height;
	// ��Сռ�������ڵı�ֵ
	const double size_weight;
	// ��־λ���Ƿ����ǿ�ƶ�λ���ؼ�����
	bool set_mouse_focus;
	// �Ż������¼�
	int pressing_key;

	void init();
	// ���������������
	void set_mouse_center();
};