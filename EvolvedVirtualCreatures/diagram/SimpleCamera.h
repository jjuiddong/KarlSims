/**
 @filename EditDiagramCamera.h
 
 
*/
#pragma once


#include "SampleCameraController.h"

class CEvc;
class CSimpleCamera : public DefaultCameraController
{
public:
	CSimpleCamera();
	virtual ~CSimpleCamera();
	virtual void onPointerInputEvent(const SampleFramework::InputEvent& ie, physx::PxU32 x, physx::PxU32 y, physx::PxReal dx, physx::PxReal dy, bool val);
};
