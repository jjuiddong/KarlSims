/**
 @filename Node.h
 
 
*/
#pragma once


class PhysXSample;
class CEvc;

namespace evc
{
	namespace genotype_parser { struct SExpr; }

	DECLARE_TYPE_NAME_SCOPE(evc, CNode)
	class CNode : public memmonitor::Monitor<CNode, TYPE_NAME(CNode)>
	{
	public:
		CNode(CEvc &sample);
		virtual ~CNode();

		bool Generate();
		bool GenerateHuman(const bool flag);
		bool GenerateHuman2(const bool flag);
		bool GenerateHuman3(const bool flag);
		bool GenerateHuman4(const bool flag);
		bool GenerateHuman5(const bool flag);
		bool GenerateHuman6(const bool flag);
		bool GenerateHuman7(const bool flag); // Revolution Join
		bool GenerateHuman8(const bool flag); // creature
		bool GenerateHuman9(const bool flag); // load by genotype script

		void Move(float dtime);


	protected:
		void GenerateByGenotype( PxRigidDynamic *parent, const genotype_parser::SExpr *pexpr, const int recursiveCnt );
		void setCollisionGroup(PxRigidActor* actor, PxU32 group);


	private:
		CEvc &m_Sample;
		PxRigidDynamic* m_pHead;
		vector<PxRigidDynamic*> m_Rigids;
		vector<PxJoint*> m_Joints;
		PxRevoluteJoint *m_RevJoint;
		float m_Force;
		float m_ElapseT;
		float m_Vel_Joint1;
		float m_Vel_Joint2;

	};
}
