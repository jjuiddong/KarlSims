
#include "stdafx.h"
#include "RenderBezierActor.h"
#include "RendererBezierShape.h"


using namespace SampleRenderer;


RenderBezierActor::RenderBezierActor(SampleRenderer::Renderer& renderer, const vector<PxVec3> &points, const PxVec3 &color)
{
	RendererShape *rs = new RendererBezierShape(renderer, points, color);
	setRenderShape(rs);
	m_color = color;
}

RenderBezierActor::~RenderBezierActor()
{

}

void RenderBezierActor::SetBezierCurve(const vector<PxVec3> &points, const PxVec3 &color)
{
	((RendererBezierShape*)getRenderShape())->SetBezierCurve(points, (color.isZero()? m_color : color));
}
