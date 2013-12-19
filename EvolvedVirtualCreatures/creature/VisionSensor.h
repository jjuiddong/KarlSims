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
	class CNode;
	class CVisionSensor : public CSensor
	{
	public:
		CVisionSensor(CEvc &sample);
		virtual ~CVisionSensor();
		virtual const vector<double>& GetOutput() override;
		void SetSensorInfo(CNode *sensorBody, const float length); 


	protected:


	private:
		CEvc &m_Sample;
		CNode *m_pSensorBody;
		RenderBaseActor *m_pSensorRenderActor;
		float m_DetectLength;
	};
}
