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
		void Move(float dtime);


	protected:
		void setCollisionGroup(PxRigidActor* actor, PxU32 group);


	public:
		CEvc &m_Sample;
		PxRigidDynamic *m_pBody;
		vector<CJoint*> m_Joints;
		CNeuralNet *m_pBrain;
		vector<float> m_OutputNeuron;

		//float m_Force;
		//float m_ElapseT;
		vector<PxRigidDynamic*> m_Rigids;
	};
}
