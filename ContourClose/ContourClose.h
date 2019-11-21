#pragma once

#include <vector>
#include <qpolygon.h>

class ContourClose
{
public:
	ContourClose();
	~ContourClose();

	void closeContour();//封闭等值线
	double getDistance(QPointF crossPoint);//计算未封闭等值线端点到边界左上图廓点的距离
	double getDistance(std::pair<int, QPointF> crossPoint);//计算未封闭等值线端点到边界左上图廓点的距离
	bool pointOnLine(QPointF tPoint, QPointF sPoint, QPointF ePoint);//判断点是否在直线上
	double twoPointDistance(QPointF point1, QPointF point2);//求两点之间的距离
	std::vector<std::pair<double, QPolygonF>> getClosedContourData();//获取新构造的闭合等值线数据

private:
	std::vector<std::pair<double, QPolygonF>> m_allContourData;//原始等值线数据
	std::vector<std::pair<int, QPointF> > m_allCrossPoints;//未封闭等值线与边界线的交点坐标
	std::vector<std::pair<double, QPolygonF> > m_allClosedContourData;//所有的闭合等值线数据
};