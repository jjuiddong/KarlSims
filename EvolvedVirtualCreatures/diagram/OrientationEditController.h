/**
 @filename OrientationEditController.h
 
*/
#pragma once


class CEvc;
class CSimpleCamera;
namespace evc
{
	class CGenotypeController;
	class CGenotypeNode;
	class CEditCursor;

	DECLARE_TYPE_NAME_SCOPE(evc, COrientationEditController)
	class COrientationEditController: public SampleFramework::InputEventListener,
														public memmonitor::Monitor<COrientationEditController, TYPE_NAME(COrientationEditController)>
	{
		enum EDIT_MODE {
			MODE_NONE, 
			MODE_POSITION,
			MODE_ROTATION,
			MODE_SCALE,
			MODE_PICKUP,
		};
		enum CURSOR_TYPE {
			CURSOR_SCALE,
			CURSOR_ROTATION,
			CURSOR_POSITION,
		};


	public:
		COrientationEditController(CEvc &sample, CGenotypeController &genotypeController);
		virtual ~COrientationEditController();

		void ControllerSceneInit();
		void SetControlDiagram(CGenotypeNode *node);

		// InputEvent from CEvc
		virtual void onPointerInputEvent(const SampleFramework::InputEvent&, physx::PxU32, physx::PxU32, physx::PxReal, physx::PxReal, bool val) override {}
		virtual void onAnalogInputEvent(const SampleFramework::InputEvent& , float val) override {}
		virtual void onDigitalInputEvent(const SampleFramework::InputEvent& , bool val) override;

		// Mouse Event from CDiagramController
		void MouseLButtonDown(physx::PxU32 x, physx::PxU32 y);
		void MouseLButtonUp(physx::PxU32 x, physx::PxU32 y);
		void MouseRButtonDown(physx::PxU32 x, physx::PxU32 y);
		void MouseRButtonUp(physx::PxU32 x, physx::PxU32 y);
		void MouseMove(physx::PxU32 x, physx::PxU32 y);


	protected:
		CGenotypeNode* CreatePhenotypeDiagram(const PxVec3 &localPos, const PxQuat &localRotate,
			genotype_parser::SExpr *expr, map<const genotype_parser::SExpr*, CGenotypeNode*> &symbols);
		void ChangeEditMode(EDIT_MODE mode);
		bool SelectNode(CGenotypeNode *node, const EDIT_MODE mode=MODE_POSITION);


	private:
		CEvc &m_sample;
		CGenotypeController &m_genotypeController;
		CSimpleCamera  *m_camera;

		CGenotypeNode *m_rootNode;
		vector<CGenotypeNode*> m_nodes; // reference
		CGenotypeNode *m_selectNode; // reference

		EDIT_MODE m_editMode;
		vector<CEditCursor*> m_allCursor;
		CEditCursor *m_cursor;
		Int2 m_ptOrig;
	};
}
