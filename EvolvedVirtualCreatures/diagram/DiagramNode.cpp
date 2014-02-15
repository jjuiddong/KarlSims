
#include "stdafx.h"
#include "DiagramNode.h"
#include "../EvolvedVirtualCreatures.h"


using namespace evc;

CDiagramNode::CDiagramNode(CEvc &sample) :
	m_Sample(sample)
,	m_pRenderNode(NULL)
{

}

CDiagramNode::~CDiagramNode()
{
	m_Sample.removeRenderObject(m_pRenderNode);
	m_pRenderNode = NULL;
}
