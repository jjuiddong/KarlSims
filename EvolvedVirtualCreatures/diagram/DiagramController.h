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

		void SetGenotype(const genotype_parser::SExpr *pexpr);
		void Move(float dtime);

		
	protected:
		//void Layout();


	private:
		CDiagramNode *m_pRootDiagram;

	};

}
