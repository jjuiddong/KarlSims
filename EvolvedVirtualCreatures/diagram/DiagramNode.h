/**
 @filename DiagramNode.h
 
 
*/
#pragma once


class RenderBaseActor;
namespace evc
{

	class CDiagramNode
	{
	public:
		CDiagramNode();
		virtual ~CDiagramNode();


	protected:


	public:
		RenderBaseActor *m_pRenderNode;
	};

}
