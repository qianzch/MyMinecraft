#pragma once
#include <QWidget>
#include <QStatusBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QThread>
#include <sstream>
#include <fstream>
#include "global.h"
#include "audio.h"
using namespace std;

#define STATUS_BAR_NAME		"statusBar"
#define DEFAULT_DATAFILE	"saved_data.txt"

class UTILS : public QWidget{
	Q_OBJECT

signals:
	void _logd(QString);
	void _open_file(QString);
	void _save_file(QString);

public:
	UTILS(QWidget *parent);

	static void save(const char* file = DEFAULT_DATAFILE);
	static void load(const char* file = DEFAULT_DATAFILE);
	static void clear(const char* file = DEFAULT_DATAFILE);

	static void logd(const char* s);
	static void alert(const char* s);

	static string int_to_string(int num);

	// ���ز��������� (��./Resources/block_texture.bmp)
	// ӳ�䵽�������½ǵ�UV���겢����ÿ����Ĵ�С
	// �磺��ʯ������(0, 0)ӳ�䵽(0, 0.75), size = 0.25
	static void uv_map(
		int cube_index_row, int cube_index_col,
		double &bl_u, double &bl_v, double &size
	);

	static string get_cube_name(int cube_type);

public slots:
	void open_file();
	void save_file();

private:
	static UTILS *foo;
	static AUDIO *audio;
};




// maybe useful later
class WORKER_THREAD : public QThread {
	Q_OBJECT

private:
	void run() override;
signals:
	void result_ready();
};