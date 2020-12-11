#include "ContourData.h"
#include "ReadData.h"

#define	MYMAX(x,y) ((x) > (y) ? (x) : (y))
#define	MYMIN(x,y) ((x) < (y) ? (x) : (y))

ContourData::ContourData(int &contour_id, double &contourvalue, QPolygonF &data)
	: m_myContourID(contour_id)
	, m_myContourValue(contourvalue)
	, m_myArrayData(data)
{
}

ContourData::~ContourData()
{
	if (!m_myArrayData.isEmpty())
	{
		m_myArrayData = QPolygonF();
	}
}

/*********************************************************************************
函数原型：bool topodecision(const ContourData & theOtherData, Topology &relation)
函数功能：判断两条等值线之间的包含关系
输入参数：const ContourData & theOtherData, 作为比较的等值线
输出参数：Topology &relation，两条等值线间的包含关系
返回参数：调用是否成功，返回类型bool
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
}

//new
bool ContourData::isInPolygon(QPolygonF randomPolygon, QPolygonF edge)
{
	bool flag;
	QPointF randomPoint;
	randomPoint = randomPolygon.at(0);
	//判断点是否在多边形内部，返回bool型
	flag = edge.containsPoint(randomPoint, Qt::OddEvenFill);
	//如果点在多边形内部，表示两个多边形是包含关系，返回true
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