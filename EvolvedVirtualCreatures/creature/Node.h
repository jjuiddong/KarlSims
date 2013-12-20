/**
@filename Node.h

*/
#pragma once


#include "PhysXSample.h"
class CEvc;

namespace evc
{
	class CEffector;
	class CSensor;
	class CJoint;
	class CNeuralNet;
	namespace genotype_parser { struct SExpr; }

	class CNode
	{
	public:
		CNode(CEvc &sample);
		virtual ~CNode();
		void InitBrain();
		void GetOutputNerves(OUT vector<double> &out) const;
		int GetNeuronCount() const;
		void Move(float dtime);
		PxRigidDynamic* GetBody();
		CNeuralNet* GetBrain();
		void GetAllSensor(OUT vector<CSensor*> &out) const;
		void GetAllEffector(OUT vector<CEffector*> &out) const;


	protected:
		void setCollisionGroup(PxRigidActor* actor, PxU32 group);
		void UpdateNeuron(float dtime);


	public:
		CEvc &m_Sample;
		string m_Name;
		PxRigidDynamic *m_pBody;
		vector<CJoint*> m_Joints;
		vector<CSensor*> m_Sensors;
		vector<CEffector*> m_Effectors;
		CSensor *m_pParentJointSensor;

		// brain
		CNeuralNet *m_pBrain;
		vector<CSensor*> m_AllConnectSensor;		// 
		vector<CEffector*> m_AllConnectEffector;	// 
		vector<double> m_Nerves;
	};

	
	inline PxRigidDynamic* CNode::GetBody() { return m_pBody; }
	inline CNeuralNet* CNode::GetBrain() { return m_pBrain; }
}
