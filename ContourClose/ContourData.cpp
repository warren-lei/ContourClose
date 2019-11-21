#include "ContourData.h"
#include "ReadData.h"

#define	MYMAX(x,y) ((x) > (y) ? (x) : (y))
#define	MYMIN(x,y) ((x) < (y) ? (x) : (y))

ContourData::ContourData(int &contour_id, double &contourvalue, QPolygonF &data)
	: m_myContourID(contour_id)
	, m_myContourValue(contourvalue)
	, m_myArrayData(data)
{
	//mycontour_id = contour_id;
	//myContourValue = contourvalue;
	//myArrayData = data;
}

ContourData::~ContourData()
{
	if (!m_myArrayData.isEmpty())
	{
		m_myArrayData = QPolygonF();
	}
}

/*********************************************************************************
����ԭ�ͣ�bool topodecision(const ContourData & theOtherData, Topology &relation)
�������ܣ��ж�������ֵ��֮��İ�����ϵ
���������const ContourData & theOtherData, ��Ϊ�Ƚϵĵ�ֵ��
���������Topology &relation��������ֵ�߼�İ�����ϵ
���ز����������Ƿ�ɹ�����������bool
*********************************************************************************/
bool ContourData::topoDecision(const ContourData &theOtherData, Topology &relation)
{
	int dataSize1 = m_myArrayData.size();
	QPolygonF myArrayData2 = theOtherData.getMyArrayData();
	int dataSize2 = myArrayData2.size();

	double value1 = getMyContourValue();
	double value2 = theOtherData.getMyContourValue();
	int crossPointNum = 0;

	if (m_myContourID == 0)
	{
		relation = INCLUSION;
		return true;
	}

	//new
	bool flag;
	flag = isInPolygon(myArrayData2, m_myArrayData);
	if (flag)
	{
		relation = INCLUSION;
		return true;
	}
	else
	{
		flag = isInPolygon(m_myArrayData, myArrayData2);
		if (flag)
		{
			relation = INCLUDED;
		}
		else
		{
			relation = PARALLEL;
		}
		return true;
	}

	//�ж�myArrayData2��myArrayData�����˹�ϵ
	//crossPointNum = evaluateCrossPointNum(myArrayData2, myArrayData) ;
	////���myArrayData2������������myArrayData����Ϊ��������Ϊ������ϵ
	//if(crossPointNum%2 != 0)
	//{
	//	relation = Inclusion;
	//	return true;
	//}
	////�����ٶ�myArrayData��myArrayData2���й�ϵ�ж�
	//else 
	//{
	//	crossPointNum = evaluateCrossPointNum(myArrayData, myArrayData2) ;  
	//	//���myArrayData2������������myArrayData����Ϊ��������Ϊ��������ϵ
	//	if(crossPointNum%2 != 0)
	//		relation = Included ;
	//	//����Ϊ���й�ϵ
	//	else
	//		relation = Parallel ;		  
	//	return true ;
	//} 
	//return false;
}

//new
bool ContourData::isInPolygon(QPolygonF randomPolygon, QPolygonF edge)
{
	bool flag;
	QPointF randomPoint;
	randomPoint = randomPolygon.at(0);
	//�жϵ��Ƿ��ڶ�����ڲ�������bool��
	flag = edge.containsPoint(randomPoint, Qt::OddEvenFill);
	//������ڶ�����ڲ�����ʾ����������ǰ�����ϵ������true
	if (flag)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ContourData::setMyArrayData(QPolygonF data)
{
	m_myArrayData = data;
}

QPolygonF ContourData::getMyArrayData() const
{
	return m_myArrayData;
}

void ContourData::setMyContourValue(double cvalue)
{
	m_myContourValue = cvalue;
}

double ContourData::getMyContourValue() const
{
	return m_myContourValue;
}

void ContourData::setMyContourID(int cid)
{
	m_myContourID = cid;
}

int ContourData::getMyContourID() const
{
	return m_myContourID;
}

/*********************************************************************************
����ԭ�ͣ�int ContourData::evaluateCrossPointNum(ContourDataArray * randomPoint,
ContourDataArray * edge)
�������ܣ������һ����ֵ��randomPoint��һ����������ˮƽ����������ĵ�ֵ��edge��
�������
���������ContourDataArray * randomPoint, �ڸõ�ֵ����ȡ��
ContourDataArray * edge���õ�ֵ�����ڼ�����֮�ཻ�Ľ������
�����������
���ز����������������������int
*********************************************************************************/
//int ContourData::evaluateCrossPointNum(QPolygonF randomPoint, QPolygonF edge)
//{
//	/*int randomPointSize = randomPoint->GetItsSize();
//	int edgeSize = edge->GetItsSize();
//	int index = 0;   
//	double myX = randomPoint->GetItsY(index) ;
//	double myY = randomPoint->GetItsX(index) ;*/
//	//feigaolei
//	//int n_x = dataprovide.n_x;
//	//int n_y = dataprovide.n_y;
//	//double *X_Pos = dataprovide.X_Pos;
//	//double *Y_Pos = dataprovide.Y_Pos; 
//
//	int randomPointSize = randomPoint.size();
//	int edgeSize = edge.size();
//	int index = 0;   
//	double myX = randomPoint.at(index).x() ;
//	double myY = randomPoint.at(index).y() ;
//
//	ReadData *readFileData = new ReadData();
//	readFileData->ReadContourData();
//	//readFileData->GetAllContourData();
//	double x_lower_bound = readFileData->GetMinX(); 
//	double x_upper_bound = readFileData->GetMaxX();
//	double y_lower_bound = readFileData->GetMinY();
//	double y_upper_bound = readFileData->GetMaxY();
//	delete readFileData;
//	//double myX = double(myX);
//	//double myY = double(myY);  
//
//
//	bool forward = true;
//	int k = 0;
//	while((myX == x_lower_bound)||(myX == x_upper_bound)
//		||(myY == y_lower_bound)||(myY == y_upper_bound))
//	{
//		if(forward)
//		{
//			index = randomPointSize/2 + k;
//		}
//		else
//		{
//			index = randomPointSize/2 - k;
//		}
//
//		if(index>=0 && index<randomPointSize)
//		{
//			myX = randomPoint.at(index).x();
//			myY = randomPoint.at(index).y();
//			//myX = double( myX);
//			//myY = double( myY); 
//		}
//		else if(forward)
//		{
//			forward = false;
//			k = 0;
//		}
//		else
//			break;
//		++k;
//	}
//
//	bool f1 = false;
//	bool f2 = false;
//	bool f3 = false;
//	double xI,yI,xIPlus1,yIPlus1,xIMinus1,yIMinus1;
//	double f3exvalue;
//	int crossPointNum = 0;
//
//
//	//myX = double( myX);
//	//myY = double( myY);
//
//	int i = 0;
//
//	while(i < edgeSize-1)
//	{
//		xI = edge.at(i).x();
//		yI = edge.at(i).y();
//		xIPlus1 = edge.at(i+1).x();
//		yIPlus1 = edge.at(i+1).y();
//
//		/*double xI = double(xI);
//		double yI = double(yI);
//		double xIPlus1 = double(xIPlus1);
//		double yIPlus1 = double(yIPlus1);*/
//
//		if(i != 0)
//		{
//			xIMinus1 = edge.at(i-1).x();
//			yIMinus1 = edge.at(i-1).y();
//		}
//
//		/* �����ʱiΪ�㣬��xIMinus1��yIMinus1ȡΪ����ĩ�㿪ʼ�����ĵڶ����� */
//		else
//		{
//			xIMinus1 = edge.at(edgeSize-2).x();
//			yIMinus1 = edge.at(edgeSize-2).y();
//		}
//		double xIMinus1_int = double(xIMinus1);
//		double yIMinus1_int = double(yIMinus1);
//
//		if(myY == yI && yI == yIPlus1)
//		{
//			int count = i+1;
//			while( (count%edgeSize) != i )
//			{
//				xIPlus1 = edge.at(count%edgeSize).x();
//				yIPlus1 = edge.at(count%edgeSize).y();
//				yIPlus1 = double( yIPlus1);
//				if(myY != yIPlus1)
//					break;
//				++count;
//			}
//			i = count - 1;
//		}
//
//		xIPlus1 = double(xIPlus1);
//		yIPlus1 = double(yIPlus1);
//
//
//		f1 = MYMAX(xI,xIPlus1) >= myX;
//
//
//
//		if(yI != myY)
//			f2 = (MYMIN(yI,yIPlus1)<myY)&&(MYMAX(yI,yIPlus1)>myY);
//		else
//			f2 = (MYMIN(yIMinus1_int,yIPlus1)<myY)&&(MYMAX(yIMinus1_int,yIPlus1)>myY);
//
//
//		if(yIPlus1 != yI)
//		{
//			f3exvalue = xI + (xIPlus1 - xI) * (myY - yI)/(yIPlus1-yI) ;
//			f3 = (f3exvalue>myX);
//		}
//		else
//		{
//			f3 = false;
//		}
//
//		if(f1&&f2&&f3)
//		{
//			++crossPointNum;
//		}
//		++i;
//	}
//
//	//randomPoint = QPolygonF();
//	//edge = QPolygonF();
//	return crossPointNum;
//}

//bool ContourData::isCrossr(ContourDataArray * randomPoint, ContourDataArray * edge)
//{
//	///�����ͬһ���ϣ��������ཻ����
//	/*if (randomPoint->GetLayer() == edge->GetLayer())
//	{
//		return false;
//	}*/
//
//	bool crossPart = false;
//	bool parallerPart = false;
//
//	int randomPointSize = randomPoint->GetItsSize();
//	int edgeSize = edge->GetItsSize();
//	int index = 0;   
//	double myX = randomPoint->GetItsY(index) ;
//	double myY = randomPoint->GetItsX(index) ;
//	//
//	/*int n_x = dataprovide.n_x;
//	int n_y = dataprovide.n_y;
//	double *X_Pos = dataprovide.X_Pos;
//	double *Y_Pos = dataprovide.Y_Pos;  */
//	
//	ReadData *readFileData = new ReadData();
//	readFileData->ReadContourData();
//	readFileData->GetAllContourData();
//	double x_lower_bound = readFileData->GetMinX(); 
//	double x_upper_bound = readFileData->GetMaxX();
//	double y_lower_bound = readFileData->GetMinY();
//	double y_upper_bound = readFileData->GetMaxY();
//
//
//	double myX = double(myX);
//	double myY = double(myY);  
//
//
//	bool forward = true;
//	int k = 0;
//
//	///////////////////
//
//	for (index = 0 ;index<(randomPointSize/2) ; index++)
//	{
//		int index1 = index + (randomPointSize-1)/2;
//
//		///�жϵ�һ����
//		myX = randomPoint->GetItsY(index);
//		myY = randomPoint->GetItsX(index);
//		myX = double( myX);
//		myY = double( myY);  	   
//		if ( !((myX==(y_lower_bound))||(myX==(y_upper_bound))
//			||(myY==(x_lower_bound))||(myY==(x_upper_bound))) )
//		{
//
//			bool f1 = false;
//			bool f2 = false;
//			bool f3 = false;
//			double xI,yI,xIPlus1,yIPlus1,xIMinus1,yIMinus1;
//			double f3exvalue;
//			int crossPointNum = 0;
//
//			myX = double( myX);
//			myY = double( myY);
//
//			int i = 0;
//
//			while(i < edgeSize-1)
//			{
//				xI = edge->GetItsY(i);
//				yI = edge->GetItsX(i);
//				xIPlus1 = edge->GetItsY(i+1);
//				yIPlus1 = edge->GetItsX(i+1);
//
//				double xI = double(xI);
//				double yI = double(yI);
//				double xIPlus1 = double(xIPlus1);
//				double yIPlus1 = double(yIPlus1);
//
//				if(i != 0)
//				{
//					xIMinus1 = edge->GetItsY(i-1);
//					yIMinus1 = edge->GetItsX(i-1);
//				}
//
//				/* �����ʱiΪ�㣬��xIMinus1��yIMinus1ȡΪ����ĩ�㿪ʼ�����ĵڶ����� */
//				else
//				{
//					xIMinus1 = edge->GetItsY(edgeSize-2);
//					yIMinus1 = edge->GetItsX(edgeSize-2);
//				}
//				double xIMinus1_int = double(xIMinus1);
//				double yIMinus1_int = double(yIMinus1);
//
//				if(myY == yI && yI == yIPlus1)
//				{
//					int count = i+1;
//					while( (count%edgeSize) != i )
//					{
//						xIPlus1 = edge->GetItsY(count%edgeSize);
//						yIPlus1 = edge->GetItsX(count%edgeSize);
//						yIPlus1 = double( yIPlus1);
//						if(myY != yIPlus1)
//							break;
//						++count;
//					}
//					i = count - 1;
//				}
//
//				xIPlus1 = double(xIPlus1);
//				yIPlus1 = double(yIPlus1);
//
//
//				f1 = MYMAX(xI,xIPlus1) >= myX;
//
//
//
//				if(yI != myY)
//					f2 = (MYMIN(yI,yIPlus1)<myY)&&(MYMAX(yI,yIPlus1)>myY);
//				else
//					f2 = (MYMIN(yIMinus1_int,yIPlus1)<myY)&&(MYMAX(yIMinus1_int,yIPlus1)>myY);
//
//
//				if(yIPlus1 != yI)
//				{
//					f3exvalue = xI + (xIPlus1 - xI) * (myY - yI)/(yIPlus1-yI) ;
//					f3 = (f3exvalue>myX);
//				}
//				else
//				{
//					f3 = false;
//				}
//
//				if(f1&&f2&&f3)
//				{
//					++crossPointNum;
//				}
//				++i;
//			}
//			if (crossPointNum%2!=0)
//			{
//				crossPart = true;
//			}
//			else
//			{
//				parallerPart = true;
//			}
//
//		}
//
//		///////////�жϵڶ�����
//		myX = randomPoint->GetItsY(index1);
//		myY = randomPoint->GetItsX(index1);
//		myX = double( myX);
//		myY = double( myY);  	   
//		if ( !((myX==(y_lower_bound))||(myX==(y_upper_bound))
//			||(myY==(x_lower_bound))||(myY==(x_upper_bound))) )
//		{
//
//			bool f1 = false;
//			bool f2 = false;
//			bool f3 = false;
//			double xI,yI,xIPlus1,yIPlus1,xIMinus1,yIMinus1;
//			double f3exvalue;
//			int crossPointNum = 0;
//
//
//			myX = double( myX);
//			myY = double( myY);
//
//			int i = 0;
//
//			while(i < edgeSize-1)
//			{
//				xI = edge->GetItsY(i);
//				yI = edge->GetItsX(i);
//				xIPlus1 = edge->GetItsY(i+1);
//				yIPlus1 = edge->GetItsX(i+1);
//
//				double xI = double(xI);
//				double yI = double(yI);
//				double xIPlus1 = double(xIPlus1);
//				double yIPlus1 = double(yIPlus1);
//
//				if(i != 0)
//				{
//					xIMinus1 = edge->GetItsY(i-1);
//					yIMinus1 = edge->GetItsX(i-1);
//				}
//
//				/* �����ʱiΪ�㣬��xIMinus1��yIMinus1ȡΪ����ĩ�㿪ʼ�����ĵڶ����� */
//				else
//				{
//					xIMinus1 = edge->GetItsY(edgeSize-2);
//					yIMinus1 = edge->GetItsX(edgeSize-2);
//				}
//				double xIMinus1_int = double(xIMinus1);
//				double yIMinus1_int = double(yIMinus1);
//
//				if(myY == yI && yI == yIPlus1)
//				{
//					int count = i+1;
//					while( (count%edgeSize) != i )
//					{
//						xIPlus1 = edge->GetItsY(count%edgeSize);
//						yIPlus1 = edge->GetItsX(count%edgeSize);
//						yIPlus1 = double( yIPlus1);
//						if(myY != yIPlus1)
//							break;
//						++count;
//					}
//					i = count - 1;
//				}
//
//				xIPlus1 = double(xIPlus1);
//				yIPlus1 = double(yIPlus1);
//
//
//				f1 = MYMAX(xI,xIPlus1) >= myX;
//
//
//
//				if(yI != myY)
//					f2 = (MYMIN(yI,yIPlus1)<myY)&&(MYMAX(yI,yIPlus1)>myY);
//				else
//					f2 = (MYMIN(yIMinus1_int,yIPlus1)<myY)&&(MYMAX(yIMinus1_int,yIPlus1)>myY);
//
//
//				if(yIPlus1 != yI)
//				{
//					f3exvalue = xI + (xIPlus1 - xI) * (myY - yI)/(yIPlus1-yI) ;
//					f3 = (f3exvalue>myX);
//				}
//				else
//				{
//					f3 = false;
//				}
//
//				if(f1&&f2&&f3)
//				{
//					++crossPointNum;
//				}
//				++i;
//			}
//			if (crossPointNum%2!=0)
//			{
//				crossPart = true;
//			}
//			else
//			{
//				parallerPart = true;
//			}
//
//		}
//
//		if (crossPart && parallerPart)
//		{
//			return true;
//		}
//
//	}
//	return false;
//}