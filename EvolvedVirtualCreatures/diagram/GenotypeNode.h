/**
 @filename DiagramNode.h
 
*/
#pragma once


class CEvc;
class RenderBezierActor;
namespace evc
{
	class CGenotypeNode;

	// SDiagramConnection struct
	struct SDiagramConnection
	{
		RenderBezierActor *transitionArrow;
		CGenotypeNode *connectNode; // reference
		SDiagramConnection() : transitionArrow(NULL), connectNode(NULL) {}
		SDiagramConnection(CGenotypeNode *node) : connectNode(node), transitionArrow(NULL) {}

		bool operator==(const SDiagramConnection &rhs) {
			return connectNode == rhs.connectNode;
		}
	};


	// CGenotypeNode class
	class CGenotypeNode
	{
	public:
		CGenotypeNode(CEvc &sample);
		virtual ~CGenotypeNode();
		void Move(float dtime);
		void	Render();
		void SetHighLight(const bool highLight);
		void AnimateLayout(const PxVec3 &target);
		void RemoveConnectNode(const CGenotypeNode *rmNode);
		void Show(const bool isShow);
		void SetWorldTransform(const PxTransform &tm);
		const PxTransform& GetWorldTransform() const;
		void SetLocalTransform(const PxTransform &tm);
		const PxTransform& GetLocalTransform() const;
		void UpdateTransform();
		genotype_parser::SConnection* GetJoint(CGenotypeNode *conNode);
		CGenotypeNode* GetConnectNode(const string &nodeName);
		PxVec3 GetPos() const;


	public:
		CEvc &m_sample;
		string m_name;
		PxVec3 m_material;
		RenderBaseActor *m_renderNode;
		vector<SDiagramConnection> m_connectDiagrams;
		genotype_parser::SExpr *m_expr;
		bool m_highLight;
		bool m_isRenderText;
		PxTransform m_worldTM;
		PxTransform m_localTM;

		bool m_isAnimationMove;
		PxVec3 m_targetPos;
		PxVec3 m_initialPos;
		PxVec3 m_moveVelocity;
		float m_elapseTime;
	};

	
	inline const PxTransform& CGenotypeNode::GetWorldTransform() const { return m_worldTM; }
	inline void CGenotypeNode::SetLocalTransform(const PxTransform &tm) { m_localTM = tm; }
	inline const PxTransform& CGenotypeNode::GetLocalTransform() const { return m_localTM; }
}
