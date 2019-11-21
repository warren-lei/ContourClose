#pragma once
#include <QPolygonF>
#include <vector>

#include "TopoTree.h"

//class ContourDataArray;
class TopologyTree;
class TreeNode;

class BuildBinTopoTree
{
public:
	/*BuildBinTopoTree(std::vector<std::pair<int, QPolygonF> > iAllPolygonData,
					 std::vector<ContourDataArray *> iallContourData);*/
	BuildBinTopoTree(std::vector<std::pair<double, QPolygonF> > &iAllPolygonData);
	~BuildBinTopoTree();


	// ���ڽ����õ�ֵ��ͼ�����˶������ĺ���
	void makeTopologyBinaryTree();
	//�������������
	void preoderTopologyBinaryTree(TreeNode * node);
	//��ȡ�ȸ������õĶ�����
	std::vector<TreeNode *> getPreorderContourTree() const;
	//void make_preorder_contour_list();
	// ��Ӧ�ڸõ�ֵ��ͼ�����˶�����
	TopologyTree * m_topologyBinaryTree;

private:
	//���е�ֵ������
	//std::vector<ContourDataArray *> m_iallContourData;
	//��ֵ�������Լ���Ӧ�ĵ�ֵ��ֵ
	std::vector<std::pair<double, QPolygonF> > m_allPolygonData;
	//�洢��������õĶ������ڵ�
	std::vector<TreeNode *> m_preorderContourVec;


	//double m_minX;//��Сxֵ
	//double m_minY;//��Сyֵ
	//double m_maxX;//���xֵ
	//double m_maxY;//���yֵ
};