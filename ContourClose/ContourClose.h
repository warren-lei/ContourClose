#pragma once

#include <vector>
#include <qpolygon.h>

class ContourClose
{
public:
	ContourClose();
	~ContourClose();

	// �Ե�ֵ�����ݽ���Ԥ����
	std::vector<std::pair<int, double>> preprocessContourData();
	void closeContour();//��յ�ֵ��
	double getDistance(QPointF crossPoint);//����δ��յ�ֵ�߶˵㵽�߽�����ͼ����ľ���
	double getDistance(std::pair<int, QPointF> crossPoint);//����δ��յ�ֵ�߶˵㵽�߽�����ͼ����ľ���
	bool pointOnLine(QPointF tPoint, QPointF sPoint, QPointF ePoint);//�жϵ��Ƿ���ֱ����
	double twoPointDistance(QPointF point1, QPointF point2);//������֮��ľ���
	std::vector<std::pair<double, QPolygonF>> getClosedContourData();//��ȡ�¹���ıպϵ�ֵ������

	void quickSort(std::vector<std::pair<int, double>> allDistVec, int l, int r);

private:
	std::vector<std::pair<double, QPolygonF>> m_allContourData;//ԭʼ��ֵ������
	std::vector<std::pair<int, QPointF> > m_allCrossPoints;//δ��յ�ֵ����߽��ߵĽ�������
	std::vector<std::pair<double, QPolygonF> > m_allClosedContourData;//���еıպϵ�ֵ������
};