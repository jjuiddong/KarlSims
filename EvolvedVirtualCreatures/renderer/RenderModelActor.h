/**
 @filename RenderModel.h
 
*/
#pragma once

#include "RenderBaseActor.h"

namespace SampleRenderer { class Renderer; }

class CRenderModelActor : public RenderBaseActor
{
public:
	CRenderModelActor(SampleRenderer::Renderer& renderer, const string &fileName);
	virtual ~CRenderModelActor();


protected:


private:

};
