
#pragma once

#include <RendererShape.h>


namespace SampleRenderer
{
	class RendererVertexBuffer;
	class RendererIndexBuffer;

	class RendererBezierShape : public RendererShape
	{
	public:
		RendererBezierShape(Renderer& renderer, const vector<PxVec3> &points);
		virtual ~RendererBezierShape();


	private:
		RendererVertexBuffer* m_vertexBuffer;
		RendererIndexBuffer*	m_indexBuffer;
	};

} // namespace SampleRenderer