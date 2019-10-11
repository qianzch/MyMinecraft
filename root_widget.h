#pragma once
#include <QWidget>
#include <QResizeEvent>
#include <QSize>
#include "utils.h"

class ROOT_WIDGET : public QWidget {
	Q_OBJECT

public:
	ROOT_WIDGET(QWidget* parent);

protected:
	void resizeEvent(QResizeEvent *event) override;

signals:
	void change_size(QSize);
};