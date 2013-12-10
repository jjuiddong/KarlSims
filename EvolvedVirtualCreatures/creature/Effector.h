/**
 @filename Effector.h
 
 
*/
#pragma once


namespace evc
{
	class CEffector
	{
	public:
		CEffector(const double period) : m_Period(period), m_ElapseT(0) {}
		virtual ~CEffector() {}
		virtual void Signal(const double dtime, const double signal) = 0;
		void SetPeriod(const double period);


	protected:
		double m_Period;
		double m_ElapseT;
	};


	inline void CEffector::SetPeriod(const double period) { m_Period = period; }
}
