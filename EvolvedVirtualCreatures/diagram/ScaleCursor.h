/**
 @filename ScaleCursor.h
 
	Scale Edit Cursor 
*/
#pragma once

#include "EditCursor.h"

class CEvc;
class RenderBezierActor;
namespace evc
{

	class CScaleCursor : public CEditCursor
	{
	public:
		CScaleCursor(CEvc &sample);
		virtual ~CScaleCursor();

		// Mouse Event
		virtual void MouseLButtonDown(physx::PxU32 x, physx::PxU32 y) override {}
		virtual void MouseLButtonUp(physx::PxU32 x, physx::PxU32 y) override {}
		virtual void MouseRButtonDown(physx::PxU32 x, physx::PxU32 y) override {}
		virtual void MouseRButtonUp(physx::PxU32 x, physx::PxU32 y) override {}
		virtual void MouseMove(physx::PxU32 x, physx::PxU32 y) override {}


	protected:
		// Event
		virtual void onSelectNode(CGenotypeNode *node);


	private:
		CEvc &m_sample;
		vector<RenderBezierActor*> m_lines;
	};

}
