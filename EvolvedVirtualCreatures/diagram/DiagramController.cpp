
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
,	m_linkNode(NULL)
{
	
}

CDiagramController::~CDiagramController()
{
	set<CDiagramNode*> diags;
	RemoveDiagram(m_rootDiagram, diags);
	m_rootDiagram = NULL;

	SAFE_DELETE(m_camera);

	BOOST_FOREACH (auto node, m_linkDiagrams)
		delete node;
	m_linkDiagrams.clear();
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
void CDiagramController::SetControlCreature(const genotype_parser::SExpr *expr)
{
	set<CDiagramNode*> diags;
	RemoveDiagram(m_rootDiagram, diags);
	m_rootDiagram = NULL;

	m_diagrams.clear();
	map<const genotype_parser::SExpr*, CDiagramNode*> diagrams;
	m_rootDiagram = CreateDiagramTree(PxVec3(0,0,0), expr, diagrams);
}


// Render
void CDiagramController::Render()
{
	BOOST_FOREACH (auto node, m_diagrams)
		node->Render();
	BOOST_FOREACH (auto node, m_linkDiagrams)
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
CDiagramNode* CDiagramController::CreateDiagramTree(const PxVec3 &pos, const genotype_parser::SExpr *expr, 
	map<const genotype_parser::SExpr*, CDiagramNode*> &symbols)
{
	using namespace genotype_parser;

	auto it = symbols.find(expr);
	if (symbols.end() != it)
	{ // Already Exist
		return it->second;
	}

	CDiagramNode *diagNode = CreateDiagramNode(expr);
	RETV(!diagNode, NULL);

	diagNode->m_renderNode->setTransform(PxTransform(pos));
	m_diagrams.push_back(diagNode);
	m_sample.addRenderObject(diagNode->m_renderNode);

	RETV(!expr, diagNode); // if sensor node return

	symbols[ expr] = diagNode; // insert

	SConnectionList *connection = expr->connection;
	SConnectionList *currentCopyConnection = NULL;
	PxVec3 offset(4,0,0);
	while (connection)
	{
		PxVec3 newNodePos = pos + offset;
		SConnection *node_con = connection->connect;
		CDiagramNode *newDiagNode = CreateDiagramTree(newNodePos, node_con->expr, symbols);

		//--------------------------------------------------------------------------------
		// copy SConnectionList
		if (!currentCopyConnection)
		{
			currentCopyConnection = new SConnectionList;
			diagNode->m_expr->connection = currentCopyConnection;
		}
		else
		{
			SConnectionList *newCopy = new SConnectionList;
			currentCopyConnection->next = newCopy;
			currentCopyConnection = newCopy;
		}

		currentCopyConnection->connect = new SConnection;
		*currentCopyConnection->connect = *connection->connect;
		currentCopyConnection->connect->expr = newDiagNode->m_expr;
		//--------------------------------------------------------------------------------


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
 @brief create CDiagramNode
 @date 2014-02-25
*/
CDiagramNode* CDiagramController::CreateDiagramNode(const genotype_parser::SExpr *expr)
{
	const bool IsSensorNode = !expr;
	CDiagramNode *node = new CDiagramNode(m_sample);
	node->m_name = expr? expr->id : "sensor";
	if (expr)
	{
		node->m_expr = new genotype_parser::SExpr();
		*node->m_expr = *expr;
	}
	else
	{ // sensor
		node->m_expr = new genotype_parser::SExpr();
		node->m_expr->id = "sensor";
		node->m_expr->dimension = genotype_parser::SVec3(0.4f,0.4f,0.4f);
		node->m_expr->material = genotype_parser::SVec3(0,0.75f,0);
		node->m_expr->connection = NULL;
	}

	PxVec3 dimension = expr? utility::Vec3toPxVec3(expr->dimension) : PxVec3(0.4f,0.4f,0.4f);

	if (IsSensorNode)
	{ // Sensor
		node->m_renderNode = SAMPLE_NEW2(RenderBoxActor)(*m_sample.getRenderer(), dimension);
	}
	else if (boost::iequals(expr->shape, "sphere"))
	{
		node->m_renderNode = SAMPLE_NEW(RenderSphereActor)(*m_sample.getRenderer(), dimension.x);
	}
	else
	{
		node->m_renderNode = SAMPLE_NEW2(RenderBoxActor)(*m_sample.getRenderer(), dimension);
	}

	PxVec3 material = expr? utility::Vec3toPxVec3(expr->material) : PxVec3(0,0.75f,0);
	node->m_renderNode->setRenderMaterial( m_sample.GetMaterial(material, false) );
	node->m_material = material;

	return node;
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


/**
 @brief select node
 @date 2014-02-24
*/
void CDiagramController::SelectNode(CDiagramNode *node)
{
	if (m_selectNode != node)
	{
		CreateLinkNode(node, node? true : false);
		m_selectNode = node;
	}
}


/**
 @brief create link node of argument node
 @date 2014-02-25
*/
void CDiagramController::CreateLinkNode(CDiagramNode *srcNode, const bool isShow) //isShow=true
{
	if (!srcNode)
	{
		BOOST_FOREACH (auto node, m_linkDiagrams)
			node->m_renderNode->setRendering(false);
		return;
	}


	if (m_linkDiagrams.empty())
	{
		PxVec3 material(0,0.75f,0);
		const float shapeSize = 0.3f;

		CDiagramNode *node1 = new CDiagramNode(m_sample);
		node1->m_name = "new Box";
		node1->m_renderNode = SAMPLE_NEW2(RenderBoxActor)(*m_sample.getRenderer(), PxVec3(shapeSize,shapeSize,shapeSize));
		node1->m_renderNode->setRenderMaterial( m_sample.GetMaterial(material, false) );
		node1->m_material = material;

		CDiagramNode *node2 = new CDiagramNode(m_sample);
		node2->m_name = "new Sphere";
		node2->m_renderNode = SAMPLE_NEW(RenderSphereActor)(*m_sample.getRenderer(), shapeSize);
		node2->m_renderNode->setRenderMaterial( m_sample.GetMaterial(material, false) );
		node2->m_material = material;

		CDiagramNode *node3 = new CDiagramNode(m_sample);
		node3->m_name = "new Sensor";
		node3->m_renderNode = SAMPLE_NEW2(RenderBoxActor)(*m_sample.getRenderer(), PxVec3(shapeSize,shapeSize,shapeSize));
		node3->m_renderNode->setRenderMaterial( m_sample.GetMaterial(material, false) );
		node3->m_material = material;

		m_sample.addRenderObject(node1->m_renderNode);
		m_sample.addRenderObject(node2->m_renderNode);
		m_sample.addRenderObject(node3->m_renderNode);

		m_linkDiagrams.push_back(node1);
		m_linkDiagrams.push_back(node2);
		m_linkDiagrams.push_back(node3);
	}


	// create current node
	if (!boost::iequals(m_linkDiagrams.back()->m_name, srcNode->m_name))
	{
		if (m_linkDiagrams.size() > 3)
		{
			SAFE_DELETE(m_linkDiagrams.back());
			m_linkDiagrams.pop_back();
		}

		CDiagramNode *newCurrentNode = CreateDiagramNode(srcNode->m_expr);
		m_sample.addRenderObject(newCurrentNode->m_renderNode);
		m_linkDiagrams.push_back(newCurrentNode);
	}


	const PxVec3 dimension = utility::Vec3toPxVec3(srcNode->m_expr->dimension);
	const float radius = max(dimension.x*2.f +0.5f, 1.4f);
	const PxVec3 pos = srcNode->m_renderNode->getTransform().p;
	float radian = 0;
	BOOST_FOREACH (auto node, m_linkDiagrams)
	{
		PxQuat q(radian, PxVec3(0,0,-1));
		PxVec3 offset = q.rotate(PxVec3(0,radius,0));

		node->m_renderNode->setTransform(PxTransform(pos+offset));
		node->m_renderNode->setRendering(isShow);
		radian += 0.9f;
	}
}
