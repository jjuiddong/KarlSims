
#include "stdafx.h"
#include "RenderComposition.h"
#include "RenderCompositionShape.h"


using namespace physx;
using namespace SampleRenderer;


/**
 @brief 
 @date 2014-01-06
*/
RenderComposition::RenderComposition(SampleRenderer::Renderer& renderer, 
	const int paletteIndex,
	const vector<PxTransform> &tmPalette,
	RendererCompositionShape *shape0, const PxTransform &tm0, 
	RendererCompositionShape *shape1, const PxTransform &tm1, 
	const PxReal* uvs) :
	m_PaletteIndex(paletteIndex)
{
	RendererShape* rs = new RendererCompositionShape(renderer, paletteIndex, tmPalette, shape0, tm0, shape1, tm1, uvs);
	setRenderShape(rs);
}


/**
 @brief 
 @date 2014-01-06
*/
RenderComposition::RenderComposition(SampleRenderer::Renderer& renderer, 
	const int paletteIndex, const vector<PxTransform> &tmPalette, SampleRenderer::RendererShape *shape0 ) :
	m_PaletteIndex(paletteIndex)
{
	RendererShape* rs = new RendererCompositionShape(renderer, paletteIndex, tmPalette, shape0);
	setRenderShape(rs);
}


RenderComposition::RenderComposition(const RenderComposition& src) : RenderBaseActor(src)
{
}

RenderComposition::~RenderComposition()
{
}


/**
 @brief 
 @date 2014-01-05
*/
void RenderComposition::render(SampleRenderer::Renderer& renderer, RenderMaterial* material, bool wireFrame)
{
	((RendererCompositionShape*)getRenderShape())->ApplyPalette();
	RenderBaseActor::render(renderer, material, wireFrame);
}
