/**
 @filename DiagramNode.h
 
*/
#pragma once


class CEvc;
class RenderBaseActor;
namespace evc
{
	class CDiagramNode;

	// SDiagramConnection struct
	struct SDiagramConnection
	{
		RenderBaseActor *transitionArrow;
		CDiagramNode *connectNode;
		SDiagramConnection() : transitionArrow(NULL), connectNode(NULL) {}
	};


	// CDiagramNode class
	class CDiagramNode
	{
	public:
		CDiagramNode(CEvc &sample);
		virtual ~CDiagramNode();
		void	Render();


	public:
		CEvc &m_Sample;
		string m_Name;
		RenderBaseActor *m_pRenderNode;
		vector<SDiagramConnection> m_ConnectDiagrams;
	};

}
