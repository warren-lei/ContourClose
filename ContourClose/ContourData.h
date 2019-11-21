/******************************************************************************
文件名：ContourData.h
作用：ContourData类的声明头文件，描述拓扑包含二叉树中一个节点所存储的等值线数据
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

	/* 设置和取得高程值 */
	//void setMyHeight(float height) {myHeight = height;}
   // float GetMyHeight() const {return myHeight;}

	/* 设置和取得等值线点数组 */
	void setMyArrayData(QPolygonF  data);
	QPolygonF  getMyArrayData() const;

	/* 设置和取得等值线的标识号 */
	//void setMycurveid(double curve_id) {mycurveid = curve_id;}
   // double GetMycurveid() const {return mycurveid;}

	/* 设置和取得等值线值 */
	void setMyContourValue(double cvalue);
	double getMyContourValue() const;

	/* 设置和取得等值线的标识号 */
	void setMyContourID(int cid);
	int getMyContourID() const;

	//bool isCrossr(ContourDataArray * randomPoint, ContourDataArray * edge);
	//int  evaluateCrossPointNum(ContourDataArray * randomPoint, ContourDataArray * edge);
	bool isCrossr(QPolygonF randomPoint, QPolygonF edge);
	int  evaluateCrossPointNum(QPolygonF randomPoint, QPolygonF edge);

private:

	//double mycurveid;                         /* 等值线的标识号 */
	//float myHeight;                          /* 等值线的高程值 */
	int m_myContourID;				          /* 等值线的标识号 */
	double m_myContourValue;						  /* 等值线的值 */
	//ContourDataArray * myArrayData;          /* 等值线对应的绘图数据 */
	QPolygonF m_myArrayData;                  /* 等值线对应的绘图数据 */
};

