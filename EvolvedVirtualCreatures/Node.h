
#pragma once


class PhysXSample;

namespace evc
{
	DECLARE_TYPE_NAME_SCOPE(evc, CNode)

	class CNode : public memmonitor::Monitor<CNode, TYPE_NAME(CNode)>
	{
	public:
		CNode(PhysXSample &sample);
		virtual ~CNode();

		bool Generate();
		bool GenerateHuman(const bool flag);
		bool GenerateHuman2(const bool flag);
		bool GenerateHuman3(const bool flag);
		bool GenerateHuman4(const bool flag);
		bool GenerateHuman5(const bool flag);

		void Move(float dtime);


	protected:


	private:
		PhysXSample &m_Sample;

		PxRigidDynamic* m_pHead;
		vector<PxRigidDynamic*> m_Joints;
		float m_Force;
		float m_ElapseT;

	};
}
