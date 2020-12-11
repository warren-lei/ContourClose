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
	ReadData *readFileData = new ReadData();
	readFileData->readContourData();

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
	double borderWidth = (maxX - minX) / 50.0;
	double borderHeight = (maxY - minY) / 50.0;

	QPainter painter(this);
	painter.save();
	painter.setRenderHint(QPainter::Antialiasing, true);
	this->resize(borderWidth*1.5, borderHeight*1.5);
	painter.setPen(Qt::gray);//设置网格线条颜色
	painter.translate(borderWidth / 4, borderHeight*1.2);

	int step = 20;
	//画网格 
	for (int i = 1; i <= 22; i++)
	{
		painter.drawLine(0, -borderHeight + step * i, borderWidth, -borderHeight + step * i);
	}

	for (int j = 1; j <= 28; j++)
	{
		painter.drawLine(j*step, -borderHeight, j*step, 0);
	}

	//绘制矩形边界
	//painter.setPen(Qt::red);
	//painter.drawRect(0, -t_height, t_width, t_height);

	//////////////////////test time
	long beginTime = clock();//获得开始时间，单位为毫秒

	BuildBinTopoTree *contourBinTree = new BuildBinTopoTree(closedPolygonData);
	contourBinTree->makeTopologyBinaryTree();
	TreeNode *root = contourBinTree->m_topologyBinaryTree->getMyRoot();
	contourBinTree->preoderTopologyBinaryTree(root);
	std::vector<TreeNode *> tempNode = contourBinTree->getPreorderContourTree();

	//所有的多边形数据以及多边形对应的颜色值
	std::vector<std::pair<QColor, QPolygonF> > oderedContourData;
	//设置起始颜色
	QColor startColor = Qt::yellow;
	//设置终点颜色
	QColor endColor = Qt::red;
	QColor qColor;
	QPolygonF qPolygon;

	//获取排序好的多边形以及该多边形对应的颜色值
	for (int i = 1; i < tempNode.size(); i++)
	{
		int t_id = tempNode[i]->getCoutourData()->getMyContourID() - 1;
		double currentVal = closedPolygonData[t_id].first;
		qPolygon = closedPolygonData[t_id].second;
		qColor = contourFill(currentVal, minV, maxV, startColor, endColor);
		oderedContourData.push_back(std::make_pair(qColor, qPolygon));
		qPolygon = QPolygonF();
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
		int t_id = tempNode[i + 1]->getCoutourData()->getMyContourID() - 1;
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
	int startR = qRed(startColor.rgb());
	int startG = qGreen(startColor.rgb());
	int startB = qBlue(startColor.rgb());
	//获取终点颜色的RBG值
	int endR = qRed(endColor.rgb());
	int endG = qGreen(endColor.rgb());
	int endB = qBlue(endColor.rgb());
	//线性插值，获取当前值对应的RBG值
	double numerator = curentValue - minValue;
	double denominator = maxValue - minValue;
	double ratio = numerator / denominator;
	//double ratio = (curentValue - minValue)/(maxValue - minValue);
	int r = (endR - startR) * ratio + startR;
	int g = (endG - startG) * ratio + startG;
	int b = (endB - startB) * ratio + startB;

	QColor color(qRgb(r, g, b));

	return color;
}

