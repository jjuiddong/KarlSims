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
		CAngularSensor() : CSensor(), m_pJoint(NULL) { }
		virtual ~CAngularSensor() {}
		void SetJoint(PxJoint *joint);
		virtual const vector<double>& GetOutput() override;


	private:
		PxJoint *m_pJoint;
	};


	inline void CAngularSensor::SetJoint(PxJoint *joint) { m_pJoint = joint; }

	/**
	 @brief return joint angle
	 @date 2013-12-10
	*/
	inline const vector<double>& CAngularSensor::GetOutput()
	{
		RETV(!m_pJoint, m_Output);
		const PxQuat q = m_pJoint->getRelativeTransform().q;
		double angle = q.getAngle();

		// scale 0 ~ 1.f
		angle = angle / (PxPi/2.f) * 2.f;
		if (angle >= 1.f)
			angle = 1.f;

		if (q.x < 0)
			angle = -angle;

		m_Output[ 0] = angle;
		return m_Output;
	}
}
