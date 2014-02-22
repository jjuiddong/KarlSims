
#pragma once

#include "RenderBaseActor.h"

namespace SampleRenderer { class Renderer; }

class CEvc;
class CBone_;
class CRenderBezierActor : public RenderBaseActor
{
public:
	CRenderBezierActor(SampleRenderer::Renderer& renderer, const vector<PxVec3> &points);
	virtual ~CRenderBezierActor();
};
