
#include "stdafx.h"
#include "SamplePreprocessor.h"
#include "EvolvedVirtualCreatures.h"
#include "SampleUtils.h"
#include "SampleConsole.h"
#include "RendererMemoryMacros.h"
#include "RenderMeshActor.h"

#include "PxPhysics.h"
#include "PxScene.h"
#include "PxRigidDynamic.h"
#include "PxShape.h"
#include "PxPhysicsAPI.h"
#include "RenderBoxActor.h"

#include <SampleBaseInputEventIds.h>
#include <SamplePlatform.h>
#include <SampleUserInput.h>
#include <SampleUserInputIds.h>
#include <SampleUserInputDefines.h>


#include "Node.h"



using namespace SampleRenderer;
using namespace SampleFramework;

REGISTER_SAMPLE(CEvc, "Evolved Virtual Creatures")

///////////////////////////////////////////////////////////////////////////////

CEvc::CEvc(PhysXSampleApplication& app) :
	PhysXSample(app)
{
}

CEvc::~CEvc()
{
	BOOST_FOREACH(auto &node, m_Nodes)
	{
		delete node;
	}
	m_Nodes.clear();

}

void CEvc::onTickPreRender(float dtime)
{
	PhysXSample::onTickPreRender(dtime);
}

void CEvc::onTickPostRender(float dtime)
{
	PhysXSample::onTickPostRender(dtime);
}

void CEvc::customizeSceneDesc(PxSceneDesc& sceneDesc)
{
	sceneDesc.flags |= PxSceneFlag::eREQUIRE_RW_LOCK;
}

void CEvc::newMesh(const RAWMesh& data)
{
}

static void gValue(Console* console, const char* text, void* userData)
{
	if(!text)
	{
		console->out("Usage: value <float>");
		return;
	}

	const float val = (float)::atof(text);
	printf("value: %f\n", val);
}

static void gExport(Console* console, const char* text, void* userData)
{
	if(!text)
	{
		console->out("Usage: export <filename>");
		return;
	}
}

static void gImport(Console* console, const char* text, void* userData)
{
	if(!text)
	{
		console->out("Usage: import <filename>");
		return;
	}
}

void CEvc::onInit()
{
	if (getConsole())
	{
		getConsole()->addCmd("value", gValue);
		getConsole()->addCmd("export", gExport);
		getConsole()->addCmd("import", gImport);
	}

	PhysXSample::onInit();

	mApplication.setMouseCursorHiding(true);
	mApplication.setMouseCursorRecentering(true);
	mCameraController.init(PxVec3(0.0f, 10.0f, 0.0f), PxVec3(0.0f, 0.0f, 0.0f));
	mCameraController.setMouseSensitivity(0.5f);


	//PxRigidDynamic* actor1 = NULL;
	//const PxVec3 pos = getCamera().getPos();
	//const PxVec3 vel = getCamera().getViewDir() * getDebugObjectsVelocity();
	//actor1 = createSphere(pos, getDebugSphereObjectRadius(), &vel, mManagedMaterials[MATERIAL_GREEN], mDefaultDensity);

	//PxRigidDynamic* actor2 = NULL;
	//PxVec3 pos2 = getCamera().getPos();
	//pos2.x += 1.f;
	//actor2 = createSphere(pos2, getDebugSphereObjectRadius(), &vel, mManagedMaterials[MATERIAL_GREEN], mDefaultDensity);


	// Joint Test
	//const float scale = 1.f;
	//const float plankDepth = 2.f;
	////PxRevoluteJoint* j = PxRevoluteJointCreate(getPhysics(), 
	////PxFixedJoint* j = PxFixedJointCreate(getPhysics(), 
	//PxSphericalJoint* j = PxSphericalJointCreate(getPhysics(), 
	//	actor1, PxTransform(PxVec3(0,0, plankDepth)*scale),
	//	actor2, PxTransform(PxVec3(0,0,-plankDepth)*scale));
	//if(j)
	//	j->setProjectionLinearTolerance(.5f);

}


PxRigidDynamic*	createJointSphere(const PxVec3& pos, PxReal radius, const PxVec3* linVel, 
	RenderMaterial* material, PxReal density )
{


	return NULL;
}


void CEvc::collectInputEvents(std::vector<const SampleFramework::InputEvent*>& inputEvents)
{
	PhysXSample::collectInputEvents(inputEvents);
	getApplication().getPlatform()->getSampleUserInput()->unregisterInputEvent(CAMERA_SPEED_INCREASE);
	getApplication().getPlatform()->getSampleUserInput()->unregisterInputEvent(CAMERA_SPEED_DECREASE);
    
	//touch events
	TOUCH_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT,	"Throw Object",		ABUTTON_5,	IBUTTON_5);
}


void CEvc::spawnNode()
{
	evc::CNode *pnode = new evc::CNode(*this);
	pnode->GenerateHuman();
	m_Nodes.push_back( pnode );
}


void CEvc::onDigitalInputEvent(const SampleFramework::InputEvent &ie, bool val)
{
	if (val)
	{
		switch (ie.m_Id)
		{
		case SPAWN_DEBUG_OBJECT:
			spawnNode();
			break;
		}
	}
}
