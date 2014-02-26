#include "stdafx.h"
#include "DiagramController.h"
#include "DiagramNode.h"
#include "../EvolvedVirtualCreatures.h"
#include "../genoype/GenotypeParser.h"
#include "../renderer/RenderModelActor.h"
#include "../renderer/RenderBezierActor.h"
#include "SimpleCamera.h"
#include "PopupDiagrams.h"



using namespace evc;

CDiagramController::CDiagramController(CEvc &sample) :
	m_sample(sample)
,	m_rootDiagram(NULL)
,	m_camera(NULL)
,	m_selectNode(NULL)
,	m_leftButtonDown(false)
,	m_rightButtonDown(false)
,	m_isLinkDrag(false)
,	m_popupDiagrams(NULL)
,	m_isLayoutAnimation(false)
{
	
}

CDiagramController::~CDiagramController()
{
	set<CDiagramNode*> diags;
	RemoveDiagram(m_rootDiagram, diags);
	m_rootDiagram = NULL;

	SAFE_DELETE(m_popupDiagrams);
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
void CDiagramController::SetControlCreature(const genotype_parser::SExpr *expr)
{
	set<CDiagramNode*> diags;
	RemoveDiagram(m_rootDiagram, diags);
	m_rootDiagram = NULL;

	m_diagrams.clear();
	map<const genotype_parser::SExpr*, CDiagramNode*> diagrams;
	m_rootDiagram = CreateDiagramTree(PxVec3(0,0,0), expr, diagrams);
}


/**
 @brief Render
 @date 2014-02-26
*/
void CDiagramController::Render()
{
	BOOST_FOREACH (auto node, m_diagrams)
		node->Render();
	if (m_popupDiagrams)
		m_popupDiagrams->Render();

	if (m_isLinkDrag)
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
void CDiagramController::Move(float dtime)
{
	BOOST_FOREACH (auto node, m_diagrams)
		node->Move(dtime);

	TransitionAnimation(dtime);
}


/**
 @brief Transition Arrow Animation
 @date 2014-02-26
*/
void CDiagramController::TransitionAnimation(const float dtime)
{
	RET(!m_isLayoutAnimation);

	m_elapsTime += dtime;
	if (m_elapsTime > 1.3f)
		m_isLayoutAnimation = false;

	BOOST_FOREACH (auto node, m_diagrams)
	{
		map<CDiagramNode*,u_int> orders;
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
 @brief caculate diagram poisitioning
 @date 2014-02-12
*/
PxVec3 CDiagramController::Layout(CDiagramNode *node, set<CDiagramNode*> &symbols, const PxVec3 &pos)
{
	RETV(!node, PxVec3(0,0,0));

	if (symbols.find(node) != symbols.end())
		return PxVec3(0,0,0);

	symbols.insert(node);
	//node->m_renderNode->setTransform(PxTransform(pos));
	node->AnimateLayout(pos);

	const PxVec3 dimension = utility::Vec3toPxVec3(node->m_expr->dimension);
	const float radius = max(dimension.x*2.f +1, 2);

	map<CDiagramNode*,u_int> orders;
	BOOST_FOREACH (auto child, node->m_connectDiagrams)
		orders[ child.connectNode] = 0;

	PxVec3 offset(radius,0,0);
	BOOST_FOREACH (auto child, node->m_connectDiagrams)
	{
		CDiagramNode *childNode = child.connectNode;
		const PxVec3 childOffset = Layout(childNode, symbols, pos+offset);
		offset.y += childOffset.y;

		if (childOffset.isZero())
			++orders[childNode];

		u_int order = orders[childNode];
		MoveTransition(child.transitionArrow, node, childNode, order);
	}

	//if (node->m_connectDiagrams.empty())
	if (offset.y == 0)
		offset.y = radius;

	return offset;
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
	int childIndex = 0;
	while (connection)
	{
		u_int order=0;
		PxVec3 newNodePos = GetDiagramPositionByIndex(expr, diagNode->m_renderNode->getTransform().p, childIndex, order);
		SConnection *node_con = connection->connect;
		CDiagramNode *newDiagNode = CreateDiagramTree(newNodePos, node_con->expr, symbols);

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

			RenderBezierActor *arrow = CreateTransition(diagNode, newDiagNode, order);
			diagramConnection.transitionArrow = arrow;
			m_sample.addRenderObject(arrow);

			diagNode->m_connectDiagrams.push_back(diagramConnection);
		}
		else
		{
			SDiagramConnection diagramConnection;
			diagramConnection.connectNode = newDiagNode;

			RenderBezierActor *arrow = CreateTransition(diagNode, newDiagNode, order);
			diagramConnection.transitionArrow = arrow;
			m_sample.addRenderObject(arrow);	

			diagNode->m_connectDiagrams.push_back(diagramConnection);
		}

		connection = connection->next;
	}

	return diagNode;
}


/**
 @brief calcuate transition bezier curve position
 @date 2014-02-26
*/
void CDiagramController::CalcuateTransitionPositions(CDiagramNode *from, CDiagramNode *to, const u_int order, OUT vector<PxVec3> &out)
{
	PxVec3 pos = from->m_renderNode->getTransform().p;

	if (from == to)
	{
		const float dimensionY = to->m_expr? to->m_expr->dimension.y : 1.f;
		PxVec3 arrowPos = pos + PxVec3(-0.f,dimensionY+0.1f,0);

		const float offset = 0.7f;
		out.push_back( arrowPos );
		out.push_back( arrowPos + PxVec3(-offset,offset,0) );
		out.push_back( arrowPos + PxVec3(offset,offset,0) );
		out.push_back( arrowPos );
	}
	else
	{
		PxVec3 newNodePos = to->m_renderNode->getTransform().p;

		PxVec3 dir = newNodePos - pos;
		float len = dir.magnitude();
		dir.normalize();

		PxVec3 interSectPos;
		if (to->m_renderNode->IntersectTri(pos, dir, interSectPos))
		{
			dir = interSectPos - pos;
			len = dir.magnitude();
			dir.normalize();
			newNodePos = pos + dir*len;
		}

		PxVec3 c = dir.cross(PxVec3(0,0,((order % 2)? -1 : 1)));
		const float curveH = 0.4f + ((float)order/2.f) * 0.5f;

		out.push_back( pos );
		out.push_back( pos + (dir*len*0.5f) - c*curveH );
		out.push_back( out[1] );
		out.push_back( newNodePos );
	}
}


/**
 @brief Create Transition
 @date 2014-02-25
*/
RenderBezierActor* CDiagramController::CreateTransition(CDiagramNode *from, CDiagramNode *to, const u_int order)//order=0
{
	vector<PxVec3> points;
	CalcuateTransitionPositions(from, to, order, points);
	RenderBezierActor *arrow = new RenderBezierActor(*m_sample.getRenderer(), points);
	return arrow;
}


/**
 @brief Move Transition Arrow
 @date 2014-02-26
*/
void CDiagramController::MoveTransition(RenderBezierActor *transition, CDiagramNode *from, CDiagramNode *to, const u_int order)
{
	RET(!transition);
	vector<PxVec3> points;
	CalcuateTransitionPositions(from, to, order, points);
	transition->SetBezierCurve(points);
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
		node->m_expr->isSensor = true;
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
 @brief remove diagram
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
}


/**
 @brief mouse position x,y to check diagram 3d object
 @date 2014-02-25
*/
CDiagramNode* CDiagramController::PickupDiagram(physx::PxU32 x, physx::PxU32 y, 
	const bool isCheckLinkDiagram, const bool isShowHighLight)
{
	PxVec3 orig, dir, pickOrig;
	m_sample.GetPicking()->computeCameraRay(orig, dir, pickOrig, x, y);

	evc::CDiagramNode *mouseOverNode = NULL;

	// Check Diagrams
	BOOST_FOREACH(auto node, m_diagrams)
	{
		PxVec3 out;
		const bool isHighLight = node->m_renderNode->IntersectTri(pickOrig, dir, out);
		if (isShowHighLight)
			node->SetHighLight(isHighLight);
		if (isHighLight)
			mouseOverNode = node;
	}

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
bool CDiagramController::InsertDiagram(CDiagramNode *node, CDiagramNode *insertNode)
{
	RETV(!node, false);
	RETV(!insertNode, false);

	using namespace genotype_parser;
	SConnectionList *conList = new SConnectionList;
	conList->next = NULL;
	conList->connect = new SConnection;
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
		insertNode->m_renderNode->setTransform(PxTransform(pos));

	SDiagramConnection diagConnection;
	diagConnection.transitionArrow = CreateTransition(node, insertNode, order);
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
 @brief calcuate diagram poisition
 @date 2014-02-25
*/
PxVec3 CDiagramController::GetDiagramPosition(CDiagramNode *parent, CDiagramNode *dispNode, OUT u_int &order)
{
	RETV(!parent, PxVec3(0,0,0));
	RETV(!dispNode, PxVec3(0,0,0));

	const PxVec3 dimension = utility::Vec3toPxVec3(parent->m_expr->dimension);
	const float radius = max(dimension.x*2.f +1, 2);

	set<string> symbols;
	PxVec3 val(radius,0,0);
	BOOST_FOREACH (auto child, parent->m_connectDiagrams)
	{
		CDiagramNode *childNode = child.connectNode;
		if (boost::iequals(parent->m_name, child.connectNode->m_name))
			continue;
		if (symbols.end() != symbols.find(childNode->m_name))
			continue; // already exist
		if (child.connectNode == dispNode)
			break;

		symbols.insert(childNode->m_name);
		const PxVec3 childDimension = utility::Vec3toPxVec3(childNode->m_expr->dimension);
		const float height = max(childDimension.y*2.f +1, 2);
		val += PxVec3(0,height,0);
	}

	// find order
	order = 0;
	BOOST_FOREACH (auto child, parent->m_connectDiagrams)
	{
		if (child.connectNode == dispNode)
			++order;
	}

	return val + parent->m_renderNode->getTransform().p;
}


/**
@brief calcuate diagram poisition
 @date 2014-02-25
*/
PxVec3 CDiagramController::GetDiagramPositionByIndex(const genotype_parser::SExpr *parent_expr, const PxVec3 &parentPos, 
	const u_int index, OUT u_int &order)
{
	RETV(!parent_expr, PxVec3(0,0,0));

	using namespace genotype_parser;
	const PxVec3 dimension = utility::Vec3toPxVec3(parent_expr->dimension);
	const float radius = max(dimension.x*2.f +1, 2);

	order = 0;
	set<string> symbols;
	PxVec3 val(radius,0,0);

	int i=-1;
	SConnectionList *conList = parent_expr->connection;
	while (conList)
	{
		++i;
		
		SConnection *con = conList->connect;
		if (!con->expr) {
			conList = conList->next; // next node
			continue;
		}
		if (boost::iequals(parent_expr->id, con->expr->id)) {
			conList = conList->next; // next node
			continue;
		}
		if (symbols.end() != symbols.find(con->expr->id))
		{
			++order;
			if (i >= (int)index)
			{
				break;
			}
			else
			{
				conList = conList->next; // next node
				continue; // already exist
			}
		}
		else
		{
			order = 0;
		}

		if (i >= (int)index)
			break;
		symbols.insert(con->expr->id);

		const PxVec3 childDimension = utility::Vec3toPxVec3(con->expr->dimension);
		const float height = max(childDimension.y*2.f +1, 2);
		val += PxVec3(0,height,0);

		conList = conList->next; // next node
	}


	//for (u_int i=0; i < parent->m_connectDiagrams.size(); ++i)
	//{
	//	CDiagramNode *childNode = parent->m_connectDiagrams[ i].connectNode;
	//	if (boost::iequals(parent->m_name, childNode->m_name))
	//		continue;
	//	if (symbols.end() != symbols.find(childNode->m_name))
	//	{
	//		++order;
	//		if (i >= index)
	//		{
	//			break;
	//		}
	//		else
	//		{
	//			continue; // already exist
	//		}
	//	}
	//	else
	//	{
	//		order = 0;
	//	}

	//	if (i >= index)
	//		break;
	//	symbols.insert(childNode->m_name);

	//	const PxVec3 childDimension = utility::Vec3toPxVec3(childNode->m_expr->dimension);
	//	const float height = max(childDimension.y*2.f +1, 2);
	//	val += PxVec3(0,height,0);
	//}

	return val + parentPos;
}


/**
 @brief MouseLButtonDown
 @date 2014-02-25
*/
void CDiagramController::MouseLButtonDown(physx::PxU32 x, physx::PxU32 y)
{
	CDiagramNode *mouseOverNode = PickupDiagram(x, y, false, true);

	SelectNode(mouseOverNode);

	if (mouseOverNode)
	{
		m_dragPos[ 0].x = (float)x/800.f;
		m_dragPos[ 0].y = (600.f - (float)y)/600.f;
	}

	if (mouseOverNode && mouseOverNode->m_expr->isSensor)
	{
		m_isLinkDrag = false; // sensor can not link to another node
	}
	else
	{
		m_isLinkDrag = mouseOverNode? true : false;
	}
}


/**
 @brief Mouse Left Button Up Event
 @date 2014-02-25
*/
void CDiagramController::MouseLButtonUp(physx::PxU32 x, physx::PxU32 y)
{
	if (m_isLinkDrag && m_selectNode)
	{
		CDiagramNode *mouseOverNode = PickupDiagram(x, y, true, true);
		if (mouseOverNode)
		{
			if (m_selectNode == mouseOverNode)
			{
				m_isLinkDrag = false;
				if (m_popupDiagrams)
					m_popupDiagrams->Close();
				return;
			}

			{
				PxSceneWriteLock scopedLock(m_sample.getActiveScene());
				InsertDiagram(m_selectNode, mouseOverNode);
				set<CDiagramNode*> symbols;
				Layout(m_rootDiagram, symbols);
				m_isLayoutAnimation = true;
				m_elapsTime = 0;
			}

			printf( "link node = %s\n", mouseOverNode->m_name.c_str() );
		}

		if (m_popupDiagrams)
			m_popupDiagrams->Close();
	}

	m_isLinkDrag = false;
}


/**
 @brief 
 @date 2014-02-25
*/
void CDiagramController::MouseRButtonDown(physx::PxU32 x, physx::PxU32 y)
{

}


/**
 @brief 
 @date 2014-02-25
*/
void CDiagramController::MouseRButtonUp(physx::PxU32 x, physx::PxU32 y)
{

}


/**
 @brief 
 @date 2014-02-25
*/
void CDiagramController::MouseMove(physx::PxU32 x, physx::PxU32 y)
{
	PickupDiagram(x, y, m_isLinkDrag, true);

	if (m_isLinkDrag)
	{
		m_dragPos[ 1].x = (float)x / 800.f;
		m_dragPos[ 1].y = (600.f-(float)y) / 600.f;
		if (m_selectNode)
			m_selectNode->SetHighLight(true);
	}
}


/**
 @brief select node
 @date 2014-02-24
*/
void CDiagramController::SelectNode(CDiagramNode *node)
{
	m_selectNode = node;

	if (!m_popupDiagrams)
		m_popupDiagrams = new CPopupDiagrams(m_sample, *this);
	m_popupDiagrams->Popup(node);
}
