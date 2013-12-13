/**
 @filename MuscleEffector.h
 
  muscle effector using joint
*/
#pragma once

#include "Effector.h"


namespace evc
{
	class CMuscleEffector : public CEffector
	{
	public:
		CMuscleEffector(const double period) : CEffector(period), m_pJoint(NULL), m_Limit(0,0,0) {}
		virtual void Signal(const double dtime, const double signal) override;
		void SetJoint(PxJoint *joint);


	private:
		PxJoint *m_pJoint;
		PxJointAngularLimitPair m_Limit;
	};


	inline void CMuscleEffector::SetJoint(PxJoint *joint) 
	{
		RET(!joint);
		m_pJoint = joint;
		if (PxJointConcreteType::eREVOLUTE == m_pJoint->getType())
		{
			m_Limit = ((PxRevoluteJoint*)m_pJoint)->getLimit();
		}
	}


	/**
	 @brief 
	@date 2013-12-10
	*/
	inline void CMuscleEffector::Signal(const double dtime, const double signal)
	{
		m_ElapseT += (dtime*m_Period);
		if (PxPi*2 < m_ElapseT)
			m_ElapseT = 0.f;

		RET(!m_pJoint);
		if (PxJointConcreteType::eREVOLUTE == m_pJoint->getType())
		{
			float velocity = signal * 5.f;

			//if (PxPi < m_ElapseT)
			//{
				((PxRevoluteJoint*)m_pJoint)->setDriveVelocity(velocity);
			//}
			//else
			//{
			//	((PxRevoluteJoint*)m_pJoint)->setDriveVelocity(-velocity);
			//}
		}
	}
}
