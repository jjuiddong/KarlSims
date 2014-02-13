
#include "stdafx.h"
#include "DiagramController.h"
#include "DiagramNode.h"
#include "../EvolvedVirtualCreatures.h"
#include "RenderBoxActor.h"


using namespace evc;

CDiagramController::CDiagramController(CEvc &sample) :
	m_Sample(sample)
,	m_pRootDiagram(NULL)
{

}

CDiagramController::~CDiagramController()
{

}


/**
 @brief 
 @date 2014-02-12
*/
void CDiagramController::SetGenotype(const genotype_parser::SExpr *expr)
{
	RemoveDiagram(m_pRootDiagram);
	m_pRootDiagram = CreateDiagramNode(expr);
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
CDiagramNode* CDiagramController::CreateDiagramNode(const genotype_parser::SExpr *expr)
{
	RETV(!expr, NULL);

	CDiagramNode *diagNode = new CDiagramNode();
	diagNode->m_pRenderNode = SAMPLE_NEW2(RenderBoxActor)(*m_Sample.getRenderer(), PxVec3(1,1,1));

	m_Sample.addRenderObject(diagNode->m_pRenderNode);

	return NULL;
}


/**
 @brief 
 @date 2014-02-12
*/
void CDiagramController::RemoveDiagram(CDiagramNode *node)
{
	RET(!node);

}
