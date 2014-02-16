
#pragma once

#include <RendererShape.h>


namespace SampleRenderer
{
	class RendererVertexBuffer;
	class RendererIndexBuffer;

	class RendererModelShape : public RendererShape
	{
	public:
		RendererModelShape(Renderer& renderer, const SBMMLoader &loader);
		virtual ~RendererModelShape();

	private:
		RendererVertexBuffer* m_vertexBuffer;
		RendererIndexBuffer*	m_indexBuffer;
	};

} // namespace SampleRenderer
