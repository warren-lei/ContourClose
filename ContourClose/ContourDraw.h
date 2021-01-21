#pragma once

#include <QtWidgets/QMainWindow>
#include <qpushbutton.h>
#include "ui_contourclose.h"

class ContouDraw : public QMainWindow
{
	Q_OBJECT

public:
	ContouDraw(QWidget *parent = Q_NULLPTR);
	~ContouDraw();

	//获取当前值对应的RBG值
	QColor contourFill(double curentValue, double minValue, double maxValue, QColor startColor, QColor endColor);

private slots:
	void handlePaintButton();
	void handleCloseButton();
	void handleColorButton();
	void handleClearButton();

protected:
	//等值线绘制及颜色填充
	void paintEvent(QPaintEvent* event);

	void setClickBtns();


private:
	Ui::ContourDrawClass ui;

private:
	QPushButton* m_paintBtn;
	QPushButton* m_closeContoursBtn;
	QPushButton* m_colorsBtn;
	QPushButton* m_clearAllBtn;

	std::vector<std::pair<double, QPolygonF>> m_originPolygonData;
	std::vector<std::pair<double, QPolygonF>> m_closedPolygonData;
	//求等值线的最大最小值
	double m_maxValue = 0.0;
	double m_minValue = 0.0;

	double m_minX = 0.0;//最小x值
	double m_minY = 0.0;//最小y值
	double m_maxX = 0.0;//最大x值
	double m_maxY = 0.0;//最大y值

	bool m_isColorsBtnClicked = false;
};
