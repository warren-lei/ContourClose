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