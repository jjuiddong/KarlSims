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
		enum AXIS { X_AXIS, Y_AXIS, Z_AXIS };

	public:
		CScaleCursor(CEvc &sample);
		virtual ~CScaleCursor();

		// Mouse Event
		virtual bool MouseLButtonDown(physx::PxU32 x, physx::PxU32 y) override;
		virtual bool MouseLButtonUp(physx::PxU32 x, physx::PxU32 y) override;
		virtual bool MouseRButtonDown(physx::PxU32 x, physx::PxU32 y) override {return false;}
		virtual bool MouseRButtonUp(physx::PxU32 x, physx::PxU32 y) override {return false;}
		virtual bool MouseMove(physx::PxU32 x, physx::PxU32 y) override;


	protected:
		// Event
		virtual void onSelectNode(CGenotypeNode *node);
		bool PickScaleAxis(physx::PxU32 x, physx::PxU32 y, OUT AXIS &out);
		void SelectAxis(const bool isSelect, const AXIS axis);
		void EditScale(CGenotypeNode *node, const Int2 initialPos, const Int2 curPos, const AXIS axis);


	private:
		CEvc &m_sample;
		vector<RenderBezierActor*> m_lines;
		vector<PxVec3> m_colors;
		vector<PxVec3> m_axises;

		AXIS m_selectAxis;
		bool m_isEditScale;
		Int2 m_mousePos;
	};

}
