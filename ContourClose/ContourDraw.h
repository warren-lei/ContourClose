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

	//��ȡ��ǰֵ��Ӧ��RBGֵ
	QColor contourFill(double curentValue, double minValue, double maxValue, QColor startColor, QColor endColor);

private slots:
	void handlePaintButton();
	void handleCloseButton();
	void handleColorButton();
	void handleClearButton();

protected:
	//��ֵ�߻��Ƽ���ɫ���
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
	//���ֵ�ߵ������Сֵ
	double m_maxValue = 0.0;
	double m_minValue = 0.0;

	double m_minX = 0.0;//��Сxֵ
	double m_minY = 0.0;//��Сyֵ
	double m_maxX = 0.0;//���xֵ
	double m_maxY = 0.0;//���yֵ

	bool m_isColorsBtnClicked = false;
};
