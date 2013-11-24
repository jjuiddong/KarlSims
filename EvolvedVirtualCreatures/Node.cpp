
#include "stdafx.h"
#include "node.h"
#include "PhysXSample.h"


using namespace evc;

CNode::CNode(PhysXSample &sample) :
	m_Sample(sample)
{

}

CNode::~CNode()
{

}


bool CNode::Generate()
{
	const PxVec3 pos = m_Sample.getCamera().getPos();
	const PxVec3 vel = m_Sample.getCamera().getViewDir() * 20.f;

	PxRigidDynamic* actor1 = NULL;
	//actor1 = m_Sample.createSphere(pos, 0.3f, &vel, m_Sample.getManageMaterial(rand()%5), 1.f);
	actor1 = m_Sample.createBox(pos, PxVec3(1.f, 0.5f, 0.3f), &vel, m_Sample.getManageMaterial(MATERIAL_RED), 1.f);

	PxRigidDynamic* actor2 = NULL;
	PxVec3 pos2 = m_Sample.getCamera().getPos();
	//pos2.x += 1.f;
	//actor2 = m_Sample.createSphere(pos2, 0.3f, &vel, m_Sample.getManageMaterial(MATERIAL_GREEN), 1.f);
	actor2 = m_Sample.createBox(pos2, PxVec3(0.3f, 0.5f, 1.0f), &vel, m_Sample.getManageMaterial(MATERIAL_GREEN), 1.f);

	 // Joint
	const float scale = 1.f;
	const float plankDepth = 1.f;
	//PxRevoluteJoint* j = PxRevoluteJointCreate(m_Sample.getPhysics(), 
	PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
	//PxSphericalJoint* j = PxSphericalJointCreate(m_Sample.getPhysics(), 
		actor1, PxTransform(PxVec3(0,0, plankDepth)*scale),
		actor2, PxTransform(PxVec3(0,0,-plankDepth)*scale));
	if(j)
		j->setProjectionLinearTolerance(.5f);

	return true;
}


bool CNode::GenerateHuman(const bool flag)
{
	const PxVec3 pos = m_Sample.getCamera().getPos();
	const PxVec3 vel = m_Sample.getCamera().getViewDir() * 20.f;

	//PxRigidDynamic* actor1 = m_Sample.createBox(pos, PxVec3(0.3f, 0.3f, 0.3f), &vel, m_Sample.getManageMaterial(MATERIAL_RED), 1.f);
	//PxRigidDynamic* actor2 = m_Sample.createBox(pos, PxVec3(0.8f, 0.3f, 0.8f), &vel, m_Sample.getManageMaterial(MATERIAL_YELLOW), 1.f);
	//PxRigidDynamic* actor3 = m_Sample.createBox(pos, PxVec3(1.f, 0.3f, 0.3f), &vel, m_Sample.getManageMaterial(MATERIAL_GREEN), 1.f);
	//PxRigidDynamic* actor3_1 = m_Sample.createBox(pos, PxVec3(0.1f, 0.1f, 0.1f), &vel, m_Sample.getManageMaterial(MATERIAL_RED), 1.f);
	//PxRigidDynamic* actor4 = m_Sample.createBox(pos, PxVec3(1.f, 0.3f, 0.3f), &vel, m_Sample.getManageMaterial(MATERIAL_BLUE), 1.f);
	//PxRigidDynamic* actor4_1 = m_Sample.createBox(pos, PxVec3(0.1f, 0.1f, 0.1f), &vel, m_Sample.getManageMaterial(MATERIAL_RED), 1.f);
	//PxRigidDynamic* actor5 = m_Sample.createBox(pos, PxVec3(1.f, 0.3f, 0.3f), &vel, m_Sample.getManageMaterial(MATERIAL_GREEN), 1.f);
	//PxRigidDynamic* actor6 = m_Sample.createBox(pos, PxVec3(1.f, 0.3f, 0.3f), &vel, m_Sample.getManageMaterial(MATERIAL_BLUE), 1.f);
	PxRigidDynamic* actor7 = m_Sample.createBox(pos, PxVec3(0.3f, 0.3f, 0.3f), &vel, m_Sample.getManageMaterial(MATERIAL_GREEN), 1.f);
	PxRigidDynamic* actor8 = m_Sample.createBox(pos, PxVec3(0.3f, 0.3f, 0.3f), &vel, m_Sample.getManageMaterial(MATERIAL_BLUE), 1.f);

	PxRigidDynamic* actor9 = m_Sample.createBox(PxVec3(pos.x-1.4f, pos.y, pos.z), PxVec3(1, 0.3f, 0.3f), NULL, m_Sample.getManageMaterial(MATERIAL_BLUE), 1.f);
	PxRigidDynamic* actor10 = m_Sample.createBox(PxVec3(pos.x+1.4f, pos.y, pos.z), PxVec3(1, 0.3f, 0.3f), NULL, m_Sample.getManageMaterial(MATERIAL_RED), 1.f);

	const float scale = 1.f;
	//if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
	//	actor1, PxTransform(PxVec3(0,0,1.4f)*scale),
	//	actor2, PxTransform(PxVec3(0,0,0)*scale)))
	//{
	//	j->setProjectionLinearTolerance(.5f);
	//}

	// left arm
	if (PxSphericalJoint* j = PxSphericalJointCreate(m_Sample.getPhysics(), 
		actor7, PxTransform(PxVec3(0,0,0)*scale),
		actor8, PxTransform(PxVec3(-.7f,0,0)*scale)))
	{
		if (flag)
		{
			j->setLimitCone(PxJointLimitCone(PxPi/2, PxPi/6, 0.01f));
			j->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
		}
		j->setProjectionLinearTolerance(0.5f);
	}

	if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
		actor9, PxTransform(PxVec3(1.4f,0,0)*scale),
		actor7, PxTransform(PxVec3(0,0,0)*scale)))
	{
		j->setProjectionLinearTolerance(.5f);
	}

	if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
		actor10, PxTransform(PxVec3(-1.4f,0,0)*scale),
		actor8, PxTransform(PxVec3(0,0,0)*scale)))
	{
		j->setProjectionLinearTolerance(.5f);
	}

	//// left arm
	//if (PxRevoluteJoint* j = PxRevoluteJointCreate(m_Sample.getPhysics(), 
	//	actor2, PxTransform(PxVec3(0,0,0)*scale),
	//	actor3_1, PxTransform(PxVec3(1.f,0, 0.8f)*scale)))
	//{
	//	PxJointAngularLimitPair limitPair(-PxPi/20.f, PxPi/20.f, 0.1f);
	//	j->setLimit(limitPair);
	//	j->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, true);
	//	j->setDriveVelocity(10.0f);
	//	j->setProjectionLinearTolerance(0.5f);
	//}

	//if (PxSphericalJoint* j = PxSphericalJointCreate(m_Sample.getPhysics(), 
	//	actor3_1, PxTransform(PxVec3(0,0,0)*scale),
	//	actor3, PxTransform(PxVec3(2.f,0, 0.8f)*scale)))
	//{
	//	if (flag)
	//	{
	//		//PxJointAngularLimitPair limitPair(-PxPi/4, PxPi/4, 0.1f);
	//		//j->setLimit(limitPair);
	//		//j->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, true);

	//		j->setLimitCone(PxJointLimitCone(PxPi/2, PxPi/6, 0.01f));
	//		j->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
	//	}

	//	j->setProjectionLinearTolerance(.5f);
	//}


	// righ arm
	//if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
	//	actor2, PxTransform(PxVec3(0,0,0)*scale),
	//	actor4_1, PxTransform(PxVec3(-0.95f,0, 0.8f)*scale)))
	//{
	//	j->setProjectionLinearTolerance(.5f);
	//}

	//if (PxSphericalJoint* j = PxSphericalJointCreate(m_Sample.getPhysics(), 
	//	actor4_1, PxTransform(PxVec3(0,0,0)*scale),
	//	actor4, PxTransform(PxVec3(-1.f,0, 0.8f)*scale)))
	//{
	//	j->setProjectionLinearTolerance(.5f);
	//}



	//PxTransform m1(PxVec3(0,0,0), PxQuat(3.14f/2.f, PxVec3(0,1,0)));
	//PxTransform m2(PxVec3(0.5f, 0, -2.f));

	//if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
	//	actor2, PxTransform(PxVec3(0,0,0)*scale),
	//	actor5, m1*m2))
	//{
	//	j->setProjectionLinearTolerance(.5f);
	//}

	//if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
	//	actor2, PxTransform(PxVec3(0,0,0)*scale),
	//	actor6, m1*PxTransform(PxVec3(-0.5f, 0, -2.f))) )
	//{
	//	j->setProjectionLinearTolerance(.5f);
	//}

	return true;
}
