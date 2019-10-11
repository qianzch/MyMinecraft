#pragma once
#include <QSound>

#define BGM_FILE			"./Resources/audio/bgm.wav"
#define BREAK_CUBE_FILE		"./Resources/audio/break_cube.wav"
#define WALKING_FILE		"./Resources/audio/walking.wav"

class AUDIO {

public:

	static void init();

	static void start_bgm();
	static void stop_bgm();

	static void start_break_cube();
	
	static void start_walking();
	static void stop_walking();

private:
	static QSound *bgm;
	static QSound *break_cube;
	static QSound *walking;
};