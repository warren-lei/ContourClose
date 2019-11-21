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

//��ֵ�߻��Ƽ���ɫ���
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

	double minX = 0.0;//��Сxֵ
	double minY = 0.0;//��Сyֵ
	double maxX = 0.0;//���xֵ
	double maxY = 0.0;//���yֵ
	minX = readFileData->getMinX();
	minY = readFileData->getMinY();
	maxX = readFileData->getMaxX();
	maxY = readFileData->getMaxY();
	delete readFileData;

	//���ñ߽�߶�����
	double t_width = (maxX - minX) / 50.0;
	double t_height = (maxY - minY) / 50.0;

	QPainter painter(this);
	painter.save();
	painter.setRenderHint(QPainter::Antialiasing, true);
	this->resize(t_width*1.5, t_height*1.5);
	painter.setPen(Qt::gray);//��������������ɫ
	painter.translate(t_width / 4, t_height*1.2);

	int step = 20;
	//������ 
	for (int i = 1; i <= 22; i++)
	{
		painter.drawLine(0, -t_height + step * i, t_width, -t_height + step * i);
	}

	for (int j = 1; j <= 28; j++)
	{
		painter.drawLine(j*step, -t_height, j*step, 0);
	}

	//���ƾ��α߽�
	//painter.setPen(Qt::red);
	//painter.drawRect(0, -t_height, t_width, t_height);

	//////////////////////test time
	long beginTime = clock();//��ÿ�ʼʱ�䣬��λΪ����

	BuildBinTopoTree *contourBinTree = new BuildBinTopoTree(closedPolygonData);
	contourBinTree->makeTopologyBinaryTree();
	//BuildBinTopoTree *contourBinTree = new BuildBinTopoTree(polygonData, contourData);
	//contourBinTree->make_preorder_contour_list();
	TreeNode *root = contourBinTree->m_topologyBinaryTree->getMyRoot();
	contourBinTree->preoderTopologyBinaryTree(root);
	//contourBinTree->make_preorder_contour_list();
	std::vector<TreeNode *> temp_node = contourBinTree->getPreorderContourTree();

	//���еĶ���������Լ�����ζ�Ӧ����ɫֵ
	std::vector<std::pair<QColor, QPolygonF> > oderedContourData;
	//������ʼ��ɫ
	QColor t_startColor = Qt::red;
	//�����յ���ɫ
	QColor t_endColor = Qt::blue;
	QColor t_color;
	QPolygonF t_polygon;

	//��ȡ����õĶ�����Լ��ö���ζ�Ӧ����ɫֵ
	for (int i = 1; i < temp_node.size(); i++)
	{
		int t_id = temp_node[i]->getCoutourData()->getMyContourID() - 1;
		double currentVal = closedPolygonData[t_id].first;
		t_polygon = closedPolygonData[t_id].second;
		t_color = contourFill(currentVal, minV, maxV, t_startColor, t_endColor);
		oderedContourData.push_back(std::make_pair(t_color, t_polygon));
		t_polygon = QPolygonF();
	}

	//����任
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
	//���ƶ�����Լ����������ɫ���
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

		//����������ʾλ��
		int place = oderedContourData[i].second.size() / 2;
		QPointF pointText = QPointF(oderedContourData[i].second.at(place));
		QFont qFont;
		//���������ʽ
		qFont.setFamily("Times New Roman");
		//���������С
		qFont.setPointSize(8);
		//���������ϸ
		qFont.setBold(false);
		painter.setFont(qFont);
		int t_id = temp_node[i + 1]->getCoutourData()->getMyContourID() - 1;
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
	int t_startR = qRed(startColor.rgb());
	int t_startG = qGreen(startColor.rgb());
	int t_startB = qBlue(startColor.rgb());
	//��ȡ�յ���ɫ��RBGֵ
	int t_endR = qRed(endColor.rgb());
	int t_endG = qGreen(endColor.rgb());
	int t_endB = qBlue(endColor.rgb());
	//���Բ�ֵ����ȡ��ǰֵ��Ӧ��RBGֵ
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

