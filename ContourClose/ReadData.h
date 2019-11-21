#pragma once

#include <vector>
#include <string>
#include <qpolygon.h>

class ReadData
{
public:
	ReadData();
	~ReadData();

	//读取等值线数据
	void readContourData();
	//获取等值线数据以及对应的等值线值
	std::vector<std::pair<double, QPolygonF>> getAllPolygonData() const;
	std::vector<double> splitString(std::string str, char ch);

	//返回最小x
	double getMinX() const;
	//返回最小y
	double getMinY() const;
	//返回最大x
	double getMaxX() const;
	//返回最大y
	double getMaxY() const;

private:
	std::vector<std::pair<double, QPolygonF>> m_allPolygonData;	//等值线数据以及对应的等值线值

	double m_minX;//最小x值
	double m_minY;//最小y值
	double m_maxX;//最大x值
	double m_maxY;//最大y值

};
