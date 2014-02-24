/**
 @filename DiagramController.h
 
	Diagram Edit and Management 
*/
#pragma once


class CEvc;
class RenderComposition;
class CSimpleCamera;
namespace evc
{
	class CDiagramNode;
	namespace genotype_parser { struct SExpr; struct SConnection; }


	DECLARE_TYPE_NAME_SCOPE(evc, CDiagramController)
	class CDiagramController : public SampleFramework::InputEventListener,
												public memmonitor::Monitor<CDiagramController, TYPE_NAME(CDiagramController)>
	{
	public:
		CDiagramController(CEvc &sample);
		virtual ~CDiagramController();

		void ControllerSceneInit();
		void SetGenotype(const genotype_parser::SExpr *expr);
		void Render();
		void Move(float dtime);
		CDiagramNode *GetRootDiagram();
		vector<CDiagramNode*>& GetDiagrams();
		void SelectNode(CDiagramNode *node);

		// InputEvnt from CEvc
		virtual void onPointerInputEvent(const SampleFramework::InputEvent&, physx::PxU32, physx::PxU32, physx::PxReal, physx::PxReal, bool val) override;
		virtual void onAnalogInputEvent(const SampleFramework::InputEvent& , float val) override;
		virtual void onDigitalInputEvent(const SampleFramework::InputEvent& , bool val) override;


	protected:
		CDiagramNode* CreateDiagramNode(const PxVec3 &pos, const genotype_parser::SExpr *expr, 
			map<const genotype_parser::SExpr*, CDiagramNode*> &diagrams);
		void RemoveDiagram(CDiagramNode *node, set<CDiagramNode*> &diagrams);
		void Layout(CDiagramNode *root);


	private:
		CEvc &m_sample;
		CDiagramNode *m_rootDiagram;
		vector<CDiagramNode*> m_diagrams;
		CSimpleCamera *m_camera;
		CDiagramNode *m_selectNode;
	};


	inline CDiagramNode *CDiagramController::GetRootDiagram() { return m_rootDiagram; }
	inline vector<CDiagramNode*>& CDiagramController::GetDiagrams() { return m_diagrams; }
}
