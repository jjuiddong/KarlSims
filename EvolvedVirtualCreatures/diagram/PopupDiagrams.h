/**
 @filename PopupDiagrams.h
 
	candidate popup diagrams
*/
#pragma once


class CEvc;
namespace evc
{
	class CGenotypeNode;
	class CGenotypeController;


	class CPopupDiagrams 
	{
	public:
		CPopupDiagrams(CEvc &sample, CGenotypeController &diagramController);
		virtual ~CPopupDiagrams();

		bool Popup(CGenotypeNode *srcNode);
		void Close();
		void Render();

		void RemoveDiagram(CGenotypeNode *rmNode);
		CGenotypeNode* PickupDiagram(physx::PxU32 x, physx::PxU32 y, const bool isShowHighLight);


	protected:
		void CreateCandidateLinkNode(CGenotypeNode *srcNode, const bool isShow=true);
		void DisplayCandidateLinkNode(const bool isShow);
		CGenotypeNode* CreateBoxDiagram();
		CGenotypeNode* CreateSphereDiagram();
		CGenotypeNode* CreateSensorDiagram();
		string GenerateId( const string &seed );


	private:
		CEvc &m_sample;
		CGenotypeController &m_diagramController;
		vector<CGenotypeNode*> m_diagrams;
	};

}
