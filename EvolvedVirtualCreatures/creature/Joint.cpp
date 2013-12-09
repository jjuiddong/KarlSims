
#include "stdafx.h"
#include "Joint.h"


using namespace evc;

// joint=NULL, velocity=1.f, period=1.f
CJoint::CJoint(PxRigidDynamic *actor0, PxRigidDynamic *actor1, PxJoint *joint, float velocity, float period) :
	m_pJoint(joint)
,	m_pActor0(actor0)
,	m_pActor1(actor1)
,	m_Velocity(velocity)
,	m_Period((period==0)? 1.f : period)
,	m_ElapseT(0.f)
,	m_RelativeAngle(0)
{

}

CJoint::~CJoint()
{

}


/**
 @brief 
 @date 2013-12-08
*/
void CJoint::Move(float dtime)
{
	RET(!m_pJoint);

	m_ElapseT += (dtime*m_Period);
	if (PxPi*2 < m_ElapseT)
		m_ElapseT = 0.f;

	if (PxJointConcreteType::eREVOLUTE == m_pJoint->getType())
	{
		if (PxPi < m_ElapseT)
		{
			((PxRevoluteJoint*)m_pJoint)->setDriveVelocity(m_Velocity);
		}
		else
		{
			((PxRevoluteJoint*)m_pJoint)->setDriveVelocity(-m_Velocity);
		}

		const PxQuat q = m_pJoint->getRelativeTransform().q;
		m_RelativeAngle = q.getAngle();
		if (q.x < 0)
			m_RelativeAngle = -m_RelativeAngle;
	}
}

