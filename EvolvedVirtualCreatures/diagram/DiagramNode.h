/**
 @filename DiagramNode.h
 
*/
#pragma once


class CEvc;
class RenderBaseActor;
namespace evc
{

	class CDiagramNode;
	struct SDiagramConnection
	{
		RenderBaseActor *transitionArrow;
		CDiagramNode *connectNode;

		SDiagramConnection() : transitionArrow(NULL), connectNode(NULL) {}
	};


	class CDiagramNode
	{
	public:
		CDiagramNode(CEvc &sample);
		virtual ~CDiagramNode();


	protected:


	public:
		CEvc &m_Sample;
		//vector<CDiagramNode*> m_ConnectNodes;
		vector<SDiagramConnection> m_ConnectDiagrams;

		RenderBaseActor *m_pRenderNode;
	};

}
