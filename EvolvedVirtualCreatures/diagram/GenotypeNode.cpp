
#include "stdafx.h"
#include "GenotypeNode.h"
#include "../EvolvedVirtualCreatures.h"
#include "../renderer/RenderBezierActor.h"


using namespace evc;

CGenotypeNode::CGenotypeNode(CEvc &sample) :
	m_sample(sample)
,	m_renderNode(NULL)
,	m_highLight(false)
,	m_expr(NULL)
,	m_isAnimationMove(false)
,	m_isRenderText(true)
{

}

CGenotypeNode::~CGenotypeNode()
{
	m_sample.removeRenderObject(m_renderNode);
	m_renderNode = NULL;
	genotype_parser::RemoveExpressoin_OnlyExpr(m_expr);
	m_expr = NULL;

	BOOST_FOREACH (auto con, m_connectDiagrams)
		m_sample.removeRenderObject(con.transitionArrow);
	m_connectDiagrams.clear();
}


// Render
void	CGenotypeNode::Render()
{
	RET(!m_renderNode);
	RET(!m_renderNode->isRendering());
	RET(!m_isRenderText);

	using namespace SampleRenderer;

	PxTransform viewTM = m_sample.getApplication().getCamera().getViewMatrix();
	PxTransform tm = m_renderNode->getTransform();
	tm.p.x = -tm.p.x; // change left hand axis

	D3DXMATRIX m;
	utility::convertD3D9(m, tm);

	D3DXMATRIX mProj;
	D3DXMatrixPerspectiveFovLH(&mProj, 55.f * PxPi / 180.0f, 800.f/600.f, 0.1f, 10000.f);

	D3DXMATRIX mView;
	{
		D3DXVECTOR3 pos(-viewTM.p.x, viewTM.p.y, viewTM.p.z);
		PxVec3 lookAt = viewTM.p + (m_sample.getApplication().getCamera().getViewDir() * 10.f);
		D3DXVECTOR3 target(-lookAt.x, lookAt.y, lookAt.z);
		D3DXMatrixLookAtLH(&mView, &pos, &target, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	}

	D3DVIEWPORT9 viewport;
	viewport.X = 0;
	viewport.Y = 0;
	viewport.Width = 800;
	viewport.Height = 600;
	viewport.MaxZ = 1;
	viewport.MinZ = 0;

	D3DXVECTOR3 v;
	D3DXVec3Project(&v, &D3DXVECTOR3(0,0,0), &viewport, &mProj, &mView, &m);

	if ((v.x < 0) || (v.y < 0) || (v.x > 800) || (v.y > 600))
		return;

	m_sample.getRenderer()->print(v.x, v.y, m_name.c_str() );
}


/**
 @brief 
 @date 2014-02-26
*/
void CGenotypeNode::Move(float dtime)
{
	if (m_isAnimationMove)
	{
		m_elapseTime += dtime;
		if (m_elapseTime > 1)
		{
			m_isAnimationMove = false;
			m_elapseTime = 1;
		}

		PxVec3 mov = (m_moveVelocity*m_elapseTime) + m_initialPos;
		m_renderNode->setTransform(PxTransform(mov));
	}
}


/**
 @brief 
 @date 2014-02-24
*/
void CGenotypeNode::SetHighLight(const bool highLight)
{
	if (m_highLight != highLight)
	{
		PxVec3 material = highLight? (m_material+PxVec3(0.25f,0.25f,0.25f)) : m_material;
		m_renderNode->setRenderMaterial( m_sample.GetMaterial(material, false) );

		m_highLight = highLight;
	}
}


/**
 @brief move diagram smoothly to target position
 @date 2014-02-26
*/
void CGenotypeNode::AnimateLayout(const PxVec3 &target)
{
	m_isAnimationMove = true;
	m_targetPos = target;
	m_initialPos = m_renderNode->getTransform().p;
	m_moveVelocity = m_targetPos - m_initialPos;
	m_elapseTime = 0;
}


/**
 @brief remove connect node in m_connectDiagrams
			remove connect node in genotype_parser::SExpr
 @date 2014-02-28
*/
void CGenotypeNode::RemoveConnectNode(const CGenotypeNode *rmNode)
{
	// remove m_connectDiagrams
	BOOST_FOREACH (auto con, m_connectDiagrams)
	{
		if (con.connectNode == rmNode)
			m_sample.removeRenderObject(con.transitionArrow);
	}

	auto newEnd = std::remove(m_connectDiagrams.begin(), m_connectDiagrams.end(), SDiagramConnection((CGenotypeNode*)rmNode));
	m_connectDiagrams.erase(newEnd, m_connectDiagrams.end());


	// remove genotype_parser::SExpr
	RET(!m_expr);

	using namespace genotype_parser;
	// find rmNode->m_expr
	
	bool isFindExpr = false;
	SConnectionList *conList = m_expr->connection;
	while (conList && !isFindExpr)
	{
		SConnection *connect = conList->connect;
		if (connect->expr == rmNode->m_expr)
			isFindExpr = true;
		conList = conList->next;
	}

	if (isFindExpr)
	{
		list<SConnectionList*> nodes;
		SConnectionList *conList = m_expr->connection;
		while (conList)
		{
			nodes.push_back(conList);
			conList = conList->next;
		}

		// remove connection
		auto it = nodes.begin();
		while (it != nodes.end())
		{
			SConnection *connect = (*it)->connect;
			if (connect->expr == rmNode->m_expr)
			{
				SAFE_DELETE(connect);
				SAFE_DELETE(*it);
				it = nodes.erase(it);
			}
			else
			{
				++it;
			}
		}

		if (nodes.empty())
		{
			m_expr->connection = NULL;
			return;
		}

		// re link connection
		SConnectionList *headConList = *nodes.begin();
		headConList->next = NULL;
		SConnectionList *curConList = headConList;

		auto it2 = ++nodes.begin();
		while (it2 != nodes.end())
		{
			curConList->next = *it2;

			curConList = curConList->next;
			curConList->next = NULL;
			++it2;
		}

		m_expr->connection = headConList;
	}
}


/**
 @brief Show
 @date 2014-03-03
*/
void CGenotypeNode::Show(const bool isShow)
{
	RET(!m_renderNode);

	m_renderNode->setRendering(isShow);
	BOOST_FOREACH (auto &con, m_connectDiagrams)
	{
		con.transitionArrow->setRendering(isShow);
	}
}


/**
 @brief return correspond node joint
 @date 2014-03-04
*/
genotype_parser::SConnection* CGenotypeNode::GetJoint(CGenotypeNode *conNode)
{
	RETV(!conNode, NULL);
	RETV(!m_expr, NULL);

	using namespace  genotype_parser;
	SConnectionList *conList = m_expr->connection;
	BOOST_FOREACH (auto &con, m_connectDiagrams)
	{
		if (con.connectNode == conNode)
		{
			return conList->connect;
		}
		conList = conList->next;
	}
	return NULL;
}


/**
 @brief return connect node
 @date 2014-03-04
*/
CGenotypeNode* CGenotypeNode::GetConnectNode(const string &nodeName)
{
	BOOST_FOREACH (auto &con, m_connectDiagrams)
	{
		if (boost::iequals(con.connectNode->m_name, nodeName))
			return con.connectNode;
	}
	return NULL;
}


/**
 @brief return position
 @date 2014-03-04
*/
PxVec3 CGenotypeNode::GetPos() const
{
	return m_renderNode->getTransform().p;
}
