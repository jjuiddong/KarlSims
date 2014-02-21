
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

	if (ANI_SKIN == m_aniType)
	{
		m_bone = new CBone_();
		m_bone->Load(0, &loader->b, false, &loader->m);
	}

	if (loader)
	{
		RendererShape *rs = NULL;
		switch (m_aniType)
		{
		case ANI_SKIN: new RendererSkinModelShape(renderer, *loader, m_bone->GetPalette()); break;
		default: rs = rs = new RendererModelShape(renderer, *loader); break;
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
		((RendererSkinModelShape*)getRenderShape())->ApplyPalette();
	}

	RenderBaseActor::render(renderer, material,wireFrame);
}


void CRenderModelActor::update(float deltaTime)
{
	RenderBaseActor::update(deltaTime);
	if (ANI_SKIN == m_aniType)
	{
		if (m_bone)
			m_bone->Animate((int)(deltaTime*1000.f));
	}
}
