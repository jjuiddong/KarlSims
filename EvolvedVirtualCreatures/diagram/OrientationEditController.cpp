
#include "stdafx.h"
#include "OrientationEditController.h"
#include "../EvolvedVirtualCreatures.h"
#include "GenotypeController.h"
#include "GenotypeNode.h"
#include "DiagramUtility.h"


using namespace evc;

COrientationEditController::COrientationEditController(CEvc &sample, CGenotypeController &diagramController) :
	m_sample(sample)
,	m_genotypeController(diagramController)
,	m_camera(NULL)
,	m_selectNode(NULL)
{

}

COrientationEditController::~COrientationEditController()
{
	SAFE_DELETE(m_camera);

}


/**
 @brief Scene Init
 @date 2014-03-02
*/
void COrientationEditController::ControllerSceneInit()
{
	if (!m_camera)
		m_camera = SAMPLE_NEW(DefaultCameraController)();

	m_sample.getApplication().setMouseCursorHiding(true);
	m_sample.getApplication().setMouseCursorRecentering(true);

	m_camera->init(PxVec3(0.0f, 3.0f, 10.0f), PxVec3(0.f, 0, 0.0f));
	m_camera->setMouseSensitivity(0.5f);

	m_sample.getApplication().setCameraController(m_camera);
	m_sample.setCameraController(m_camera);
}


/**
 @brief setting control diagramnode
 @date 2014-03-02
*/
void COrientationEditController::SetControlDiagram(CGenotypeNode *node)
{
	m_selectNode = node;

	// setting camera
	vector<CGenotypeNode*> nodes;
	m_genotypeController.GetDiagramsLinkto(node, nodes);
	if (nodes.empty())
		return;

	CGenotypeNode *parentNode = nodes[ 0];
	const PxVec3 parentPos = parentNode->m_renderNode->getTransform().p;
	PxVec3 dir = parentPos - node->m_renderNode->getTransform().p;
	dir.normalize();

	PxVec3 left = PxVec3(0,1,0).cross(dir);
	left.normalize();
	PxVec3 camPos = node->m_renderNode->getTransform().p + (left*3.f) + PxVec3(0,2.5f,0);
	PxVec3 camDir = parentPos - camPos;
	camDir.normalize();
	camPos -= (camDir * .5f);

	m_sample.getCamera().lookAt(camPos, parentPos);
	const PxTransform viewTm = m_sample.getCamera().getViewMatrix();
	m_camera->init(viewTm);

	map<const genotype_parser::SExpr*, CGenotypeNode*> symbols;
	m_rootDiagram = CreatePhenotypeDiagram( PxVec3(0,0,0), parentNode->m_expr, symbols);
}


/**
 @brief 
 @date 2014-03-02
*/
void COrientationEditController::MouseLButtonDown(physx::PxU32 x, physx::PxU32 y)
{

}


void COrientationEditController::MouseLButtonUp(physx::PxU32 x, physx::PxU32 y)
{

}


void COrientationEditController::MouseRButtonDown(physx::PxU32 x, physx::PxU32 y)
{

}


void COrientationEditController::MouseRButtonUp(physx::PxU32 x, physx::PxU32 y)
{

}


void COrientationEditController::MouseMove(physx::PxU32 x, physx::PxU32 y)
{

}


/**
 @brief create phenotype diagram
 @date 2014-03-03
*/
CGenotypeNode* COrientationEditController::CreatePhenotypeDiagram(const PxVec3 &pos, genotype_parser::SExpr *expr,
	map<const genotype_parser::SExpr*, CGenotypeNode*> &symbols)
{
	using namespace genotype_parser;

	auto it = symbols.find(expr);
	if (symbols.end() != it)
	{ // Already Exist
		return it->second;
	}

	CGenotypeNode *diagNode = CreateGenotypeNode(m_sample, expr);
	RETV(!diagNode, NULL);

	diagNode->m_renderNode->setTransform(PxTransform(pos));
	m_diagrams.push_back(diagNode);
	m_sample.addRenderObject(diagNode->m_renderNode);

	symbols[ expr] = diagNode; // insert

	SConnectionList *connection = expr->connection;
	SConnectionList *currentCopyConnection = NULL;
	int childIndex = 0;
	while (connection)
	{
		u_int order=0;
		PxVec3 newNodePos ;//= GetDiagramPositionByIndex(expr, diagNode->m_renderNode->getTransform().p, childIndex, order);
		SConnection *node_con = connection->connect;
		CGenotypeNode *newDiagNode = CreatePhenotypeDiagram(newNodePos, node_con->expr, symbols);

		childIndex++;
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

			RenderBezierActor *arrow = CreateTransition(m_sample, diagNode, newDiagNode, order);
			diagramConnection.transitionArrow = arrow;
			m_sample.addRenderObject(arrow);

			diagNode->m_connectDiagrams.push_back(diagramConnection);
		}
		else
		{
			SDiagramConnection diagramConnection;
			diagramConnection.connectNode = newDiagNode;

			RenderBezierActor *arrow = CreateTransition(m_sample, diagNode, newDiagNode, order);
			diagramConnection.transitionArrow = arrow;
			m_sample.addRenderObject(arrow);	

			diagNode->m_connectDiagrams.push_back(diagramConnection);
		}

		connection = connection->next;
	}

	return diagNode;
}
