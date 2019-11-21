/******************************************************************************
�ļ�����ContourData.h
���ã�ContourData�������ͷ�ļ����������˰�����������һ���ڵ����洢�ĵ�ֵ������
*******************************************************************************/

#pragma once

#include "TreeNode.h"
#include <QPolygonF>

class ContourData
{
public:
	/*ContourData(double curve_id, float height, ContourDataArray * data,DataProvide inData):
	  mycurveid(curve_id),myHeight(height), myArrayData(data),dataprovide(inData){}*/
	ContourData(int &contour_id, double &contourvalue, QPolygonF &data);
	~ContourData();

	bool topoDecision(const ContourData & theOtherData, Topology &relation);
	bool isInPolygon(QPolygonF randomPolygon, QPolygonF edge);

	/* ���ú�ȡ�ø߳�ֵ */
	//void setMyHeight(float height) {myHeight = height;}
   // float GetMyHeight() const {return myHeight;}

	/* ���ú�ȡ�õ�ֵ�ߵ����� */
	void setMyArrayData(QPolygonF  data);
	QPolygonF  getMyArrayData() const;

	/* ���ú�ȡ�õ�ֵ�ߵı�ʶ�� */
	//void setMycurveid(double curve_id) {mycurveid = curve_id;}
   // double GetMycurveid() const {return mycurveid;}

	/* ���ú�ȡ�õ�ֵ��ֵ */
	void setMyContourValue(double cvalue);
	double getMyContourValue() const;

	/* ���ú�ȡ�õ�ֵ�ߵı�ʶ�� */
	void setMyContourID(int cid);
	int getMyContourID() const;

	//bool isCrossr(ContourDataArray * randomPoint, ContourDataArray * edge);
	//int  evaluateCrossPointNum(ContourDataArray * randomPoint, ContourDataArray * edge);
	bool isCrossr(QPolygonF randomPoint, QPolygonF edge);
	int  evaluateCrossPointNum(QPolygonF randomPoint, QPolygonF edge);

private:

	//double mycurveid;                         /* ��ֵ�ߵı�ʶ�� */
	//float myHeight;                          /* ��ֵ�ߵĸ߳�ֵ */
	int m_myContourID;				          /* ��ֵ�ߵı�ʶ�� */
	double m_myContourValue;						  /* ��ֵ�ߵ�ֵ */
	//ContourDataArray * myArrayData;          /* ��ֵ�߶�Ӧ�Ļ�ͼ���� */
	QPolygonF m_myArrayData;                  /* ��ֵ�߶�Ӧ�Ļ�ͼ���� */
};

