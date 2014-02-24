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
		void SetHighLight(const bool highLight);


	public:
		CEvc &m_sample;
		string m_name;
		PxVec3 m_material;
		RenderBaseActor *m_renderNode;
		vector<SDiagramConnection> m_connectDiagrams;
		bool m_highLight;
	};

}
