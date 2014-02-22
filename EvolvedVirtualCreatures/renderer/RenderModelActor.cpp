
#include "stdafx.h"
#include "RenderModelActor.h"
#include "RendererModelShape.h"
#include "RendererSkinModelShape.h"
#include "Bone.h"


using namespace SampleRenderer;


CRenderModelActor::CRenderModelActor(SampleRenderer::Renderer& renderer, const string &fileName) :
	m_isLoadSuccess(false)
,	m_bone(NULL)
{
	SBMMLoader *loader = CFileLoader::Get()->LoadModel(fileName);
	m_aniType = (ANI_TYPE)loader->type;

	if (loader)
	{
		RendererShape *rs = NULL;
		switch (m_aniType)
		{
		case ANI_SKIN: 
			m_bone = new CBone_();
			m_bone->Load(0, &loader->b, false, &loader->m);
			rs = new RendererSkinModelShape(renderer, *loader, m_bone->GetPalette()); 
			break;

		default: 
			rs = new RendererModelShape(renderer, *loader); 
			break;
		}

		setRenderShape(rs);
		m_isLoadSuccess = true;
	}
	else
	{
		m_isLoadSuccess = false;
		setRendering(false);
	}
}

CRenderModelActor::~CRenderModelActor()
{
	SAFE_DELETE(m_bone);
}


void CRenderModelActor::render(SampleRenderer::Renderer& renderer, RenderMaterial* material, bool wireFrame)
	// material=NULL, wireFrame = false
{
	if (ANI_SKIN == m_aniType)
	{
		if (m_bone)
			m_bone->Animate(1);

		//m_bone->GetPalette()[ 0].SetRotationX(1.f);
		//m_bone->GetPalette()[ 1].SetRotationX(.5f);
		//m_bone->GetPalette()[ 2].SetRotationX(.5f);

		RendererSkinModelShape *rs = (RendererSkinModelShape*)getRenderShape();
		if (rs)
			rs->ApplyPalette();
	}

	RenderBaseActor::render(renderer, material,wireFrame);
}
