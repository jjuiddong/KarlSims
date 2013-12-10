/**
 @filename AngularSensor.h
 
 
*/
#pragma once

#include "Sensor.h"


namespace evc
{
	class CAngularSensor : public CSensor
	{
	public:
		CAngularSensor() : m_pJoint(NULL) {}
		virtual ~CAngularSensor() {}
		void SetJoint(PxJoint *joint);
		virtual double GetOutput() override;


	private:
		PxJoint *m_pJoint;
	};


	inline void CAngularSensor::SetJoint(PxJoint *joint) { m_pJoint = joint; }

	/**
	 @brief return joint angle
	 @date 2013-12-10
	*/
	inline double CAngularSensor::GetOutput()
	{
		RETV(!m_pJoint, 0);
		const PxQuat q = m_pJoint->getRelativeTransform().q;
		double angle = q.getAngle();
		if (q.x < 0)
			angle = -angle;
		return angle;
	}
}
