/**
 @filename Node.h
  
*/
#pragma once


#include "PhysXSample.h"
class CEvc;

namespace evc
{
	class CJoint;
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
		PxRigidDynamic* GenerateByGenotype( const genotype_parser::SExpr *pexpr, const int recursiveCnt );
		void setCollisionGroup(PxRigidActor* actor, PxU32 group);
		MaterialIndex GetMaterialType(const string &materialStr);


	private:
		CEvc &m_Sample;
		vector<PxRigidDynamic*> m_Rigids;
		vector<CJoint*> m_Joints;
		float m_Force;
		float m_ElapseT;
	};
}
