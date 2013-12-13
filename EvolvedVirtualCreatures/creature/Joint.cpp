
#include "stdafx.h"
#include "Joint.h"
#include "Node.h"
#include "AngularSensor.h"
#include "MuscleEffector.h"



using namespace evc;

// joint=NULL, velocity=1.f, period=1.f
CJoint::CJoint(CNode *actor0, CNode *actor1, PxJoint *joint, float velocity, float period) :
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
	//SAFE_RELEASE(m_pJoint);

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
		//if (PxPi < m_ElapseT)
		//{
		//	((PxRevoluteJoint*)m_pJoint)->setDriveVelocity(m_Velocity);
		//}
		//else
		//{
		//	((PxRevoluteJoint*)m_pJoint)->setDriveVelocity(-m_Velocity);
		//}

		//const PxQuat q = m_pJoint->getRelativeTransform().q;
		//m_RelativeAngle = q.getAngle();
		//if (q.x < 0)
		//	m_RelativeAngle = -m_RelativeAngle;
	}
}


/**
 @brief 
 @date 2013-12-10
*/
void CJoint::GetOutputNerves( OUT vector<double> &out) const
{
	if (m_pActor1)
		m_pActor1->GetOutputNerves(out);
}


/**
 @brief return total neuron count
 @date 2013-12-10
*/
int CJoint::GetNeuronCount() const
{
	int count = 0;
	if (m_pActor1)
		count += m_pActor1->GetNeuronCount();
	return count;
}


/**
 @brief 
 @date 2013-12-10
*/
void CJoint::ApplySensor(CAngularSensor &sensor)
{
	sensor.SetJoint(m_pJoint);
}


/**
 @brief 
 @date 2013-12-10
*/
void CJoint::ApplyEffector(CMuscleEffector &effector)
{
	effector.SetJoint(m_pJoint);
}

