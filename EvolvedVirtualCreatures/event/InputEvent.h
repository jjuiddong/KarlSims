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
		virtual void onPointerInputEvent(const SampleFramework::InputEvent&ie, physx::PxU32 x, physx::PxU32 y, physx::PxReal dx, physx::PxReal dy, bool val) =0;
		virtual void onAnalogInputEvent(const SampleFramework::InputEvent&ie , float val) =0;
		virtual void onDigitalInputEvent(const SampleFramework::InputEvent&ie, bool val) =0;

		// Mouse Event
		virtual void MouseLButtonDown(physx::PxU32 x, physx::PxU32 y) = 0;
		virtual void MouseLButtonUp(physx::PxU32 x, physx::PxU32 y) = 0;
		virtual void MouseRButtonDown(physx::PxU32 x, physx::PxU32 y) = 0;
		virtual void MouseRButtonUp(physx::PxU32 x, physx::PxU32 y) = 0;
		virtual void MouseMove(physx::PxU32 x, physx::PxU32 y) = 0;
	};

}
