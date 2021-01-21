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

	m_paintBtn = new QPushButton(QStringLiteral("���Ƶ�ֵ��"));
	m_paintBtn->setFixedSize(QSize(78, 28));
	m_paintBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
	m_paintBtn->setStyleSheet("background-color: rgb(175,238,238)");
	vlay->addWidget(m_paintBtn);

	m_closeContoursBtn = new QPushButton(QStringLiteral("�պϵ�ֵ��"));
	m_closeContoursBtn->setFixedSize(QSize(78, 28));
	m_closeContoursBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
	m_closeContoursBtn->setStyleSheet("background-color: rgb(175,238,238)");
	vlay->addWidget(m_closeContoursBtn);

	m_colorsBtn = new QPushButton(QStringLiteral("��ɫ"));
	m_colorsBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
	m_colorsBtn->setStyleSheet("background-color: rgb(175,238,238)");
	m_colorsBtn->setFixedSize(QSize(78, 28));
	vlay->addWidget(m_colorsBtn);

	m_clearAllBtn = new QPushButton(QStringLiteral("���"));
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

	//��ȡԭʼ��ֵ������
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

		//�����ֵ�ߵ���Сֵ
		if (m_minValue > tempValue)
		{
			m_minValue = tempValue;
		}

		//�����ֵ�ߵ����ֵ
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
		QMessageBox::about(NULL, QStringLiteral("��ܰ��ʾ"), QStringLiteral("���Ȼ��Ʊպϵĵ�ֵ�ߡ�"));
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
	//��ȡԭʼ��ֵ������
	ReadData* readFileData = new ReadData();
	readFileData->readContourData();
	m_minX = readFileData->getMinX();
	m_minY = readFileData->getMinY();
	m_maxX = readFileData->getMaxX();
	m_maxY = readFileData->getMaxY();
	delete readFileData;

	//���ñ߽�߶�����
	double borderWidth = (m_maxX - m_minX) / 50.0;
	double borderHeight = (m_maxY - m_minY) / 50.0;

	QPainter painter(this);
	painter.save();
	painter.setRenderHint(QPainter::Antialiasing, true);
	this->resize(borderWidth * 1.5, borderHeight * 1.5);
	painter.setPen(Qt::gray);//��������������ɫ
	painter.translate(borderWidth / 4, borderHeight * 1.2);

	int step = 20;
	//������ 
	for (int i = 1; i <= 22; i++)
	{
		painter.drawLine(0, -borderHeight + step * i, borderWidth, -borderHeight + step * i);
	}

	for (int j = 1; j <= 28; j++)
	{
		painter.drawLine(j * step, -borderHeight, j * step, 0);
	}

	//���ƾ��α߽�
	//painter.setPen(Qt::red);
	painter.drawRect(0, -borderHeight, borderWidth, borderHeight);

	//////////////////////test time
	long beginTime = clock();//��ÿ�ʼʱ�䣬��λΪ����

    //���еĶ���������Լ�����ζ�Ӧ����ɫֵ
	std::vector<std::pair<QColor, QPolygonF> > orderedContourData;
	if (!m_originPolygonData.empty())
	{
		//����任
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
		//���ƶ�����Լ����������ɫ���
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

		//������ʼ��ɫ
		QColor startColor = Qt::yellow;
		//�����յ���ɫ
		QColor endColor = Qt::red;
		QColor qColor;
		QPolygonF qPolygon;

		//��ȡ����õĶ�����Լ��ö���ζ�Ӧ����ɫֵ
		for (int i = 1; i < tempNode.size(); i++)
		{
			int contourId = tempNode[i]->getCoutourData()->getMyContourID() - 1;
			double currentVal = m_closedPolygonData[contourId].first;
			qPolygon = m_closedPolygonData[contourId].second;
			qColor = contourFill(currentVal, m_minValue, m_maxValue, startColor, endColor);
			orderedContourData.push_back(std::make_pair(qColor, qPolygon));
			qPolygon = QPolygonF();
		}

		//����任
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
		//���ƶ�����Լ����������ɫ���
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

			//����������ʾλ��
			int place = orderedContourData[i].second.size() / 2;
			QPointF pointText = QPointF(orderedContourData[i].second.at(place));
			QFont qFont;
			//���������ʽ
			qFont.setFamily("Times New Roman");
			//���������С
			qFont.setPointSize(8);
			//���������ϸ
			qFont.setBold(false);
			painter.setFont(qFont);
			int contourId = tempNode[i + 1]->getCoutourData()->getMyContourID() - 1;
			int currentVal = m_closedPolygonData[contourId].first;

			QString qstrEleInfo = QVariant(currentVal).toString();
			painter.drawText(pointText, qstrEleInfo);
		}
		long endTime = clock();//��ý���ʱ�� 

		////////////////////////end test time
		delete contourBinTree;
	}

	painter.restore();
}

//��ֵ�߻��Ƽ���ɫ���
void ContouDraw::paintEvent1(QPaintEvent *event)
{
	std::vector<std::pair<double, QPolygonF> > closedPolygonData;
	ContourClose* closedData = new ContourClose();
	closedData->closeContour();
	closedPolygonData = closedData->getClosedContourData();

	//���ֵ�ߵ������Сֵ
	double maxV, minV;
	maxV = minV = closedPolygonData[0].first;
	for (int i = 1; i < closedPolygonData.size(); i++)
	{
		double tempValue = closedPolygonData[i].first;

		//�����ֵ�ߵ���Сֵ
		if (minV > tempValue)
		{
			minV = tempValue;
		}

		//�����ֵ�ߵ����ֵ
		if (maxV < tempValue)
		{
			maxV = tempValue;
		}
	}

	Rectangle rect = closedData->getRectange();
	double minX = rect.rectPointA.x();//��Сxֵ
	double minY = rect.rectPointB.y();//��Сyֵ
	double maxX = rect.rectPointC.x();//���xֵ
	double maxY = rect.rectPointD.y();//���yֵ

	//���ñ߽�߶�����
	double borderWidth = (maxX - minX) / 50.0;
	double borderHeight = (maxY - minY) / 50.0;

	QPainter painter(this);
	painter.save();
	painter.setRenderHint(QPainter::Antialiasing, true);
	this->resize(borderWidth * 1.5, borderHeight * 1.5);
	painter.setPen(Qt::gray);//��������������ɫ
	painter.translate(borderWidth / 4, borderHeight * 1.2);

	int step = 20;
	//������ 
	for (int i = 1; i <= 22; i++)
	{
		painter.drawLine(0, -borderHeight + step * i, borderWidth, -borderHeight + step * i);
	}

	for (int j = 1; j <= 28; j++)
	{
		painter.drawLine(j * step, -borderHeight, j * step, 0);
	}

	//���ƾ��α߽�
	//painter.setPen(Qt::red);
	//painter.drawRect(0, -borderHeight, borderWidth, borderHeight);

	//////////////////////test time
	long beginTime = clock();//��ÿ�ʼʱ�䣬��λΪ����

	BuildBinTopoTree* contourBinTree = new BuildBinTopoTree(closedPolygonData);
	contourBinTree->makeTopologyBinaryTree();
	TreeNode* root = contourBinTree->m_topologyBinaryTree->getMyRoot();
	contourBinTree->preoderTopologyBinaryTree(root);
	std::vector<TreeNode*> tempNode = contourBinTree->getPreorderContourTree();

	//���еĶ���������Լ�����ζ�Ӧ����ɫֵ
	std::vector<std::pair<QColor, QPolygonF> > orderedContourData;
	//������ʼ��ɫ
	QColor startColor = Qt::yellow;
	//�����յ���ɫ
	QColor endColor = Qt::red;
	QColor qColor;
	QPolygonF qPolygon;

	//��ȡ����õĶ�����Լ��ö���ζ�Ӧ����ɫֵ
	for (int i = 1; i < tempNode.size(); i++)
	{
		int t_id = tempNode[i]->getCoutourData()->getMyContourID() - 1;
		double currentVal = closedPolygonData[t_id].first;
		qPolygon = closedPolygonData[t_id].second;
		qColor = contourFill(currentVal, minV, maxV, startColor, endColor);
		orderedContourData.push_back(std::make_pair(qColor, qPolygon));
		qPolygon = QPolygonF();
	}

	//����任
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
	//���ƶ�����Լ����������ɫ���
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

		//����������ʾλ��
		int place = orderedContourData[i].second.size() / 2;
		QPointF pointText = QPointF(orderedContourData[i].second.at(place));
		QFont qFont;
		//���������ʽ
		qFont.setFamily("Times New Roman");
		//���������С
		qFont.setPointSize(8);
		//���������ϸ
		qFont.setBold(false);
		painter.setFont(qFont);
		int t_id = tempNode[i + 1]->getCoutourData()->getMyContourID() - 1;
		int currentVal = closedPolygonData[t_id].first;

		QString qstrEleInfo = QVariant(currentVal).toString();
		painter.drawText(pointText, qstrEleInfo);
	}
	long endTime = clock();//��ý���ʱ�� 

	////////////////////////end test time
	delete contourBinTree;
	painter.restore();
}


//ͨ�����Բ�ֵ��ȡ��ǰ����ζ�Ӧ��RBGֵ
QColor ContouDraw::contourFill(double curentValue, double minValue, double maxValue, QColor startColor, QColor endColor)
{
	//��ȡ��ʼ��ɫ��RGBֵ
	int startR = qRed(startColor.rgb());
	int startG = qGreen(startColor.rgb());
	int startB = qBlue(startColor.rgb());
	//��ȡ�յ���ɫ��RBGֵ
	int endR = qRed(endColor.rgb());
	int endG = qGreen(endColor.rgb());
	int endB = qBlue(endColor.rgb());
	//���Բ�ֵ����ȡ��ǰֵ��Ӧ��RBGֵ
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

