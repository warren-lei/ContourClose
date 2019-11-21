#include "BuilBinaryTree.h"
#include "readdata.h"
#include "contourdata.h"
#include "topotree.h"
#include "treenode.h"

// ���캯��
BuildBinTopoTree::BuildBinTopoTree(std::vector<std::pair<double, QPolygonF> > &iAllPolygonData)
	:m_allPolygonData(iAllPolygonData)
{
	//m_iallContourData = iallContourData;
	//m_allPolygonData = iAllPolygonData;
	m_topologyBinaryTree = nullptr;
	//m_minX = minX;
	//m_minY = minY;
	//m_maxX = maxX;
	//m_maxY = maxY;
}

BuildBinTopoTree::~BuildBinTopoTree()
{
	if (m_topologyBinaryTree != nullptr)
	{
		delete m_topologyBinaryTree;
		m_topologyBinaryTree = nullptr;
	}

	if (!m_preorderContourVec.empty())
	{
		m_preorderContourVec.clear();
	}
}

// ���˶����������ĺ���
void BuildBinTopoTree::makeTopologyBinaryTree()
{

	if (m_topologyBinaryTree != nullptr)
	{
		delete m_topologyBinaryTree;
		m_topologyBinaryTree = nullptr;
	}

	//**************************************
	// ��������ʼ�����ڵ�
	//**************************************
	int contour_id = 1;

	ReadData *readFileData = new ReadData();
	readFileData->readContourData();
	//readFileData->GetAllContourData();
	double m_minX = readFileData->getMinX();
	double m_maxX = readFileData->getMaxX();
	double m_minY = readFileData->getMinY();
	double m_maxY = readFileData->getMaxY();
	delete readFileData;

	double root_x[] = { m_maxX, m_minX, m_minX, m_maxX, m_maxX };
	double root_y[] = { m_minY, m_minY, m_maxY, m_maxY, m_minY };
	QPolygonF root_array_data;
	for (int i = 0; i < 5; i++)
	{
		root_array_data.append(QPointF(root_x[i], root_y[i]));
	}

	const int root_node_size = root_array_data.size();
	int root_id = 0;
	double root_value = 0;
	//ContourDataArray * root_array_data = new ContourDataArray( root_y, root_x, root_node_size);

	ContourData * root_data = new ContourData(root_id, root_value, root_array_data);
	TreeNode *root = new TreeNode(root_data, nullptr, nullptr, nullptr);
	m_topologyBinaryTree = new TopologyTree(root);
	//�����ж�
	if (0 == m_allPolygonData.size())
	{
		return;
	}

	//�������е�ֵ������,������������
	for (int i = 0; i < m_allPolygonData.size(); i++)
	{
		double contourValue = m_allPolygonData[i].first;
		//ContourData * treenodedata = new ContourData(contour_id, contourValue, m_iallContourData[i]);
		ContourData * treenodedata = new ContourData(contour_id, contourValue, m_allPolygonData[i].second);
		m_topologyBinaryTree->insert(treenodedata);
		++contour_id;
	}
}

//�������������
void BuildBinTopoTree::preoderTopologyBinaryTree(TreeNode *node)
{
	if (node != nullptr)
	{
		m_preorderContourVec.push_back(node);
		preoderTopologyBinaryTree(node->getMyLeftChild());
		preoderTopologyBinaryTree(node->getMyRightChild());
	}
}

//// ��ȡ�ȸ������õĶ������ĺ���
//void BuildBinTopoTree::make_preorder_contour_list()
//{
//	if(!preorder_contour_list.empty())
//	{
//		make_topo_binary_tree();
//		if( topo_binary_tree != nullptr )
//		{
//			preorder_topo_binary_tree(topo_binary_tree->GetMyRoot());
//		}
//	}
//}

//��ȡ�ȸ������õĶ�����
std::vector<TreeNode *> BuildBinTopoTree::getPreorderContourTree() const
{
	return m_preorderContourVec;
}
