
#include "stdafx.h"
#include "RenderBezierActor.h"
#include "RendererBezierShape.h"


using namespace SampleRenderer;


CRenderBezierActor::CRenderBezierActor(SampleRenderer::Renderer& renderer, const vector<PxVec3> &points)
{
	RendererShape *rs = new RendererBezierShape(renderer, points);
	setRenderShape(rs);
}

CRenderBezierActor::~CRenderBezierActor()
{

}
