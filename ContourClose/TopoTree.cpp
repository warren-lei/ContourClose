/***********************************************************
���ã�����һ�����˰���������, ��TopologyTree��ʵ���ļ�
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

// �����µ�ֵ�������ݵĺ���
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