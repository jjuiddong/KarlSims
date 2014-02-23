
#include "stdafx.h"
#include "DiagramController.h"
#include "DiagramNode.h"
#include "../EvolvedVirtualCreatures.h"
#include "RenderBoxActor.h"
#include "RenderSphereActor.h"
#include "../genoype/GenotypeParser.h"
#include "../renderer/RenderModelActor.h"
#include "../renderer/RenderBezierActor.h"



using namespace evc;

CDiagramController::CDiagramController(CEvc &sample) :
	m_Sample(sample)
,	m_pRootDiagram(NULL)
{

}

CDiagramController::~CDiagramController()
{
	set<CDiagramNode*> diags;
	RemoveDiagram(m_pRootDiagram, diags);
	m_pRootDiagram = NULL;

}


/**
 @brief 
 @date 2014-02-12
*/
void CDiagramController::SetGenotype(const genotype_parser::SExpr *expr)
{
	set<CDiagramNode*> diags;
	RemoveDiagram(m_pRootDiagram, diags);
	m_pRootDiagram = NULL;

	m_Diagrams.clear();
	map<const genotype_parser::SExpr*, CDiagramNode*> diagrams;
	m_pRootDiagram = CreateDiagramNode(PxVec3(0,0,0), expr, diagrams);
}


// Render
void CDiagramController::Render()
{
	BOOST_FOREACH (auto node, m_Diagrams)
		node->Render();
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
	diagNode->m_Name = expr? expr->id : "sensor";
	PxVec3 dimension = expr? utility::Vec3toPxVec3(expr->dimension) : PxVec3(1,1,1);
	m_Diagrams.push_back(diagNode);

	if (IsSensorNode)
	{
		diagNode->m_pRenderNode = SAMPLE_NEW2(RenderBoxActor)(*m_Sample.getRenderer(), dimension);
	}
	else if (boost::iequals(expr->shape, "sphere"))
	{
		diagNode->m_pRenderNode = SAMPLE_NEW(RenderSphereActor)(*m_Sample.getRenderer(), dimension.x);
	}
	else
	{ // Sensor
		diagNode->m_pRenderNode = SAMPLE_NEW2(RenderBoxActor)(*m_Sample.getRenderer(), dimension);
	}

	PxVec3 material = expr? utility::Vec3toPxVec3(expr->material) : PxVec3(0,0.75f,0);
	diagNode->m_pRenderNode->setTransform(PxTransform(pos));
	diagNode->m_pRenderNode->setRenderMaterial( m_Sample.GetMaterial(material, false) );
	m_Sample.addRenderObject(diagNode->m_pRenderNode);

	RETV(!expr, diagNode); // if sensor node return

	diagrams[ expr] = diagNode; // insert

	SConnectionList *connection = expr->connection;
	PxVec3 offset(4,0,0);
	while (connection)
	{
		PxVec3 newNodePos = pos + offset;
		SConnection *node_con = connection->connect;
		CDiagramNode *newDiagNode = CreateDiagramNode(newNodePos, node_con->expr, diagrams);

		if (newDiagNode != diagNode)
		{
			SDiagramConnection diagramConnection;
			diagramConnection.connectNode = newDiagNode;

			// transition arrow direction
			PxVec3 dir = newNodePos - pos;
			float len = dir.magnitude();
			dir.normalize();

			PxVec3 interSectPos;
			if (newDiagNode->m_pRenderNode->IntersectTri(pos, dir, interSectPos))
			{
				dir = interSectPos - pos;
				len = dir.magnitude();
				dir.normalize();
				newNodePos = pos + dir*len;
			}

			PxVec3 c = dir.cross(PxVec3(0,0,1));
			vector<PxVec3> points;
			points.push_back( pos );
			points.push_back( pos + (dir*len*0.5f) - c*1.f );
			points.push_back( points[1] );
			points.push_back( newNodePos );

			CRenderBezierActor *arrow = new CRenderBezierActor(*m_Sample.getRenderer(), points);
			diagramConnection.transitionArrow = arrow;
			m_Sample.addRenderObject(diagramConnection.transitionArrow);

			diagNode->m_ConnectDiagrams.push_back(diagramConnection);

			offset += PxVec3(0,3,0);
		}
		else
		{
			SDiagramConnection diagramConnection;
			diagramConnection.connectNode = newDiagNode;

			const float dimensionY = node_con->expr? node_con->expr->dimension.y : 1.f;
			PxVec3 arrowPos = pos + PxVec3(-0.1f,dimensionY+0.2f,0);

			const float offset = 0.7f;
			vector<PxVec3> points;
			points.push_back( arrowPos );
			points.push_back( arrowPos + PxVec3(-offset,offset,0) );
			points.push_back( arrowPos + PxVec3(offset,offset,0) );
			points.push_back( arrowPos );
			CRenderBezierActor *arrow = new CRenderBezierActor(*m_Sample.getRenderer(), points);
			m_Sample.addRenderObject(arrow);	

			diagramConnection.transitionArrow = arrow;
			diagNode->m_ConnectDiagrams.push_back(diagramConnection);
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
