
#include "stdafx.h"
#include "PhotoSensor.h"
#include "../EvolvedVirtualCreatures.h"
#include "phenotypenode.h"


using namespace evc;


CPhotoSensor::CPhotoSensor(CEvc &sample) :
	CSensor(2)
,	m_Sample(sample)
,	m_pSensorBody(NULL)
,	m_pSensorRenderActor(NULL)
,	m_DetectLength(10)
{
}

CPhotoSensor::~CPhotoSensor()
{

}


/**
 @brief calculate sensor output
 @date 2013-12-19
*/
const vector<double>& CPhotoSensor::GetOutput()
{
	RETV(!m_pSensorRenderActor, m_Output);

	m_Dir = m_pSensorRenderActor->getTransform().rotate(PxVec3(0,1,0));
	m_Dir.normalize();


	return m_Output;
}


/**
 @brief 
 @date 2013-12-19
*/
void CPhotoSensor::SetSensorInfo(CPhenotypeNode *sensorBody, const PxVec3 &dir, const float length)
{
	m_pSensorBody = sensorBody;
	m_Dir = dir;
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
