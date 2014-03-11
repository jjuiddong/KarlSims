
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
	const PxTransform identTm = PxTransform::createIdentity();
	m_rootNode = CreatePhenotypeDiagram(identTm,identTm,identTm, parentNode->m_expr, symbols);

	// Camera Setting
	const PxVec3 parentPos = parentNode->GetWorldTransform().p;
	PxVec3 dir = parentPos - node->GetWorldTransform().p;
	dir.normalize();

	PxVec3 left = PxVec3(0,1,0).cross(dir);
	left.normalize();
	PxVec3 camPos = node->GetWorldTransform().p + (left*3.f) + PxVec3(0,2.5f,0);
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
		SelectNode(selectNode);
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

			PxTransform tm = m_selectNode->GetWorldTransform() * m_selectNode->GetLocalTransform();
			PxReal angle;
			PxVec3 axis;
			tm.q.toRadiansAndUnitAxis(angle, axis);

			joint->orient.angle = angle;
			joint->orient.dir = utility::PxVec3toVec3(axis);

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

			PxQuat rotateToXAxis;
			if (boost::iequals(joint->type, "revolute"))
			{
				PxVec3 jointAxis = PxVec3(0,0,1).cross(dir);
				jointAxis.normalize();
				utility::quatRotationArc(rotateToXAxis, jointAxis, PxVec3(1,0,0));
			}
			else
			{
				rotateToXAxis = m_rootNode->GetWorldTransform().q;
			}

			{ // setting parent orientation
				PxReal angle;
				PxVec3 axis;
				rotateToXAxis.toRadiansAndUnitAxis(angle, axis);
				joint->parentOrient.angle = angle;
				joint->parentOrient.dir = utility::PxVec3toVec3(axis);

				printf( "parent angle = %f, dir= %f %f %f\n", angle, axis.x, axis.y, axis.z);
			}
			
			{ // setting select orientation
				PxVec3 pos;
				PxTransform rotTm;
				const PxTransform tm = m_selectNode->GetWorldTransform();
				if (boost::iequals(joint->type, "revolute"))
				{
					rotTm = PxTransform(rotateToXAxis) * PxTransform(tm.q);
					pos = PxVec3(0,len,0);
				}
				else
				{
					rotTm = PxTransform(tm.q);
					pos = tm.p;
				}

				PxReal angle;
				PxVec3 axis;
				rotTm.q.toRadiansAndUnitAxis(angle, axis);
				joint->orient.angle = angle;
				joint->orient.dir = utility::PxVec3toVec3(axis);
				joint->pos = utility::PxVec3toVec3(pos);

				printf( "connect angle = %f, dir= %f %f %f\n", angle, axis.x, axis.y, axis.z);
			}

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
			//const PxTransform tm = GetJointTransformAccumulate(m_rootNode, m_selectNode);
			const SConnection *joint = m_rootNode->GetJoint(m_selectNode);
			BRK(!joint);
	
			const PxVec3 initialPos = utility::Vec3toPxVec3(joint->pos);
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

			m_selectNode->SetWorldTransform(PxTransform(pos));
			//m_selectNode->m_renderNode->setTransform(PxTransform(pos));
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

			//const Int2 diff = pos - m_ptOrig;
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


			PxTransform tm = m_selectNode->GetLocalTransform();
			PxTransform m = PxTransform(PxQuat(-0.03f, v0)) * tm;
			m_selectNode->SetLocalTransform(m);
			m_selectNode->UpdateTransform();

			//PxQuat q0(diff.x*0.05f, PxVec3(0,1,0));
			//PxQuat q1(-diff.y*0.05f, PxVec3(1,0,0));
			//PxTransform m = tm * PxTransform(q1) * PxTransform(q0);
			//m_selectNode->m_renderNode->setTransform(m);
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
CGenotypeNode* COrientationEditController::CreatePhenotypeDiagram(
	const PxTransform &parentTm, const PxTransform &tm0, const PxTransform &tm1, 
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

	PxTransform curTm = parentTm * tm0.getInverse() * tm1;
	diagNode->SetWorldTransform(curTm);
	//diagNode->m_renderNode->setTransform(curTm);
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
		PxTransform newTm0, newTm1;
		GetJointTransform(NULL, node_con, newTm0, newTm1);
		newTm0 = newTm0.getInverse(); // genotype script return inverse transform to use joint parameter
		newTm1 = newTm1.getInverse();
		CGenotypeNode *newDiagNode = CreatePhenotypeDiagram(curTm, newTm0, newTm1, node_con->expr, symbols);

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

			RenderBezierActor *arrow = CreateTransition(m_sample, diagNode, newDiagNode, PxVec3(0,0,0), order);
			diagramConnection.transitionArrow = arrow;
			m_sample.addRenderObject(arrow);

			diagNode->m_connectDiagrams.push_back(diagramConnection);
		}
		else
		{
			SDiagramConnection diagramConnection;
			diagramConnection.connectNode = newDiagNode;

			RenderBezierActor *arrow = CreateTransition(m_sample, diagNode, newDiagNode, PxVec3(0,0,0), order);
			diagramConnection.transitionArrow = arrow;
			m_sample.addRenderObject(arrow);	

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
