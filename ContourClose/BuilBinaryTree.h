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


	// 用于建立该等值线图的拓扑二叉树的函数
	void makeTopologyBinaryTree();
	//先序遍历二叉树
	void preoderTopologyBinaryTree(TreeNode * node);
	//获取先跟遍历好的二叉树
	std::vector<TreeNode *> getPreorderContourTree() const;
	//void make_preorder_contour_list();
	// 对应于该等值线图的拓扑二叉树
	TopologyTree * m_topologyBinaryTree;

private:
	//所有等值线数据
	//std::vector<ContourDataArray *> m_iallContourData;
	//等值线数据以及对应的等值线值
	std::vector<std::pair<double, QPolygonF> > m_allPolygonData;
	//存储先序遍历好的二叉树节点
	std::vector<TreeNode *> m_preorderContourVec;


	//double m_minX;//最小x值
	//double m_minY;//最小y值
	//double m_maxX;//最大x值
	//double m_maxY;//最大y值
};