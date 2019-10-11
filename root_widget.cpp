#include "root_widget.h"

ROOT_WIDGET::ROOT_WIDGET(QWidget *parent) : QWidget(parent) {}

void ROOT_WIDGET::resizeEvent(QResizeEvent *event) {
	QSize new_size = event->size();
	emit change_size(new_size);
	// —› æ”√
	string tip = "Resize to " +
		UTILS::int_to_string(new_size.width()) + " " +
		UTILS::int_to_string(new_size.height());
	UTILS::logd(tip.c_str());
}