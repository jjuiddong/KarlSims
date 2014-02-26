
#include "stdafx.h"
#include "RenderBezierActor.h"
#include "RendererBezierShape.h"


using namespace SampleRenderer;


RenderBezierActor::RenderBezierActor(SampleRenderer::Renderer& renderer, const vector<PxVec3> &points)
{
	RendererShape *rs = new RendererBezierShape(renderer, points);
	setRenderShape(rs);
}

RenderBezierActor::~RenderBezierActor()
{

}

void RenderBezierActor::SetBezierCurve(const vector<PxVec3> &points)
{
	((RendererBezierShape*)getRenderShape())->SetBezierCurve(points);
}
