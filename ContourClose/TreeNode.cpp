#include "TreeNode.h"
#include "ContourData.h"

#include <vector>

TreeNode::TreeNode()
	: m_countourData(nullptr)
	, m_myLeftChild(nullptr)
	, m_myRightChild(nullptr)
	, m_myFather(nullptr)
{
}

TreeNode::TreeNode(ContourData * newData, TreeNode * left, TreeNode * right, TreeNode * father)
	: m_countourData(newData)
	, m_myLeftChild(left)
	, m_myRightChild(right)
	, m_myFather(father)
{
}

// 析构函数
TreeNode::~TreeNode()
{
	if (m_myLeftChild != nullptr)
	{
		delete m_myLeftChild;
		m_myLeftChild = nullptr;
	}

	if (m_myRightChild != nullptr)
	{
		delete m_myRightChild;
		m_myRightChild = nullptr;
	}

	if (m_countourData != nullptr)
	{
		delete m_countourData;
		m_countourData = nullptr;
	}
}

// 插入新数据的函数
TreeNode * TreeNode::insert(ContourData *newData)
{
	Topology relation;
	m_countourData->topoDecision(*newData, relation);

	switch (relation)
	{
	case PARALLEL:
		if (m_myRightChild != nullptr)
		{
			m_myRightChild = m_myRightChild->insert(newData);
			return this;
		}
		else
		{
			TreeNode * newNode = new TreeNode(newData, nullptr, nullptr, this);
			m_myRightChild = newNode;
			return this;
		}

	case INCLUSION:
	{
		if (m_myLeftChild != nullptr)
		{
			m_myLeftChild = m_myLeftChild->insert(newData);
			return this;
		}
		else
		{
			TreeNode * newNode = new TreeNode(newData, nullptr, nullptr, this);
			m_myLeftChild = newNode;
			return this;
		}
	}

	case INCLUDED:
	{
		TreeNode * newNode = new TreeNode(newData, this, nullptr, m_myFather);
		std::vector<TreeNode*> nodeVec;
		TreeNode * pCurrent = m_myRightChild;
		while (pCurrent != nullptr)
		{
			nodeVec.push_back(pCurrent);
			pCurrent = pCurrent->m_myRightChild;
		}

		for (int ii = 0; ii < nodeVec.size(); ii++)
		{
			while (pCurrent != nullptr)
			{
				pCurrent = nodeVec.at(ii);

				Topology localRelation;
				ContourData * newNodeData = newNode->m_countourData;
				ContourData * currentData = pCurrent->m_countourData;
				newNodeData->topoDecision(*currentData, localRelation);
				switch (localRelation)
				{
				case PARALLEL:
				{
					TreeNode * pNext = newNode;
					while (pNext->m_myRightChild != nullptr)
					{
						pNext = pNext->m_myRightChild;
					}

					TreeNode * temp_right = pCurrent->m_myRightChild;

					if (temp_right != nullptr)
					{
						temp_right->m_myFather = pCurrent->m_myFather;
						pCurrent->m_myRightChild = nullptr;
					}
					pCurrent->m_myFather->m_myRightChild = temp_right;
					pCurrent->m_myFather = pNext;
					pNext->m_myRightChild = pCurrent;
					break;
				}

				case INCLUSION:
					break;
				default:
					break;
				}
			}
		}

		delete pCurrent;
		pCurrent = nullptr;

		return newNode;
	}
	default:
		return (nullptr);
	}
}

void TreeNode::setCoutourData(ContourData * data)
{
	m_countourData = data;
}

ContourData * TreeNode::getCoutourData() const
{
	return m_countourData;
}

void TreeNode::setMyLeftChild(TreeNode * node)
{
	m_myLeftChild = node;
}

TreeNode * TreeNode::getMyLeftChild() const
{
	return m_myLeftChild;
}

void TreeNode::setMyRightChild(TreeNode * node)
{
	m_myRightChild = node;
}

TreeNode * TreeNode::getMyRightChild() const
{
	return m_myRightChild;
}

void TreeNode::setMyFather(TreeNode * node)
{
	m_myFather = node;
}

TreeNode * TreeNode::getMyFather() const
{
	return m_myFather;
}

