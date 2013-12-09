/**
 @filename Joint.h
 
  joint class
*/
#pragma once


namespace evc
{
	class CJoint
	{
	public:
		CJoint(PxRigidDynamic *actor0, PxRigidDynamic *actor1, PxJoint *joint=NULL, float velocity=1.f, float period=1.f);
		virtual ~CJoint();
		void Move(float dtime);
		float GetAngle() const;


	private:
		PxRigidDynamic *m_pActor0;
		PxRigidDynamic *m_pActor1;
		PxJoint *m_pJoint;
		float m_Velocity;
		float m_Period; // (dtime*m_Period) =  dtime
		float m_ElapseT;
		float m_RelativeAngle; // radian, -pi ~ 0 ~ pi
	};


	inline float CJoint::GetAngle() const { return m_RelativeAngle; }
}
