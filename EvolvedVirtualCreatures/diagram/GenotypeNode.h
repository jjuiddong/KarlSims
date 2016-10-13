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

		void UpdateTransform(const PxTransform &parentTm=PxTransform::createIdentity());
		void SetLocalPosition(const PxVec3 &pos);
		void SetLocalRotate(const PxQuat &q);
		PxVec3 GetLocalPosition() const;
		PxQuat GetLocalRotate() const;

		genotype_parser::SConnection* GetJoint(CGenotypeNode *conNode);
		CGenotypeNode* GetConnectNode(const string &nodeName);
		PxVec3 GetPos() const;
		PxTransform GetTransform() const;
		void SetTransform(const PxTransform &tm);


	protected:
		void UpdateTransformRec(const PxTransform &parentTm, set<CGenotypeNode*> &symbols);


	public:
		CEvc &m_sample;
		string m_name;
		PxVec3 m_material;
		RenderBaseActor *m_renderNode;
		vector<SDiagramConnection> m_connectDiagrams;
		genotype_parser::SExpr *m_expr;
		bool m_highLight;
		bool m_isRenderText;
		PxVec3 m_localPos;
		PxQuat m_localRotate;

		bool m_isAnimationMove;
		PxVec3 m_targetPos;
		PxVec3 m_initialPos;
		PxVec3 m_moveVelocity;
		float m_elapseTime;
	};

	
	inline PxVec3 CGenotypeNode::GetLocalPosition() const { return m_localPos; }
	inline PxQuat CGenotypeNode::GetLocalRotate() const { return m_localRotate; }
}
