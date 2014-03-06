
#include "stdafx.h"
#include "popupDiagrams.h"
#include "../EvolvedVirtualCreatures.h"
#include "GenotypeNode.h"
#include "GenotypeController.h"
#include "DiagramUtility.h"


using namespace evc;


CPopupDiagrams::CPopupDiagrams(CEvc &sample, CGenotypeController &diagramController) :
	m_sample(sample)
,	m_diagramController(diagramController)
{

}

CPopupDiagrams::~CPopupDiagrams()
{
	BOOST_FOREACH (auto node, m_diagrams)
		delete node;
	m_diagrams.clear();
}


/**
 @brief Popup Diagrams
 @date 2014-02-26
*/
bool CPopupDiagrams::Popup(CGenotypeNode *srcNode)
{
	if (srcNode)
	{
		// if sensor diagram return
		if (srcNode->m_expr->isSensor)
			return false;
	}

	const bool isShowPopup = srcNode? true : false;
	CreateCandidateLinkNode(srcNode, isShowPopup);
	return isShowPopup;
}


/**
 @brief close popup diagrams
 @date 2014-02-26
*/
void CPopupDiagrams::Close()
{
	DisplayCandidateLinkNode(false);
}


/**
 @brief  render
 @date 2014-02-26
*/
void CPopupDiagrams::Render()
{
	BOOST_FOREACH (auto node, m_diagrams)
	{
		if (node)
			node->Render();
	}
}



/**
 @brief create link node of argument node
 @date 2014-02-25
*/
void CPopupDiagrams::CreateCandidateLinkNode(CGenotypeNode *srcNode, const bool isShow) //isShow=true
{
	if (!srcNode)
	{
		DisplayCandidateLinkNode(false);
		return;
	}

	if (m_diagrams.empty())
	{
		using namespace genotype_parser;
		PxVec3 material(0,0.75f,0);
		const float shapeSize = 0.3f;

		CGenotypeNode *node1 = CreateBoxDiagram();
		CGenotypeNode *node2 = CreateSphereDiagram();
		CGenotypeNode *node3 = CreateSensorDiagram();

		m_diagrams.push_back(node1);
		m_diagrams.push_back(node2);
		m_diagrams.push_back(node3);
	}

	// create current node
	if (!boost::iequals(m_diagrams.back()->m_name, srcNode->m_name))
	{
		if (m_diagrams.size() > 3)
		{
			SAFE_DELETE(m_diagrams.back());
			m_diagrams.pop_back();
		}

		CGenotypeNode *newCurrentNode = CreateGenotypeNode(m_sample, srcNode->m_expr);
		m_sample.addRenderObject(newCurrentNode->m_renderNode);
		m_diagrams.push_back(newCurrentNode);
	}

	// layout
	const PxVec3 dimension = utility::Vec3toPxVec3(srcNode->m_expr->dimension);
	//const float radius = max(dimension.x*2.f +0.5f, 1.4f);
	const float radius = 1.4f;
	const PxVec3 pos = srcNode->m_renderNode->getTransform().p;
	float radian = 0;
	BOOST_FOREACH (auto node, m_diagrams)
	{
		if (!node)
			continue;

		PxQuat q(radian, PxVec3(0,0,-1));
		PxVec3 offset = q.rotate(PxVec3(0,radius,0));

		node->m_renderNode->setTransform(PxTransform(pos+offset));
		node->m_renderNode->setRendering(isShow);
		radian += 0.9f;
	}
}


/**
 @brief create box
 @date 2014-02-26
*/
CGenotypeNode* CPopupDiagrams::CreateBoxDiagram()
{
	using namespace genotype_parser;
	PxVec3 material(0,0.75f,0);
	const float shapeSize = 0.3f;

	CGenotypeNode *node = new CGenotypeNode(m_sample);
	node->m_name = GenerateId("new Box");
	node->m_renderNode = SAMPLE_NEW2(RenderBoxActor)(*m_sample.getRenderer(), PxVec3(shapeSize,shapeSize,shapeSize));
	node->m_renderNode->setRenderMaterial( m_sample.GetMaterial(material, false) );
	node->m_material = material;
	node->m_isRenderText = false;

	node->m_expr = new SExpr;
	node->m_expr->shape = "box";
	node->m_expr->id = node->m_name;
	node->m_expr->dimension = SVec3(shapeSize,shapeSize,shapeSize);
	node->m_expr->material = utility::PxVec3toVec3(material);

	m_sample.addRenderObject(node->m_renderNode);

	return node;
}


/**
 @brief create sphere
 @date 2014-02-26
*/
CGenotypeNode* CPopupDiagrams::CreateSphereDiagram()
{
	using namespace genotype_parser;
	PxVec3 material(0,0.75f,0);
	const float shapeSize = 0.3f;

	CGenotypeNode *node = new CGenotypeNode(m_sample);
	node->m_name = GenerateId("new Sphere");
	node->m_renderNode = SAMPLE_NEW(RenderSphereActor)(*m_sample.getRenderer(), shapeSize);
	node->m_renderNode->setRenderMaterial( m_sample.GetMaterial(material, false) );
	node->m_material = material;
	node->m_isRenderText = false;

	node->m_expr = new SExpr;
	node->m_expr->shape = "sphere";
	node->m_expr->id = node->m_name;
	node->m_expr->dimension = SVec3(shapeSize,shapeSize,shapeSize);
	node->m_expr->material = utility::PxVec3toVec3(material);

	m_sample.addRenderObject(node->m_renderNode);

	return node;
}


/**
 @brief create sensor
 @date 2014-02-26
*/
CGenotypeNode* CPopupDiagrams::CreateSensorDiagram()
{
	using namespace genotype_parser;
	PxVec3 material(0.75f,0,0);
	const float shapeSize = 0.3f;

	CGenotypeNode *node = new CGenotypeNode(m_sample);
	node->m_name = GenerateId("new Sensor");
	node->m_renderNode = SAMPLE_NEW2(RenderBoxActor)(*m_sample.getRenderer(), PxVec3(shapeSize,shapeSize,shapeSize));
	node->m_renderNode->setRenderMaterial( m_sample.GetMaterial(material, false) );
	node->m_material = material;

	node->m_expr = new SExpr;
	node->m_expr->shape = "sensor";
	node->m_expr->id = node->m_name;
	node->m_expr->dimension = SVec3(shapeSize,shapeSize,shapeSize);
	node->m_expr->material = utility::PxVec3toVec3(material);
	node->m_expr->isSensor = true;

	m_sample.addRenderObject(node->m_renderNode);

	return node;
}


/**
 @brief candiate LinkNode Display Setting
 @date 2014-02-26
*/
void CPopupDiagrams::DisplayCandidateLinkNode(const bool isShow)
{
	BOOST_FOREACH (auto node, m_diagrams)
	{
		if (node)
			node->m_renderNode->setRendering(isShow);
	}
}


/**
 @brief mouse position x,y to check diagram 3d object
 @date 2014-02-25
*/
CGenotypeNode* CPopupDiagrams::PickupDiagram(physx::PxU32 x, physx::PxU32 y, const bool isShowHighLight)
{
	PxVec3 orig, dir, pickOrig;
	m_sample.GetPicking()->computeCameraRay(orig, dir, pickOrig, x, y);

	evc::CGenotypeNode *mouseOverNode = NULL;

	BOOST_FOREACH(auto node, m_diagrams)
	{
		if (!node)
			continue;

		PxVec3 out;
		const bool isHighLight = node->m_renderNode->IntersectTri(pickOrig, dir, out);
		if (isShowHighLight)
			node->SetHighLight(isHighLight);
		if (isHighLight)
			mouseOverNode = node;
	}

	return mouseOverNode;
}


/**
 @brief remove from popup diagrams
 @date 2014-02-26
*/
void CPopupDiagrams::RemoveDiagram(CGenotypeNode *rmNode)
{
	for (u_int i=0; i < m_diagrams.size(); ++i)
	{
		if (m_diagrams[ i] == rmNode)
		{
			switch (i)
			{
			case 0: m_diagrams[ i] = CreateBoxDiagram(); break;
			case 1: m_diagrams[ i] = CreateSphereDiagram(); break;
			case 2: m_diagrams[ i] = CreateSensorDiagram(); break;
			}
			break;
		}
	}
}


/**
 @brief generate unique Id
 @date 2014-02-26
*/
string CPopupDiagrams::GenerateId( const string &seed )
{
	set<string> symbols;
	BOOST_FOREACH (auto node, m_diagramController.GetDiagrams())
		symbols.insert(node->m_name);

	int count = 1;
	string genId = seed;
	while (symbols.end() != symbols.find(genId))
	{
		genId = common::format("%s%d", seed.c_str(), count++);
	}

	return genId;
}
