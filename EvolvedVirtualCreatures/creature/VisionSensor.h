/**
 @filename VisionSensor.h
 
  detect creature, obstacle using sensor direction
*/
#pragma once

#include "Sensor.h"


class CEvc;
class RenderBaseActor;
namespace evc
{
	class CPhysNode;
	class CVisionSensor : public CSensor
	{
	public:
		CVisionSensor(CEvc &sample);
		virtual ~CVisionSensor();
		virtual const vector<double>& GetOutput() override;
		void SetSensorInfo(CPhysNode *sensorBody, const float length); 


	private:
		CEvc &m_Sample;
		CPhysNode *m_pSensorBody;
		RenderBaseActor *m_pSensorRenderActor;
		float m_DetectLength;
	};
}
