/**
 @filename EditCursor.h
 
 phenotype edit cursor class
*/
#pragma once

#include "../event/InputEvent.h"

namespace evc
{
	class CGenotypeNode;

	class CEditCursor : public IInputEvent
	{
	public:
		CEditCursor();
		virtual ~CEditCursor();

		void SelectNode(CGenotypeNode *node);

		// Key Event
		virtual bool onPointerInputEvent(const SampleFramework::InputEvent&, physx::PxU32, physx::PxU32, physx::PxReal, physx::PxReal, bool val) override {return false;}
		virtual bool onAnalogInputEvent(const SampleFramework::InputEvent& , float val) override {return false;}
		virtual bool onDigitalInputEvent(const SampleFramework::InputEvent& , bool val) override {return false;}

		// Mouse Event
		virtual bool MouseLButtonDown(physx::PxU32 x, physx::PxU32 y) override {return false;}
		virtual bool MouseLButtonUp(physx::PxU32 x, physx::PxU32 y) override {return false;}
		virtual bool MouseRButtonDown(physx::PxU32 x, physx::PxU32 y) override {return false;}
		virtual bool MouseRButtonUp(physx::PxU32 x, physx::PxU32 y) override {return false;}
		virtual bool MouseMove(physx::PxU32 x, physx::PxU32 y) override {return false;}


	protected:
		// Event
		virtual void onSelectNode(CGenotypeNode *node) {}


	protected:
		CGenotypeNode *m_selectNode;
	};

}
