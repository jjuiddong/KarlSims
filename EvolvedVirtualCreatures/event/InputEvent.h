/**
 @filename MouseEvent.h
  
*/
#pragma once


namespace evc
{

	class IInputEvent
	{
	public:
		// Key Event
		virtual bool onPointerInputEvent(const SampleFramework::InputEvent&ie, physx::PxU32 x, physx::PxU32 y, physx::PxReal dx, physx::PxReal dy, bool val) =0;
		virtual bool onAnalogInputEvent(const SampleFramework::InputEvent&ie , float val) =0;
		virtual bool onDigitalInputEvent(const SampleFramework::InputEvent&ie, bool val) =0;

		// Mouse Event
		virtual bool MouseLButtonDown(physx::PxU32 x, physx::PxU32 y) = 0;
		virtual bool MouseLButtonUp(physx::PxU32 x, physx::PxU32 y) = 0;
		virtual bool MouseRButtonDown(physx::PxU32 x, physx::PxU32 y) = 0;
		virtual bool MouseRButtonUp(physx::PxU32 x, physx::PxU32 y) = 0;
		virtual bool MouseMove(physx::PxU32 x, physx::PxU32 y) = 0;
	};

}
