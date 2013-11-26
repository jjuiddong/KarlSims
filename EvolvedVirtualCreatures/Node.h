
#pragma once


class PhysXSample;


//public 

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


	protected:


	private:
		PhysXSample &m_Sample;
	};
}
