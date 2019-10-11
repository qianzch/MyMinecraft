#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_minecraft.h"

class Minecraft : public QMainWindow
{
	Q_OBJECT

public:
	Minecraft(QWidget *parent = Q_NULLPTR);

private:
	Ui::MinecraftClass ui;
};
