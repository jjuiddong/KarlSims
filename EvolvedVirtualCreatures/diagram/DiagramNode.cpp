
#include "stdafx.h"
#include "DiagramNode.h"
#include "../EvolvedVirtualCreatures.h"
//#include "Renderer.h"
//#include <d3d9.h>

using namespace evc;

CDiagramNode::CDiagramNode(CEvc &sample) :
	m_sample(sample)
,	m_renderNode(NULL)
,	m_highLight(false)
,	m_expr(NULL)
{

}

CDiagramNode::~CDiagramNode()
{
	m_sample.removeRenderObject(m_renderNode);
	m_renderNode = NULL;
	genotype_parser::RemoveExpressoin_OnlyExpr(m_expr);
	m_expr = NULL;
}


// Render
void	CDiagramNode::Render()
{
	RET(!m_renderNode);
	RET(!m_renderNode->isRendering());

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
 @date 2014-02-24
*/
void CDiagramNode::SetHighLight(const bool highLight)
{
	if (m_highLight != highLight)
	{
		PxVec3 material = highLight? (m_material+PxVec3(0.25f,0.25f,0.25f)) : m_material;
		m_renderNode->setRenderMaterial( m_sample.GetMaterial(material, false) );

		m_highLight = highLight;
	}
}
