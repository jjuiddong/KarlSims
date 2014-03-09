/**
 @filename RendererBezierShape.h
*/
#pragma once

#include <RendererShape.h>


namespace SampleRenderer
{
	class RendererVertexBuffer;
	class RendererIndexBuffer;

	class RendererBezierShape : public RendererShape
	{
	public:
		RendererBezierShape(Renderer& renderer, const vector<PxVec3> &points, const PxVec3 &color=PxVec3(0,0,0));
		virtual ~RendererBezierShape();
		void SetBezierCurve(const vector<PxVec3> &points, const PxVec3 &color=PxVec3(0,0,0));


	private:
		RendererVertexBuffer* m_vertexBuffer;
		RendererIndexBuffer*	m_indexBuffer;
	};

} // namespace SampleRenderer
