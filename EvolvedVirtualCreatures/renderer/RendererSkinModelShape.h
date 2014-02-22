
#pragma once

#include <RendererShape.h>


namespace SampleRenderer
{
	class RendererVertexBuffer;
	class RendererIndexBuffer;

	class RendererSkinModelShape : public RendererShape
	{
	public:
		RendererSkinModelShape(Renderer& renderer, const SBMMLoader &loader, Matrix44 *palette);
		virtual ~RendererSkinModelShape();
		void ApplyPalette();

	private:
		const SBMMLoader &m_loader;
		const SPhysiqueLoader *m_physiq;

		Matrix44 *m_palette;
		RendererVertexBuffer* m_vertexBuffer;
		RendererIndexBuffer*	m_indexBuffer;
	};

} // namespace SampleRenderer
