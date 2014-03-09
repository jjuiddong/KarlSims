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
		virtual bool MouseLButtonDown(physx::PxU32 x, physx::PxU32 y) override;
		virtual bool MouseLButtonUp(physx::PxU32 x, physx::PxU32 y) override {return false;}
		virtual bool MouseRButtonDown(physx::PxU32 x, physx::PxU32 y) override {return false;}
		virtual bool MouseRButtonUp(physx::PxU32 x, physx::PxU32 y) override {return false;}
		virtual bool MouseMove(physx::PxU32 x, physx::PxU32 y) override {return false;}


	protected:
		// Event
		virtual void onSelectNode(CGenotypeNode *node);
		RenderBezierActor* PickScaleAxis();


	private:
		CEvc &m_sample;
		vector<RenderBezierActor*> m_lines;
	};

}
