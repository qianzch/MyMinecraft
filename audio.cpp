#include "audio.h"

QSound* AUDIO::bgm(nullptr);
QSound* AUDIO::break_cube(nullptr);
QSound* AUDIO::walking(nullptr);

void AUDIO::init() {
	bgm = new QSound(BGM_FILE);
	break_cube = new QSound(BREAK_CUBE_FILE);
	walking = new QSound(WALKING_FILE);
	bgm->setLoops(INFINITY);
}

void AUDIO::start_bgm() {
	bgm->play();
}

void AUDIO::stop_bgm() {
	bgm->stop();
}

void AUDIO::start_break_cube() {
	break_cube->play();
}

void AUDIO::start_walking() {
	if(walking->isFinished())
		walking->play();
}
void AUDIO::stop_walking() {
	walking->stop();
}