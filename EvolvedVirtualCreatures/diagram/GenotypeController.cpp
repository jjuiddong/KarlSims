#include "stdafx.h"
#include "GenotypeController.h"
#include "GenotypeNode.h"
#include "../EvolvedVirtualCreatures.h"
#include "../genoype/GenotypeParser.h"
#include "SimpleCamera.h"
#include "PopupDiagrams.h"
#include "../creature/Creature.h"
#include "OrientationEditController.h"
#include "DiagramUtility.h"


using namespace evc;

CGenotypeController::CGenotypeController(CEvc &sample) :
	m_sample(sample)
,	m_rootDiagram(NULL)
,	m_camera(NULL)
,	m_selectNode(NULL)
,	m_leftButtonDown(false)
,	m_rightButtonDown(false)
,	m_popupDiagrams(NULL)
,	m_isLayoutAnimation(false)
,	m_creature(NULL)
,	m_controlMode(MODE_NONE)
,	m_OrientationEditController(NULL)
{

}

CGenotypeController::~CGenotypeController()
{
	RemoveGenotypeTree(m_sample, m_rootDiagram);
	m_rootDiagram = NULL;

	SAFE_DELETE(m_OrientationEditController);
	SAFE_DELETE(m_popupDiagrams);
	SAFE_DELETE(m_camera);
}


/**
 @brief Init Diagram Controller Scene
 @date 2014-02-24
*/
void CGenotypeController::ControllerSceneInit()
{
	if (!m_camera)
		m_camera = SAMPLE_NEW(CSimpleCamera)();

	if (m_OrientationEditController)
	{
		m_OrientationEditController->SetControlDiagram(NULL);//initialize
	}
	else
	{
		m_OrientationEditController = new COrientationEditController(m_sample, *this);
	}

	m_sample.getApplication().setMouseCursorHiding(false);
	m_sample.getApplication().setMouseCursorRecentering(false);

	m_camera->init(PxVec3(0.0f, 3.0f, 10.0f), PxVec3(0.f, 0, 0.0f));
	m_camera->setMouseSensitivity(0.5f);

	m_sample.getApplication().setCameraController(m_camera);
	m_sample.setCameraController(m_camera);

	Show(true);
	
	ChangeControllerMode(MODE_NONE);
}


/**
 @brief SetControlCreature
 @date 2014-02-12
*/
void CGenotypeController::SetControlCreature(CCreature *creature)//const genotype_parser::SExpr *expr)
{
	m_creature = creature;

	RemoveGenotypeTree(m_sample, m_rootDiagram);
	m_rootDiagram = NULL;

	m_diagrams.clear();
	map<const genotype_parser::SExpr*, CGenotypeNode*> diagrams;
	m_rootDiagram = CreateGenotypeDiagramTree(PxVec3(0,0,0), creature->GetGenotype(), diagrams);

	Layout();
}


/**
 @brief Render
 @date 2014-02-26
*/
void CGenotypeController::Render()
{
	BOOST_FOREACH (auto node, m_diagrams)
		node->Render();
	if (m_popupDiagrams)
		m_popupDiagrams->Render();

	if (MODE_LINK==m_controlMode)
	{
		using namespace SampleRenderer;
		PxReal vertices[] = {m_dragPos[0].x, m_dragPos[0].y, m_dragPos[1].x, m_dragPos[1].y};
		RendererColor colors[] = {RendererColor(0,255,0), RendererColor(0,255,0) };
		m_sample.getRenderer()->drawLines2D(2, vertices, colors );
	}
}


/**
 @brief 
 @date 2014-02-12
*/
void CGenotypeController::Move(float dtime)
{
	BOOST_FOREACH (auto node, m_diagrams)
		node->Move(dtime);

	TransitionAnimation(dtime);
}


/**
 @brief Transition Arrow Animation
 @date 2014-02-26
*/
void CGenotypeController::TransitionAnimation(const float dtime)
{
	RET(!m_isLayoutAnimation);

	m_elapsTime += dtime;
	if (m_elapsTime > 1.3f)
		m_isLayoutAnimation = false;

	BOOST_FOREACH (auto node, m_diagrams)
	{
		map<CGenotypeNode*,u_int> orders;
		BOOST_FOREACH  (auto conNode, node->m_connectDiagrams)
			orders[ conNode.connectNode] = 0; // init

		BOOST_FOREACH  (auto conNode, node->m_connectDiagrams)
		{
			++orders[ conNode.connectNode];
			MoveTransition(conNode.transitionArrow, node, conNode.connectNode, orders[ conNode.connectNode]-1);
		}
	}
}


/**
 @brief calcuate Layout
 @date 2014-02-27
*/
void CGenotypeController::Layout(const PxVec3 &pos) // pos=PxVec3(0,0,0)
{
	set<CGenotypeNode*> symbols;
	LayoutRec(m_rootDiagram, symbols, pos);

	m_isLayoutAnimation = true;
	m_elapsTime = 0;
}


/**
 @brief caculate diagram poisitioning
 @date 2014-02-12
*/
PxVec3 CGenotypeController::LayoutRec(CGenotypeNode *node, set<CGenotypeNode*> &symbols, const PxVec3 &pos)
{
	RETV(!node, PxVec3(0,0,0));

	if (symbols.find(node) != symbols.end())
		return PxVec3(0,0,0);

	symbols.insert(node);
	//node->m_renderNode->setTransform(PxTransform(pos));
	node->AnimateLayout(pos);

	const PxVec3 dimension = utility::Vec3toPxVec3(node->m_expr->dimension);
	const float radius = max(dimension.x*2.f+1, 2);

	map<CGenotypeNode*,u_int> orders;
	BOOST_FOREACH (auto child, node->m_connectDiagrams)
		orders[ child.connectNode] = 0;

	PxVec3 offset(radius,0,0);
	BOOST_FOREACH (auto child, node->m_connectDiagrams)
	{
		CGenotypeNode *childNode = child.connectNode;
		const PxVec3 childOffset = LayoutRec(childNode, symbols, pos+offset);
		offset.y += childOffset.y;

		if (childOffset.isZero())
			++orders[childNode];

		//u_int order = orders[childNode];
		//MoveTransition(child.transitionArrow, node, childNode, order);
	}

	//if (node->m_connectDiagrams.empty())
	if (offset.y < radius)
		offset.y = radius;

	return offset;
}


/**
 @brief create diagram node
 @date 2014-02-12
*/
CGenotypeNode* CGenotypeController::CreateGenotypeDiagramTree(const PxVec3 &pos, const genotype_parser::SExpr *expr, 
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

	//diagNode->m_renderNode->setTransform(PxTransform(pos));
	diagNode->SetWorldTransform(PxTransform(pos));
	m_diagrams.push_back(diagNode);
	m_sample.addRenderObject(diagNode->m_renderNode);

	RETV(!expr, diagNode); // if sensor node return

	symbols[ expr] = diagNode; // insert

	SConnectionList *connection = expr->connection;
	SConnectionList *currentCopyConnection = NULL;
	int childIndex = 0;
	while (connection)
	{
		u_int order=0;
		PxVec3 newNodePos = GetDiagramPositionByIndex(expr, diagNode->GetWorldTransform().p, childIndex, order);
		SConnection *node_con = connection->connect;
		CGenotypeNode *newDiagNode = CreateGenotypeDiagramTree(newNodePos, node_con->expr, symbols);

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
 @brief Mouse Event
 @date 2014-02-24
*/
void CGenotypeController::onPointerInputEvent(const SampleFramework::InputEvent&ie, 
	physx::PxU32 x, physx::PxU32 y, physx::PxReal dx, physx::PxReal dy, bool val)
{
	const bool isLeftBtnDown = ((GetKeyState(VK_LBUTTON) & 0x80) != 0);
	const bool isRightBtnDown = ((GetKeyState(VK_RBUTTON) & 0x80) != 0);

	// Mouse Left Button Event Check
	if (m_leftButtonDown)
	{
		if (!isLeftBtnDown)
		{
			MouseLButtonUp(x,y);
			m_leftButtonDown = false;
			return;
		}
	}
	else
	{
		if (isLeftBtnDown)
		{
			MouseLButtonDown(x,y);
			m_leftButtonDown = true;
			return;
		}
	}


	// Mouse Right Button Event Check
	if (m_rightButtonDown)
	{
		if (!isRightBtnDown)
		{
			MouseRButtonUp(x,y);
			m_rightButtonDown = false;
			return;
		}
	}
	else
	{
		if (isRightBtnDown)
		{
			MouseRButtonDown(x,y);
			m_rightButtonDown = true;
			return;
		}
	}

	// else mouse move event
	MouseMove(x,y);

	//switch (m_controlMode)
	//{
	//case MODE_ORIENT:
	//	if (m_OrientationEditController)
	//		m_OrientationEditController->onPointerInputEvent(ie,x,y,dx,dy,val);
	//	break;
	//}
}


/**
 @brief Digital Input Event Handler
 @date 2014-02-27
*/
void CGenotypeController::onDigitalInputEvent(const SampleFramework::InputEvent &ie, bool val)
{
	switch (m_controlMode)
	{
	case MODE_LINK:
		switch (ie.m_Id)
		{
		case REMOVE_OBJECT: 
			if (m_selectNode)
			{
				if (RemoveDiagram(m_selectNode))
				{
					RemoveUnlinkDiagram();
					m_selectNode = NULL;
					Layout();
					UpdateCreature();
				}
			}
			break;
		}
		break;

	case MODE_ORIENT:
		if (m_OrientationEditController)
			m_OrientationEditController->onDigitalInputEvent(ie, val);
		break;
	}
}


/**
 @brief mouse position x,y to check diagram 3d object
 @date 2014-02-25
*/
CGenotypeNode* CGenotypeController::PickupDiagram(physx::PxU32 x, physx::PxU32 y, 
	const bool isCheckLinkDiagram, const bool isShowHighLight)
{
	//PxVec3 orig, dir, pickOrig;
	//m_sample.GetPicking()->computeCameraRay(orig, dir, pickOrig, x, y);
	//evc::CGenotypeNode *mouseOverNode = NULL;

	//// Check Diagrams
	//BOOST_FOREACH(auto node, m_diagrams)
	//{
	//	PxVec3 out;
	//	const bool isHighLight = node->m_renderNode->IntersectTri(pickOrig, dir, out);
	//	if (isShowHighLight)
	//		node->SetHighLight(isHighLight);
	//	if (isHighLight)
	//		mouseOverNode = node;
	//}

	evc::CGenotypeNode *mouseOverNode = evc::PickupNodes(m_sample, m_diagrams, x, y, isShowHighLight);
	if (mouseOverNode)
		return mouseOverNode;

	// Check Link Diagrams
	if (isCheckLinkDiagram && m_popupDiagrams)
	{
		mouseOverNode = m_popupDiagrams->PickupDiagram(x,y,isShowHighLight);
	}

	return mouseOverNode;
}


/**
 @brief Insert Diagram
 @date 2014-02-25
*/
bool CGenotypeController::InsertDiagram(CGenotypeNode *node, CGenotypeNode *insertNode)
{
	RETV(!node, false);
	RETV(!insertNode, false);

	using namespace genotype_parser;
	SConnectionList *conList = new SConnectionList;
	conList->next = NULL;
	conList->connect = new SConnection;
	if (insertNode->m_expr->isSensor)
	{
		conList->connect->conType = "sensor";
		conList->connect->type = "vision";
	}
	conList->connect->expr = insertNode->m_expr;

	// insert joint connection
	if (node->m_expr->connection)
	{
		SConnectionList *nextCon = node->m_expr->connection;
		while (nextCon->next) {
			nextCon = nextCon->next;
		}
		nextCon->next = conList;
	}
	else
	{
		node->m_expr->connection = conList;
	}

	const bool isSameNodeDirected = boost::iequals(node->m_name, insertNode->m_name);
	const bool isNewDiagramNode = (m_diagrams.end() == find(m_diagrams.begin(), m_diagrams.end(), insertNode)) &&
														!isSameNodeDirected;

	if (isSameNodeDirected)
		insertNode = node;

	// update inserNode position
	u_int order = 0;
	PxVec3 pos = GetDiagramPosition(node, insertNode, order);
	if (isNewDiagramNode)
		insertNode->SetWorldTransform(PxTransform(pos));
		//insertNode->m_renderNode->setTransform(PxTransform(pos));
	insertNode->m_isRenderText = true;

	SDiagramConnection diagConnection;
	diagConnection.transitionArrow = CreateTransition(m_sample, node, insertNode, order);
	diagConnection.connectNode = insertNode;
	m_sample.addRenderObject(diagConnection.transitionArrow);
	node->m_connectDiagrams.push_back(diagConnection);

	if (isNewDiagramNode)
		m_diagrams.push_back(insertNode);

	// remove from candidateLinkDiagrams
	if (m_popupDiagrams)
		m_popupDiagrams->RemoveDiagram(insertNode);

	return true;
}


/**
 @brief Remove DiagramNode
			remove SExpr Node
 @date 2014-02-27
*/
bool CGenotypeController::RemoveDiagram(CGenotypeNode *rmNode)
{
	RETV(!rmNode, false);

	printf( "delete node : %s\n", rmNode->m_name.c_str());
	
	// remove pointing to rmNode
	BOOST_FOREACH (auto node, m_diagrams)
	{
		node->RemoveConnectNode(rmNode);
	}

	auto newEnd = remove(m_diagrams.begin(), m_diagrams.end(), rmNode);
	const bool isRemoveNode = m_diagrams.end() != newEnd;
	m_diagrams.erase(newEnd, m_diagrams.end());

	SAFE_DELETE(rmNode);
	return isRemoveNode;
}


/**
 @brief remove non pointing node
 @date 2014-02-28
*/
void CGenotypeController::RemoveUnlinkDiagram()
{
	map<CGenotypeNode*, int> nodesPointing;
	BOOST_FOREACH (auto node, m_diagrams)
		nodesPointing[ node] = 0;

	BOOST_FOREACH (auto node, m_diagrams)
	{
		BOOST_FOREACH (auto con, node->m_connectDiagrams)
		{
			if (node == con.connectNode)
				continue; // recursive connection
			++nodesPointing[ con.connectNode];
		}
	}

	bool isFindNonPointingNode = false;
	BOOST_FOREACH (auto kv, nodesPointing)
	{
		if (kv.second > 0)
			continue;
		CGenotypeNode *nonPointingNode = kv.first;
		if (boost::iequals(nonPointingNode->m_name, "main"))
			continue; // except main diagram

		isFindNonPointingNode = true;
		RemoveDiagram(nonPointingNode);
	}

	if (isFindNonPointingNode)
		RemoveUnlinkDiagram();
}


/**
 @brief MouseLButtonDown
 @date 2014-02-25
*/
void CGenotypeController::MouseLButtonDown(physx::PxU32 x, physx::PxU32 y)
{
	switch (m_controlMode)
	{
	case MODE_NONE:
	case MODE_LINK:
		{
			CGenotypeNode *mouseOverNode = PickupDiagram(x, y, false, true);
			SelectNode(mouseOverNode);
			if (mouseOverNode)
			{
				m_dragPos[ 0].x = (float)x/800.f;
				m_dragPos[ 0].y = (600.f - (float)y)/600.f;
			}
			if (mouseOverNode && mouseOverNode->m_expr->isSensor)
			{
				ChangeControllerMode(MODE_NONE);
			}
			else
			{
				ChangeControllerMode(mouseOverNode? MODE_LINK : MODE_NONE);
			}
		}
		break;

	case MODE_ORIENT:
		if (m_OrientationEditController)
			m_OrientationEditController->MouseLButtonDown(x,y);
		break;
	}
}


/**
 @brief Mouse Left Button Up Event
 @date 2014-02-25
*/
void CGenotypeController::MouseLButtonUp(physx::PxU32 x, physx::PxU32 y)
{
	switch (m_controlMode)
	{
	case MODE_NONE:
		break;

	case MODE_LINK:
		{
			if (m_selectNode)
			{
				CGenotypeNode *mouseOverNode = PickupDiagram(x, y, true, true);
				if (mouseOverNode)
				{
					if (m_selectNode == mouseOverNode)
					{
						ChangeControllerMode(MODE_NONE);
						if (m_popupDiagrams)
							m_popupDiagrams->Close();
						return;
					}

					PxSceneWriteLock scopedLock(m_sample.getActiveScene());
					InsertDiagram(m_selectNode, mouseOverNode);
					Layout();
					UpdateCreature();
					//printf( "link node = %s\n", mouseOverNode->m_name.c_str() );
				}

				if (m_popupDiagrams)
					m_popupDiagrams->Close();
			}

			ChangeControllerMode(MODE_NONE);
		}
		break;

	case MODE_ORIENT:
		if (m_OrientationEditController)
			m_OrientationEditController->MouseLButtonUp(x,y);
		break;
	}
}


/**
 @brief Mouse Right Button Down
 @date 2014-02-25
*/
void CGenotypeController::MouseRButtonDown(physx::PxU32 x, physx::PxU32 y)
{
	switch (m_controlMode)
	{
	case MODE_NONE:
	case MODE_LINK:
		{
			CGenotypeNode *mouseOverNode = PickupDiagram(x, y, false, true);
			SelectNode(mouseOverNode, false);
			if (mouseOverNode)
			{
				m_dragPos[ 0].x = (float)x/800.f;
				m_dragPos[ 0].y = (600.f - (float)y)/600.f;
			}
			if (mouseOverNode)
			{
				ChangeControllerMode(MODE_ORIENT);
				m_OrientationEditController->SetControlDiagram(mouseOverNode);
			}
			else
			{
				ChangeControllerMode(MODE_NONE);
			}
		}
		break;

	case MODE_ORIENT:
		if (m_OrientationEditController)
			m_OrientationEditController->MouseRButtonDown(x,y);
		break;
	}
}


/**
 @brief Mouse Event
 @date 2014-02-25
*/
void CGenotypeController::MouseRButtonUp(physx::PxU32 x, physx::PxU32 y)
{
	switch (m_controlMode)
	{
	case MODE_ORIENT:
		if (m_OrientationEditController)
			m_OrientationEditController->MouseRButtonUp(x,y);
		break;
	}
}


/**
 @brief 
 @date 2014-02-25
*/
void CGenotypeController::MouseMove(physx::PxU32 x, physx::PxU32 y)
{
	switch (m_controlMode)
	{
	case MODE_NONE:
	case MODE_LINK:
		{
			PickupDiagram(x, y, (MODE_LINK==m_controlMode), true);

			if (MODE_LINK==m_controlMode)
			{
				m_dragPos[ 1].x = (float)x / 800.f;
				m_dragPos[ 1].y = (600.f-(float)y) / 600.f;
				if (m_selectNode)
					m_selectNode->SetHighLight(true);
			}
		}
		break;

	case MODE_ORIENT:
		if (m_OrientationEditController)
			m_OrientationEditController->MouseMove(x,y);
		break;
	}
}


/**
 @brief select node
 @date 2014-02-24
*/
void CGenotypeController::SelectNode(CGenotypeNode *node, const bool isShowPopupDiagrams) //isShowPopupDiagrams=true
{
	m_selectNode = node;

	if (isShowPopupDiagrams)
	{
		if (!m_popupDiagrams)
			m_popupDiagrams = new CPopupDiagrams(m_sample, *this);
		m_popupDiagrams->Popup(node);
	}
}


/**
 @brief creature genotype update
 @date 2014-02-27
*/
void CGenotypeController::UpdateCreature()
{
	RET(!m_creature);

	m_creature->SetMaxGrowCount(g_pDbgConfig->generationRecursiveCount);
	m_creature->GenerateProgressive(CopyGenotype(m_rootDiagram->m_expr), m_creature->GetPos()+PxVec3(0,5,0), NULL);

	//m_creature = new evc::CCreature(m_sample); 
	////m_creature->GenerateProgressive(CopyGenotype(m_rootDiagram->m_expr), m_creature->GetPos()+PxVec3(0,5,0), NULL);
	//m_creature->GenerateProgressive(m_rootDiagram->m_expr, m_creature->GetPos()+PxVec3(0,5,0), NULL);
	////m_creature->GenerateImmediate(CopyGenotype(m_rootDiagram->m_expr), m_creature->GetPos()+PxVec3(0,5,0), NULL);
}


/**
 @brief Change ControllerScene
 @date 2014-03-02
*/
void CGenotypeController::ChangeControllerMode(const MODE mode)
{
	switch (mode)
	{
	case MODE_NONE: break;

	case MODE_LINK: 
		{
			//ControllerSceneInit();
		}
		break;

	case MODE_ORIENT: 
		{
			Show(false);
			m_OrientationEditController->ControllerSceneInit();
		}
		break;
	}

	m_controlMode = mode;
}


/**
 @brief get diagrams pointing to DiagramNode*
 @date 2014-03-02
*/
bool CGenotypeController::GetDiagramsLinkto(CGenotypeNode *to, OUT vector<CGenotypeNode*> &out)
{
	RETV(!m_rootDiagram, false);

	BOOST_FOREACH (auto node, m_diagrams)
	{
		BOOST_FOREACH (auto con, node->m_connectDiagrams)
		{
			if (node == con.connectNode)
				continue; // recursive connection
			if (to == con.connectNode)
				out.push_back(node);
		}
	}

	// unique
	std::sort(out.begin(), out.end());
	out.erase(std::unique(out.begin(), out.end()), out.end());
	return true;
}


/**
 @brief get from diagram child node
 @date 2014-03-02
*/
bool CGenotypeController::GetDiagramsLinkfrom(CGenotypeNode *from, OUT vector<CGenotypeNode*> &out)
{
	RETV(!from, false);

	BOOST_FOREACH (auto &con, from->m_connectDiagrams)
	{
		out.push_back(con.connectNode);
	}

	// unique
	std::sort(out.begin(), out.end());
	out.erase(std::unique(out.begin(), out.end()), out.end());
	return true;
}


/**
 @brief show/hide diagrams
 @date 2014-03-03
*/
void CGenotypeController::Show(const bool isShow)
{
	BOOST_FOREACH (auto &node, m_diagrams)
		node->Show(isShow);
}
