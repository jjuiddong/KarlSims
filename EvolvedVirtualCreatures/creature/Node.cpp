#include "stdafx.h"
#include "node.h"
#include "../EvolvedVirtualCreatures.h"
#include "../genoype/GenotypeParser.h"
#include "Joint.h"
#include "NeuralNet.h"


using namespace evc;


void setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask)
{
	PxFilterData filterData;
	filterData.word0 = filterGroup; // word0 = own ID
	filterData.word1 = filterMask;	// word1 = ID mask to filter pairs that trigger a contact callback;
	const PxU32 numShapes = actor->getNbShapes();
	PxShape** shapes = (PxShape**)SAMPLE_ALLOC(sizeof(PxShape*)*numShapes);
	actor->getShapes(shapes, numShapes);
	for(PxU32 i = 0; i < numShapes; i++)
	{
		PxShape* shape = shapes[i];
		shape->setSimulationFilterData(filterData);
	}
	SAMPLE_FREE(shapes);
}


CNode::CNode(CEvc &sample) :
	m_Sample(sample)
//,	m_Force(1.f)
//,	m_ElapseT(0)
,	m_pBrain(NULL)
,	m_pBody(NULL)
{
	m_pBrain = new CNeuralNet(2, 1, 0, 0); 

}

CNode::~CNode()
{
	SAFE_DELETE(m_pBrain);
	BOOST_FOREACH (auto joint, m_Joints)
	{
		SAFE_DELETE(joint);
	}
}


/**
 @brief 
 @date 2013-12-05
*/
void CNode::setCollisionGroup(PxRigidActor* actor, PxU32 group)
{
	PxSceneWriteLock scopedLock(m_Sample.getActiveScene());

	PxU32 nbShapes = actor->getNbShapes();
	if( nbShapes )
	{
		SampleArray<PxShape*> shapes(nbShapes);
		actor->getShapes( &shapes[0], nbShapes);
		for( PxU32 j = 0; j < nbShapes; j++)
		{
			PxFilterData fd = shapes[j]->getSimulationFilterData();
			fd.word0 = group;
			shapes[j]->setSimulationFilterData(fd);
		}
	}
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
	const PxVec3 pos = m_Sample.getCamera().getPos() + (m_Sample.getCamera().getViewDir()*10.f);
	const PxVec3 vel = m_Sample.getCamera().getViewDir() * 20.f;

	//PxRigidDynamic* actor1 = m_Sample.createBox(pos, PxVec3(0.3f, 0.3f, 0.3f), &vel, m_Sample.getManageMaterial(MATERIAL_RED), 1.f);
	//PxRigidDynamic* actor2 = m_Sample.createBox(pos, PxVec3(0.8f, 0.3f, 0.8f), &vel, m_Sample.getManageMaterial(MATERIAL_YELLOW), 1.f);
	//PxRigidDynamic* actor3 = m_Sample.createBox(pos, PxVec3(1.f, 0.3f, 0.3f), &vel, m_Sample.getManageMaterial(MATERIAL_GREEN), 1.f);
	//PxRigidDynamic* actor3_1 = m_Sample.createBox(pos, PxVec3(0.1f, 0.1f, 0.1f), &vel, m_Sample.getManageMaterial(MATERIAL_RED), 1.f);
	//PxRigidDynamic* actor4 = m_Sample.createBox(pos, PxVec3(1.f, 0.3f, 0.3f), &vel, m_Sample.getManageMaterial(MATERIAL_BLUE), 1.f);
	//PxRigidDynamic* actor4_1 = m_Sample.createBox(pos, PxVec3(0.1f, 0.1f, 0.1f), &vel, m_Sample.getManageMaterial(MATERIAL_RED), 1.f);
	//PxRigidDynamic* actor5 = m_Sample.createBox(pos, PxVec3(1.f, 0.3f, 0.3f), &vel, m_Sample.getManageMaterial(MATERIAL_GREEN), 1.f);
	//PxRigidDynamic* actor6 = m_Sample.createBox(pos, PxVec3(1.f, 0.3f, 0.3f), &vel, m_Sample.getManageMaterial(MATERIAL_BLUE), 1.f);

	PxRigidDynamic* head = m_Sample.createSphere(pos+PxVec3(0,0,1.6f), 0.5f, NULL, m_Sample.getManageMaterial(MATERIAL_GREEN), 1.f);
	PxRigidDynamic* body = m_Sample.createBox(pos, PxVec3(1,.3f,1), NULL, m_Sample.getManageMaterial(MATERIAL_YELLOW), 1.f);

	PxRigidDynamic* left_shoulder_joint1 = m_Sample.createSphere(pos+PxVec3(1.4f,0,.7f), 0.3f, NULL, m_Sample.getManageMaterial(MATERIAL_GREEN), 1.f);
	PxRigidDynamic* left_shoulder_joint2 = m_Sample.createSphere(pos+PxVec3(2.0f,0,.7f), 0.3f, NULL, m_Sample.getManageMaterial(MATERIAL_BLUE), 1.f);
	//PxRigidDynamic* left_arm_joint1 = m_Sample.createSphere(pos+PxVec3(3.6f,0,.7f), 0.3f, &vel, m_Sample.getManageMaterial(MATERIAL_GREEN), 1.f);
	//PxRigidDynamic* left_arm_joint2 = m_Sample.createSphere(pos+PxVec3(3.4f,0,.7f), 0.3f, &vel, m_Sample.getManageMaterial(MATERIAL_BLUE), 1.f);
	PxRigidDynamic* left_arm_1 = m_Sample.createBox(pos+PxVec3(3.3f,0,.7f), PxVec3(1, 0.3f, 0.3f), NULL, m_Sample.getManageMaterial(MATERIAL_GREY), 1.f);
	//PxRigidDynamic* left_arm_2 = m_Sample.createBox(pos+PxVec3(4.4f,0,.7f), PxVec3(1, 0.3f, 0.3f), NULL, m_Sample.getManageMaterial(MATERIAL_RED), 1.f);
	PxRigidDynamic* left_pelvis_joint1 = m_Sample.createSphere(pos+PxVec3(.7f,0,-1.4f), 0.3f, NULL, m_Sample.getManageMaterial(MATERIAL_GREEN), 1.f);
	PxRigidDynamic* left_pelvis_joint2 = m_Sample.createSphere(pos+PxVec3(.7f,0,-2.0f), 0.3f, NULL, m_Sample.getManageMaterial(MATERIAL_BLUE), 1.f);
	PxRigidDynamic* left_leg_1 = m_Sample.createBox(pos+PxVec3(1,0,-3.3f), PxVec3(0.3f, 0.3f, 1), NULL, m_Sample.getManageMaterial(MATERIAL_GREY), 1.f);


	PxRigidDynamic* right_shoulder_joint1 = m_Sample.createSphere(pos+PxVec3(-1.4f,0,.7f), 0.3f, NULL, m_Sample.getManageMaterial(MATERIAL_GREEN), 1.f);
	PxRigidDynamic* right_shoulder_joint2 = m_Sample.createSphere(pos+PxVec3(-2.0f,0,.7f), 0.3f, NULL, m_Sample.getManageMaterial(MATERIAL_BLUE), 1.f);
	//PxRigidDynamic* right_arm_joint1 = m_Sample.createSphere(pos, 0.3f, &vel, m_Sample.getManageMaterial(MATERIAL_GREEN), 1.f);
	//PxRigidDynamic* right_arm_joint2 = m_Sample.createSphere(pos, 0.3f, &vel, m_Sample.getManageMaterial(MATERIAL_BLUE), 1.f);
	PxRigidDynamic* right_arm_1 = m_Sample.createBox(pos+PxVec3(-3.3f,0,.7f), PxVec3(1, 0.3f, 0.3f), NULL, m_Sample.getManageMaterial(MATERIAL_BLUE), 1.f);
	//PxRigidDynamic* right_arm_2 = m_Sample.createBox(PxVec3(pos.x+1.4f, pos.y, pos.z), PxVec3(1, 0.3f, 0.3f), NULL, m_Sample.getManageMaterial(MATERIAL_RED), 1.f);
	PxRigidDynamic* right_pelvis_joint1 = m_Sample.createSphere(pos+PxVec3(-.7f,0,-1.4f), 0.3f, NULL, m_Sample.getManageMaterial(MATERIAL_GREEN), 1.f);
	PxRigidDynamic* right_pelvis_joint2 = m_Sample.createSphere(pos+PxVec3(-.7f,0,-2.0f), 0.3f, NULL, m_Sample.getManageMaterial(MATERIAL_BLUE), 1.f);
	PxRigidDynamic* right_leg_1 = m_Sample.createBox(pos+PxVec3(-1,0,-3.3f), PxVec3(0.3f, 0.3f, 1), NULL, m_Sample.getManageMaterial(MATERIAL_BLUE), 1.f);

	//m_pHead = head;

	m_Rigids.push_back(left_shoulder_joint1);
	m_Rigids.push_back(left_shoulder_joint2);
	m_Rigids.push_back(left_pelvis_joint1);
	m_Rigids.push_back(left_pelvis_joint2);
	m_Rigids.push_back(right_shoulder_joint1);
	m_Rigids.push_back(right_shoulder_joint2);
	m_Rigids.push_back(right_pelvis_joint1);
	m_Rigids.push_back(right_pelvis_joint2);

 	const float scale = 1.f;
	// body - head
	if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
		body, PxTransform(PxVec3(0,0,0)),
		head, PxTransform(PxVec3(0,0,1.6f))
		))
	{
		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}

	// body - left shoulder
	if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
		body, PxTransform(PxVec3(0,0,0)),
		left_shoulder_joint1, PxTransform(PxVec3(1.4f,0,.7f))
		))
	{
		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}

	// left shoulder joint
	if (PxSphericalJoint* j = PxSphericalJointCreate(m_Sample.getPhysics(), 
		left_shoulder_joint1, PxTransform(PxVec3(0,0,0)*scale),
		left_shoulder_joint2, PxTransform(PxVec3(.6f,0,0)*scale)))
	{
		//if (flag)
		//{
		//	j->setLimitCone(PxJointLimitCone(0, PxPi/6, 0.01f));
		//	j->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
		//}
		j->setProjectionLinearTolerance(0.1f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}

	// left shoulder - left arm
	if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
		left_shoulder_joint2, PxTransform(PxVec3(0,0,0)*scale),
		left_arm_1, PxTransform(PxVec3(1.3f,0,0)*scale)))
	{
		j->setProjectionLinearTolerance(0);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}


	// body - left pelvis joint
	if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
		body, PxTransform(PxVec3(0,0,0)),
		left_pelvis_joint1, PxTransform(PxVec3(.7f,0,-1.4f))
		))
	{
		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}

	// left pelvis  joint
	if (PxSphericalJoint* j = PxSphericalJointCreate(m_Sample.getPhysics(), 
		left_pelvis_joint1, PxTransform(PxVec3(0,0,0)*scale),
		left_pelvis_joint2, PxTransform(PxVec3(0,0,-.6f)*scale)))
	{
		//if (flag)
		//{
		//	j->setLimitCone(PxJointLimitCone(0, PxPi/6, 0.01f));
		//	j->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
		//}
		j->setProjectionLinearTolerance(0.1f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}

	// left pelvis joint - left leg
	if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
		left_pelvis_joint2, PxTransform(PxVec3(0,0,0)*scale),
		left_leg_1, PxTransform(PxVec3(0,0,-1.3f)*scale)))
	{
		j->setProjectionLinearTolerance(0);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}



	// body - right shoulder
	if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
		body, PxTransform(PxVec3(0,0,0)),
		right_shoulder_joint1, PxTransform(PxVec3(-1.4f,0,.7f))
		))
	{
		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}

	// right shoulder joint
	if (PxSphericalJoint* j = PxSphericalJointCreate(m_Sample.getPhysics(), 
		right_shoulder_joint1, PxTransform(PxVec3(0,0,0)*scale),
		right_shoulder_joint2, PxTransform(PxVec3(-.6f,0,0)*scale)))
	{
		//if (flag)
		//{
		//	j->setLimitCone(PxJointLimitCone(0, PxPi/6, 0.01f));
		//	j->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
		//}
		j->setProjectionLinearTolerance(0.1f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}

	// right shoulder - right arm
	if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
		right_shoulder_joint2, PxTransform(PxVec3(0,0,0)*scale),
		right_arm_1, PxTransform(PxVec3(-1.3f,0,0)*scale)))
	{
		j->setProjectionLinearTolerance(0);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}


	// body - right pelvis joint
	if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
		body, PxTransform(PxVec3(0,0,0)),
		right_pelvis_joint1, PxTransform(PxVec3(-.7f,0,-1.4f))
		))
	{
		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}
	
	// right pelvis  joint
	if (PxSphericalJoint* j = PxSphericalJointCreate(m_Sample.getPhysics(), 
		right_pelvis_joint1, PxTransform(PxVec3(0,0,0)*scale),
		right_pelvis_joint2, PxTransform(PxVec3(0,0,-.6f)*scale)))
	{
		//if (flag)
		//{
		//	j->setLimitCone(PxJointLimitCone(0, PxPi/6, 0.01f));
		//	j->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
		//}
		j->setProjectionLinearTolerance(0.1f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}

	// right pelvis joint - left leg
	if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
		right_pelvis_joint2, PxTransform(PxVec3(0,0,0)*scale),
		right_leg_1, PxTransform(PxVec3(0,0,-1.3f)*scale)))
	{
		j->setProjectionLinearTolerance(0);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}



	// left arm
	//if (PxSphericalJoint* j = PxSphericalJointCreate(m_Sample.getPhysics(), 
	//	left_arm_joint1, PxTransform(PxVec3(0,0,0)*scale),
	//	left_arm_joint2, PxTransform(PxVec3(-.7f,0,0)*scale)))
	//{
	//	if (flag)
	//	{
	//		j->setLimitCone(PxJointLimitCone(PxPi/2, PxPi/6, 0.01f));
	//		j->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
	//	}
	//	j->setProjectionLinearTolerance(0.5f);
	//}

	//if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
	//	left_arm_1, PxTransform(PxVec3(0,0,0)*scale),
	//	left_arm_joint1, PxTransform(PxVec3(1,0,0)*scale)))
	//{
	//	j->setProjectionLinearTolerance(.5f);
	//}

	//if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
	//	left_arm_2, PxTransform(PxVec3(-1.4f,0,0)*scale),
	//	left_arm_joint2, PxTransform(PxVec3(0,0,0)*scale)))
	//{
	//	j->setProjectionLinearTolerance(.5f);
	//}




	// right shoulder
	//if (PxSphericalJoint* j = PxSphericalJointCreate(m_Sample.getPhysics(), 
	//	right_shoulder_joint1, PxTransform(PxVec3(0,0,0)*scale),
	//	right_shoulder_joint2, PxTransform(PxVec3(-.7f,0,0)*scale)))
	//{
	//	if (flag)
	//	{
	//		j->setLimitCone(PxJointLimitCone(PxPi/2, PxPi/6, 0.01f));
	//		j->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
	//	}
	//	j->setProjectionLinearTolerance(0.5f);
	//}

	//if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
	//	body, PxTransform(PxVec3(0,0,0)*scale),
	//	right_shoulder_joint1, PxTransform(PxVec3(-1,0,.7f)*scale)))
	//{
	//	j->setProjectionLinearTolerance(.5f);
	//}





	// right arm
	//if (PxSphericalJoint* j = PxSphericalJointCreate(m_Sample.getPhysics(), 
	//	right_arm_joint1, PxTransform(PxVec3(0,0,0)*scale),
	//	right_arm_joint2, PxTransform(PxVec3(-.7f,0,0)*scale)))
	//{
	//	if (flag)
	//	{
	//		j->setLimitCone(PxJointLimitCone(PxPi/2, PxPi/6, 0.01f));
	//		j->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
	//	}
	//	j->setProjectionLinearTolerance(0.5f);
	//}

	//if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
	//	right_arm_1, PxTransform(PxVec3(1.4f,0,0)*scale),
	//	right_arm_joint1, PxTransform(PxVec3(0,0,0)*scale)))
	//{
	//	j->setProjectionLinearTolerance(.5f);
	//}

	//if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
	//	right_arm_2, PxTransform(PxVec3(-1.4f,0,0)*scale),
	//	right_arm_joint2, PxTransform(PxVec3(0,0,0)*scale)))
	//{
	//	j->setProjectionLinearTolerance(.5f);
	//}









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


bool CNode::GenerateHuman2(const bool flag)
{
	const PxVec3 pos = m_Sample.getCamera().getPos() + (m_Sample.getCamera().getViewDir()*10.f);
	const PxVec3 vel = m_Sample.getCamera().getViewDir() * 20.f;

	PxVec3 inPosition = pos;
	PxReal yRot(0);
	PxRigidDynamic *mSubmarineActor;

	//PX_ASSERT(mSubmarineActor == NULL);

	std::vector<PxTransform> localPoses;
	std::vector<const PxGeometry*> geometries;

	// cabin
	PxSphereGeometry cabinGeom(1.5f);
	PxTransform	cabinPose = PxTransform(PxIdentity); 
	cabinPose.p.x = -0.5f;

	// engine
	PxBoxGeometry engineGeom(0.25f, 1.0f, 1.0f);
	PxTransform	enginePose = PxTransform(PxIdentity); 
	enginePose.p.x = cabinPose.p.x + cabinGeom.radius + engineGeom.halfExtents.x;

	// tanks
	PxCapsuleGeometry tankGeom(0.5f, 1.8f);
	PxTransform	tank1Pose = PxTransform(PxIdentity); 
	tank1Pose.p = PxVec3(0,-cabinGeom.radius, cabinGeom.radius);
	PxTransform	tank2Pose = PxTransform(PxIdentity); 
	tank2Pose.p = PxVec3(0,-cabinGeom.radius, -cabinGeom.radius);

	localPoses.push_back(cabinPose);
	geometries.push_back(&cabinGeom);
	localPoses.push_back(enginePose);
	geometries.push_back(&engineGeom);
	localPoses.push_back(tank1Pose);
	geometries.push_back(&tankGeom);
	localPoses.push_back(tank2Pose);
	geometries.push_back(&tankGeom);

	// put the shapes together into one actor
	mSubmarineActor = m_Sample.createCompound(inPosition, localPoses, geometries, 0, m_Sample.getManageMaterial(MATERIAL_YELLOW), 
		1.0f)->is<PxRigidDynamic>();

	//if(!mSubmarineActor) fatalError("createCompound failed!");

	//disable the current and buoyancy effect for the sub.
	//mSubmarineActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

	// set the filtering group for the submarine
	//setupFiltering(mSubmarineActor, FilterGroup::eSUBMARINE, FilterGroup::eMINE_HEAD | FilterGroup::eMINE_LINK);

	mSubmarineActor->setLinearDamping(0.15f);
	mSubmarineActor->setAngularDamping(15.0f);

	PxTransform globalPose; 
	globalPose.p = inPosition;
	globalPose.q = PxQuat(yRot, PxVec3(0,1,0));
	mSubmarineActor->setGlobalPose(globalPose);

	mSubmarineActor->setCMassLocalPose(PxTransform(PxIdentity));

	return true;
}


/**
 @brief generate snowMan
 @date 2013-12-03
*/
bool CNode::GenerateHuman3(const bool flag)
{
	const PxVec3 pos = m_Sample.getCamera().getPos() + (m_Sample.getCamera().getViewDir()*10.f);
	const PxVec3 vel = m_Sample.getCamera().getViewDir() * 20.f;

	PxRigidDynamic* snowmanActor = m_Sample.getPhysics().createRigidDynamic(PxTransform(pos));
	//if(!snowmanActor)
	//	fatalError("create snowman actor failed");

	PxMaterial&	material = m_Sample.getDefaultMaterial();
	PxShape* armL = NULL; PxShape* armR = NULL;

	{
		PxShape* shape = NULL;
		shape = snowmanActor->createShape(PxSphereGeometry(.2),material);
		shape->setLocalPose(PxTransform(PxVec3(0,-.29,0)));
		PxRigidBodyExt::updateMassAndInertia(*snowmanActor,10);

		shape = snowmanActor->createShape(PxSphereGeometry(.5),material);
		shape = snowmanActor->createShape(PxSphereGeometry(.4),material);
		shape->setLocalPose(PxTransform(PxVec3(0,.6,0)));

		shape = snowmanActor->createShape(PxSphereGeometry(.3),material);
		shape->setLocalPose(PxTransform(PxVec3(0,1.1,0)));

		armL = snowmanActor->createShape(PxCapsuleGeometry(.1,.1),material);
		armL->setLocalPose(PxTransform(PxVec3(-.4,.7,0)));

		armR = snowmanActor->createShape(PxCapsuleGeometry(.1,.1),material);
		armR->setLocalPose(PxTransform(PxVec3( .4,.7,0)));
	}

	m_Sample.createRenderObjectsFromActor(snowmanActor, m_Sample.getManageMaterial(MATERIAL_GREY));

	m_Sample.getActiveScene().addActor(*snowmanActor);

	return true;
}


/**
 @brief 
 @date 2013-12-03
*/
bool CNode::GenerateHuman4(const bool flag)
{
	const PxVec3 pos = m_Sample.getCamera().getPos() + (m_Sample.getCamera().getViewDir()*10.f);
	const PxVec3 vel = m_Sample.getCamera().getViewDir() * 20.f;

	PxRigidDynamic* head = m_Sample.createSphere(pos+PxVec3(0,0,0), 0.5f, NULL, m_Sample.getManageMaterial(MATERIAL_GREEN), 1.f);
	PxRigidDynamic* body = m_Sample.createBox(pos+PxVec3(0,-2.5f,0), PxVec3(1,2,0.8f), NULL, m_Sample.getManageMaterial(MATERIAL_YELLOW), 10.f);

	// body - head
	if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
		head, PxTransform(PxVec3(0,0,0)),
		body, PxTransform(PxVec3(0,2.5f,0))) )
	{
		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}

	//head->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	//body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	setCollisionGroup(head, NodeGroup::HEAD);
	setCollisionGroup(body, NodeGroup::BODY);

	PxRigidDynamic* left_shoulder_joint1 = m_Sample.createSphere(pos+PxVec3(-1.4f,0,.7f), 0.3f, NULL, m_Sample.getManageMaterial(MATERIAL_GREEN), 1.f);
	PxRigidDynamic* left_shoulder_joint2 = m_Sample.createSphere(pos+PxVec3(-2.0f,0,.7f), 0.3f, NULL, m_Sample.getManageMaterial(MATERIAL_BLUE), 1.f);
	PxRigidDynamic* left_arm_1 = m_Sample.createBox(pos+PxVec3(-3.3f,0,.7f), PxVec3(1, 0.3f, 0.3f), NULL, m_Sample.getManageMaterial(MATERIAL_GREY), 1.f);

	setCollisionGroup(left_arm_1, NodeGroup::L_ARM);
	//left_shoulder_joint1->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	//left_shoulder_joint2->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);


	float scale = 1.f;

	// body - left shoulder
	if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
		body, PxTransform(PxVec3(0,0,0)),
		left_shoulder_joint1, PxTransform(PxVec3(1.4f,-2,0))
		))
	{
		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}

	// left shoulder joint
	if (PxSphericalJoint* j = PxSphericalJointCreate(m_Sample.getPhysics(), 
		left_shoulder_joint1, PxTransform(PxVec3(0,0,0)*scale),
		left_shoulder_joint2, PxTransform(PxVec3(.6f,0,0)*scale)))
	{
		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}

	// left shoulder - left arm
	if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
		left_shoulder_joint2, PxTransform(PxVec3(0,0,0)*scale),
		left_arm_1, PxTransform(PxVec3(1.3f,0,0)*scale)))
	{
		j->setProjectionLinearTolerance(0);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}


	// right
	PxRigidDynamic* right_shoulder_joint1 = m_Sample.createSphere(pos+PxVec3(1.4f,0,.7f), 0.3f, NULL, m_Sample.getManageMaterial(MATERIAL_BLUE), 1.f);
	PxRigidDynamic* right_shoulder_joint2 = m_Sample.createSphere(pos+PxVec3(2.0f,0,.7f), 0.3f, NULL, m_Sample.getManageMaterial(MATERIAL_BLUE), 1.f);
	PxRigidDynamic* right_arm_1 = m_Sample.createBox(pos+PxVec3(3.3f,0,.7f), PxVec3(1, 0.3f, 0.3f), NULL, m_Sample.getManageMaterial(MATERIAL_BLUE), 1.f);

	setCollisionGroup(right_arm_1, NodeGroup::R_ARM);

	// body - left shoulder
	if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
		body, PxTransform(PxVec3(0,0,0)),
		right_shoulder_joint1, PxTransform(PxVec3(-1.4f,-2,0))
		))
	{
		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}

	// left shoulder joint
	if (PxSphericalJoint* j = PxSphericalJointCreate(m_Sample.getPhysics(), 
		right_shoulder_joint1, PxTransform(PxVec3(0,0,0)*scale),
		right_shoulder_joint2, PxTransform(PxVec3(-.6f,0,0)*scale)))
	{
		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}

	// left shoulder - left arm
	if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
		right_shoulder_joint2, PxTransform(PxVec3(0,0,0)*scale),
		right_arm_1, PxTransform(PxVec3(-1.3f,0,0)*scale)))
	{
		j->setProjectionLinearTolerance(0);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}

	//setupFiltering(left_arm_1, FilterGroup::eSUBMARINE, FilterGroup::eMINE_HEAD | FilterGroup::eMINE_LINK);
	//setupFiltering(body, FilterGroup::eMINE_HEAD, FilterGroup::eSUBMARINE);

	//m_pHead = head;
	m_Rigids.push_back(left_shoulder_joint1);
	m_Rigids.push_back(left_shoulder_joint2);
	//m_Joints.push_back(left_pelvis_joint1);
	//m_Joints.push_back(left_pelvis_joint2);
	m_Rigids.push_back(right_shoulder_joint1);
	m_Rigids.push_back(right_shoulder_joint2);
	//m_Joints.push_back(right_pelvis_joint1);
	//m_Joints.push_back(right_pelvis_joint2);
/**/

	return true;
}


/**
 @brief 
 @date 2013-12-03
*/
bool CNode::GenerateHuman5(const bool flag)
{
	const PxVec3 pos = m_Sample.getCamera().getPos() + (m_Sample.getCamera().getViewDir()*10.f);
	const PxVec3 vel = m_Sample.getCamera().getViewDir() * 20.f;

	PxRigidDynamic* body = m_Sample.createBox(pos+PxVec3(0,-2.5f,0), PxVec3(0.3f,0.3f,0.3f), NULL, m_Sample.getManageMaterial(MATERIAL_YELLOW), 1.f);

	PxRigidDynamic* left_shoulder_joint1 = m_Sample.createBox(pos+PxVec3(-1.4f,0,.7f), PxVec3(0.3f,0.3f,0.3f), NULL, m_Sample.getManageMaterial(MATERIAL_GREEN), 1.f);
	PxRigidDynamic* left_shoulder_joint2 = m_Sample.createBox(pos+PxVec3(-2.0f,0,.7f), PxVec3(0.3f,0.3f,0.3f), NULL, m_Sample.getManageMaterial(MATERIAL_BLUE), 1.f);
	PxRigidDynamic* left_arm_1 = m_Sample.createBox(pos+PxVec3(-3.3f,0,.7f), PxVec3(1, 0.3f, 0.3f), NULL, m_Sample.getManageMaterial(MATERIAL_GREY), 1.f);

	setCollisionGroup(left_arm_1, NodeGroup::L_ARM);
	left_shoulder_joint1->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	//left_shoulder_joint2->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
		body, PxTransform(PxVec3(0,0,0)),
		left_shoulder_joint1, PxTransform(PxVec3(0.6f,0,0))
		))
	{
		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}

	// left shoulder joint
	if (PxSphericalJoint* j = PxSphericalJointCreate(m_Sample.getPhysics(), 
		left_shoulder_joint1, PxTransform(PxVec3(0,0,0)),
		left_shoulder_joint2, PxTransform(PxVec3(1,0,0))))
	{
		if (flag)
		{
			j->setLimitCone(PxJointLimitCone(PxPi/2, PxPi/6, PxSpring(0,0)));
			j->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
		}
		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}

	// left shoulder - left arm
	if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
		left_shoulder_joint2, PxTransform(PxVec3(0,0,0)),
		left_arm_1, PxTransform(PxVec3(1.3f,0,0))))
	{
		j->setProjectionLinearTolerance(0);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}

	m_Rigids.push_back(left_shoulder_joint1);
	m_Rigids.push_back(left_shoulder_joint2);

	return true;
}


/**
 @brief joint test
 @date 2013-12-04
*/
bool CNode::GenerateHuman6(const bool flag)
{
	const PxVec3 pos = m_Sample.getCamera().getPos() + (m_Sample.getCamera().getViewDir()*10.f);
	const PxVec3 vel = m_Sample.getCamera().getViewDir() * 20.f;

	PxRigidDynamic* body = m_Sample.createBox(pos+PxVec3(0,-2.5f,0), PxVec3(0.3f,0.3f,0.3f), NULL, m_Sample.getManageMaterial(MATERIAL_YELLOW), 1.f);

	PxRigidDynamic* left_shoulder_joint1 = m_Sample.createBox(pos+PxVec3(-1.4f,0,.7f), PxVec3(0.3f,0.3f,0.3f), NULL, m_Sample.getManageMaterial(MATERIAL_GREEN), 1.f);
	PxRigidDynamic* left_shoulder_joint2 = m_Sample.createBox(pos+PxVec3(-2.0f,0,.7f), PxVec3(0.3f,0.3f,0.3f), NULL, m_Sample.getManageMaterial(MATERIAL_BLUE), 1.f);
	PxRigidDynamic* left_arm_1 = m_Sample.createBox(pos+PxVec3(-3.3f,0,.7f), PxVec3(1, 0.3f, 0.3f), NULL, m_Sample.getManageMaterial(MATERIAL_GREY), 1.f);

	setCollisionGroup(left_arm_1, NodeGroup::L_ARM);
	//left_shoulder_joint1->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	//left_shoulder_joint2->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
		body, PxTransform(PxVec3(0,0,0)),
		left_shoulder_joint1, PxTransform(PxVec3(0.6f,0,0))
		))
	{
		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}

	// left shoulder joint
	if (PxSphericalJoint* j = PxSphericalJointCreate(m_Sample.getPhysics(), 
		left_shoulder_joint1, PxTransform(PxVec3(0,0,0)),
		left_shoulder_joint2, PxTransform(PxVec3(1,0,0))))
	{
		if (flag)
		{
			j->setLimitCone(PxJointLimitCone(PxPi/2, PxPi/6, PxSpring(0,0)));
			j->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
		}
		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}

	// left shoulder - left arm
	if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
		left_shoulder_joint2, PxTransform(PxVec3(0,0,0)),
		left_arm_1, PxTransform(PxVec3(1.3f,0,0))))
	{
		j->setProjectionLinearTolerance(0);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}

	m_Rigids.push_back(left_shoulder_joint1);
	m_Rigids.push_back(left_shoulder_joint2);

	return true;
}


/**
 @brief Revolution Join
 @date 2013-12-05
*/
bool CNode::GenerateHuman7(const bool flag)
{
	const PxVec3 pos = m_Sample.getCamera().getPos() + (m_Sample.getCamera().getViewDir()*10.f);
	const PxVec3 vel = m_Sample.getCamera().getViewDir() * 20.f;

	//PxRigidDynamic* body = m_Sample.createBox(pos+PxVec3(0,-2.5f,0), PxVec3(0.3f,0.3f,0.3f), NULL, m_Sample.getManageMaterial(MATERIAL_YELLOW), 1.f);

	PxRigidDynamic* left_shoulder_joint1 = m_Sample.createBox(pos+PxVec3(-1.4f,0,.7f), PxVec3(0.5f,0.3f,0.1f), NULL, m_Sample.getManageMaterial(MATERIAL_GREEN), 1.f);
	PxRigidDynamic* left_shoulder_joint2 = m_Sample.createBox(pos+PxVec3(-1.4f,-1,.7f), PxVec3(0.5f,0.3f,0.1f), NULL, m_Sample.getManageMaterial(MATERIAL_BLUE), 1.f);
	//PxRigidDynamic* left_arm_1 = m_Sample.createBox(pos+PxVec3(-3.3f,0,.7f), PxVec3(1, 0.3f, 0.3f), NULL, m_Sample.getManageMaterial(MATERIAL_GREY), 1.f);

	//setCollisionGroup(left_arm_1, NodeGroup::L_ARM);
	//left_shoulder_joint1->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	//left_shoulder_joint2->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
/*
	if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
		body, PxTransform(PxVec3(0,0,0)),
		left_shoulder_joint1, PxTransform(PxVec3(0.6f,0,0))
		))
	{
		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}*/

	// left shoulder joint
	if (PxRevoluteJoint* j = PxRevoluteJointCreate(m_Sample.getPhysics(), 
		left_shoulder_joint1, PxTransform(PxVec3(0,0,0)),
		left_shoulder_joint2, PxTransform(PxVec3(0,.6f,0))))
	{
		if (flag)
		{
			j->setLimit(PxJointAngularLimitPair(-PxPi/4, PxPi/4, 0.1f)); // upper, lower, tolerance
			j->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, true);
			j->setDriveVelocity(g_pDbgConfig->value1);
			j->setRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_ENABLED, true);
		}

		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);

		//m_Joints.push_back(j);
	}

	//// left shoulder - left arm
	//if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
	//	left_shoulder_joint2, PxTransform(PxVec3(0,0,0)),
	//	left_arm_1, PxTransform(PxVec3(1.3f,0,0))))
	//{
	//	j->setProjectionLinearTolerance(0);
	//	j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	//}

	m_Rigids.push_back(left_shoulder_joint1);
	m_Rigids.push_back(left_shoulder_joint2);

	return true;
}


/**
 @brief Revolution Join
 @date 2013-12-05
*/
bool CNode::GenerateHuman8(const bool flag)
{
	const PxVec3 pos = m_Sample.getCamera().getPos() + (m_Sample.getCamera().getViewDir()*10.f);
	const PxVec3 vel = m_Sample.getCamera().getViewDir() * 20.f;

	PxRigidDynamic* body = m_Sample.createBox(pos+PxVec3(0,-2.5f,0), PxVec3(0.3f,4.f,0.1f), NULL, m_Sample.getManageMaterial(MATERIAL_YELLOW), 1.f);

	PxRigidDynamic* left_shoulder_joint1 = m_Sample.createBox(pos+PxVec3(-1.4f,0,.7f), PxVec3(0.5f,0.3f,0.1f), NULL, m_Sample.getManageMaterial(MATERIAL_GREEN), 1.f);
	PxRigidDynamic* left_shoulder_joint2 = m_Sample.createBox(pos+PxVec3(-1.4f,-1,.7f), PxVec3(0.5f,0.3f,0.1f), NULL, m_Sample.getManageMaterial(MATERIAL_BLUE), 1.f);
	PxRigidDynamic* left_shoulder_joint3 = m_Sample.createBox(pos+PxVec3(-1.4f,-2,.7f), PxVec3(0.5f,0.3f,0.1f), NULL, m_Sample.getManageMaterial(MATERIAL_GREEN), 1.f);
	PxRigidDynamic* left_shoulder_joint4 = m_Sample.createBox(pos+PxVec3(-1.4f,-3,.7f), PxVec3(0.5f,0.3f,0.1f), NULL, m_Sample.getManageMaterial(MATERIAL_BLUE), 1.f);
	PxRigidDynamic* left_shoulder_joint5 = m_Sample.createBox(pos+PxVec3(-1.4f,-4,.7f), PxVec3(0.5f,0.3f,0.1f), NULL, m_Sample.getManageMaterial(MATERIAL_GREEN), 1.f);
	PxRigidDynamic* left_shoulder_joint6 = m_Sample.createBox(pos+PxVec3(-1.4f,-5,.7f), PxVec3(0.5f,0.3f,0.1f), NULL, m_Sample.getManageMaterial(MATERIAL_BLUE), 1.f);
	//PxRigidDynamic* left_arm_1 = m_Sample.createBox(pos+PxVec3(-3.3f,0,.7f), PxVec3(1, 0.3f, 0.3f), NULL, m_Sample.getManageMaterial(MATERIAL_GREY), 1.f);


	PxRigidDynamic* right_shoulder_joint1 = m_Sample.createBox(pos+PxVec3(1.4f,0,.7f), PxVec3(0.5f,0.3f,0.1f), NULL, m_Sample.getManageMaterial(MATERIAL_GREEN), 1.f);
	PxRigidDynamic* right_shoulder_joint2 = m_Sample.createBox(pos+PxVec3(1.4f,-1,.7f), PxVec3(0.5f,0.3f,0.1f), NULL, m_Sample.getManageMaterial(MATERIAL_BLUE), 1.f);
	PxRigidDynamic* right_shoulder_joint3 = m_Sample.createBox(pos+PxVec3(1.4f,-2,.7f), PxVec3(0.5f,0.3f,0.1f), NULL, m_Sample.getManageMaterial(MATERIAL_GREEN), 1.f);
	PxRigidDynamic* right_shoulder_joint4 = m_Sample.createBox(pos+PxVec3(1.4f,-3,.7f), PxVec3(0.5f,0.3f,0.1f), NULL, m_Sample.getManageMaterial(MATERIAL_BLUE), 1.f);
	PxRigidDynamic* right_shoulder_joint5 = m_Sample.createBox(pos+PxVec3(1.4f,-4,.7f), PxVec3(0.5f,0.3f,0.1f), NULL, m_Sample.getManageMaterial(MATERIAL_GREEN), 1.f);
	PxRigidDynamic* right_shoulder_joint6 = m_Sample.createBox(pos+PxVec3(1.4f,-5,.7f), PxVec3(0.5f,0.3f,0.1f), NULL, m_Sample.getManageMaterial(MATERIAL_BLUE), 1.f);

	//setCollisionGroup(left_arm_1, NodeGroup::L_ARM);
	//left_shoulder_joint1->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	//left_shoulder_joint2->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
		body, PxTransform(PxVec3(0,0,0)),
		left_shoulder_joint1, PxTransform(PxQuat(PxPi/2.f, PxVec3(0,0,1))) * PxTransform(PxVec3(0.6f,0,0))
		))
	{
		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}

	if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
		body, PxTransform(PxVec3(0,0,0)),
		left_shoulder_joint3, PxTransform(PxQuat(PxPi/2.f, PxVec3(0,0,1))) * PxTransform(PxVec3(0.6f,-2,0))
		))
	{
		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}

	if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
		body, PxTransform(PxVec3(0,0,0)),
		left_shoulder_joint5, PxTransform(PxQuat(PxPi/2.f, PxVec3(0,0,1))) * PxTransform(PxVec3(0.6f,-4,0))
		))
	{
		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}


	// left shoulder joint
	if (PxRevoluteJoint* j = PxRevoluteJointCreate(m_Sample.getPhysics(), 
		left_shoulder_joint1, PxTransform(PxVec3(0,0,0)),
		left_shoulder_joint2, PxTransform(PxVec3(0,.6f,0))))
	{
		if (flag)
		{
			j->setLimit(PxJointAngularLimitPair(-PxPi/4, PxPi/4, 0.1f)); // upper, lower, tolerance
			j->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, true);
		}

		j->setDriveVelocity(1.f);
		j->setRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_ENABLED, true);
		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);

		//m_Joints.push_back(j);
	}

	// left shoulder joint 2
	if (PxRevoluteJoint* j = PxRevoluteJointCreate(m_Sample.getPhysics(), 
		left_shoulder_joint3, PxTransform(PxVec3(0,0,0)),
		left_shoulder_joint4, PxTransform(PxVec3(0,.6f,0))))
	{
		if (flag)
		{
			j->setLimit(PxJointAngularLimitPair(-PxPi/4, PxPi/4, 0.1f)); // upper, lower, tolerance
			j->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, true);
		}

		j->setDriveVelocity(1.f);
		j->setRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_ENABLED, true);
		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);

		//m_Joints.push_back(j);
	}


	if (PxRevoluteJoint* j = PxRevoluteJointCreate(m_Sample.getPhysics(), 
		left_shoulder_joint5, PxTransform(PxVec3(0,0,0)),
		left_shoulder_joint6, PxTransform(PxVec3(0,.6f,0))))
	{
		if (flag)
		{
			j->setLimit(PxJointAngularLimitPair(-PxPi/4, PxPi/4, 0.1f)); // upper, lower, tolerance
			j->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, true);
		}

		j->setDriveVelocity(1.f);
		j->setRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_ENABLED, true);
		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);

		//m_Joints.push_back(j);
	}


	// body - right shoulder
	if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
		body, PxTransform(PxVec3(0,0,0)),
		right_shoulder_joint1, PxTransform(PxQuat(-PxPi/2.f, PxVec3(0,0,1))) * PxTransform(PxVec3(-0.6f,0,0))
		))
	{
		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}

	if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
		body, PxTransform(PxVec3(0,0,0)),
		right_shoulder_joint3, PxTransform(PxQuat(-PxPi/2.f, PxVec3(0,0,1))) * PxTransform(PxVec3(-0.6f,-2,0))
		))
	{
		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}

	if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
		body, PxTransform(PxVec3(0,0,0)),
		right_shoulder_joint5, PxTransform(PxQuat(-PxPi/2.f, PxVec3(0,0,1))) * PxTransform(PxVec3(-0.6f,-4,0))
		))
	{
		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}


	// right shoulder joint
	if (PxRevoluteJoint* j = PxRevoluteJointCreate(m_Sample.getPhysics(), 
		right_shoulder_joint1, PxTransform(PxVec3(0,0,0)),
		right_shoulder_joint2, PxTransform(PxVec3(0,.6f,0))))
	{
		if (flag)
		{
			j->setLimit(PxJointAngularLimitPair(-PxPi/4, PxPi/4, 0.1f)); // upper, lower, tolerance
			j->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, true);
		}
		j->setDriveVelocity(1.f);
		j->setRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_ENABLED, true);
		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);

		//m_Joints.push_back(j);
	}


	if (PxRevoluteJoint* j = PxRevoluteJointCreate(m_Sample.getPhysics(), 
		right_shoulder_joint3, PxTransform(PxVec3(0,0,0)),
		right_shoulder_joint4, PxTransform(PxVec3(0,.6f,0))))
	{
		if (flag)
		{
			j->setLimit(PxJointAngularLimitPair(-PxPi/4, PxPi/4, 0.1f)); // upper, lower, tolerance
			j->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, true);
		}
		j->setDriveVelocity(1.f);
		j->setRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_ENABLED, true);
		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);

		//m_Joints.push_back(j);
	}

	if (PxRevoluteJoint* j = PxRevoluteJointCreate(m_Sample.getPhysics(), 
		right_shoulder_joint5, PxTransform(PxVec3(0,0,0)),
		right_shoulder_joint6, PxTransform(PxVec3(0,.6f,0))))
	{
		if (flag)
		{
			j->setLimit(PxJointAngularLimitPair(-PxPi/4, PxPi/4, 0.1f)); // upper, lower, tolerance
			j->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, true);
		}
		j->setDriveVelocity(1.f);
		j->setRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_ENABLED, true);
		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);

		//m_Joints.push_back(j);
	}


	//// left shoulder - left arm
	//if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
	//	left_shoulder_joint2, PxTransform(PxVec3(0,0,0)),
	//	left_arm_1, PxTransform(PxVec3(1.3f,0,0))))
	//{
	//	j->setProjectionLinearTolerance(0);
	//	j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	//}

	m_Rigids.push_back(left_shoulder_joint1);
	m_Rigids.push_back(left_shoulder_joint2);

	return true;
}


/**
 @brief load by genotype script
 @date 2013-12-06
*/
bool CNode::GenerateHuman9(const bool flag)
{
	genotype_parser::CGenotypeParser parser;
	genotype_parser::SExpr *pexpr = parser.Parse("genotype.txt");

	GenerateByGenotype(pexpr, g_pDbgConfig->generationRecursiveCount);

	genotype_parser::RemoveExpression(pexpr);
	return true;
}


/**
 @brief create creature by genotype script
 @date 2013-12-06
*/
PxRigidDynamic* CNode::GenerateByGenotype( const genotype_parser::SExpr *pexpr, const int recursiveCnt )
{
	if (!pexpr)
		return NULL;
	if (recursiveCnt < 0)
		return NULL;

	const PxVec3 pos = m_Sample.getCamera().getPos() + (m_Sample.getCamera().getViewDir()*10.f);
	const PxVec3 vel = m_Sample.getCamera().getViewDir() * 20.f;

	PxRigidDynamic* rigid = NULL;
	PxVec3 dimension(pexpr->dimension.x, pexpr->dimension.y, pexpr->dimension.z);
	MaterialIndex material = GetMaterialType(pexpr->material);
	const float mass = pexpr->mass;

	if (boost::iequals(pexpr->shape, "box"))
	{
		rigid = m_Sample.createBox(pos+PxVec3(0,0,0), dimension, NULL, m_Sample.getManageMaterial(material), mass);
	}
	else if (boost::iequals(pexpr->shape, "sphere"))
	{
		rigid = m_Sample.createSphere(pos+PxVec3(0,0,0), dimension.x, NULL, m_Sample.getManageMaterial(material), mass);
	}

	genotype_parser::SJointList *pnode = pexpr->connection;
	while (pnode)
	{
		PxRigidDynamic *child = GenerateByGenotype( pnode->joint->expr, recursiveCnt-1 );

		if (child)
		{
			genotype_parser::SJoint *joint = pnode->joint;
			PxVec3 dir0(joint->parentOrient.dir.x, joint->parentOrient.dir.y, joint->parentOrient.dir.z);
			PxVec3 dir1(joint->orient.dir.x, joint->orient.dir.y, joint->orient.dir.z);
			PxVec3 pos(joint->pos.x, joint->pos.y, joint->pos.z);
			PxVec3 limit(joint->limit.x, joint->limit.y, joint->limit.z);
			PxVec3 velocity(joint->velocity.x, joint->velocity.y, joint->velocity.z);

			PxTransform tm0 = (dir0.isZero())? PxTransform::createIdentity() : PxTransform(PxQuat(joint->parentOrient.angle, dir0));
			PxTransform tm1 = (dir1.isZero())? PxTransform(PxVec3(pos)) : 
				(PxTransform(PxQuat(joint->orient.angle, dir1)) * PxTransform(PxVec3(pos)));

			PxJoint* newJoint = NULL;

			if (boost::iequals(joint->type, "fixed"))
			{
				PxFixedJoint *j = PxFixedJointCreate(m_Sample.getPhysics(), 
					rigid, tm0, 
					child, tm1 );
				newJoint = j;
			}
			else if(boost::iequals(joint->type, "spherical"))
			{
				if (PxSphericalJoint *j = PxSphericalJointCreate(m_Sample.getPhysics(), 
					rigid, tm0,
					child, tm1) )
				{
					if (!limit.isZero())
					{
						j->setLimitCone(PxJointLimitCone(limit.x, limit.y, PxSpring(0,0)));
						j->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
					}

					j->setProjectionLinearTolerance(0.0f);
					j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
					newJoint = j;
				}
			}
			else if (boost::iequals(joint->type, "revolute"))
			{
				if (PxRevoluteJoint*j = PxRevoluteJointCreate(m_Sample.getPhysics(), 
					rigid, tm0,
					child, tm1) )
				{
					if (!limit.isZero())
					{
						j->setLimit(PxJointAngularLimitPair(limit.x, limit.y, limit.z)); // upper, lower, tolerance
						j->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, true);
					}
					
					if (!velocity.isZero())
					{
						j->setDriveVelocity(velocity.x);
						j->setRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_ENABLED, true);
					}

					j->setProjectionLinearTolerance(0.0f);
					j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
					newJoint = j;
				}
			}

			m_Joints.push_back( new CJoint(rigid, child, newJoint, velocity.x, joint->period) );
		}

		pnode = pnode->next;
	}

	return rigid;
}


/**
 @brief genotype script material field to MaterialIndex value
 @date 2013-12-07
*/
MaterialIndex CNode::GetMaterialType(const string &materialStr)
{
	map<string, MaterialIndex> materialMap;
	materialMap[ "grey"] = MATERIAL_GREY;
	materialMap[ "red"] = MATERIAL_RED;
	materialMap[ "green"] = MATERIAL_GREEN;
	materialMap[ "blue"] = MATERIAL_BLUE;
	materialMap[ "yellow"] = MATERIAL_YELLOW;

	if (materialMap.find(materialStr) != materialMap.end())
		return materialMap[ materialStr];
	return MATERIAL_GREY;
}


/**
 @brief 
 @date 2013-12-03
*/
void CNode::Move(float dtime)
{
	BOOST_FOREACH(auto joint, m_Joints)
		joint->Move(dtime);
}

