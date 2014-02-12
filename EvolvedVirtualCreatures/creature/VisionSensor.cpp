
#include "stdafx.h"
#include "VisionSensor.h"
#include "../EvolvedVirtualCreatures.h"
#include "physnode.h"
#include "Creature.h"


using namespace evc;


CVisionSensor::CVisionSensor(CEvc &sample) :
	CSensor(2)
,	m_Sample(sample)
,	m_pSensorBody(NULL)
,	m_pSensorRenderActor(NULL)
,	m_DetectLength(10)
{

}

CVisionSensor::~CVisionSensor()
{

}


/**
 @brief 
 @date 2013-12-19
*/
const vector<double>& CVisionSensor::GetOutput()
{
	RETV(!m_pSensorRenderActor, m_Output);

	m_Dir = m_pSensorRenderActor->getTransform().rotate(PxVec3(0,1,0));
	m_Dir.normalize();

	PxVec3 curPos = m_pSensorBody->GetBody()->getGlobalPose().p;

	// find obstacle
	float minLen = 10000.f;
	CCreature *closeObstacle = NULL;
	PxVec3 lenObstacle;
	BOOST_FOREACH (auto &obstacle, m_Sample.GetAllObstacle())
	{
		PxVec3 diff = curPos - obstacle->GetPos();
		const float len = diff.magnitude();
		if (m_DetectLength < len)
			continue;

		// calc angle
		PxVec3 tmp = diff;
		diff.normalize();
		const float angle = m_Dir.dot(diff);
		if (angle > 1)
			continue;

		if (minLen > len)
		{
			closeObstacle = obstacle;
			lenObstacle = tmp;
			minLen = len;
		}
	}

	if (closeObstacle)
	{
		m_Output[ 0] = lenObstacle.x;
		m_Output[ 1] = lenObstacle.z;
	}
	else
	{
		m_Output[ 0] = 0;
		m_Output[ 1] = 0;
	}

	return m_Output;
}


/**
 @brief 
 @date 2013-12-19
*/
void CVisionSensor::SetSensorInfo(CPhysNode *sensorBody, const float length)
{
	m_pSensorBody = sensorBody;
	m_DetectLength = length;

	PxRigidDynamic *actor = m_pSensorBody->GetBody();
	RET(!actor);

	PxU32 nbShapes = actor->getNbShapes();
	RET(!nbShapes);

	PxShape** shapes = (PxShape**)SAMPLE_ALLOC(sizeof(PxShape*)*nbShapes);
	PxU32 nb = actor->getShapes(shapes, nbShapes);
	PX_ASSERT(nb==nbShapes);
	m_pSensorRenderActor = m_Sample.getRenderActor(m_pSensorBody->GetBody(), shapes[ 0]);
	SAMPLE_FREE(shapes);
}
