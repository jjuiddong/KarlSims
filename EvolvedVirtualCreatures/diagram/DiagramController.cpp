
#include "stdafx.h"
#include "DiagramController.h"
#include "DiagramNode.h"
#include "../EvolvedVirtualCreatures.h"
#include "RenderBoxActor.h"
#include "RenderSphereActor.h"
#include "../genoype/GenotypeParser.h"


using namespace evc;

CDiagramController::CDiagramController(CEvc &sample) :
	m_Sample(sample)
,	m_pRootDiagram(NULL)
{

}

CDiagramController::~CDiagramController()
{
	RemoveDiagram(m_pRootDiagram);

}


/**
 @brief 
 @date 2014-02-12
*/
void CDiagramController::SetGenotype(const genotype_parser::SExpr *expr)
{
	RemoveDiagram(m_pRootDiagram);

	map<const genotype_parser::SExpr*, CDiagramNode*> diagrams;
	m_pRootDiagram = CreateDiagramNode(PxVec3(0,0,0), expr, diagrams);
}


/**
 @brief 
 @date 2014-02-12
*/
void CDiagramController::Move(float dtime)
{

}


/**
 @brief caculate layout diagram poisition
 @date 2014-02-12
*/
void CDiagramController::Layout(CDiagramNode *root)
{

}


/**
 @brief create diagram node
 @date 2014-02-12
*/
CDiagramNode* CDiagramController::CreateDiagramNode(const PxVec3 &pos, const genotype_parser::SExpr *expr, 
	map<const genotype_parser::SExpr*, CDiagramNode*> &diagrams)
{
	using namespace genotype_parser;

	auto it = diagrams.find(expr);
	if (diagrams.end() != it)
	{ // Already Exist
		return it->second;
	}

	const bool IsSensorNode = !expr;
	CDiagramNode *diagNode = new CDiagramNode(m_Sample);

	if (IsSensorNode)
	{
		diagNode->m_pRenderNode = SAMPLE_NEW2(RenderBoxActor)(*m_Sample.getRenderer(), PxVec3(1,1,1));
	}
	else if (boost::iequals(expr->shape, "sphere"))
	{
		diagNode->m_pRenderNode = SAMPLE_NEW(RenderSphereActor)(*m_Sample.getRenderer(), 1.f);
	}
	else
	{
		diagNode->m_pRenderNode = SAMPLE_NEW2(RenderBoxActor)(*m_Sample.getRenderer(), PxVec3(1,1,1));
	}

	diagNode->m_pRenderNode->setTransform(PxTransform(pos));
	m_Sample.addRenderObject(diagNode->m_pRenderNode);

	RETV(!expr, NULL); // if sensor node return

	diagrams[ expr] = diagNode; // insert

	SConnectionList *connection = expr->connection;
	PxVec3 offset(3,0,0);
	while (connection)
	{
		SConnection *node_con = connection->connect;
		CDiagramNode *newNode = CreateDiagramNode(pos+offset, node_con->expr, diagrams);
		diagNode->m_ConnectNodes.push_back(newNode);

		offset += PxVec3(0,3,0);
		connection = connection->next;
	}

	return diagNode;
}


/**
 @brief 
 @date 2014-02-12
*/
void CDiagramController::RemoveDiagram(CDiagramNode *node, set<CDiagramNode*> &diagrams)
{
	RET(!node);
	if (diagrams.end() != diagrams.find(node))
		return; // already removed

	diagrams.insert(node);

	BOOST_FOREACH (auto conNode, node->m_ConnectNodes)
	{
		RemoveDiagram(conNode, diagrams);
	}

	SAFE_DELETE(node);
}
