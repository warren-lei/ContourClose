#pragma once

#include <vector>
#include <string>
#include <qpolygon.h>

class ReadData
{
public:
	ReadData();
	~ReadData();

	//��ȡ��ֵ������
	void readContourData();
	//��ȡ��ֵ�������Լ���Ӧ�ĵ�ֵ��ֵ
	std::vector<std::pair<double, QPolygonF>> getAllPolygonData() const;
	std::vector<double> splitString(std::string str, char ch);

	//������Сx
	double getMinX() const;
	//������Сy
	double getMinY() const;
	//�������x
	double getMaxX() const;
	//�������y
	double getMaxY() const;

private:
	std::vector<std::pair<double, QPolygonF>> m_allPolygonData;	//��ֵ�������Լ���Ӧ�ĵ�ֵ��ֵ

	double m_minX;//��Сxֵ
	double m_minY;//��Сyֵ
	double m_maxX;//���xֵ
	double m_maxY;//���yֵ

};
