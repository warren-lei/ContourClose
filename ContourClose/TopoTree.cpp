/***********************************************************
作用：描述一颗拓扑包含二叉树, 类TopologyTree的实现文件
************************************************************/
#include "TopoTree.h"

TopologyTree::TopologyTree(TreeNode * root) : m_myRoot(root)
{
}

TopologyTree::~TopologyTree()
{
	delete m_myRoot;
	m_myRoot = nullptr;
}

// 插入新等值曲线数据的函数
void TopologyTree::insert(ContourData * theData)
{
	m_myRoot->insert(theData);
}

void TopologyTree::setMyRoot(TreeNode * root)
{
	m_myRoot = root;
}

TreeNode * TopologyTree::getMyRoot() const
{
	return m_myRoot;
}