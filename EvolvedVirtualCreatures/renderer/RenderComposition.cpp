
#include "stdafx.h"
#include "RenderComposition.h"
#include "RenderCompositionShape.h"


using namespace physx;
using namespace SampleRenderer;

RenderComposition::RenderComposition(SampleRenderer::Renderer& renderer, 
	const PxVec3& pos1, const PxVec3& extents1, 
	const PxVec3& pos2, const PxVec3& extents2,
	const PxReal* uvs)
{
	RendererShape* rs = new RendererCompositionShape(renderer, pos1, extents1, pos2, extents2, uvs);
	setRenderShape(rs);
}

RenderComposition::RenderComposition(SampleRenderer::Renderer& renderer, 
	RendererShape *shape0, const PxTransform &tm0, 
	RendererShape *shape1, const PxTransform &tm1, 
	const PxReal* uvs)
{
	RendererShape* rs = new RendererCompositionShape(renderer, shape0, tm0, shape1, tm1, uvs);
	setRenderShape(rs);
}

RenderComposition::RenderComposition(SampleRenderer::Renderer& renderer, 
	SampleRenderer::RendererShape *shape0 )
{
	RendererShape* rs = new RendererCompositionShape(renderer, shape0);
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
	


	RenderBaseActor::render(renderer, material, wireFrame);
}
