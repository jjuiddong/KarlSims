
#include "stdafx.h"
#include "DiagramNode.h"
#include "../EvolvedVirtualCreatures.h"
//#include "Renderer.h"
//#include <d3d9.h>

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


// Render
void	CDiagramNode::Render()
{
	using namespace SampleRenderer;

	//((D3D9Renderer*)m_Sample.getRenderer());//->getShaderEnvironment();

	PxTransform viewTM = m_Sample.getApplication().getCamera().getViewMatrix();
	PxMat44 projTM = m_Sample.getApplication().getCamera().getProjMatrix().getPxMat44();
	PxTransform tm = m_pRenderNode->getTransform();

	PxMat44 m1(tm);
	PxMat44 m2(viewTM);
	PxMat44 mat = projTM * m2.inverseRT() * m1;

	D3DXMATRIX m, vm, pm;
	convert2D3D9(m, tm);
	convert2D3D9(vm, m2.inverseRT());
	convert2D3D9(pm, projTM);

	D3DXMATRIX m0 = pm * vm * m;
	//PxTransform mat = tm * viewTM * projTM;
	//m_Sample.getRenderer()->print(mat.p.x, mat.p.y, m_Name.c_str() );

}
