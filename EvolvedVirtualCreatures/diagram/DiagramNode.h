/**
 @filename DiagramNode.h
 
*/
#pragma once


class CEvc;
class RenderBaseActor;
namespace evc
{

	class CDiagramNode
	{
	public:
		CDiagramNode(CEvc &sample);
		virtual ~CDiagramNode();


	protected:


	public:
		CEvc &m_Sample;
		vector<CDiagramNode*> m_ConnectNodes;
		RenderBaseActor *m_pRenderNode;
	};

}
