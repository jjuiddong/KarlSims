/**
 @filename RenderModel.h
 
*/
#pragma once

#include "RenderBaseActor.h"

namespace SampleRenderer { class Renderer; }

class CBone_;
class CRenderModelActor : public RenderBaseActor
{
public:
	CRenderModelActor(SampleRenderer::Renderer& renderer, const string &fileName);
	virtual ~CRenderModelActor();
	bool IsLoadSuccess();
	virtual void update(float deltaTime) override;
	virtual void render(SampleRenderer::Renderer& renderer, RenderMaterial* material=NULL, bool wireFrame = false) override;


private:
	bool m_isLoadSuccess;
	CBone_ *m_bone;
	ANI_TYPE m_aniType;
};


inline bool CRenderModelActor::IsLoadSuccess() { return m_isLoadSuccess; }
