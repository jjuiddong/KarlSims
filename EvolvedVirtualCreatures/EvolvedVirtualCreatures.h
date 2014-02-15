/**
 @filename EvolvedVirtualCreatures.h
 
 Evolving Virtual Creature Main class (E.V.C)
*/

#pragma once

#include "PhysXSample.h"

namespace evc 
{ 
	class CCreature;
	class CDiagramController;
}

// Evc, Evolved Virtual Creatures
DECLARE_TYPE_NAME(CEvc)
class CEvc : public PhysXSample
				, public PxSimulationEventCallback
				, public memmonitor::Monitor<CEvc, TYPE_NAME(CEvc)>
{
public:
	CEvc(PhysXSampleApplication& app);
	virtual ~CEvc();

	RenderMaterial* GetMaterial(const PxVec3 &rgb, bool applyVertexColor=true);
	const list<evc::CCreature*>& GetAllCreature();
	const list<evc::CCreature*>& GetAllObstacle();


	// Implements PxSimulationEventCallback
	virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {}
	virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) {}
	virtual void onConstraintBreak(PxConstraintInfo*, PxU32) {}
	virtual void onWake(PxActor** , PxU32 ) {}
	virtual void onSleep(PxActor** , PxU32 ){}


	//virtual	void onTickPreRender(float dtime) override;
	virtual	void customizeRender() override;
	virtual	void	onTickPostRender(float dtime) override;
	virtual	void customizeSceneDesc(PxSceneDesc&);
	virtual	void onSubstepSetup(float dtime, pxtask::BaseTask* cont);

	//virtual	void	newMesh(const RAWMesh&);
	virtual	void	onInit();
	virtual	void	onSubstep(float dtime);

	virtual void collectInputEvents(std::vector<const SampleFramework::InputEvent*>& inputEvents);
	virtual void helpRender(PxU32 x, PxU32 y, PxU8 textAlpha) {}
	virtual	void descriptionRender(PxU32 x, PxU32 y, PxU8 textAlpha) {}
	virtual PxU32 getDebugObjectTypes() const;


protected:
	void spawnNode(const int key);
	void pickup();
	void gotoNextGenration();
	virtual void onDigitalInputEvent(const SampleFramework::InputEvent& , bool val);
	virtual void onPointerInputEvent(const SampleFramework::InputEvent& ie, physx::PxU32 x, physx::PxU32 y, physx::PxReal dx, physx::PxReal dy, bool val) override;
	virtual void onShutdown();
	void RemoveAllCreatures();


private:
	list<evc::CCreature*> m_Creatures;
	list<evc::CCreature*> m_Obstacles;
	vector<PxRigidActor*> m_Planet; // composite ground physx actor
	evc::CDiagramController *m_DiagramController;
	map<int, RenderMaterial*> m_Materials; // key=r*100,g*10,b, value = material

	double m_ElapsTime; // for gentic algorithm epoch
	int m_Age;
	float m_Gap;
	bool m_IsApplyCustomGravity;
};


inline PxU32 CEvc::getDebugObjectTypes() const { 
	return DEBUG_OBJECT_BOX | DEBUG_OBJECT_SPHERE | DEBUG_OBJECT_CAPSULE | DEBUG_OBJECT_CONVEX;
}

inline const list<evc::CCreature*>& CEvc::GetAllCreature() { return m_Creatures; }
inline const list<evc::CCreature*>& CEvc::GetAllObstacle() { return m_Obstacles; }

