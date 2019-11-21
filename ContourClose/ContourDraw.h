#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_contourclose.h"

class ContouDraw : public QMainWindow
{
	Q_OBJECT

public:
	ContouDraw(QWidget *parent = Q_NULLPTR);
	~ContouDraw();

	//��ȡ��ǰֵ��Ӧ��RBGֵ
	QColor contourFill(double curentValue, double minValue, double maxValue, QColor startColor, QColor endColor);

protected:
	//��ֵ�߻��Ƽ���ɫ���
	void paintEvent(QPaintEvent *event);


private:
	Ui::ContourDrawClass ui;
	
};
