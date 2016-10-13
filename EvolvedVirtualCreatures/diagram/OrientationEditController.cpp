
#include "stdafx.h"
#include "OrientationEditController.h"
#include "../EvolvedVirtualCreatures.h"
#include "GenotypeController.h"
#include "GenotypeNode.h"
#include "DiagramUtility.h"
#include "../creature/Creature.h"
#include "SimpleCamera.h"
#include "ScaleCursor.h"


using namespace evc;

COrientationEditController::COrientationEditController(CEvc &sample, CGenotypeController &genotypeController) :
	m_sample(sample)
,	m_genotypeController(genotypeController)
,	m_camera(NULL)
,	m_selectNode(NULL)
,	m_rootNode(NULL)
,	m_editMode(MODE_NONE)
,	m_cursor(NULL)
{
	
}

COrientationEditController::~COrientationEditController()
{
	RemoveGenotypeTree(m_sample, m_rootNode);
	m_nodes.clear();
	m_rootNode = NULL;

	SAFE_DELETE(m_camera);

	BOOST_FOREACH (auto cursor, m_allCursor)
		delete cursor;
	m_allCursor.clear();
}


/**
 @brief Scene Init
 @date 2014-03-02
*/
void COrientationEditController::ControllerSceneInit()
{
	if (!m_camera)
		m_camera = SAMPLE_NEW(CSimpleCamera)();

	if (m_allCursor.empty())
	{
		m_allCursor.push_back( new CScaleCursor(m_sample) );
	}

	m_sample.getApplication().setMouseCursorHiding(false);
	m_sample.getApplication().setMouseCursorRecentering(false);

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
	RemoveGenotypeTree(m_sample, m_rootNode);
	m_nodes.clear();
	m_rootNode = NULL;

	vector<CGenotypeNode*> nodes;
	m_genotypeController.GetDiagramsLinkto(node, nodes);
	if (nodes.empty())
		return;

	// Create Phenotype Node
	CGenotypeNode *parentNode = nodes[ 0];
	map<const genotype_parser::SExpr*, CGenotypeNode*> symbols;
	m_rootNode = CreatePhenotypeDiagram(PxVec3(0,0,0), PxQuat(0,PxVec3(1,0,0)), parentNode->m_expr, symbols);
	m_rootNode->UpdateTransform();

	// Camera Setting
	const PxVec3 parentPos = parentNode->GetTransform().p;
	PxVec3 dir = parentPos - node->GetTransform().p;
	dir.normalize();

	PxVec3 left = PxVec3(0,1,0).cross(dir);
	left.normalize();
	PxVec3 camPos = node->GetTransform().p + (left*3.f) + PxVec3(0,2.5f,0);
	PxVec3 camDir = parentPos - camPos;
	camDir.normalize();
	camPos -= (camDir * .5f);

	m_sample.getCamera().lookAt(camPos, parentPos);
	const PxTransform viewTm = m_sample.getCamera().getViewMatrix();
	m_camera->init(viewTm);

	// select Orientation Control node
	if (CGenotypeNode *selectNode = m_rootNode->GetConnectNode(node->m_name))
	{
		selectNode->SetHighLight(true);
		//SelectNode(selectNode);
		SelectNode(NULL);
	}
}


/**
 @brief MouseLButtonDown
 @date 2014-03-02
*/
void COrientationEditController::MouseLButtonDown(physx::PxU32 x, physx::PxU32 y)
{
	switch (m_editMode)
	{
	case MODE_NONE:
	case MODE_POSITION:
		break;

	case MODE_PICKUP:
		if (SelectNode(PickupNodes(m_sample, m_nodes, x, y, true), MODE_ROTATION))
		{
			m_ptOrig = Int2(x,y);
		}
		break;

	case MODE_SCALE:
		if (!m_cursor->MouseLButtonDown(x,y))
		{
			if (SelectNode(PickupNodes(m_sample, m_nodes, x, y, true), MODE_SCALE))
			{
				m_ptOrig = Int2(x,y);
			}
		}
		break;
	}

}


/**
 @brief MouseLButtonUp
 @date 2014-03-04
*/
void COrientationEditController::MouseLButtonUp(physx::PxU32 x, physx::PxU32 y)
{
	switch (m_editMode)
	{
	case MODE_NONE:
	case MODE_POSITION:
	case MODE_PICKUP:
		break;

	case MODE_ROTATION:
		{
			RET(!m_selectNode);

			using namespace genotype_parser;
			SConnection *joint = m_rootNode->GetJoint(m_selectNode);
			RET(!joint);

			PxReal angle;
			PxVec3 axis;
			m_selectNode->GetLocalRotate().toRadiansAndUnitAxis(angle, axis);
			joint->rotate.angle = angle;
			joint->rotate.dir = utility::PxVec3toVec3(axis);

			SelectNode(NULL);
		}
		break;
	}

	if (m_cursor)
		m_cursor->MouseLButtonUp(x,y);
}


/**
 @brief MouseRButtonDown
 @date 2014-03-04
*/
void COrientationEditController::MouseRButtonDown(physx::PxU32 x, physx::PxU32 y)
{
	switch (m_editMode)
	{
	case MODE_NONE:
	case MODE_POSITION:
		break;

	case MODE_PICKUP:
		SelectNode(PickupNodes(m_sample, m_nodes, x, y, true));
		break;
	}

	if (m_cursor)
		m_cursor->MouseRButtonDown(x,y);
}


/**
 @brief MouseRButtonUp
 @date 2014-03-04
*/
void COrientationEditController::MouseRButtonUp(physx::PxU32 x, physx::PxU32 y)
{
	switch (m_editMode)
	{
	case MODE_POSITION:
		{
			RET(!m_selectNode);

			using namespace genotype_parser;

			SConnection *joint = m_rootNode->GetJoint(m_selectNode);
			RET(!joint);

			PxVec3 dir = m_selectNode->GetPos() - m_rootNode->GetPos();
			const float len = dir.magnitude();
			dir.normalize();

			PxVec3 jointAxis = PxVec3(0,0,1).cross(dir);
			jointAxis.normalize();

			joint->jointAxis = utility::PxVec3toVec3(jointAxis);
			joint->orient = utility::PxVec3toVec3(dir*len);
			m_selectNode->SetLocalPosition(dir*len);
			m_rootNode->UpdateTransform();

			SelectNode(NULL);
		}
		break;
	}

	if (m_cursor)
		m_cursor->MouseRButtonUp(x,y);
}


/**
 @brief Mouse Move
 @date 2014-03-04
*/
void COrientationEditController::MouseMove(physx::PxU32 x, physx::PxU32 y)
{
	switch (m_editMode)
	{
	case MODE_NONE: 
		break;

	case MODE_POSITION:
		{
			RET(!m_selectNode);

			using namespace genotype_parser;
			const SConnection *joint = m_rootNode->GetJoint(m_selectNode);
			BRK(!joint);
	
			const PxVec3 initialPos = utility::Vec3toPxVec3(joint->orient);
			const PxVec3 dimension = utility::Vec3toPxVec3(m_selectNode->m_expr->dimension);
			const float len = initialPos.magnitude() + dimension.magnitude();

			PxVec3 orig, dir, pickOrig;
			m_sample.GetPicking()->computeCameraRay(orig, dir, pickOrig, x, y);
			PxVec3 pos = pickOrig + (dir*3);
			 
			PxVec3 v = pos - m_rootNode->GetPos();
			if (v.magnitude() > len)
			{
				v.normalize();
				pos = m_rootNode->GetPos() + (v*len);
			}

			m_selectNode->SetTransform(PxTransform(pos) * PxTransform(m_selectNode->GetLocalRotate()));
		}
		break;

	case MODE_PICKUP:
		PickupNodes(m_sample, m_nodes, x, y, true);
		break;

	case MODE_ROTATION:
		{
			PxVec3 v0;
			const Int2 pos(x,y);
			if (pos == m_ptOrig)
				break;

			{
				PxVec3 orig0, dir0, pickOrig0;
				m_sample.GetPicking()->computeCameraRay(orig0, dir0, pickOrig0, x, y);
				PxVec3 orig1, dir1, pickOrig1;
				m_sample.GetPicking()->computeCameraRay(orig1, dir1, pickOrig1, m_ptOrig.x, m_ptOrig.y);

				PxVec3 v1 = pickOrig0 - pickOrig1;
				v1.normalize();

				v0 = dir0.cross(v1);
				v0.normalize();
			}
			m_ptOrig = pos;

			const PxQuat rotate = m_selectNode->GetLocalRotate() * PxQuat(-0.03f, v0);
			m_selectNode->SetLocalRotate(rotate);
			m_rootNode->UpdateTransform();
		}
		break;
	}

	if (m_cursor)
		m_cursor->MouseMove(x,y);
}


/**
 @brief create phenotype diagram
 @date 2014-03-03
*/
CGenotypeNode* COrientationEditController::CreatePhenotypeDiagram( const PxVec3 &localPos, const PxQuat &localRotate,
	genotype_parser::SExpr *expr, map<const genotype_parser::SExpr*, CGenotypeNode*> &symbols)
{
	using namespace genotype_parser;

	auto it = symbols.find(expr);
	if (symbols.end() != it)
	{ // Already Exist
		return it->second;
	}

	CGenotypeNode *diagNode = CreateGenotypeNode(m_sample, expr);
	RETV(!diagNode, NULL);

	diagNode->SetLocalPosition(localPos);
	diagNode->SetLocalRotate(localRotate);

	m_nodes.push_back(diagNode);
	m_sample.addRenderObject(diagNode->m_renderNode);

	symbols[ expr] = diagNode; // insert

	SConnectionList *connection = expr->connection;
	SConnectionList *currentCopyConnection = NULL;
	int childIndex = 0;
	while (connection)
	{
		u_int order=0;
		SConnection *node_con = connection->connect;
		const PxVec3 nodeLocalPos = utility::Vec3toPxVec3(node_con->orient);
		const PxQuat nodeLocalRotate = PxQuat(node_con->rotate.angle, utility::Vec3toPxVec3(node_con->rotate.dir));
		CGenotypeNode *newDiagNode = CreatePhenotypeDiagram(nodeLocalPos, nodeLocalRotate, node_con->expr, symbols);

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
			//RenderBezierActor *arrow = CreateTransition(m_sample, diagNode, newDiagNode, PxVec3(0,0,0), order);
			//diagramConnection.transitionArrow = arrow;
			//m_sample.addRenderObject(arrow);

			diagNode->m_connectDiagrams.push_back(diagramConnection);
		}
		else
		{
			SDiagramConnection diagramConnection;
			diagramConnection.connectNode = newDiagNode;

			//RenderBezierActor *arrow = CreateTransition(m_sample, diagNode, newDiagNode, PxVec3(0,0,0), order);
			//diagramConnection.transitionArrow = arrow;
			//m_sample.addRenderObject(arrow);	

			diagNode->m_connectDiagrams.push_back(diagramConnection);
		}

		connection = connection->next;
	}

	return diagNode;
}


/**
 @brief ChangeEditMode
 @date 2014-03-04
*/
void COrientationEditController::ChangeEditMode(EDIT_MODE mode)
{
	m_editMode = mode;

	switch (mode)
	{
	case MODE_NONE:
		{
			using namespace genotype_parser;
			if (CGenotypeNode *rootNode = m_genotypeController.GetRootDiagram())
			{
				genotype_parser::SExpr *expr = FindGenotype(rootNode->m_expr, m_rootNode->m_expr->id);
				RET(!expr);
				AssignGenotype(expr, m_rootNode->m_expr);
				m_genotypeController.UpdateCreature();
			}

			m_genotypeController.ControllerSceneInit();
			if (m_cursor)
				m_cursor->SelectNode(NULL);
			m_cursor = NULL;
		}
		break;

	case MODE_SCALE:
		{
			m_cursor = m_allCursor[ CURSOR_SCALE];
			m_cursor->SelectNode(NULL);
		}
		break;

	case MODE_PICKUP:
	case MODE_POSITION:
		m_cursor = NULL;
		break;
	}
}


/**
 @brief Select Node
 @date 2014-03-06
*/
bool COrientationEditController::SelectNode(CGenotypeNode *node, const EDIT_MODE mode)
	// mode = MODE_POSITION
{
	m_selectNode = node;

	if (!node && m_cursor)
		m_cursor->SelectNode(NULL);

	if (node)
		ChangeEditMode(mode);
	else
		ChangeEditMode(MODE_PICKUP);

	// call after changeEditMode 
	if (m_cursor)
		m_cursor->SelectNode(node);

	return (node? true : false);
}


/**
 @brief Digital Input Event Handler
 @date 2014-02-27
*/
void COrientationEditController::onDigitalInputEvent(const SampleFramework::InputEvent &ie, bool val)
{
	switch (ie.m_Id)
	{
	case GOTO_GENOTYPE_CONTROLLER:
		ChangeEditMode(MODE_NONE);
		break;
	
	case SCALE_EDIT_MODE:
		ChangeEditMode(MODE_SCALE);
		break;

	case NEXT_OBJECT:
		break;
	}

	if (m_cursor)
		m_cursor->onDigitalInputEvent(ie,val);
}
