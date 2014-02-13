/**
 @filename DiagramController.h
 
	Diagram Edit and Management 
*/
#pragma once


class CEvc;
class RenderComposition;
namespace evc
{
	class CDiagramNode;
	namespace genotype_parser { struct SExpr; struct SConnection; }


	DECLARE_TYPE_NAME_SCOPE(evc, CDiagramController)
	class CDiagramController : public memmonitor::Monitor<CDiagramController, TYPE_NAME(CDiagramController)>
	{
	public:
		CDiagramController(CEvc &sample);
		virtual ~CDiagramController();

		void SetGenotype(const genotype_parser::SExpr *expr);
		void Move(float dtime);

		
	protected:
		CDiagramNode* CreateDiagramNode(const genotype_parser::SExpr *expr);
		void RemoveDiagram(CDiagramNode *node);
		void Layout(CDiagramNode *root);


	private:
		CEvc &m_Sample;
		CDiagramNode *m_pRootDiagram;

	};
}
