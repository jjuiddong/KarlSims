
#include "stdafx.h"
#include "node.h"
#include "PhysXSample.h"
#include "EvolvedVirtualCreatures.h"


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
,	m_Force(1.f)
,	m_pHead(NULL)
,	m_ElapseT(0)
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

	m_pHead = head;

	m_Joints.push_back(left_shoulder_joint1);
	m_Joints.push_back(left_shoulder_joint2);
	m_Joints.push_back(left_pelvis_joint1);
	m_Joints.push_back(left_pelvis_joint2);
	m_Joints.push_back(right_shoulder_joint1);
	m_Joints.push_back(right_shoulder_joint2);
	m_Joints.push_back(right_pelvis_joint1);
	m_Joints.push_back(right_pelvis_joint2);

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

	m_pHead = head;
	m_Joints.push_back(left_shoulder_joint1);
	m_Joints.push_back(left_shoulder_joint2);
	//m_Joints.push_back(left_pelvis_joint1);
	//m_Joints.push_back(left_pelvis_joint2);
	m_Joints.push_back(right_shoulder_joint1);
	m_Joints.push_back(right_shoulder_joint2);
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

	m_Joints.push_back(left_shoulder_joint1);
	m_Joints.push_back(left_shoulder_joint2);

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

	m_Joints.push_back(left_shoulder_joint1);
	m_Joints.push_back(left_shoulder_joint2);

	return true;
}


/**
 @brief 
 @date 2013-12-05
*/
bool CNode::GenerateHuman7(const bool flag)
{
	const PxVec3 pos = m_Sample.getCamera().getPos() + (m_Sample.getCamera().getViewDir()*10.f);
	const PxVec3 vel = m_Sample.getCamera().getViewDir() * 20.f;

	//PxRigidDynamic* body = m_Sample.createBox(pos+PxVec3(0,-2.5f,0), PxVec3(0.3f,0.3f,0.3f), NULL, m_Sample.getManageMaterial(MATERIAL_YELLOW), 1.f);

	PxRigidDynamic* left_shoulder_joint1 = m_Sample.createBox(pos+PxVec3(-1.4f,0,.7f), PxVec3(0.3f,0.3f,0.3f), NULL, m_Sample.getManageMaterial(MATERIAL_GREEN), 1.f);
	PxRigidDynamic* left_shoulder_joint2 = m_Sample.createBox(pos+PxVec3(-2.0f,0,.7f), PxVec3(0.3f,0.3f,0.3f), NULL, m_Sample.getManageMaterial(MATERIAL_BLUE), 1.f);
	//PxRigidDynamic* left_arm_1 = m_Sample.createBox(pos+PxVec3(-3.3f,0,.7f), PxVec3(1, 0.3f, 0.3f), NULL, m_Sample.getManageMaterial(MATERIAL_GREY), 1.f);

	//setCollisionGroup(left_arm_1, NodeGroup::L_ARM);
	left_shoulder_joint1->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
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
	if (PxPrismaticJoint* j = PxPrismaticJointCreate(m_Sample.getPhysics(), 
		left_shoulder_joint1, PxTransform(PxVec3(0,0,0)),
		left_shoulder_joint2, PxTransform(PxVec3(.6f,0,0))))
	{
		if (flag)
		{
			//j->setLimit( PxJointLinearLimitPair(0.6f, 10.0f, PxSpring(0,0)));
			//j->setPrismaticJointFlag(PxPrismaticJointFlag::eLIMIT_ENABLED, true);
		}

		j->setProjectionLinearTolerance(0.0f);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	}

	//// left shoulder - left arm
	//if (PxFixedJoint* j = PxFixedJointCreate(m_Sample.getPhysics(), 
	//	left_shoulder_joint2, PxTransform(PxVec3(0,0,0)),
	//	left_arm_1, PxTransform(PxVec3(1.3f,0,0))))
	//{
	//	j->setProjectionLinearTolerance(0);
	//	j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
	//}

	m_Joints.push_back(left_shoulder_joint1);
	m_Joints.push_back(left_shoulder_joint2);

	return true;
}


/**
 @brief 
 @date 2013-12-03
*/
void CNode::Move(float dtime)
{
	//if (!m_pHead)
	//	return;

	//PxTransform m = m_left_pelvis_joint2->getGlobalPose();
	//m.p.x += (dtime * 40.f);
	//m_left_pelvis_joint2->setGlobalPose(m);
	//const PxVec3 pos = m_Sample.getCamera().getPos() + (m_Sample.getCamera().getViewDir()*10.f);
	//const PxVec3 vel = m_Sample.getCamera().getViewDir() * 20.f;

	//static float elapseT = 0.f;
	//elapseT += dtime;
	m_ElapseT += dtime;

	PxVec3 force;
	if (m_ElapseT < 1.f)
	{
		force = PxVec3(0,1,0);//*m_Force*dtime;
	}
	else
	{
		force = PxVec3(0,-1,0);//*m_Force*dtime;

		//if (m_ElapseT > 2.f)
		//	elapseT = 0.f;
	}

	//if (m_Joints.size() < 3)
	//	return;

	//PxTransform mh = m_pHead->getGlobalPose();
	PxTransform m0 = m_Joints[ 0]->getGlobalPose();
	PxTransform m1 = m_Joints[ 1]->getGlobalPose();
	//PxTransform m2 = m_Joints[ 2]->getGlobalPose();

	PxQuat q(m_Force*m_ElapseT, PxVec3(1,0,0));
	//PxTransform qm(q);
	//PxVec3 v0 = m1.p - m0.p;
	//PxTransform vm = qm * PxTransform(v0);

	PxTransform mov(m1.p, q);
	//m_Joints[ 1]->clearTorque();
	//m_Joints[ 1]->addTorque(PxVec3(1,0,0)*m_Force);
	//m_Joints[ 1]->setAngularVelocity(PxVec3(0,1,0)*m_Force);

	//PxVec3 dir = m1.p - m2.p;
	//dir.normalize();

	//PxVec3 front = dir.cross(PxVec3(0,1,0));
	//front.normalize();

	//if (PxPi < m_ElapseT)
	//	front = -front;
	 
	//m_Joints[ 1]->addForce(PxVec3(1,0,0)* m_Force);




	//
	////PxVec3 v0 = m0.p - mh.p;
	////v0.normalize();
	////PxVec3 y0(0,1,0);
	////PxVec3 dir = y0.cross(v0);
	////dir.normalize();

	//PxQuat q(m_Force*m_ElapseT, PxVec3(1,0,0));
	////PxQuat q(.0f, PxVec3(1,0,0));
	//PxTransform qm(q);

	////PxVec3 dist = m1.p - m0.p;
	//PxVec3 dist(0,.6f,0);
	//PxTransform m(dist);
	//PxTransform vm = qm * m;

	//m1.p = m0.p + vm.p;
	//PxTransform mov(m1.p);
	//m_Joints[ 1]->clearForce();
	//m_Joints[ 1]->setGlobalPose(mov);
	//m_Joints[ 1]->addForce(PxVec3(

	//m_Joints[ 1]->addForce(force);
	//m_Joints[ 3]->addForce(force);
	//m_Joints[ 5]->addForce(force);
	//m_Joints[ 7]->addForce(force);

	if (PxPi*2 < m_ElapseT)
	{
		m_Joints[ 1]->addForce(PxVec3(0,1,0)*m_Force);

		m_ElapseT = 0.f;
	}
/*
	PxQuat q2(PxPi, PxVec3(1,0,0));
	PxTransform qm2(q2);

	PxQuat q3(PxPi/2.f, PxVec3(1,0,0));
	PxTransform qm3(q3);

	PxQuat q4(PxPi*100 + PxPi/2.f, PxVec3(1,0,0));
	PxTransform qm4(q4);

	PxTransform vm2 = qm2 * PxTransform(PxVec3(0,1,0));
	PxTransform vm3 = qm3 * PxTransform(PxVec3(0,1,0));
	PxTransform vm4 = qm4 * PxTransform(PxVec3(0,1,0));*/

}


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
