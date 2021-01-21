#include "ContourDraw.h"
#include "ReadData.h"
#include "ContourClose.h"
#include "BuilBinaryTree.h"

#include <QPen>
#include <QBrush>
#include <QPainter>
#include <time.h>
#include <qvariant.h>
#include <QVBoxLayout>
#include <qmessagebox.h>

ContouDraw::ContouDraw(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	setWindowTitle("ContourLines");
	setFixedSize(QSize(850, 700));

	setClickBtns();
}

ContouDraw::~ContouDraw()
{
	if (m_paintBtn != nullptr)
	{
		delete m_paintBtn;
	}

	if (m_closeContoursBtn != nullptr)
	{
		delete m_closeContoursBtn;
	}

	if (m_colorsBtn != nullptr)
	{
		delete m_colorsBtn;
	}
}

void ContouDraw::setClickBtns()
{
	QWidget* wdg = new QWidget(this);
	QVBoxLayout* vlay = new QVBoxLayout(wdg);

	m_paintBtn = new QPushButton(QStringLiteral("绘制等值线"));
	m_paintBtn->setFixedSize(QSize(78, 28));
	m_paintBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
	m_paintBtn->setStyleSheet("background-color: rgb(175,238,238)");
	vlay->addWidget(m_paintBtn);

	m_closeContoursBtn = new QPushButton(QStringLiteral("闭合等值线"));
	m_closeContoursBtn->setFixedSize(QSize(78, 28));
	m_closeContoursBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
	m_closeContoursBtn->setStyleSheet("background-color: rgb(175,238,238)");
	vlay->addWidget(m_closeContoursBtn);

	m_colorsBtn = new QPushButton(QStringLiteral("着色"));
	m_colorsBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
	m_colorsBtn->setStyleSheet("background-color: rgb(175,238,238)");
	m_colorsBtn->setFixedSize(QSize(78, 28));
	vlay->addWidget(m_colorsBtn);

	m_clearAllBtn = new QPushButton(QStringLiteral("清除"));
	m_clearAllBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
	m_clearAllBtn->setStyleSheet("background-color: rgb(175,238,238)");
	m_clearAllBtn->setFixedSize(QSize(78, 28));
	vlay->addWidget(m_clearAllBtn);

	wdg->setLayout(vlay);
	setCentralWidget(wdg);

	// Connect button signal to appropriate slot
	connect(m_paintBtn, &QPushButton::clicked, this, &ContouDraw::handlePaintButton);
	connect(m_closeContoursBtn, &QPushButton::clicked, this, &ContouDraw::handleCloseButton);
	connect(m_colorsBtn, &QPushButton::clicked, this, &ContouDraw::handleColorButton);
	connect(m_clearAllBtn, &QPushButton::clicked, this, &ContouDraw::handleClearButton);
}

void ContouDraw::handlePaintButton()
{
	m_originPolygonData.clear();
	m_closedPolygonData.clear();

	m_isColorsBtnClicked = false;

	//读取原始等值线数据
	ReadData* readFileData = new ReadData();
	readFileData->readContourData();
	m_originPolygonData = readFileData->getAllPolygonData();
	delete readFileData;

	update();
}

void ContouDraw::handleCloseButton()
{
	m_originPolygonData.clear();
	m_closedPolygonData.clear();

	m_isColorsBtnClicked = false;

	ContourClose* closedData = new ContourClose();
	closedData->closeContour();
	m_closedPolygonData = closedData->getClosedContourData();

	m_maxValue = m_minValue = m_closedPolygonData[0].first;
	for (int i = 1; i < m_closedPolygonData.size(); i++)
	{
		double tempValue = m_closedPolygonData[i].first;

		//求出等值线的最小值
		if (m_minValue > tempValue)
		{
			m_minValue = tempValue;
		}

		//求出等值线的最大值
		if (m_maxValue < tempValue)
		{
			m_maxValue = tempValue;
		}
	}

	update();
}

void ContouDraw::handleColorButton()
{
	if (m_closedPolygonData.empty())
	{
		QMessageBox::about(NULL, QStringLiteral("温馨提示"), QStringLiteral("请先绘制闭合的等值线。"));
	}
	else
	{
		m_isColorsBtnClicked = true;
		update();
	}
}

void ContouDraw::handleClearButton()
{
	m_originPolygonData.clear();
	m_closedPolygonData.clear();
	m_isColorsBtnClicked = false;

	update();
}

void ContouDraw::paintEvent(QPaintEvent* event)
{
	//读取原始等值线数据
	ReadData* readFileData = new ReadData();
	readFileData->readContourData();
	m_minX = readFileData->getMinX();
	m_minY = readFileData->getMinY();
	m_maxX = readFileData->getMaxX();
	m_maxY = readFileData->getMaxY();
	delete readFileData;

	//设置边界高度与宽度
	double borderWidth = (m_maxX - m_minX) / 50.0;
	double borderHeight = (m_maxY - m_minY) / 50.0;

	QPainter painter(this);
	painter.save();
	painter.setRenderHint(QPainter::Antialiasing, true);
	this->resize(borderWidth * 1.5, borderHeight * 1.5);
	painter.setPen(Qt::gray);//设置网格线条颜色
	painter.translate(borderWidth / 4, borderHeight * 1.2);

	int step = 20;
	//画网格 
	for (int i = 1; i <= 22; i++)
	{
		painter.drawLine(0, -borderHeight + step * i, borderWidth, -borderHeight + step * i);
	}

	for (int j = 1; j <= 28; j++)
	{
		painter.drawLine(j * step, -borderHeight, j * step, 0);
	}

	//绘制矩形边界
	//painter.setPen(Qt::red);
	painter.drawRect(0, -borderHeight, borderWidth, borderHeight);

	//////////////////////test time
	long beginTime = clock();//获得开始时间，单位为毫秒

    //所有的多边形数据以及多边形对应的颜色值
	std::vector<std::pair<QColor, QPolygonF> > orderedContourData;
	if (!m_originPolygonData.empty())
	{
		//坐标变换
		for (int i = 0; i < m_originPolygonData.size(); ++i)
		{
			QPointF pointF;
			for (int j = 0; j < m_originPolygonData[i].second.size(); ++j)
			{
				pointF = m_originPolygonData[i].second.at(j);
				pointF.rx() = (pointF.x() - m_minX) / 50.0;
				pointF.ry() = -(pointF.y() - m_minY) / 50.0;
				m_originPolygonData[i].second.replace(j, pointF);
			}
		}

		painter.setPen(QPen(Qt::black));
		//绘制多边形以及对其进行颜色填充
		for (int i = 0; i < m_originPolygonData.size(); i++)
		{
			QPolygonF tempPolygon;
			tempPolygon = m_originPolygonData[i].second;
			painter.drawPolyline(tempPolygon);
		}
				
	}
	else if (!m_closedPolygonData.empty())
	{
		BuildBinTopoTree* contourBinTree = new BuildBinTopoTree(m_closedPolygonData);
		contourBinTree->makeTopologyBinaryTree();
		TreeNode* root = contourBinTree->m_topologyBinaryTree->getMyRoot();
		contourBinTree->preoderTopologyBinaryTree(root);
		std::vector<TreeNode*> tempNode = contourBinTree->getPreorderContourTree();

		//设置起始颜色
		QColor startColor = Qt::yellow;
		//设置终点颜色
		QColor endColor = Qt::red;
		QColor qColor;
		QPolygonF qPolygon;

		//获取排序好的多边形以及该多边形对应的颜色值
		for (int i = 1; i < tempNode.size(); i++)
		{
			int contourId = tempNode[i]->getCoutourData()->getMyContourID() - 1;
			double currentVal = m_closedPolygonData[contourId].first;
			qPolygon = m_closedPolygonData[contourId].second;
			qColor = contourFill(currentVal, m_minValue, m_maxValue, startColor, endColor);
			orderedContourData.push_back(std::make_pair(qColor, qPolygon));
			qPolygon = QPolygonF();
		}

		//坐标变换
		for (int i = 0; i < orderedContourData.size(); ++i)
		{
			QPointF pointF;
			for (int j = 0; j < orderedContourData[i].second.size(); ++j)
			{
				pointF = orderedContourData[i].second.at(j);
				pointF.rx() = (pointF.x() - m_minX) / 50.0;
				pointF.ry() = -(pointF.y() - m_minY) / 50.0;
				orderedContourData[i].second.replace(j, pointF);
			}
		}

		painter.setPen(QPen(Qt::black));
		//绘制多边形以及对其进行颜色填充
		for (int i = 0; i < orderedContourData.size(); i++)
		{
			if (m_isColorsBtnClicked)
			{
				QColor tempColor = orderedContourData[i].first;
			    painter.setBrush(QBrush(tempColor));
			}

			QPolygonF tempPolygon;
			tempPolygon = orderedContourData[i].second;
			painter.drawPolygon(tempPolygon);

			tempPolygon = QPolygonF();

			//设置字体显示位置
			int place = orderedContourData[i].second.size() / 2;
			QPointF pointText = QPointF(orderedContourData[i].second.at(place));
			QFont qFont;
			//设置字体格式
			qFont.setFamily("Times New Roman");
			//设置字体大小
			qFont.setPointSize(8);
			//设置字体粗细
			qFont.setBold(false);
			painter.setFont(qFont);
			int contourId = tempNode[i + 1]->getCoutourData()->getMyContourID() - 1;
			int currentVal = m_closedPolygonData[contourId].first;

			QString qstrEleInfo = QVariant(currentVal).toString();
			painter.drawText(pointText, qstrEleInfo);
		}
		long endTime = clock();//获得结束时间 

		////////////////////////end test time
		delete contourBinTree;
	}

	painter.restore();
}

//等值线绘制及颜色填充
void ContouDraw::paintEvent1(QPaintEvent *event)
{
	std::vector<std::pair<double, QPolygonF> > closedPolygonData;
	ContourClose* closedData = new ContourClose();
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

	Rectangle rect = closedData->getRectange();
	double minX = rect.rectPointA.x();//最小x值
	double minY = rect.rectPointB.y();//最小y值
	double maxX = rect.rectPointC.x();//最大x值
	double maxY = rect.rectPointD.y();//最大y值

	//设置边界高度与宽度
	double borderWidth = (maxX - minX) / 50.0;
	double borderHeight = (maxY - minY) / 50.0;

	QPainter painter(this);
	painter.save();
	painter.setRenderHint(QPainter::Antialiasing, true);
	this->resize(borderWidth * 1.5, borderHeight * 1.5);
	painter.setPen(Qt::gray);//设置网格线条颜色
	painter.translate(borderWidth / 4, borderHeight * 1.2);

	int step = 20;
	//画网格 
	for (int i = 1; i <= 22; i++)
	{
		painter.drawLine(0, -borderHeight + step * i, borderWidth, -borderHeight + step * i);
	}

	for (int j = 1; j <= 28; j++)
	{
		painter.drawLine(j * step, -borderHeight, j * step, 0);
	}

	//绘制矩形边界
	//painter.setPen(Qt::red);
	//painter.drawRect(0, -borderHeight, borderWidth, borderHeight);

	//////////////////////test time
	long beginTime = clock();//获得开始时间，单位为毫秒

	BuildBinTopoTree* contourBinTree = new BuildBinTopoTree(closedPolygonData);
	contourBinTree->makeTopologyBinaryTree();
	TreeNode* root = contourBinTree->m_topologyBinaryTree->getMyRoot();
	contourBinTree->preoderTopologyBinaryTree(root);
	std::vector<TreeNode*> tempNode = contourBinTree->getPreorderContourTree();

	//所有的多边形数据以及多边形对应的颜色值
	std::vector<std::pair<QColor, QPolygonF> > orderedContourData;
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
		orderedContourData.push_back(std::make_pair(qColor, qPolygon));
		qPolygon = QPolygonF();
	}

	//坐标变换
	for (int i = 0; i < orderedContourData.size(); ++i)
	{
		QPointF pointF;
		for (int j = 0; j < orderedContourData[i].second.size(); ++j)
		{
			pointF = orderedContourData[i].second.at(j);
			pointF.rx() = (pointF.x() - minX) / 50.0;
			pointF.ry() = -(pointF.y() - minY) / 50.0;
			orderedContourData[i].second.replace(j, pointF);
		}
	}

	painter.setPen(QPen(Qt::black));
	//绘制多边形以及对其进行颜色填充
	for (int i = 0; i < orderedContourData.size(); i++)
		//for (int i = oderedContourData.size()-1; i >= 0; i--)
		//for (int i = 42; i < 45; i++)
	{
		QColor tempColor = orderedContourData[i].first;
		painter.setBrush(QBrush(tempColor));
		QPolygonF tempPolygon;
		tempPolygon = orderedContourData[i].second;
		painter.drawPolygon(tempPolygon);
		//painter.drawPolyline(tempPolygon);
		tempPolygon = QPolygonF();

		//设置字体显示位置
		int place = orderedContourData[i].second.size() / 2;
		QPointF pointText = QPointF(orderedContourData[i].second.at(place));
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

