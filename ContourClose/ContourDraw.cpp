#include "ContourDraw.h"
#include "ReadData.h"
#include "ContourClose.h"
#include "BuilBinaryTree.h"

#include <QPen>
#include <QBrush>
#include <QPainter>
#include <time.h>
#include <qvariant.h>

ContouDraw::ContouDraw(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	setWindowTitle("ContourLines");
}

ContouDraw::~ContouDraw()
{

}

//等值线绘制及颜色填充
void ContouDraw::paintEvent(QPaintEvent *event)
{
	//std::vector<ContourDataArray *> contourData;
	//std::vector<std::pair<double, QPolygonF> > polygonData;
	ReadData *readFileData = new ReadData();
	readFileData->readContourData();
	//contourData = readFileData->GetAllContourData();
	//polygonData = readFileData->GetAllPolygonData();

	std::vector<std::pair<double, QPolygonF> > closedPolygonData;
	ContourClose *closedData = new ContourClose();
	closedData->closeContour();
	closedPolygonData = closedData->getClosedContourData();

	//求等值线的最大最小值
	double maxV, minV;
	maxV = minV = closedPolygonData[0].first;
	for (int i = 1; i < closedPolygonData.size(); i++)
	{
		double tempValue = closedPolygonData[i].first;

		//求出等值线的最小值
		if (minV > tempValue)
		{
			minV = tempValue;
		}

		//求出等值线的最大值
		if (maxV < tempValue)
		{
			maxV = tempValue;
		}
	}

	double minX = 0.0;//最小x值
	double minY = 0.0;//最小y值
	double maxX = 0.0;//最大x值
	double maxY = 0.0;//最大y值
	minX = readFileData->getMinX();
	minY = readFileData->getMinY();
	maxX = readFileData->getMaxX();
	maxY = readFileData->getMaxY();
	delete readFileData;

	//设置边界高度与宽度
	double t_width = (maxX - minX) / 50.0;
	double t_height = (maxY - minY) / 50.0;

	QPainter painter(this);
	painter.save();
	painter.setRenderHint(QPainter::Antialiasing, true);
	this->resize(t_width*1.5, t_height*1.5);
	painter.setPen(Qt::gray);//设置网格线条颜色
	painter.translate(t_width / 4, t_height*1.2);

	int step = 20;
	//画网格 
	for (int i = 1; i <= 22; i++)
	{
		painter.drawLine(0, -t_height + step * i, t_width, -t_height + step * i);
	}

	for (int j = 1; j <= 28; j++)
	{
		painter.drawLine(j*step, -t_height, j*step, 0);
	}

	//绘制矩形边界
	//painter.setPen(Qt::red);
	//painter.drawRect(0, -t_height, t_width, t_height);

	//////////////////////test time
	long beginTime = clock();//获得开始时间，单位为毫秒

	BuildBinTopoTree *contourBinTree = new BuildBinTopoTree(closedPolygonData);
	contourBinTree->makeTopologyBinaryTree();
	//BuildBinTopoTree *contourBinTree = new BuildBinTopoTree(polygonData, contourData);
	//contourBinTree->make_preorder_contour_list();
	TreeNode *root = contourBinTree->m_topologyBinaryTree->getMyRoot();
	contourBinTree->preoderTopologyBinaryTree(root);
	//contourBinTree->make_preorder_contour_list();
	std::vector<TreeNode *> temp_node = contourBinTree->getPreorderContourTree();

	//所有的多边形数据以及多边形对应的颜色值
	std::vector<std::pair<QColor, QPolygonF> > oderedContourData;
	//设置起始颜色
	QColor t_startColor = Qt::red;
	//设置终点颜色
	QColor t_endColor = Qt::blue;
	QColor t_color;
	QPolygonF t_polygon;

	//获取排序好的多边形以及该多边形对应的颜色值
	for (int i = 1; i < temp_node.size(); i++)
	{
		int t_id = temp_node[i]->getCoutourData()->getMyContourID() - 1;
		double currentVal = closedPolygonData[t_id].first;
		t_polygon = closedPolygonData[t_id].second;
		t_color = contourFill(currentVal, minV, maxV, t_startColor, t_endColor);
		oderedContourData.push_back(std::make_pair(t_color, t_polygon));
		t_polygon = QPolygonF();
	}

	//坐标变换
	for (int i = 0; i < oderedContourData.size(); ++i)
	{
		QPointF pointF;
		for (int j = 0; j < oderedContourData[i].second.size(); ++j)
		{
			pointF = oderedContourData[i].second.at(j);
			pointF.rx() = (pointF.x() - minX) / 50.0;
			pointF.ry() = -(pointF.y() - minY) / 50.0;
			oderedContourData[i].second.replace(j, pointF);
		}
	}

	painter.setPen(QPen(Qt::black));
	//绘制多边形以及对其进行颜色填充
	for (int i = 0; i < oderedContourData.size(); i++)
		//for (int i = oderedContourData.size()-1; i >= 0; i--)
		//for (int i = 42; i < 45; i++)
	{
		QColor tempColor = oderedContourData[i].first;
		painter.setBrush(QBrush(tempColor));
		QPolygonF tempPolygon;
		tempPolygon = oderedContourData[i].second;
		painter.drawPolygon(tempPolygon);
		//painter.drawPolyline(tempPolygon);
		tempPolygon = QPolygonF();

		//设置字体显示位置
		int place = oderedContourData[i].second.size() / 2;
		QPointF pointText = QPointF(oderedContourData[i].second.at(place));
		QFont qFont;
		//设置字体格式
		qFont.setFamily("Times New Roman");
		//设置字体大小
		qFont.setPointSize(8);
		//设置字体粗细
		qFont.setBold(false);
		painter.setFont(qFont);
		int t_id = temp_node[i + 1]->getCoutourData()->getMyContourID() - 1;
		int currentVal = closedPolygonData[t_id].first;
	
		QString qstrEleInfo = QVariant(currentVal).toString();
		painter.drawText(pointText, qstrEleInfo);
	}
	long endTime = clock();//获得结束时间 

	////////////////////////end test time
	delete contourBinTree;
	painter.restore();
}


//通过线性插值获取当前多边形对应的RBG值
QColor ContouDraw::contourFill(double curentValue, double minValue, double maxValue, QColor startColor, QColor endColor)
{
	//获取起始颜色的RGB值
	int t_startR = qRed(startColor.rgb());
	int t_startG = qGreen(startColor.rgb());
	int t_startB = qBlue(startColor.rgb());
	//获取终点颜色的RBG值
	int t_endR = qRed(endColor.rgb());
	int t_endG = qGreen(endColor.rgb());
	int t_endB = qBlue(endColor.rgb());
	//线性插值，获取当前值对应的RBG值
	double numerator = curentValue - minValue;
	double denominator = maxValue - minValue;
	double ratio = numerator / denominator;
	//double ratio = (curentValue - minValue)/(maxValue - minValue);
	int r = (t_endR - t_startR) * ratio + t_startR;
	int g = (t_endG - t_startG) * ratio + t_startG;
	int b = (t_endB - t_startB) * ratio + t_startB;

	QColor color(qRgb(r, g, b));

	return color;
}

