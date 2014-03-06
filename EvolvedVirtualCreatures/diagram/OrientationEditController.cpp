
#include "stdafx.h"
#include "OrientationEditController.h"
#include "../EvolvedVirtualCreatures.h"
#include "GenotypeController.h"
#include "GenotypeNode.h"
#include "DiagramUtility.h"
#include "../creature/Creature.h"


using namespace evc;

COrientationEditController::COrientationEditController(CEvc &sample, CGenotypeController &genotypeController) :
	m_sample(sample)
,	m_genotypeController(genotypeController)
,	m_camera(NULL)
,	m_selectNode(NULL)
,	m_rootNode(NULL)
,	m_editMode(MODE_NONE)
{
	
}

COrientationEditController::~COrientationEditController()
{
	RemoveGenotypeTree(m_sample, m_rootNode);
	m_rootNode = NULL;

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
	m_rootNode = NULL;

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
	const PxTransform tm = PxTransform::createIdentity();
	m_rootNode = CreatePhenotypeDiagram(tm,tm,tm, parentNode->m_expr, symbols);

	m_selectNode = m_rootNode->GetConnectNode(node->m_name);

	ChangeEditMode(MODE_POSITION);
}


/**
 @brief MouseLButtonDown
 @date 2014-03-02
*/
void COrientationEditController::MouseLButtonDown(physx::PxU32 x, physx::PxU32 y)
{

}


/**
 @brief MouseLButtonUp
 @date 2014-03-04
*/
void COrientationEditController::MouseLButtonUp(physx::PxU32 x, physx::PxU32 y)
{

}


/**
 @brief MouseRButtonDown
 @date 2014-03-04
*/
void COrientationEditController::MouseRButtonDown(physx::PxU32 x, physx::PxU32 y)
{

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

			PxQuat q;
			if (boost::iequals(joint->type, "revolute"))
			{
				PxVec3 jointAxis = PxVec3(0,0,1).cross(dir);
				jointAxis.normalize();
				utility::quatRotationArc(q, jointAxis, PxVec3(1,0,0));
			}
			else
			{
				q = m_rootNode->m_renderNode->getTransform().q;
			}

			{
				PxReal angle;
				PxVec3 axis;
				q.toRadiansAndUnitAxis(angle, axis);
				joint->parentOrient.angle = angle;
				joint->parentOrient.dir = utility::PxVec3toVec3(axis);
			}
			
			{
				PxVec3 pos;
				PxTransform rotTm;
				const PxTransform tm = m_selectNode->m_renderNode->getTransform();
				if (boost::iequals(joint->type, "revolute"))
				{
					rotTm = PxTransform(q) * PxTransform(tm.q);
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
			}

			ChangeEditMode(MODE_NONE);
		}
		break;
	}
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
			const PxTransform tm = GetJointTransformAccumulate(m_rootNode, m_selectNode);
			const SConnection *joint = m_rootNode->GetJoint(m_selectNode);
			RET(!joint);

			PxTransform tm0, tm1;
			GetJointTransform(NULL, joint, tm0, tm1);
			const PxTransform rootTm = tm * tm1;
	
			const PxVec3 initialPos = utility::Vec3toPxVec3(joint->pos);
			const float len = initialPos.magnitude();

			PxVec3 orig, dir, pickOrig;
			m_sample.GetPicking()->computeCameraRay(orig, dir, pickOrig, x, y);
			PxVec3 pos = pickOrig + (dir*3);
			 
			PxVec3 v = pos - m_rootNode->GetPos();
			if (v.magnitude() > len)
			{
				v.normalize();
				pos = m_rootNode->GetPos() + (v*len);
			}

			m_selectNode->m_renderNode->setTransform(PxTransform(pos));
		}
		break;
	}
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
	diagNode->m_renderNode->setTransform(curTm);
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
		}
		break;

	case MODE_POSITION:
		break;
	}
}
