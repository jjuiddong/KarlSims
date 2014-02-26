
#pragma once

#include "RenderBaseActor.h"

namespace SampleRenderer { class Renderer; }

class CEvc;
class CBone_;
class RenderBezierActor : public RenderBaseActor
{
public:
	RenderBezierActor(SampleRenderer::Renderer& renderer, const vector<PxVec3> &points);
	virtual ~RenderBezierActor();
	void SetBezierCurve(const vector<PxVec3> &points);
};
