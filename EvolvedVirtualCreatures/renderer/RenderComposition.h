/**
 @filename RenderComposition.h
 
*/
#pragma once


#include "RenderBaseActor.h"
#include "foundation/PxVec3.h"

namespace SampleRenderer { class Renderer; class RendererShape; }

class RenderComposition : public RenderBaseActor
{
public:
	RenderComposition(SampleRenderer::Renderer& renderer, 
		const PxVec3& pos1, const PxVec3& extents1, 
		const PxVec3& pos2, const PxVec3& extents2,
		const PxReal* uvs=NULL);

	RenderComposition(SampleRenderer::Renderer& renderer, 
		SampleRenderer::RendererShape *shape0, const PxTransform &tm0, 
		SampleRenderer::RendererShape *shape1, const PxTransform &tm1, 
		const PxReal* uvs=NULL);

	RenderComposition(SampleRenderer::Renderer& renderer, 
		SampleRenderer::RendererShape *shape0 );

	RenderComposition(const RenderComposition&);
	virtual ~RenderComposition();

	virtual void render(SampleRenderer::Renderer& renderer, RenderMaterial* material=NULL, bool wireFrame = false) override;

};
