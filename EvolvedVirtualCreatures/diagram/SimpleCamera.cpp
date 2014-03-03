
#include "stdafx.h"
#include "SimpleCamera.h"
#include "GenotypeController.h"
#include "GenotypeNode.h"
#include "../EvolvedVirtualCreatures.h"
#include "SampleUserInputIds.h"


CSimpleCamera::CSimpleCamera()
{

}

CSimpleCamera::~CSimpleCamera()
{

}


/**
 @brief mouse event
 @date 2014-02-24
*/
void CSimpleCamera::onPointerInputEvent(const SampleFramework::InputEvent& ie, 
	physx::PxU32 x, physx::PxU32 y, physx::PxReal dx, physx::PxReal dy, bool val)
{
	// nothing~
}
