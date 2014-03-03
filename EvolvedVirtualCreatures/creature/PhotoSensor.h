/**
 @filename PhotoSensor.h
 
  detect  light
*/
#pragma once

#include "Sensor.h"


class CEvc;
class RenderBaseActor;
namespace evc
{
	class CPhenotypeNode;
	class CPhotoSensor : public CSensor
	{
	public:
		CPhotoSensor(CEvc &sample);
		virtual ~CPhotoSensor();
		virtual const vector<double>& GetOutput() override;
		void SetSensorInfo(CPhenotypeNode *sensorBody, const PxVec3 &dir, const float length); 


	private:
		CEvc &m_Sample;
		CPhenotypeNode *m_pSensorBody;
		RenderBaseActor *m_pSensorRenderActor;
		float m_DetectLength;
	};
}
