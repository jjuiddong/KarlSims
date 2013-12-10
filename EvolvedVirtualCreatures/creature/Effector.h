/**
 @filename Effector.h
 
 
*/
#pragma once


namespace evc
{
	class CEffector
	{
	public:
		CEffector();
		virtual ~CEffector();
		virtual void Signal(const double signal) = 0;


	private:
	};
}
