/**
@filename Node.h

*/
#pragma once


#include "PhysXSample.h"
class CEvc;

namespace evc
{
	class CJoint;
	class CNeuralNet;
	namespace genotype_parser { struct SExpr; }

	class CNode
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


	//private:
	public:
		CEvc &m_Sample;
		PxRigidDynamic *m_pBody;
		vector<PxRigidDynamic*> m_Rigids;
		vector<CJoint*> m_Joints;
		CNeuralNet *m_pBrain;
		//float m_Force;
		//float m_ElapseT;
	};
}
