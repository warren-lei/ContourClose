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

//��ȡ��ֵ������
void ReadData::readContourData()
{
	std::ifstream fileName("contourdata.txt");
	std::string lineStr;//�õ�һ������
	
	double pointX, pointY;
	QPolygonF polygonData;
	double contourValue;

	while(getline(fileName, lineStr))
	{
		std::vector<double> lineStrList;//�õ����ո�ֿ���һ������
		lineStrList = splitString(lineStr, ' ');//�����Կո�ֿ�

		for (int i = 2; i < lineStrList.size() - 1; i = i + 2)
		{
			pointX = lineStrList[i];//�õ�x����
			pointY = lineStrList[i + 1];//�õ�y����
			polygonData.append(QPointF(pointX, pointY));
		}
		
		//��ȡ��Сx
		if (pointX < m_minX)
		{
			m_minX = pointX;
		}

		//��ȡ���x
		if (pointX > m_maxX)
		{
			m_maxX = pointX;
		}

		//��ȡ��Сy
		if (pointY < m_minY)
		{
			m_minY = pointY;
		}

		//��ȡ���y
		if (pointY > m_maxY)
		{
			m_maxY = pointY;
		}

		//ȡtxt�ı���ÿ�еĵڶ���������Ϊ��ֵ�ߵ�ֵ
		contourValue = lineStrList[1];
		//���е�ֵ������
		//m_allContourData.append(tempData);						
		//��ֵ�������Լ���Ӧ�ĵ�ֵ��ֵ
		m_allPolygonData.push_back(std::make_pair(contourValue, polygonData));
		polygonData = QPolygonF();
	}

	fileName.close();
}

//��ȡ��ֵ�������Լ���Ӧ�ĵ�ֵ��ֵ
std::vector<std::pair<double, QPolygonF> > ReadData::getAllPolygonData() const
{
	return m_allPolygonData;
}

std::vector<double> ReadData::splitString(std::string str, char ch)
{
	std::vector<double> doubleVec;
	std::vector<std::string> strVec;

	//��ǰ�ָ���λ�ú�ǰһ���ָ���λ��
	size_t currentPos = 0;
	size_t prevPos = 0;
	
	while ((currentPos = str.find_first_of(ch, prevPos)) != std::string::npos)
	{
		std::string subStr = str.substr(prevPos, currentPos - prevPos);
		strVec.push_back(subStr);
		prevPos = currentPos + 1;
	}

	//����ָ�����û������ҲҪ����
	if (!str.empty())
	{
		strVec.push_back(&str[prevPos]);
	}

	// ��stringת��Ϊdouble
	for (size_t i = 0; i < strVec.size(); i++)
	{
		char *ptr;
		double tmpPt = strtod(strVec.at(i).c_str(), &ptr);
		doubleVec.push_back(tmpPt);
	}

	return doubleVec;
}

//������Сx
double ReadData::getMinX() const
{
	return m_minX;
}

//������Сy
double ReadData::getMinY() const
{
	return m_minY;
}

//�������x
double ReadData::getMaxX() const
{
	return m_maxX;
}

//�������y
double ReadData::getMaxY() const
{
	return m_maxY;
}