
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
	m_pRootDiagram = NULL;

}


/**
 @brief 
 @date 2014-02-12
*/
void CDiagramController::SetGenotype(const genotype_parser::SExpr *expr)
{
	RemoveDiagram(m_pRootDiagram);
	m_pRootDiagram = NULL;

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
	diagNode->m_pRenderNode->setRenderMaterial( m_Sample.GetMaterial(PxVec3(0, 0.75f, 0), false) );
	m_Sample.addRenderObject(diagNode->m_pRenderNode);

	RETV(!expr, diagNode); // if sensor node return

	diagrams[ expr] = diagNode; // insert

	SConnectionList *connection = expr->connection;
	PxVec3 offset(4,0,0);
	while (connection)
	{
		const PxVec3 newNodePos = pos + offset;
		SConnection *node_con = connection->connect;
		CDiagramNode *newDiagNode = CreateDiagramNode(newNodePos, node_con->expr, diagrams);

		if (newDiagNode != diagNode)
		{
			SDiagramConnection diagramConnection;
			diagramConnection.connectNode = newDiagNode;

			// transition arrow direction
			PxVec3 arrowPos = (pos + newNodePos) / 2.f;
			PxVec3 dir = newNodePos - pos;
			const float len = dir.magnitude();
			dir.normalize();

			PxTransform arrowTm(arrowPos);
			PxQuat arrowDir;
			quatRotationArc(arrowDir, dir, PxVec3(0,1,0));
			arrowTm = arrowTm * PxTransform(arrowDir);
			//

			diagramConnection.transitionArrow = SAMPLE_NEW2(RenderBoxActor)(*m_Sample.getRenderer(), PxVec3(0.05f, len/2.f, 0.05f));
			diagramConnection.transitionArrow->setTransform( arrowTm );
			diagramConnection.transitionArrow->setRenderMaterial( m_Sample.GetMaterial(PxVec3(0.f, 0.f, 0.f), false) );
			m_Sample.addRenderObject(diagramConnection.transitionArrow);

			diagNode->m_ConnectDiagrams.push_back(diagramConnection);

			offset += PxVec3(0,3,0);
		}

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

	BOOST_FOREACH (auto conNode, node->m_ConnectDiagrams)
	{
		m_Sample.removeRenderObject(conNode.transitionArrow);
		RemoveDiagram(conNode.connectNode, diagrams);
	}
	node->m_ConnectDiagrams.clear();

	SAFE_DELETE(node);
}
