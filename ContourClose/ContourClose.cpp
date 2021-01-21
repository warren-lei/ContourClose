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
	//读取原始等值线数据
	ReadData *readFileData = new ReadData();
	readFileData->readContourData();
	m_allContourData = readFileData->getAllPolygonData();
	m_minX = readFileData->getMinX();
	m_minY = readFileData->getMinY();
	m_maxX = readFileData->getMaxX();
	m_maxY = readFileData->getMaxY();
	delete readFileData;

	//矩形边界的四个顶点坐标
	m_rect.rectPointA = QPointF(m_minX, m_maxY);
	m_rect.rectPointD = QPointF(m_maxX, m_maxY);
	m_rect.rectPointC = QPointF(m_maxX, m_minY);
	m_rect.rectPointB = QPointF(m_minX, m_minY);

	int contourSize = m_allContourData.size();
	QPointF startPoint, endPoint;
	//遍历所有等值线，找出未封闭的等值线，并找出其与边界线的交点
	for (int i = 0; i < contourSize; i++)
	{
		startPoint = m_allContourData[i].second.at(0);
		int thePointSize = m_allContourData[i].second.size();
		endPoint = m_allContourData[i].second.at(thePointSize - 1);
		double xval = fabs(startPoint.x() - endPoint.x());
		double yval = fabs(startPoint.y() - endPoint.y());
		//首尾为同一点，表示等值线原本就是闭合的
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

	//将矩形的四个顶点加入到点集
	m_allCrossPoints.push_back(std::make_pair(-1, m_rect.rectPointA));
	m_allCrossPoints.push_back(std::make_pair(-1, m_rect.rectPointB));
	m_allCrossPoints.push_back(std::make_pair(-1, m_rect.rectPointC));
	m_allCrossPoints.push_back(std::make_pair(-1, m_rect.rectPointD));

	//计算所有未封闭等值线端点到边界左上图廓点的距离
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

//封闭等值线
void ContourClose::closeContour()
{ 
	//每个端点参与构造封闭面的次数，每个交叉点的封闭次数（除顶点外，都应该为两次）
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

		//用于临时记录一条封闭等值线的所有端点所在等值线的值
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

		double sumVal = 0.0; //用来求一条封闭等值线的所有端点所在等值线的值的和
		double theVal; //存储新封闭的等值线的值
		for (auto& value : newContourValue)
		{
			sumVal = sumVal + value;
		}
		theVal = sumVal / newContourValue.size();
		//将新封闭的等值线以及其值存起来
		m_allClosedContourData.push_back(std::make_pair(theVal, newContour));
		newContour = QPolygonF();
	}

	delete []closeNum;
	closeNum = nullptr;
}

//计算未封闭等值线端点到边界左上图廓点的距离（任意边界适用）
double ContourClose::getDistance(std::pair<int, QPointF> crossPoint)
{
	//矩形边界的四个顶点坐标
	QPointF rectPointA = m_rect.rectPointA;
	QPointF rectPointD = m_rect.rectPointD;
	QPointF rectPointC = m_rect.rectPointC;
	QPointF rectPointB = m_rect.rectPointB;

	//边界顶点集合
	std::vector<QPointF> boundaryVec;
	boundaryVec.push_back(rectPointA);
	boundaryVec.push_back(rectPointB);
	boundaryVec.push_back(rectPointC);
	boundaryVec.push_back(rectPointD);
	boundaryVec.push_back(rectPointA);

	bool flag;
	//边界点序列号
	int boundID;
	//如果交点为边界顶点，直接求出该边界序号
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
			//判断点是否在边界线，如果在，求出其所在边界的第一个点的序列号
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
	//计算第boundID个点到第一个点的距离
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

//判断点是否在直线上
bool ContourClose::isPointOnLine(QPointF tPoint, QPointF sPoint, QPointF ePoint)
{
	double x1, y1, x2, y2, x0, y0;
	x1 = sPoint.x();
	y1 = sPoint.y();
	x2 = ePoint.x();
	y2 = ePoint.y();
	x0 = tPoint.x();
	y0 = tPoint.y();

	//根据直线方程，判断点是否在直线上
	if (fabs(y0*(x2 - x1) - ((y2 - y1)*(x0 - x1) + y1 * (x2 - x1))) < 1e-6)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//求两点之间的距离
double ContourClose::calculateTwoPointDistance(QPointF point1, QPointF point2)
{
	return sqrt((point1.x() - point2.x()) * (point1.x() - point2.x())
		+ (point1.y() - point2.y()) * (point1.y() - point2.y()));
}

//获取新构造的闭合等值线数据
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
		quickSort(allDistVec, l, i - 1); // 递归调用   
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
