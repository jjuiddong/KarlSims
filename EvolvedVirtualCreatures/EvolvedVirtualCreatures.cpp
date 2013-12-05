
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

#include "Picking.h"

#include "Node.h"



using namespace SampleRenderer;
using namespace SampleFramework;

REGISTER_SAMPLE(CEvc, "Evolved Virtual Creatures")

///////////////////////////////////////////////////////////////////////////////

CEvc::CEvc(PhysXSampleApplication& app) :
	PhysXSample(app)
,	m_ApplyJoint(true)
,	m_Force(5000.f)
,	m_Value1(0)
,	m_Value2(0)
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


void CEvc::newMesh(const RAWMesh& data)
{
}

//class Console;
//static void gValue(Console* console, const char* text, void* userData)
//{
//	if(!text)
//	{
//		console->out("Usage: value <float>");
//		return;
//	}
//	const float val = (float)::atof(text);
//	printf("value: %f\n", val);
//}
//
//static void gExport(Console* console, const char* text, void* userData)
//{
//	if(!text)
//	{
//		console->out("Usage: export <filename>");
//		return;
//	}
//}
//
//static void gImport(Console* console, const char* text, void* userData)
//{
//	if(!text)
//	{
//		console->out("Usage: import <filename>");
//		return;
//	}
//}

void	CEvc::onTickPostRender(float dtime)
{
	PhysXSample::onTickPostRender(dtime);

	PxReal vertices[] = {0, 0, 1, 1};
	RendererColor colors[] = {RendererColor(0,255,0), RendererColor(0,255,0) };
	getRenderer()->drawLines2D(2, vertices, colors );
	PxReal vertices2[] = {1, 0, 0, 1};
	getRenderer()->drawLines2D(2, vertices2, colors );
}


void CEvc::onInit()
{
	//if (getConsole())
	//{
	//	getConsole()->addCmd("value", gValue);
	//	getConsole()->addCmd("export", gExport);
	//	getConsole()->addCmd("import", gImport);
	//}

	PhysXSample::onInit();

	mApplication.setMouseCursorHiding(true);
	mApplication.setMouseCursorRecentering(true);
	mCameraController.init(PxVec3(0.0f, 10.0f, 0.0f), PxVec3(0.0f, 0.0f, 0.0f));
	mCameraController.setMouseSensitivity(0.5f);

	//getPhysics().setParameter();
	PxSetGroupCollisionFlag(NodeGroup::BODY, NodeGroup::L_ARM, false);
	PxSetGroupCollisionFlag(NodeGroup::BODY, NodeGroup::R_ARM, false);
	PxSetGroupCollisionFlag(NodeGroup::HEAD, NodeGroup::L_ARM, false);
	PxSetGroupCollisionFlag(NodeGroup::HEAD, NodeGroup::R_ARM, false);


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
	DIGITAL_INPUT_EVENT_DEF(PICKUP, WKEY_SPACE,			XKEY_1,			PS3KEY_1,		AKEY_UNKNOWN,	OSXKEY_1,		PSP2KEY_UNKNOWN,	IKEY_UNKNOWN,	LINUXKEY_1,			WIIUKEY_UNKNOWN		);
	DIGITAL_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT2, WKEY_2,			XKEY_1,			PS3KEY_1,		AKEY_UNKNOWN,	OSXKEY_1,		PSP2KEY_UNKNOWN,	IKEY_UNKNOWN,	LINUXKEY_1,			WIIUKEY_UNKNOWN		);
	DIGITAL_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT3, WKEY_3,			XKEY_1,			PS3KEY_1,		AKEY_UNKNOWN,	OSXKEY_1,		PSP2KEY_UNKNOWN,	IKEY_UNKNOWN,	LINUXKEY_1,			WIIUKEY_UNKNOWN		);
	DIGITAL_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT4, WKEY_4,			XKEY_1,			PS3KEY_1,		AKEY_UNKNOWN,	OSXKEY_1,		PSP2KEY_UNKNOWN,	IKEY_UNKNOWN,	LINUXKEY_1,			WIIUKEY_UNKNOWN		);
	DIGITAL_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT5, WKEY_5,			XKEY_1,			PS3KEY_1,		AKEY_UNKNOWN,	OSXKEY_1,		PSP2KEY_UNKNOWN,	IKEY_UNKNOWN,	LINUXKEY_1,			WIIUKEY_UNKNOWN		);
	DIGITAL_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT6, WKEY_6,			XKEY_1,			PS3KEY_1,		AKEY_UNKNOWN,	OSXKEY_1,		PSP2KEY_UNKNOWN,	IKEY_UNKNOWN,	LINUXKEY_1,			WIIUKEY_UNKNOWN		);
	DIGITAL_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT7, WKEY_7,			XKEY_1,			PS3KEY_1,		AKEY_UNKNOWN,	OSXKEY_1,		PSP2KEY_UNKNOWN,	IKEY_UNKNOWN,	LINUXKEY_1,			WIIUKEY_UNKNOWN		);

	TOUCH_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT,	"Throw Object", ABUTTON_5,	IBUTTON_5);
	TOUCH_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT2,	"Throw Object", ABUTTON_5,	IBUTTON_5);
	TOUCH_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT3,	"Throw Object", ABUTTON_5,	IBUTTON_5);
	TOUCH_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT4,	"Throw Object", ABUTTON_5,	IBUTTON_5);
	TOUCH_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT5,	"Throw Object", ABUTTON_5,	IBUTTON_5);
	TOUCH_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT6,	"Throw Object", ABUTTON_5,	IBUTTON_5);
	TOUCH_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT7,	"Throw Object", ABUTTON_5,	IBUTTON_5);
	TOUCH_INPUT_EVENT_DEF(PICKUP,	"PickUp", ABUTTON_0, ABUTTON_0);
}


/**
 @brief 
 @date 2013-12-03
*/
void CEvc::spawnNode(const int key)
{
	PxSceneWriteLock scopedLock(*mScene);
	evc::CNode *pnode = new evc::CNode(*this);
	switch (key)
	{
	case SPAWN_DEBUG_OBJECT: pnode->GenerateHuman(m_ApplyJoint); break;
	case SPAWN_DEBUG_OBJECT2: pnode->GenerateHuman2(m_ApplyJoint); break;
	case SPAWN_DEBUG_OBJECT3: pnode->GenerateHuman3(m_ApplyJoint); break;
	case SPAWN_DEBUG_OBJECT4: pnode->GenerateHuman4(m_ApplyJoint); break;
	case SPAWN_DEBUG_OBJECT5: pnode->GenerateHuman5(m_ApplyJoint); break;
	case SPAWN_DEBUG_OBJECT6: pnode->GenerateHuman6(m_ApplyJoint); break;
	case SPAWN_DEBUG_OBJECT7: pnode->GenerateHuman7(m_ApplyJoint); break;
	}
	m_Nodes.push_back( pnode );
}


/**
 @brief 
 @date 2013-12-03
*/
void CEvc::pickup()
{
	PxU32 width;
	PxU32 height;
	mApplication.getPlatform()->getWindowSize(width, height);
	mPicking->moveCursor(width/2,height/2);
	mPicking->lazyPick();
	PxActor *actor = mPicking->letGo();
	//PxRigidDynamic *rigidActor = static_cast<PxRigidDynamic*>(actor->is<PxRigidDynamic>());
	PxRigidDynamic *rigidActor = (PxRigidDynamic*)actor;
	if (rigidActor)
	{
		const PxVec3 pos = getCamera().getPos() + (getCamera().getViewDir()*10.f);
		const PxVec3 vel = getCamera().getViewDir() * 20.f;

		rigidActor->addForce( getCamera().getViewDir()*m_Force );

		PxU32 nbShapes = rigidActor->getNbShapes();
		if(!nbShapes)
			return;

		PxShape** shapes = (PxShape**)SAMPLE_ALLOC(sizeof(PxShape*)*nbShapes);
		PxU32 nb = rigidActor->getShapes(shapes, nbShapes);
		PX_ASSERT(nb==nbShapes);
		for(PxU32 i=0;i<nbShapes;i++)
		{
			RenderBaseActor *renderActor = getRenderActor(rigidActor, shapes[i]);
			if (renderActor)
			{
				renderActor->setRenderMaterial(mManagedMaterials[ 1]);
			}
		}
		SAMPLE_FREE(shapes);
	}	
}


/**
 @brief 
 @date 2013-12-03
*/
void CEvc::onDigitalInputEvent(const SampleFramework::InputEvent &ie, bool val)
{
	if (val)
	{
		switch (ie.m_Id)
		{
		case SPAWN_DEBUG_OBJECT:
		case SPAWN_DEBUG_OBJECT2:
		case SPAWN_DEBUG_OBJECT3:
		case SPAWN_DEBUG_OBJECT4:
		case SPAWN_DEBUG_OBJECT5:
		case SPAWN_DEBUG_OBJECT6:
		case SPAWN_DEBUG_OBJECT7:
			spawnNode(ie.m_Id);
			break;

		case PICKUP:
			pickup();
			break;
		}
	}
}


/**
 @brief 
 @date 2013-12-03
*/
void CEvc::onSubstepSetup(float dtime, pxtask::BaseTask* cont)
{
	PhysXSample::onSubstepSetup(dtime, cont);

	BOOST_FOREACH (auto &node, m_Nodes)
	{
		node->Move(dtime);
	}
}


PxFilterFlags SampleSubmarineFilterShader(	
	PxFilterObjectAttributes attributes0, PxFilterData filterData0, 
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	// let triggers through
	if(PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	// generate contacts for all that were not filtered above
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;

	// trigger the contact callback for pairs (A,B) where 
	// the filtermask of A contains the ID of B and vice versa.
	if((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

	return PxFilterFlag::eDEFAULT;
}


void CEvc::customizeSceneDesc(PxSceneDesc& sceneDesc)
{
	//sceneDesc.filterShader = SampleSubmarineFilterShader;
	//sceneDesc.simulationEventCallback = this;
	sceneDesc.flags |= PxSceneFlag::eREQUIRE_RW_LOCK;
}
