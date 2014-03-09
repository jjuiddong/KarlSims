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
		virtual void onPointerInputEvent(const SampleFramework::InputEvent&, physx::PxU32, physx::PxU32, physx::PxReal, physx::PxReal, bool val) override {}
		virtual void onAnalogInputEvent(const SampleFramework::InputEvent& , float val) override {}
		virtual void onDigitalInputEvent(const SampleFramework::InputEvent& , bool val) override {}

		// Mouse Event
		virtual void MouseLButtonDown(physx::PxU32 x, physx::PxU32 y) override {}
		virtual void MouseLButtonUp(physx::PxU32 x, physx::PxU32 y) override {}
		virtual void MouseRButtonDown(physx::PxU32 x, physx::PxU32 y) override {}
		virtual void MouseRButtonUp(physx::PxU32 x, physx::PxU32 y) override {}
		virtual void MouseMove(physx::PxU32 x, physx::PxU32 y) override {}


	protected:
		// Event
		virtual void onSelectNode(CGenotypeNode *node) {}


	private:
		CGenotypeNode *m_selectNode;
	};

}
