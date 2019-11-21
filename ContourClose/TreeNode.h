/***********************************************************
作用：描述拓扑包含二叉树上的树节点, 类tree_node的声明头文件
************************************************************/
#pragma once

#include <cstddef>

enum Topology
{
	INCLUSION,
	INCLUDED,
	PARALLEL
};

class ContourData;
class TreeNode;

class TreeNode
{
public:
	TreeNode();
	TreeNode(ContourData * newData, TreeNode * left, TreeNode * right, TreeNode * father);
	~TreeNode();

	TreeNode * insert(ContourData * newData);
	void setCoutourData(ContourData * data);
	ContourData * getCoutourData() const;
	void setMyLeftChild(TreeNode * node);
	TreeNode * getMyLeftChild() const;
	void setMyRightChild(TreeNode * node);
	TreeNode * getMyRightChild() const;
	void setMyFather(TreeNode * node);
	TreeNode * getMyFather() const;

private:
	ContourData * m_countourData;
	TreeNode * m_myLeftChild;
	TreeNode * m_myRightChild;
	TreeNode * m_myFather;
};
