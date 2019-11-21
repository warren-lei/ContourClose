#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_contourclose.h"

class ContouDraw : public QMainWindow
{
	Q_OBJECT

public:
	ContouDraw(QWidget *parent = Q_NULLPTR);
	~ContouDraw();

	//获取当前值对应的RBG值
	QColor contourFill(double curentValue, double minValue, double maxValue, QColor startColor, QColor endColor);

protected:
	//等值线绘制及颜色填充
	void paintEvent(QPaintEvent *event);


private:
	Ui::ContourDrawClass ui;
	
};
