
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

	PxTransform viewTM = m_Sample.getApplication().getCamera().getViewMatrix();
	PxTransform tm = m_pRenderNode->getTransform();
	tm.p.x = -tm.p.x; // change left hand axis

	D3DXMATRIX m;
	convertD3D9(m, tm);

	D3DXMATRIX mProj;
	D3DXMatrixPerspectiveFovLH(&mProj, 55.f * PxPi / 180.0f, 800.f/600.f, 0.1f, 10000.f);

	D3DXMATRIX mView;
	{
		D3DXVECTOR3 pos(-viewTM.p.x, viewTM.p.y, viewTM.p.z);
		PxVec3 lookAt = viewTM.p + (m_Sample.getApplication().getCamera().getViewDir() * 10.f);
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

	m_Sample.getRenderer()->print(v.x, v.y, m_Name.c_str() );
}
