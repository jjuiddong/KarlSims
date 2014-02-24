
#include "stdafx.h"
#include "DiagramController.h"
#include "DiagramNode.h"
#include "../EvolvedVirtualCreatures.h"
#include "../genoype/GenotypeParser.h"
#include "../renderer/RenderModelActor.h"
#include "../renderer/RenderBezierActor.h"
#include "SimpleCamera.h"



using namespace evc;

CDiagramController::CDiagramController(CEvc &sample) :
	m_sample(sample)
,	m_rootDiagram(NULL)
,	m_camera(NULL)
,	m_selectNode(NULL)
{
	
}

CDiagramController::~CDiagramController()
{
	set<CDiagramNode*> diags;
	RemoveDiagram(m_rootDiagram, diags);
	m_rootDiagram = NULL;
	SAFE_DELETE(m_camera);
}


/**
 @brief Init Diagram Controller Scene
 @date 2014-02-24
*/
void CDiagramController::ControllerSceneInit()
{
	if (!m_camera)
		m_camera = SAMPLE_NEW(CSimpleCamera)();

	m_sample.getApplication().setMouseCursorHiding(false);
	m_sample.getApplication().setMouseCursorRecentering(false);

	m_camera->init(PxVec3(0.0f, 3.0f, 10.0f), PxVec3(0.f, 0, 0.0f));
	m_camera->setMouseSensitivity(0.5f);

	m_sample.getApplication().setCameraController(m_camera);
	m_sample.setCameraController(m_camera);

}


/**
 @brief 
 @date 2014-02-12
*/
void CDiagramController::SetGenotype(const genotype_parser::SExpr *expr)
{
	set<CDiagramNode*> diags;
	RemoveDiagram(m_rootDiagram, diags);
	m_rootDiagram = NULL;

	m_diagrams.clear();
	map<const genotype_parser::SExpr*, CDiagramNode*> diagrams;
	m_rootDiagram = CreateDiagramNode(PxVec3(0,0,0), expr, diagrams);
}


// Render
void CDiagramController::Render()
{
	BOOST_FOREACH (auto node, m_diagrams)
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
	CDiagramNode *diagNode = new CDiagramNode(m_sample);
	diagNode->m_name = expr? expr->id : "sensor";
	PxVec3 dimension = expr? utility::Vec3toPxVec3(expr->dimension) : PxVec3(0.4f,0.4f,0.4f);
	m_diagrams.push_back(diagNode);

	if (IsSensorNode)
	{
		diagNode->m_renderNode = SAMPLE_NEW2(RenderBoxActor)(*m_sample.getRenderer(), dimension);
	}
	else if (boost::iequals(expr->shape, "sphere"))
	{
		diagNode->m_renderNode = SAMPLE_NEW(RenderSphereActor)(*m_sample.getRenderer(), dimension.x);
	}
	else
	{ // Sensor
		diagNode->m_renderNode = SAMPLE_NEW2(RenderBoxActor)(*m_sample.getRenderer(), dimension);
	}

	PxVec3 material = expr? utility::Vec3toPxVec3(expr->material) : PxVec3(0,0.75f,0);
	diagNode->m_renderNode->setTransform(PxTransform(pos));
	diagNode->m_renderNode->setRenderMaterial( m_sample.GetMaterial(material, false) );
	diagNode->m_material = material;
	m_sample.addRenderObject(diagNode->m_renderNode);

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
			if (newDiagNode->m_renderNode->IntersectTri(pos, dir, interSectPos))
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

			CRenderBezierActor *arrow = new CRenderBezierActor(*m_sample.getRenderer(), points);
			diagramConnection.transitionArrow = arrow;
			m_sample.addRenderObject(diagramConnection.transitionArrow);

			diagNode->m_connectDiagrams.push_back(diagramConnection);

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
			CRenderBezierActor *arrow = new CRenderBezierActor(*m_sample.getRenderer(), points);
			m_sample.addRenderObject(arrow);	

			diagramConnection.transitionArrow = arrow;
			diagNode->m_connectDiagrams.push_back(diagramConnection);
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

	BOOST_FOREACH (auto conNode, node->m_connectDiagrams)
	{
		m_sample.removeRenderObject(conNode.transitionArrow);
		RemoveDiagram(conNode.connectNode, diagrams);
	}
	node->m_connectDiagrams.clear();

	SAFE_DELETE(node);
}


/**
 @brief 
 @date 2014-02-24
*/
void CDiagramController::SelectNode(CDiagramNode *node)
{
	if (m_selectNode != node)
	{
		m_selectNode = node;
	}
}


/**
 @brief Mouse Event
 @date 2014-02-24
*/
void CDiagramController::onPointerInputEvent(const SampleFramework::InputEvent&ie, 
	physx::PxU32 x, physx::PxU32 y, physx::PxReal dx, physx::PxReal dy, bool val)
{
	PxVec3 orig, dir, pickOrig;
	m_sample.GetPicking()->computeCameraRay(orig, dir, pickOrig, x, y);

	evc::CDiagramNode *mouseOverNode = NULL;
	BOOST_FOREACH(auto node, m_diagrams)
	{
		PxVec3 out;
		const bool isHighLight = node->m_renderNode->IntersectTri(pickOrig, dir, out);
		node->SetHighLight(isHighLight);
		if (isHighLight)
			mouseOverNode = node;
	}

	if ((GetKeyState(VK_LBUTTON) & 0x80) != 0)
	{
		SelectNode(mouseOverNode);
	}
	if ((GetKeyState(VK_RBUTTON) & 0x80) != 0)
	{
		SelectNode(mouseOverNode);
	}
}


/**
 @brief 
 @date 2014-02-24
*/
void CDiagramController::onAnalogInputEvent(const SampleFramework::InputEvent&ie , float val)
{

}


/**
 @brief 
 @date 2014-02-24
*/
void CDiagramController::onDigitalInputEvent(const SampleFramework::InputEvent&ie , bool val)
{

}
