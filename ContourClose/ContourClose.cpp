#include "ContourClose.h"
#include "ReadData.h"

#include <math.h>
#include <QLineF>
#include <assert.h>

ContourClose::ContourClose()
{
	preprocessContourData();
}

ContourClose::~ContourClose()
{
	//do not forget to release the memory!!!
}

void ContourClose::preprocessContourData()
{
	//��ȡԭʼ��ֵ������
	ReadData *readFileData = new ReadData();
	readFileData->readContourData();
	m_allContourData = readFileData->getAllPolygonData();
	m_minX = readFileData->getMinX();
	m_minY = readFileData->getMinY();
	m_maxX = readFileData->getMaxX();
	m_maxY = readFileData->getMaxY();
	delete readFileData;

	//���α߽���ĸ���������
	m_rect.rectPointA = QPointF(m_minX, m_maxY);
	m_rect.rectPointD = QPointF(m_maxX, m_maxY);
	m_rect.rectPointC = QPointF(m_maxX, m_minY);
	m_rect.rectPointB = QPointF(m_minX, m_minY);

	int contourSize = m_allContourData.size();
	QPointF startPoint, endPoint;
	//�������е�ֵ�ߣ��ҳ�δ��յĵ�ֵ�ߣ����ҳ�����߽��ߵĽ���
	for (int i = 0; i < contourSize; i++)
	{
		startPoint = m_allContourData[i].second.at(0);
		int thePointSize = m_allContourData[i].second.size();
		endPoint = m_allContourData[i].second.at(thePointSize - 1);
		double xval = fabs(startPoint.x() - endPoint.x());
		double yval = fabs(startPoint.y() - endPoint.y());
		//��βΪͬһ�㣬��ʾ��ֵ��ԭ�����Ǳպϵ�
		if (xval < 1e-6 && yval < 1e-6)
		{
			m_allClosedContourData.push_back(m_allContourData[i]);
			continue;
		}
		else
		{
			m_allCrossPoints.push_back(std::make_pair(i, startPoint));
			m_allCrossPoints.push_back(std::make_pair(i, endPoint));
		}
	}

	//�����ε��ĸ�������뵽�㼯
	m_allCrossPoints.push_back(std::make_pair(-1, m_rect.rectPointA));
	m_allCrossPoints.push_back(std::make_pair(-1, m_rect.rectPointB));
	m_allCrossPoints.push_back(std::make_pair(-1, m_rect.rectPointC));
	m_allCrossPoints.push_back(std::make_pair(-1, m_rect.rectPointD));

	//��������δ��յ�ֵ�߶˵㵽�߽�����ͼ����ľ���
	for (int i = 0; i < m_allCrossPoints.size(); i++)
	{
		double tmpDis = getDistance(m_allCrossPoints[i]);
		m_allCrossedPointsSortedByDist.push_back(std::make_tuple(tmpDis, 
			m_allCrossPoints[i].first, m_allCrossPoints[i].second));
	}

	std::sort(std::begin(m_allCrossedPointsSortedByDist), std::end(m_allCrossedPointsSortedByDist),
		[](std::tuple<double, int, QPointF> const &t1, std::tuple<double, int, QPointF> const &t2)
	{
		return std::get<0>(t1) < std::get<0>(t2);
	});
}

//��յ�ֵ��
void ContourClose::closeContour()
{ 
	//ÿ���˵���빹������Ĵ�����ÿ�������ķ�մ������������⣬��Ӧ��Ϊ���Σ�
	size_t totalSize = m_allCrossedPointsSortedByDist.size();
	int *closeNum = new int[totalSize];
	for (int j = 0; j < totalSize; j++)
	{
		closeNum[j] = 0;
	}


	for (size_t i = 0; i < totalSize; i++)
	{
		QPointF startPoint = std::get<2>(m_allCrossedPointsSortedByDist[i]);
		int startContourId = std::get<1>(m_allCrossedPointsSortedByDist[i]);
		QPointF endPoint;

		if (startContourId != -1)
		{
			endPoint = getAnotherEndPointOnContour(startContourId, startPoint);
		}
		
		if ((i != 0 && startContourId == -1) || closeNum[i] == 2)
		{
			continue;
		}

		++closeNum[i];

		QPolygonF newContour;
		newContour.push_back(startPoint);
		int index = i + 1;
		int indexContourId = (((unsigned int)(-1)) >> 1); // INT_MAX

		//������ʱ��¼һ����յ�ֵ�ߵ����ж˵����ڵ�ֵ�ߵ�ֵ
		std::vector<double> newContourValue;

		while (index < totalSize &&
			!isTheSamePoint(endPoint, std::get<2>(m_allCrossedPointsSortedByDist[index])))
		{
			QPointF indexPoint = std::get<2>(m_allCrossedPointsSortedByDist[index]);
			indexContourId = std::get<1>(m_allCrossedPointsSortedByDist[index]);

			if (indexContourId != -1)
			{
				++closeNum[index];
				QPolygonF indexContour = m_allContourData[indexContourId].second;
				int indexPointId = getPointIndexInContour(indexContourId, indexPoint);

				if (indexPointId == 0)
				{
					newContour.append(indexContour);
				}
				else
				{
					for (int p = indexContour.size() - 1; p >= 0; p--)
					{
						newContour.push_back(indexContour[p]);
					}
				}

				newContourValue.push_back(m_allContourData[indexContourId].first);
				QPointF endPointOnIndexContour = getAnotherEndPointOnContour(indexContourId, indexPoint);
				index = getIndexInAllCrossedPointsSortedByDist(endPointOnIndexContour);
				assert(index != -1 && "index error!");
				++closeNum[index];
				index++;
			}
			else
			{
				newContour.push_back(indexPoint);
				index++;
			}
		}

		if (index < totalSize)
		{
			++closeNum[index];
			int indexPointId = getPointIndexInContour(startContourId, endPoint);
			QPolygonF startContour = m_allContourData[startContourId].second;
			if (indexPointId == 0)
			{			
				for (int id = 0; id < startContour.size() - 2; id++)
				{
					newContour.push_back(startContour[id]);
				}
			}
			else
			{
				for (int id = startContour.size() - 1; id > 0; id--)
				{
					newContour.push_back(startContour[id]);
				}
			}
			newContourValue.push_back(m_allContourData[startContourId].first);
		}

		double sumVal = 0.0; //������һ����յ�ֵ�ߵ����ж˵����ڵ�ֵ�ߵ�ֵ�ĺ�
		double theVal; //�洢�·�յĵ�ֵ�ߵ�ֵ
		for (auto& value : newContourValue)
		{
			sumVal = sumVal + value;
		}
		theVal = sumVal / newContourValue.size();
		//���·�յĵ�ֵ���Լ���ֵ������
		m_allClosedContourData.push_back(std::make_pair(theVal, newContour));
		newContour = QPolygonF();
	}

	delete []closeNum;
	closeNum = nullptr;
}

//����δ��յ�ֵ�߶˵㵽�߽�����ͼ����ľ��루����߽����ã�
double ContourClose::getDistance(std::pair<int, QPointF> crossPoint)
{
	//���α߽���ĸ���������
	QPointF rectPointA = m_rect.rectPointA;
	QPointF rectPointD = m_rect.rectPointD;
	QPointF rectPointC = m_rect.rectPointC;
	QPointF rectPointB = m_rect.rectPointB;

	//�߽綥�㼯��
	std::vector<QPointF> boundaryVec;
	boundaryVec.push_back(rectPointA);
	boundaryVec.push_back(rectPointB);
	boundaryVec.push_back(rectPointC);
	boundaryVec.push_back(rectPointD);
	boundaryVec.push_back(rectPointA);

	bool flag;
	//�߽�����к�
	int boundID;
	//�������Ϊ�߽綥�㣬ֱ������ñ߽����
	if (crossPoint.first == -1)
	{
		for (int i = 0; i < boundaryVec.size(); i++)
		{
			if (fabs(crossPoint.second.x() - boundaryVec[i].x()) < 1e-6
				&& fabs(crossPoint.second.y() - boundaryVec[i].y()) < 1e-6)
			{
				boundID = i;
				break;
			}
		}
	}
	else
	{
		for (int i = 0; i < boundaryVec.size(); i++)
		{
			//�жϵ��Ƿ��ڱ߽��ߣ�����ڣ���������ڱ߽�ĵ�һ��������к�
			flag = isPointOnLine(crossPoint.second, boundaryVec[i], boundaryVec[i + 1]);
			if (flag)
			{
				boundID = i;
				break;
			}
		}
	}

	double ptpDistance = 0.0;
	double disSum = 0.0;
	//�����boundID���㵽��һ����ľ���
	if (boundID == 0)
	{
		ptpDistance = calculateTwoPointDistance(crossPoint.second, boundaryVec[0]);
	}
	else
	{
		for (int i = 0; i < boundID; i++)
		{
			disSum = disSum + calculateTwoPointDistance(boundaryVec[i], boundaryVec[i + 1]);
		}

		ptpDistance = disSum + calculateTwoPointDistance(crossPoint.second, boundaryVec[boundID]);
	}

	return ptpDistance;

}

//�жϵ��Ƿ���ֱ����
bool ContourClose::isPointOnLine(QPointF tPoint, QPointF sPoint, QPointF ePoint)
{
	double x1, y1, x2, y2, x0, y0;
	x1 = sPoint.x();
	y1 = sPoint.y();
	x2 = ePoint.x();
	y2 = ePoint.y();
	x0 = tPoint.x();
	y0 = tPoint.y();

	//����ֱ�߷��̣��жϵ��Ƿ���ֱ����
	if (fabs(y0*(x2 - x1) - ((y2 - y1)*(x0 - x1) + y1 * (x2 - x1))) < 1e-6)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//������֮��ľ���
double ContourClose::calculateTwoPointDistance(QPointF point1, QPointF point2)
{
	return sqrt((point1.x() - point2.x()) * (point1.x() - point2.x())
		+ (point1.y() - point2.y()) * (point1.y() - point2.y()));
}

//��ȡ�¹���ıպϵ�ֵ������
std::vector<std::pair<double, QPolygonF> > ContourClose::getClosedContourData()
{
	return m_allClosedContourData;
}

void ContourClose::quickSort(std::vector<std::pair<int, double>> allDistVec, int l, int r)
{
	if (allDistVec.size() > 0 && l < r)
	{
		int i = l, j = r;
		std::pair<int, double> x = allDistVec[l];
		while (i < j)
		{
			while (i < j && allDistVec[j].second >= x.second)
				--j;
			if (i < j)
			{
				std::swap(allDistVec[i], allDistVec[j]);
				std::swap(m_allCrossPoints[i], m_allCrossPoints[j]);
				i++;
			}

			while (i < j && allDistVec[i].second < x.second)
				++i;
			if (i < j)
			{
				std::swap(allDistVec[j], allDistVec[i]);
				std::swap(m_allCrossPoints[j], m_allCrossPoints[i]);
				j--;
			}
		}
		std::swap(allDistVec[i], x);
		quickSort(allDistVec, l, i - 1); // �ݹ����   
		quickSort(allDistVec, i + 1, r);
	}
}

bool ContourClose::isTheSamePoint(QPointF p1, QPointF p2)
{
	if (fabs(p1.x() - p2.x()) < 1e-6 && fabs(p1.y() - p2.y()) < 1e-6)
	{
		return true;
	}

	return false;
}

int ContourClose::getIndexInAllCrossedPointsSortedByDist(QPointF point)
{
	for (size_t i = 0; i < m_allCrossedPointsSortedByDist.size(); i++)
	{
		QPointF p = std::get<2>(m_allCrossedPointsSortedByDist[i]);
		if (isTheSamePoint(p, point))
		{
			return i;
		}
	}

	return -1;
}

QPointF ContourClose::getAnotherEndPointOnContour(int contourId, QPointF point)
{
	QPointF endPoint1 = m_allContourData[contourId].second.first();
	QPointF endPoint2 = m_allContourData[contourId].second.back();
	if (isTheSamePoint(endPoint1, point))
	{
		return endPoint2;
	}

	return endPoint1;
}

int ContourClose::getPointIndexInContour(int contourId, QPointF point)
{
	QPointF endPoint1 = m_allContourData[contourId].second.first();
	QPointF endPoint2 = m_allContourData[contourId].second.back();

	if (isTheSamePoint(point, endPoint1))
	{
		return 0;
	}

	return m_allContourData[contourId].second.size() - 1;
}

Rectangle ContourClose::getRectange() const
{
	return m_rect;
}
