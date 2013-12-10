/**
 @filename EvolvedVirtualCreatures.h
 
 Evolving Virtual Creature Main class
*/

#pragma once

#include "PhysXSample.h"
#include "Creature/Node.h"
#include "Creature/Creature.h"


struct NodeGroup
{
	enum Enum
	{
		HEAD		= (1 << 0),
		BODY		= (1 << 1),
		L_ARM		= (1 << 2),
		R_ARM		= (1 << 3),
	};
};


DECLARE_TYPE_NAME(CEvc)

// Evc, Evolved Virtual Creatures
class CEvc : public PhysXSample
				, public PxSimulationEventCallback
				, public memmonitor::Monitor<CEvc, TYPE_NAME(CEvc)>
{
public:
	CEvc(PhysXSampleApplication& app);
	virtual ~CEvc();


	// Implements PxSimulationEventCallback
	virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {}
	virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) {}
	virtual void onConstraintBreak(PxConstraintInfo*, PxU32) {}
	virtual void onWake(PxActor** , PxU32 ) {}
	virtual void onSleep(PxActor** , PxU32 ){}


	//virtual	void onTickPreRender(float dtime);
	virtual	void	onTickPostRender(float dtime);
	virtual	void customizeSceneDesc(PxSceneDesc&);
	virtual	void onSubstepSetup(float dtime, pxtask::BaseTask* cont);

	virtual	void	newMesh(const RAWMesh&);
	virtual	void	onInit();

	virtual void collectInputEvents(std::vector<const SampleFramework::InputEvent*>& inputEvents);
	virtual void helpRender(PxU32 x, PxU32 y, PxU8 textAlpha) {}
	virtual	void descriptionRender(PxU32 x, PxU32 y, PxU8 textAlpha) {}
	virtual PxU32 getDebugObjectTypes() const;

	// test
	PxRigidDynamic*	createJointSphere(const PxVec3& pos, PxReal radius, const PxVec3* linVel=NULL, 
		RenderMaterial* material=NULL, PxReal density=1.0f);


protected:
	void spawnNode(const int key);
	void pickup();
	virtual void onDigitalInputEvent(const SampleFramework::InputEvent& , bool val);


private:
	list<evc::CCreature*> m_Creatures;

};


inline PxU32 CEvc::getDebugObjectTypes() const { 
	return DEBUG_OBJECT_BOX | DEBUG_OBJECT_SPHERE | DEBUG_OBJECT_CAPSULE | DEBUG_OBJECT_CONVEX;
}
