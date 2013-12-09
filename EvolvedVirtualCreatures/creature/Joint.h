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
		CJoint(PxJoint *joint=NULL, float velocity=1.f, float period=1.f);
		virtual ~CJoint();
		void Move(float dtime);
		

	private:
		PxJoint *m_pJoint;
		float m_Velocity;
		float m_Period; // (dtime*m_Period) =  dtime
		float m_ElapseT;
	};
}
