
#pragma once


class PhysXSample;

namespace evc
{
	class CNode
	{
	public:
		CNode(PhysXSample &sample);
		virtual ~CNode();

		bool Generate();
		bool GenerateHuman();


	protected:


	private:
		PhysXSample &m_Sample;
	};
}
