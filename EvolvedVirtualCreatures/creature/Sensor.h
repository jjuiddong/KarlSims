/**
 @filename Sensor.h
 
 sensor interface 
*/
#pragma once


namespace evc
{
	class CSensor
	{
	public:
		CSensor() {}
		virtual ~CSensor() {}
		virtual double GetOutput() = 0;

	protected:
		double m_Output;
	};
}
