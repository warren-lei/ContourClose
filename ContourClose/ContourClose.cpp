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
	//��ȡԭʼ��ֵ������
	ReadData *readFileData = new ReadData();
	readFileData->readContourData();
	m_allContourData = readFileData->getAllPolygonData();
	double minX = readFileData->getMinX();
	double minY = readFileData->getMinY();
	double maxX = readFileData->getMaxX();
	double maxY = readFileData->getMaxY();
	delete readFileData;

	//���α߽���ĸ���������
	QPointF rectPointA = QPointF(minX, maxY);
	QPointF rectPointD = QPointF(maxX, maxY);
	QPointF rectPointC = QPointF(maxX, minY);
	QPointF rectPointB = QPointF(minX, minY);

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
	m_allCrossPoints.push_back(std::make_pair(-1, rectPointA));
	m_allCrossPoints.push_back(std::make_pair(-1, rectPointB));
	m_allCrossPoints.push_back(std::make_pair(-1, rectPointC));
	m_allCrossPoints.push_back(std::make_pair(-1, rectPointD));

	//���ڼ�¼��ֵ�߸��˵㵽�߽�����ͼ����ľ���
	std::vector<std::pair<int, double>> allDistance;
	double tmpDis;
	//��������δ��յ�ֵ�߶˵㵽�߽�����ͼ����ľ���
	for (int i = 0; i < m_allCrossPoints.size(); i++)
	{
		//tmpDis = getDistance(mAllCrossPoints[i].second);
		tmpDis = getDistance(m_allCrossPoints[i]);
		allDistance.push_back(std::make_pair(m_allCrossPoints[i].first, tmpDis));
	}

	return allDistance;
}

//��յ�ֵ��
void ContourClose::closeContour()
{
	std::vector<std::pair<int, double>> allDistance = preprocessContourData();
	//���ݵ����Ͻ�ͼ��������С����δ��յ�ֵ����߽��ߵĽ�������
	quickSort(allDistance, 0, allDistance.size() - 1);

	//ÿ���˵���빹������Ĵ���
	int *closeNum = new int[m_allCrossPoints.size()];
	for (int j = 0; j < m_allCrossPoints.size(); j++)
	{
		closeNum[j] = 0;
	}

	//�����Ѿ�����Ķ˵㣬Ѱ���ܹ���һ���պϵ�ֵ�ߵĸ��β��պ��߶Σ�����������
	for (int i = 0; i < m_allCrossPoints.size() - 1; i++)
	{
		//���ڴ洢�±պϵĵ�ֵ��
		QPolygonF tempNewContour;
		//���ڴ洢�±պϵ�ֵ�ߵ�ֵ
		double tempVal = 0.0;

		//�ڱ߽��������ȡ��һ�㣬���˵���뵽������ ,���ұ����ʹ�ô���
		QPointF firstPoint;
		firstPoint = m_allCrossPoints[i].second;
		tempNewContour.push_back(firstPoint);

		//�ڱ߽�������� ,�ҳ�������һ�� 
		QPointF nextPoint;
		nextPoint = m_allCrossPoints[i + 1].second;

		//closeFlag[i+1] = true;

		int firtContourID;
		int nextContourID;
		firtContourID = m_allCrossPoints[i].first;
		nextContourID = m_allCrossPoints[i + 1].first;
		QPointF nextOtherPoint;
		QPolygonF nextContour;

		//������ʱ��¼һ����յ�ֵ�ߵ����ж˵����ڵ�ֵ�ߵ�ֵ
		std::vector<double> theContourValue;
		//������ʱ��¼ĳһ���˵����ڵ�ֵ�ߵ�ֵ
		double tempContourValue;

		//���õ㲻Ϊ�߽綥�㣬��¼�����ڵ�ֵ�ߵ�ֵ
		if (nextContourID != -1)
		{
			tempContourValue = m_allContourData[nextContourID].first;
			theContourValue.push_back(tempContourValue);
		}

		//qDebug() << closeNum[i] << endl;

		//ÿ���˵㶼Ӧ�ò������η��
		if (closeNum[i] < 2)
		{
			//����Ǳ߽綥�㣬�����մ���ֻ��Ҫһ��
			if ((closeNum[i] == 1) && (firtContourID == -1))
			{
				continue;
			}

			//i+1����ķ�մ�����1
			++closeNum[i + 1];
			//һֱѭ����ֱ���ص���ʼ��Ϊֹ
			//double xPval = fabs(nextPoint.x() - firstPoint.x());
			//double yPval = fabs(nextPoint.y() - firstPoint.y());
			while (fabs(nextPoint.x() - firstPoint.x()) > 1e-6
				|| fabs(nextPoint.y() - firstPoint.y()) > 1e-6)//bug
			{
				//����˵��ID��Ϊ-1����ʾ��Ϊ�߽綥�㣬���˵���뵽������ ,���ұ����ʹ�ô���
				//����Ϊ��ֵ�ߵĶ˵㣬�ж϶˵�Ϊ��ʼ������յ㣬���˵���뵽������ ,���ұ����ʹ�ô���
				if (nextContourID == -1)
				{
					tempNewContour.push_back(nextPoint);

					//��Ѱ�õ�����к�
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

					//���õ㲻Ϊ�߽綥�㣬��¼�����ڵ�ֵ�ߵ�ֵ
					if (nextContourID != -1)
					{
						tempContourValue = m_allContourData[nextContourID].first;
						theContourValue.push_back(tempContourValue);
					}
				}
				else//there is a bug
				{
					nextContour = m_allContourData[nextContourID].second;

					//���˵�Ϊ��ֵ�ߵ��յ� ,��������ֵ�߷�����뵽������ ,Ȼ��ѵ�ֵ�ߵ���һ�˵�
					//������ǰ�㣬��������ţ������ʹ�ô�����������������һ����
					double xxPval = fabs(nextPoint.x() - nextContour.at(nextContour.size() - 1).x());
					double yyPval = fabs(nextPoint.y() - nextContour.at(nextContour.size() - 1).y());
					if (xxPval < 1e-6 && yyPval < 1e-6)
					{
						for (int j = nextContour.size() - 1; j >= 0; j--)
						{
							tempNewContour.push_back(nextContour.at(j));
						}

						nextOtherPoint = nextContour.at(0);
						//��ֵ�ߵ���һ�˵���Ϊ��ʼ�㣬������ֵ�߷�ս�������nextPoint��Ϊ�õ㣨whileѭ��������������
						if (fabs(nextOtherPoint.x() - firstPoint.x()) < 1e-6
							&& fabs(nextOtherPoint.y() - firstPoint.y()) < 1e-6)
						{
							nextPoint = m_allCrossPoints[i].second;
							nextContourID = m_allCrossPoints[i].first;
							++closeNum[i];

							//���õ㲻Ϊ�߽綥�㣬��¼�����ڵ�ֵ�ߵ�ֵ
							if (nextContourID != -1)
							{
								tempContourValue = m_allContourData[nextContourID].first;
								theContourValue.push_back(tempContourValue);
							}
						}
						else
						{
							int nextOtherID;
							//��Ѱ�õ�����к�
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

							//���õ㲻Ϊ�߽綥�㣬��¼�����ڵ�ֵ�ߵ�ֵ
							if (nextOtherContourID != -1)
							{
								tempContourValue = m_allContourData[nextOtherContourID].first;
								theContourValue.push_back(tempContourValue);
							}

							//���Ѱ�ҵ����һ���㣬����һ������Ϊ��ʼ��
							if (nextOtherID == (m_allCrossPoints.size() - 1))
							{
								nextPoint = m_allCrossPoints[0].second;
								nextContourID = m_allCrossPoints[0].first;
								++closeNum[0];

								//���õ㲻Ϊ�߽綥�㣬��¼�����ڵ�ֵ�ߵ�ֵ
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

								//���õ㲻Ϊ�߽綥�㣬��¼�����ڵ�ֵ�ߵ�ֵ
								if (nextContourID != -1)
								{
									tempContourValue = m_allContourData[nextContourID].first;
									theContourValue.push_back(tempContourValue);
								}
							}
						}
					}
					else
					{//���˵�Ϊ��ֵ�ߵ���ʼ�� ,��������ֵ��������뵽������ ,Ȼ��ѵ�ֵ�ߵ���һ�˵�
					//������ǰ�㣬��������ţ������ʹ�ô�����������������һ����
						for (int j = 0; j < nextContour.size(); j++)
						{
							tempNewContour.push_back(nextContour.at(j));
						}

						nextOtherPoint = nextContour.at(nextContour.size() - 1);
						//��ֵ�ߵ���һ�˵���Ϊ��ʼ�㣬������ֵ�߷�ս�������nextPoint��Ϊ�õ㣨whileѭ��������������
						if (fabs(nextOtherPoint.x() - firstPoint.x()) < 1e-6
							&& fabs(nextOtherPoint.y() - firstPoint.y()) < 1e-6)
						{
							nextPoint = m_allCrossPoints[i].second;
							nextContourID = m_allCrossPoints[i].first;
							++closeNum[i];

							//���õ㲻Ϊ�߽綥�㣬��¼�����ڵ�ֵ�ߵ�ֵ
							if (nextContourID != -1)
							{
								tempContourValue = m_allContourData[nextContourID].first;
								theContourValue.push_back(tempContourValue);
							}
						}
						else
						{
							int nextOtherID;
							//��Ѱ�õ�����к�
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

							//���õ㲻Ϊ�߽綥�㣬��¼�����ڵ�ֵ�ߵ�ֵ
							if (nextOtherContourID != -1)
							{
								tempContourValue = m_allContourData[nextOtherContourID].first;
								theContourValue.push_back(tempContourValue);
							}

							//���Ѱ�ҵ����һ���㣬����һ������Ϊ��ʼ��
							if (nextOtherID == (m_allCrossPoints.size() - 1))
							{
								nextPoint = m_allCrossPoints[0].second;
								nextContourID = m_allCrossPoints[0].first;
								++closeNum[0];

								//���õ㲻Ϊ�߽綥�㣬��¼�����ڵ�ֵ�ߵ�ֵ
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

								//���õ㲻Ϊ�߽綥�㣬��¼�����ڵ�ֵ�ߵ�ֵ
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

			double sumVal = 0.0;//������һ����յ�ֵ�ߵ����ж˵����ڵ�ֵ�ߵ�ֵ�ĺ�
			double theVal;//�洢�·�յĵ�ֵ�ߵ�ֵ
			int theVecSize = theContourValue.size();
			for (int k = 0; k < theVecSize; k++)
			{
				sumVal = sumVal + theContourValue[k];
			}
			theVal = sumVal / theVecSize;
			//���·�յĵ�ֵ���Լ���ֵ������
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

//����δ��յ�ֵ�߶˵㵽�߽�����ͼ����ľ��루������α߽����ã�
double ContourClose::getDistance(QPointF crossPoint)
{
	double minX = 0.0;//��Сxֵ
	double minY = 0.0;//��Сyֵ
	double maxX = 0.0;//���xֵ
	double maxY = 0.0;//���yֵ

	//��ȡ�߽�����ֵ
	ReadData *readFileData = new ReadData();
	readFileData->readContourData();
	minX = readFileData->getMinX();
	minY = readFileData->getMinY();
	maxX = readFileData->getMaxX();
	maxY = readFileData->getMaxY();
	delete readFileData;

	//���α߽���ĸ���������
	QPointF rectPointA = QPointF(minX, maxY);
	QPointF rectPointD = QPointF(maxX, maxY);
	QPointF rectPointC = QPointF(maxX, minY);
	QPointF rectPointB = QPointF(minX, minY);
	double disAB = fabs(rectPointB.y() - rectPointA.y());
	double disBC = fabs(rectPointC.x() - rectPointB.x());
	double disCD = fabs(rectPointD.y() - rectPointC.y());
	//��ֵ�߶˵㵽�߽�����ͼ����ľ���
	double ptpDistance = 0.0;//ptp means point to point

	//�˵����߶�AB��
	if (fabs(crossPoint.x() - rectPointA.x()) < 1e-6)
	{
		ptpDistance = fabs(crossPoint.y() - rectPointA.y());
		return ptpDistance;
	}

	//�˵����߶�BC��
	else if (fabs(crossPoint.y() - rectPointB.y()) < 1e-6)
	{
		ptpDistance = fabs(crossPoint.x() - rectPointB.x()) + disAB;
		return ptpDistance;
	}

	//�˵����߶�CD��
	else if (fabs(crossPoint.x() - rectPointC.x()) < 1e-6)
	{
		ptpDistance = fabs(crossPoint.y() - rectPointC.y()) + disAB + disBC;
		return ptpDistance;
	}

	//�˵����߶�DA��
	else if (fabs(crossPoint.y() - rectPointD.y()) < 1e-6)
	{
		ptpDistance = fabs(crossPoint.x() - rectPointD.x()) + disAB + disBC + disCD;
		return ptpDistance;
	}

	//�˵㲻�ڱ߽����ϣ�����
	else
	{
		return ptpDistance;
	}
}

//����δ��յ�ֵ�߶˵㵽�߽�����ͼ����ľ��루����߽����ã�
double ContourClose::getDistance(std::pair<int, QPointF> crossPoint)
{
	double minX = 0.0;//��Сxֵ
	double minY = 0.0;//��Сyֵ
	double maxX = 0.0;//���xֵ
	double maxY = 0.0;//���yֵ

	//��ȡ�߽�����ֵ
	ReadData *readFileData = new ReadData();
	readFileData->readContourData();
	minX = readFileData->getMinX();
	minY = readFileData->getMinY();
	maxX = readFileData->getMaxX();
	maxY = readFileData->getMaxY();
	delete readFileData;

	//���α߽���ĸ���������
	QPointF rectPointA = QPointF(minX, maxY);
	QPointF rectPointD = QPointF(maxX, maxY);
	QPointF rectPointC = QPointF(maxX, minY);
	QPointF rectPointB = QPointF(minX, minY);

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
	//�����boundID���㵽��һ����ľ���
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

//�жϵ��Ƿ���ֱ����
bool ContourClose::pointOnLine(QPointF tPoint, QPointF sPoint, QPointF ePoint)
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
double ContourClose::twoPointDistance(QPointF point1, QPointF point2)
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

