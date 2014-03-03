/**
 @filename DiagramController.h
 
	Diagram Edit and Management 
*/
#pragma once

#include "DiagramGlobal.h"


class CEvc;
class RenderComposition;
class CSimpleCamera;
class RenderBezierActor;
namespace evc
{
	class CCreature;
	class CGenotypeNode;
	class CPopupDiagrams;
	class COrientationEditController;
	namespace genotype_parser { struct SExpr; struct SConnection; }


	DECLARE_TYPE_NAME_SCOPE(evc, CGenotypeController)
	class CGenotypeController : public SampleFramework::InputEventListener,
												public memmonitor::Monitor<CGenotypeController, TYPE_NAME(CGenotypeController)>
	{
		enum MODE { MODE_NONE, MODE_LINK, MODE_ORIENT };

	public:
		CGenotypeController(CEvc &sample);
		virtual ~CGenotypeController();

		void ControllerSceneInit();
		void SetControlCreature(CCreature *creature);//const genotype_parser::SExpr *expr);
		void Render();
		void Move(float dtime);
		CGenotypeNode *GetRootDiagram();
		vector<CGenotypeNode*>& GetDiagrams();
		bool GetDiagramsLinkto(CGenotypeNode *to, OUT vector<CGenotypeNode*> &out);
		bool GetDiagramsLinkfrom(CGenotypeNode *from, OUT vector<CGenotypeNode*> &out);
		void Show(const bool isShow);

		// InputEvent from CEvc
		virtual void onPointerInputEvent(const SampleFramework::InputEvent&, physx::PxU32, physx::PxU32, physx::PxReal, physx::PxReal, bool val) override;
		virtual void onAnalogInputEvent(const SampleFramework::InputEvent& , float val) override {}
		virtual void onDigitalInputEvent(const SampleFramework::InputEvent& , bool val) override;


	protected:
		CGenotypeNode* CreateGenotypeDiagramTree(const PxVec3 &pos, const genotype_parser::SExpr *expr, 
			map<const genotype_parser::SExpr*, CGenotypeNode*> &symbols);

		void Layout(const PxVec3 &pos=PxVec3(0,0,0));
		PxVec3 LayoutRec(CGenotypeNode *node, set<CGenotypeNode*> &symbols, const PxVec3 &pos=PxVec3(0,0,0));
		void SelectNode(CGenotypeNode *node, const bool isShowPopupDiagrams=true);
		CGenotypeNode* PickupDiagram(physx::PxU32 x, physx::PxU32 y, const bool isCheckLinkDiagram, const bool isShowHighLight);
		void TransitionAnimation(const float dtime);

		bool InsertDiagram(CGenotypeNode *node, CGenotypeNode *insertNode);
		bool RemoveDiagram(CGenotypeNode *rmNode);
		void RemoveUnlinkDiagram();

		void UpdateCreature();
		void ChangeControllerMode(const MODE scene);

		// Event Handler
		void MouseLButtonDown(physx::PxU32 x, physx::PxU32 y);
		void MouseLButtonUp(physx::PxU32 x, physx::PxU32 y);
		void MouseRButtonDown(physx::PxU32 x, physx::PxU32 y);
		void MouseRButtonUp(physx::PxU32 x, physx::PxU32 y);
		void MouseMove(physx::PxU32 x, physx::PxU32 y);


	private:
		CEvc &m_sample;
		CSimpleCamera *m_camera;
		COrientationEditController *m_OrientationEditController;

		CCreature *m_creature; // reference
		CGenotypeNode *m_rootDiagram;
		vector<CGenotypeNode*> m_diagrams; // reference
		CGenotypeNode *m_selectNode; // reference
		CPopupDiagrams *m_popupDiagrams;

		bool m_leftButtonDown;
		bool m_rightButtonDown;
		MODE m_controlMode;
		PxVec2 m_dragPos[ 2];

		// layout animation to move transition
		bool m_isLayoutAnimation;
		float m_elapsTime;
	};


	inline CGenotypeNode *CGenotypeController::GetRootDiagram() { return m_rootDiagram; }
	inline vector<CGenotypeNode*>& CGenotypeController::GetDiagrams() { return m_diagrams; }
}
