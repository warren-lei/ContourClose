#include "ContourClose.h"
#include "ReadData.h"

#include <math.h>
#include <QLineF>

ContourClose::ContourClose()
{

}

ContourClose::~ContourClose()
{
	//do not forget to release the memory!!!
}

std::vector<std::pair<int, double>> ContourClose::preprocessContourData()
{
	//读取原始等值线数据
	ReadData *readFileData = new ReadData();
	readFileData->readContourData();
	m_allContourData = readFileData->getAllPolygonData();
	double minX = readFileData->getMinX();
	double minY = readFileData->getMinY();
	double maxX = readFileData->getMaxX();
	double maxY = readFileData->getMaxY();
	delete readFileData;

	//矩形边界的四个顶点坐标
	QPointF rectPointA = QPointF(minX, maxY);
	QPointF rectPointD = QPointF(maxX, maxY);
	QPointF rectPointC = QPointF(maxX, minY);
	QPointF rectPointB = QPointF(minX, minY);

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
	m_allCrossPoints.push_back(std::make_pair(-1, rectPointA));
	m_allCrossPoints.push_back(std::make_pair(-1, rectPointB));
	m_allCrossPoints.push_back(std::make_pair(-1, rectPointC));
	m_allCrossPoints.push_back(std::make_pair(-1, rectPointD));

	//用于记录等值线各端点到边界左上图廓点的距离
	std::vector<std::pair<int, double>> allDistance;
	double tmpDis;
	//计算所有未封闭等值线端点到边界左上图廓点的距离
	for (int i = 0; i < m_allCrossPoints.size(); i++)
	{
		//tmpDis = getDistance(mAllCrossPoints[i].second);
		tmpDis = getDistance(m_allCrossPoints[i]);
		allDistance.push_back(std::make_pair(m_allCrossPoints[i].first, tmpDis));
	}

	return allDistance;
}

//封闭等值线
void ContourClose::closeContour()
{
	std::vector<std::pair<int, double>> allDistance = preprocessContourData();
	//根据到左上角图廓点距离大小，对未封闭等值线与边界线的交点排序
	quickSort(allDistance, 0, allDistance.size() - 1);

	//每个端点参与构造封闭面的次数
	int *closeNum = new int[m_allCrossPoints.size()];
	for (int j = 0; j < m_allCrossPoints.size(); j++)
	{
		closeNum[j] = 0;
	}

	//根据已经排序的端点，寻找能构成一条闭合等值线的各段不闭合线段，构造封闭曲面
	for (int i = 0; i < m_allCrossPoints.size() - 1; i++)
	{
		//用于存储新闭合的等值线
		QPolygonF tempNewContour;
		//用于存储新闭合等值线的值
		double tempVal = 0.0;

		//在边界点序列中取出一点，将此点加入到区域中 ,并且标记其使用次数
		QPointF firstPoint;
		firstPoint = m_allCrossPoints[i].second;
		tempNewContour.push_back(firstPoint);

		//在边界点序列中 ,找出它的下一点 
		QPointF nextPoint;
		nextPoint = m_allCrossPoints[i + 1].second;

		//closeFlag[i+1] = true;

		int firtContourID;
		int nextContourID;
		firtContourID = m_allCrossPoints[i].first;
		nextContourID = m_allCrossPoints[i + 1].first;
		QPointF nextOtherPoint;
		QPolygonF nextContour;

		//用于临时记录一条封闭等值线的所有端点所在等值线的值
		std::vector<double> theContourValue;
		//用于临时记录某一个端点所在等值线的值
		double tempContourValue;

		//若该点不为边界顶点，记录其所在等值线的值
		if (nextContourID != -1)
		{
			tempContourValue = m_allContourData[nextContourID].first;
			theContourValue.push_back(tempContourValue);
		}

		//qDebug() << closeNum[i] << endl;

		//每个端点都应该参与两次封闭
		if (closeNum[i] < 2)
		{
			//如果是边界顶点，则其封闭次数只需要一次
			if ((closeNum[i] == 1) && (firtContourID == -1))
			{
				continue;
			}

			//i+1个点的封闭次数加1
			++closeNum[i + 1];
			//一直循环，直到回到起始点为止
			//double xPval = fabs(nextPoint.x() - firstPoint.x());
			//double yPval = fabs(nextPoint.y() - firstPoint.y());
			while (fabs(nextPoint.x() - firstPoint.x()) > 1e-6
				|| fabs(nextPoint.y() - firstPoint.y()) > 1e-6)//bug
			{
				//如果端点的ID号为-1，表示其为边界顶点，将此点加入到区域中 ,并且标记其使用次数
				//否则为等值线的端点，判断端点为起始点或者终点，将此点加入到区域中 ,并且标记其使用次数
				if (nextContourID == -1)
				{
					tempNewContour.push_back(nextPoint);

					//搜寻该点的序列号
					int nextID;
					for (int k = 0; k < m_allCrossPoints.size(); k++)
					{
						//this is a safe way to find the next point,but it is not efficient. 
						QPointF tmpPoint = m_allCrossPoints[k].second;
						double xxTmpVal = fabs(tmpPoint.x() - nextPoint.x());
						double yyTmpVal = fabs(tmpPoint.y() - nextPoint.y());
						if (xxTmpVal < 1e-6	&& yyTmpVal < 1e-6)
						{
							nextID = k;
						}
					}

					nextPoint = m_allCrossPoints[nextID + 1].second;
					nextContourID = m_allCrossPoints[nextID + 1].first;
					++closeNum[nextID + 1];

					//若该点不为边界顶点，记录其所在等值线的值
					if (nextContourID != -1)
					{
						tempContourValue = m_allContourData[nextContourID].first;
						theContourValue.push_back(tempContourValue);
					}
				}
				else//there is a bug
				{
					nextContour = m_allContourData[nextContourID].second;

					//若此点为等值线的终点 ,将此条等值线反向加入到区域中 ,然后把等值线的另一端点
					//当做当前点，保存其序号，标记其使用次数，继续向后查找下一个点
					double xxPval = fabs(nextPoint.x() - nextContour.at(nextContour.size() - 1).x());
					double yyPval = fabs(nextPoint.y() - nextContour.at(nextContour.size() - 1).y());
					if (xxPval < 1e-6 && yyPval < 1e-6)
					{
						for (int j = nextContour.size() - 1; j >= 0; j--)
						{
							tempNewContour.push_back(nextContour.at(j));
						}

						nextOtherPoint = nextContour.at(0);
						//等值线的另一端点若为起始点，该条等值线封闭结束，把nextPoint置为该点（while循环结束的条件）
						if (fabs(nextOtherPoint.x() - firstPoint.x()) < 1e-6
							&& fabs(nextOtherPoint.y() - firstPoint.y()) < 1e-6)
						{
							nextPoint = m_allCrossPoints[i].second;
							nextContourID = m_allCrossPoints[i].first;
							++closeNum[i];

							//若该点不为边界顶点，记录其所在等值线的值
							if (nextContourID != -1)
							{
								tempContourValue = m_allContourData[nextContourID].first;
								theContourValue.push_back(tempContourValue);
							}
						}
						else
						{
							int nextOtherID;
							//搜寻该点的序列号
							for (int k = 0; k < m_allCrossPoints.size(); k++)
							{
								QPointF tmpPoint = m_allCrossPoints[k].second;
								double xTmpVal = fabs(tmpPoint.x() - nextOtherPoint.x());
								double yTmpVal = fabs(tmpPoint.y() - nextOtherPoint.y());
								if (xTmpVal < 1e-6 && yTmpVal < 1e-6)
								{
									nextOtherID = k;
									break;
								}
							}

							++closeNum[nextOtherID];
							int nextOtherContourID = m_allCrossPoints[nextOtherID].first;

							//若该点不为边界顶点，记录其所在等值线的值
							if (nextOtherContourID != -1)
							{
								tempContourValue = m_allContourData[nextOtherContourID].first;
								theContourValue.push_back(tempContourValue);
							}

							//如果寻找到最后一个点，其下一个点则为起始点
							if (nextOtherID == (m_allCrossPoints.size() - 1))
							{
								nextPoint = m_allCrossPoints[0].second;
								nextContourID = m_allCrossPoints[0].first;
								++closeNum[0];

								//若该点不为边界顶点，记录其所在等值线的值
								if (nextContourID != -1)
								{
									tempContourValue = m_allContourData[nextContourID].first;
									theContourValue.push_back(tempContourValue);
								}
							}
							else
							{
								nextPoint = m_allCrossPoints[nextOtherID + 1].second;
								nextContourID = m_allCrossPoints[nextOtherID + 1].first;
								++closeNum[nextOtherID + 1];

								//若该点不为边界顶点，记录其所在等值线的值
								if (nextContourID != -1)
								{
									tempContourValue = m_allContourData[nextContourID].first;
									theContourValue.push_back(tempContourValue);
								}
							}
						}
					}
					else
					{//若此点为等值线的起始点 ,将此条等值线正向加入到区域中 ,然后把等值线的另一端点
					//当做当前点，保存其序号，标记其使用次数，继续向后查找下一个点
						for (int j = 0; j < nextContour.size(); j++)
						{
							tempNewContour.push_back(nextContour.at(j));
						}

						nextOtherPoint = nextContour.at(nextContour.size() - 1);
						//等值线的另一端点若为起始点，该条等值线封闭结束，把nextPoint置为该点（while循环结束的条件）
						if (fabs(nextOtherPoint.x() - firstPoint.x()) < 1e-6
							&& fabs(nextOtherPoint.y() - firstPoint.y()) < 1e-6)
						{
							nextPoint = m_allCrossPoints[i].second;
							nextContourID = m_allCrossPoints[i].first;
							++closeNum[i];

							//若该点不为边界顶点，记录其所在等值线的值
							if (nextContourID != -1)
							{
								tempContourValue = m_allContourData[nextContourID].first;
								theContourValue.push_back(tempContourValue);
							}
						}
						else
						{
							int nextOtherID;
							//搜寻该点的序列号
							for (int k = 0; k < m_allCrossPoints.size(); k++)
							{
								QPointF tmpPoint = m_allCrossPoints[k].second;
								double xxTmpVal = fabs(tmpPoint.x() - nextOtherPoint.x());
								double yyTmpVal = fabs(tmpPoint.y() - nextOtherPoint.y());
								if (xxTmpVal < 1e-6	&& yyTmpVal < 1e-6)
								{
									nextOtherID = k;
								}
							}

							++closeNum[nextOtherID];
							int nextOtherContourID = m_allCrossPoints[nextOtherID].first;

							//若该点不为边界顶点，记录其所在等值线的值
							if (nextOtherContourID != -1)
							{
								tempContourValue = m_allContourData[nextOtherContourID].first;
								theContourValue.push_back(tempContourValue);
							}

							//如果寻找到最后一个点，其下一个点则为起始点
							if (nextOtherID == (m_allCrossPoints.size() - 1))
							{
								nextPoint = m_allCrossPoints[0].second;
								nextContourID = m_allCrossPoints[0].first;
								++closeNum[0];

								//若该点不为边界顶点，记录其所在等值线的值
								if (nextContourID != -1)
								{
									tempContourValue = m_allContourData[nextContourID].first;
									theContourValue.push_back(tempContourValue);
								}
							}
							else
							{
								nextPoint = m_allCrossPoints[nextOtherID + 1].second;
								nextContourID = m_allCrossPoints[nextOtherID + 1].first;
								++closeNum[nextOtherID + 1];

								//若该点不为边界顶点，记录其所在等值线的值
								if (nextContourID != -1)
								{
									tempContourValue = m_allContourData[nextContourID].first;
									theContourValue.push_back(tempContourValue);
								}
							}
						}

					}
				}
			}

			double sumVal = 0.0;//用来求一条封闭等值线的所有端点所在等值线的值的和
			double theVal;//存储新封闭的等值线的值
			int theVecSize = theContourValue.size();
			for (int k = 0; k < theVecSize; k++)
			{
				sumVal = sumVal + theContourValue[k];
			}
			theVal = sumVal / theVecSize;
			//将新封闭的等值线以及其值存起来
			m_allClosedContourData.push_back(std::make_pair(theVal, tempNewContour));
			tempNewContour = QPolygonF();
			theContourValue.clear();
		}
	}

	std::vector<int> tst;
	for (int j = 0; j < m_allCrossPoints.size(); j++)
	{
		tst.push_back(closeNum[j]);
	}

	delete[] closeNum;
	closeNum = nullptr;
}

//计算未封闭等值线端点到边界左上图廓点的距离（特殊矩形边界适用）
double ContourClose::getDistance(QPointF crossPoint)
{
	double minX = 0.0;//最小x值
	double minY = 0.0;//最小y值
	double maxX = 0.0;//最大x值
	double maxY = 0.0;//最大y值

	//读取边界数据值
	ReadData *readFileData = new ReadData();
	readFileData->readContourData();
	minX = readFileData->getMinX();
	minY = readFileData->getMinY();
	maxX = readFileData->getMaxX();
	maxY = readFileData->getMaxY();
	delete readFileData;

	//矩形边界的四个顶点坐标
	QPointF rectPointA = QPointF(minX, maxY);
	QPointF rectPointD = QPointF(maxX, maxY);
	QPointF rectPointC = QPointF(maxX, minY);
	QPointF rectPointB = QPointF(minX, minY);
	double disAB = fabs(rectPointB.y() - rectPointA.y());
	double disBC = fabs(rectPointC.x() - rectPointB.x());
	double disCD = fabs(rectPointD.y() - rectPointC.y());
	//等值线端点到边界左上图廓点的距离
	double ptpDistance = 0.0;//ptp means point to point

	//端点在线段AB上
	if (fabs(crossPoint.x() - rectPointA.x()) < 1e-6)
	{
		ptpDistance = fabs(crossPoint.y() - rectPointA.y());
		return ptpDistance;
	}

	//端点在线段BC上
	else if (fabs(crossPoint.y() - rectPointB.y()) < 1e-6)
	{
		ptpDistance = fabs(crossPoint.x() - rectPointB.x()) + disAB;
		return ptpDistance;
	}

	//端点在线段CD上
	else if (fabs(crossPoint.x() - rectPointC.x()) < 1e-6)
	{
		ptpDistance = fabs(crossPoint.y() - rectPointC.y()) + disAB + disBC;
		return ptpDistance;
	}

	//端点在线段DA上
	else if (fabs(crossPoint.y() - rectPointD.y()) < 1e-6)
	{
		ptpDistance = fabs(crossPoint.x() - rectPointD.x()) + disAB + disBC + disCD;
		return ptpDistance;
	}

	//端点不在边界线上，出错
	else
	{
		return ptpDistance;
	}
}

//计算未封闭等值线端点到边界左上图廓点的距离（任意边界适用）
double ContourClose::getDistance(std::pair<int, QPointF> crossPoint)
{
	double minX = 0.0;//最小x值
	double minY = 0.0;//最小y值
	double maxX = 0.0;//最大x值
	double maxY = 0.0;//最大y值

	//读取边界数据值
	ReadData *readFileData = new ReadData();
	readFileData->readContourData();
	minX = readFileData->getMinX();
	minY = readFileData->getMinY();
	maxX = readFileData->getMaxX();
	maxY = readFileData->getMaxY();
	delete readFileData;

	//矩形边界的四个顶点坐标
	QPointF rectPointA = QPointF(minX, maxY);
	QPointF rectPointD = QPointF(maxX, maxY);
	QPointF rectPointC = QPointF(maxX, minY);
	QPointF rectPointB = QPointF(minX, minY);

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
			flag = pointOnLine(crossPoint.second, boundaryVec[i], boundaryVec[i + 1]);
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
		ptpDistance = twoPointDistance(crossPoint.second, boundaryVec[0]);
	}
	else
	{
		for (int i = 0; i < boundID; i++)
		{
			disSum = disSum + twoPointDistance(boundaryVec[i], boundaryVec[i + 1]);
		}

		ptpDistance = disSum + twoPointDistance(crossPoint.second, boundaryVec[boundID]);
	}

	return ptpDistance;

}

//判断点是否在直线上
bool ContourClose::pointOnLine(QPointF tPoint, QPointF sPoint, QPointF ePoint)
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
double ContourClose::twoPointDistance(QPointF point1, QPointF point2)
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

