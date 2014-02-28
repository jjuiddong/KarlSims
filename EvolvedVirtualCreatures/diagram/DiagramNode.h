/**
 @filename DiagramNode.h
 
*/
#pragma once


class CEvc;
class RenderBezierActor;
namespace evc
{
	class CDiagramNode;

	// SDiagramConnection struct
	struct SDiagramConnection
	{
		RenderBezierActor *transitionArrow;
		CDiagramNode *connectNode; // reference
		SDiagramConnection() : transitionArrow(NULL), connectNode(NULL) {}
		SDiagramConnection(CDiagramNode *node) : connectNode(node), transitionArrow(NULL) {}

		bool operator==(const SDiagramConnection &rhs) {
			return connectNode == rhs.connectNode;
		}
	};


	// CDiagramNode class
	class CDiagramNode
	{
	public:
		CDiagramNode(CEvc &sample);
		virtual ~CDiagramNode();
		void Move(float dtime);
		void	Render();
		void SetHighLight(const bool highLight);
		void AnimateLayout(const PxVec3 &target);
		void RemoveConnectNode(const CDiagramNode *rmNode);


	public:
		CEvc &m_sample;
		string m_name;
		PxVec3 m_material;
		RenderBaseActor *m_renderNode;
		vector<SDiagramConnection> m_connectDiagrams;
		genotype_parser::SExpr *m_expr;
		bool m_highLight;
		bool m_isRenderText;

		bool m_isAnimationMove;
		PxVec3 m_targetPos;
		PxVec3 m_initialPos;
		PxVec3 m_moveVelocity;
		float m_elapseTime;		
	};
}
