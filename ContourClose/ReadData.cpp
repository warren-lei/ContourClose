#include "ReadData.h"

#include <fstream>
#include <sstream>

ReadData::ReadData()
	: m_minX(100000000)
	, m_minY(100000000)
	, m_maxX(-100000000)
	, m_maxY(-100000000)
{
}

ReadData::~ReadData()
{
}

//读取等值线数据
void ReadData::readContourData()
{
	std::ifstream fileName("contourdata.txt");
	std::string lineStr;//得到一行数据
	
	double pointX, pointY;
	QPolygonF polygonData;
	double contourValue;

	while(getline(fileName, lineStr))
	{
		std::vector<double> lineStrList;//得到按空格分开的一行数据
		lineStrList = splitString(lineStr, ' ');//数据以空格分开

		for (int i = 2; i < lineStrList.size() - 1; i = i + 2)
		{
			pointX = lineStrList[i];//得到x坐标
			pointY = lineStrList[i + 1];//得到y坐标
			polygonData.append(QPointF(pointX, pointY));
		}
		
		//获取最小x
		if (pointX < m_minX)
		{
			m_minX = pointX;
		}

		//获取最大x
		if (pointX > m_maxX)
		{
			m_maxX = pointX;
		}

		//获取最小y
		if (pointY < m_minY)
		{
			m_minY = pointY;
		}

		//获取最大y
		if (pointY > m_maxY)
		{
			m_maxY = pointY;
		}

		//取txt文本中每行的第二个数，其为等值线的值
		contourValue = lineStrList[1];
		//所有等值线数据
		//m_allContourData.append(tempData);						
		//等值线数据以及对应的等值线值
		m_allPolygonData.push_back(std::make_pair(contourValue, polygonData));
		polygonData = QPolygonF();
	}

	fileName.close();
}

//获取等值线数据以及对应的等值线值
std::vector<std::pair<double, QPolygonF> > ReadData::getAllPolygonData() const
{
	return m_allPolygonData;
}

std::vector<double> ReadData::splitString(std::string str, char ch)
{
	std::vector<double> doubleVec;
	std::vector<std::string> strVec;

	//当前分隔符位置和前一个分隔符位置
	size_t currentPos = 0;
	size_t prevPos = 0;
	
	while ((currentPos = str.find_first_of(ch, prevPos)) != std::string::npos)
	{
		std::string subStr = str.substr(prevPos, currentPos - prevPos);
		strVec.push_back(subStr);
		prevPos = currentPos + 1;
	}

	//如果分隔符后没有数据也要拷入
	if (!str.empty())
	{
		strVec.push_back(&str[prevPos]);
	}

	// 将string转换为double
	for (size_t i = 0; i < strVec.size(); i++)
	{
		char *ptr;
		double tmpPt = strtod(strVec.at(i).c_str(), &ptr);
		doubleVec.push_back(tmpPt);
	}

	return doubleVec;
}

//返回最小x
double ReadData::getMinX() const
{
	return m_minX;
}

//返回最小y
double ReadData::getMinY() const
{
	return m_minY;
}

//返回最大x
double ReadData::getMaxX() const
{
	return m_maxX;
}

//返回最大y
double ReadData::getMaxY() const
{
	return m_maxY;
}