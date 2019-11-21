/***********************************************************
作用：描述一颗拓扑包含二叉树, 类TopologyTree的声明头文件
************************************************************/

#ifndef TOPOTREE_H
#define TOPOTREE_H

#include "ContourData.h"
#include "TreeNode.h"

class TopologyTree
{
public:
	TopologyTree(TreeNode * root);
	~TopologyTree();

	void insert(ContourData * theData);
	void setMyRoot(TreeNode * root);
	TreeNode * getMyRoot() const;

private:
	TreeNode * m_myRoot;
};

#endif

