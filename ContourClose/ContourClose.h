#pragma once

#include <vector>
#include <map>
#include <qpolygon.h>

enum PointLocation
{
	kAB = 0,
	kBC,
	kCD,
	kDA,
	kErrorLocation
};


/******************矩形结构********************
        A _____________ D
		 |             |
		 |             |
		 |             | 
		 |_____________|
        B               C
*********************************************/
struct Rectangle
{
	QPointF rectPointA;
	QPointF rectPointB;
	QPointF rectPointC;
	QPointF rectPointD;
};

class ContourClose
{
public:
	ContourClose();
	~ContourClose();

	// 对等值线数据进行预处理
	void preprocessContourData();
	void closeContour();//封闭等值线
	void closeContour1();
	double getDistance(std::pair<int, QPointF> crossPoint);//计算未封闭等值线端点到边界左上图廓点的距离
	bool isPointOnLine(QPointF tPoint, QPointF sPoint, QPointF ePoint);//判断点是否在直线上
	double calculateTwoPointDistance(QPointF point1, QPointF point2);//求两点之间的距离
	std::vector<std::pair<double, QPolygonF>> getClosedContourData();//获取新构造的闭合等值线数据

	void quickSort(std::vector<std::pair<int, double>> allDistVec, int l, int r);
	PointLocation getPointLocation(QPointF point);
	// 计算梯形的面积
	double calculateTrapezoidArea(QPointF point1, QPointF point2, QPointF point3, QPointF point4);
	// 获取面积较小的梯形所包含的两个边界点
	std::pair<QPointF, QPointF> getRectPointsInTrapezoid(QPointF start, QPointF end, PointLocation location);
	bool isTheSamePoint(QPointF p1, QPointF p2);
	int getIndexInAllCrossedPointsSortedByDist(QPointF point);
	QPointF getAnotherEndPointOnContour(int contourId, QPointF point);
	int getPointIndexInContour(int contourId, QPointF point);

private:
	std::vector<std::pair<double, QPolygonF>> m_allContourData;//原始等值线数据
	std::vector<std::pair<int, QPointF>> m_allCrossPoints;//未封闭等值线与边界线的交点坐标
	std::vector<std::pair<double, QPolygonF>> m_allClosedContourData;//所有的闭合等值线数据
	std::vector<std::tuple<double, int, QPointF>> m_allCrossedPointsSortedByDist;

	double m_minX = 0.0;//最小x值
	double m_minY = 0.0;//最小y值
	double m_maxX = 0.0;//最大x值
	double m_maxY = 0.0;//最大y值

	Rectangle m_rect;
};