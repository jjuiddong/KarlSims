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
		CMuscleEffector() : m_pJoint(NULL) {}
		virtual void Signal(const double signal) override;
		void SetJoint(PxJoint *joint);


	private:
		PxJoint *m_pJoint;
	};


	inline void CMuscleEffector::SetJoint(PxJoint *joint) { m_pJoint = joint; }


	/**
	 @brief 
	@date 2013-12-10
	*/
	inline void CMuscleEffector::Signal(const double signal)
	{
		RET(!m_pJoint);
		((PxRevoluteJoint*)m_pJoint)->setDriveVelocity(signal);
	}
}
