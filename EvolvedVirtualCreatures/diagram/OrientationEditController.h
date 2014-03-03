/**
 @filename OrientationEditController.h
 
*/
#pragma once


class CEvc;
namespace evc
{
	class CGenotypeController;
	class CGenotypeNode;

	DECLARE_TYPE_NAME_SCOPE(evc, COrientationEditController)
	class COrientationEditController: public SampleFramework::InputEventListener,
														public memmonitor::Monitor<COrientationEditController, TYPE_NAME(COrientationEditController)>
	{
	public:
		COrientationEditController(CEvc &sample, CGenotypeController &diagramController);
		virtual ~COrientationEditController();

		void ControllerSceneInit();
		void SetControlDiagram(CGenotypeNode *node);

		// InputEvent from CEvc
		virtual void onPointerInputEvent(const SampleFramework::InputEvent&, physx::PxU32, physx::PxU32, physx::PxReal, physx::PxReal, bool val) override {}
		virtual void onAnalogInputEvent(const SampleFramework::InputEvent& , float val) override {}
		virtual void onDigitalInputEvent(const SampleFramework::InputEvent& , bool val) override {}

		// Mouse Event from CDiagramController
		void MouseLButtonDown(physx::PxU32 x, physx::PxU32 y);
		void MouseLButtonUp(physx::PxU32 x, physx::PxU32 y);
		void MouseRButtonDown(physx::PxU32 x, physx::PxU32 y);
		void MouseRButtonUp(physx::PxU32 x, physx::PxU32 y);
		void MouseMove(physx::PxU32 x, physx::PxU32 y);


	protected:
		CGenotypeNode* CreatePhenotypeDiagram(const PxVec3 &pos, genotype_parser::SExpr *expr,
			map<const genotype_parser::SExpr*, CGenotypeNode*> &symbols);


	private:
		CEvc &m_sample;
		CGenotypeController &m_genotypeController;
		DefaultCameraController *m_camera;

		CGenotypeNode *m_rootDiagram;
		vector<CGenotypeNode*> m_diagrams; // reference
		CGenotypeNode *m_selectNode; // reference
	};
}
