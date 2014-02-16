
#include "stdafx.h"
#include "RenderModelActor.h"
#include "RendererModelShape.h"


using namespace SampleRenderer;

CRenderModelActor::CRenderModelActor(SampleRenderer::Renderer& renderer, const string &fileName)
{
	SBMMLoader *loader = CFileLoader::Get()->LoadModel(fileName);
	if (loader)
	{
		RendererModelShape *rs = new RendererModelShape(renderer, *loader);
		setRenderShape(rs);
	}
}

CRenderModelActor::~CRenderModelActor()
{
}

