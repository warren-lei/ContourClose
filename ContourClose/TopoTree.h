/***********************************************************
���ã�����һ�����˰���������, ��TopologyTree������ͷ�ļ�
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

