/**
 @filename PopupDiagrams.h
 
	candidate popup diagrams
*/
#pragma once


class CEvc;
namespace evc
{
	class CDiagramNode;
	class CDiagramController;


	class CPopupDiagrams 
	{
	public:
		CPopupDiagrams(CEvc &sample, CDiagramController &diagramController);
		virtual ~CPopupDiagrams();

		bool Popup(CDiagramNode *srcNode);
		void Close();
		void Render();

		void RemoveDiagram(CDiagramNode *rmNode);
		CDiagramNode* PickupDiagram(physx::PxU32 x, physx::PxU32 y, const bool isShowHighLight);


	protected:
		void CreateCandidateLinkNode(CDiagramNode *srcNode, const bool isShow=true);
		void DisplayCandidateLinkNode(const bool isShow);
		CDiagramNode* CreateBoxDiagram();
		CDiagramNode* CreateSphereDiagram();
		CDiagramNode* CreateSensorDiagram();
		string GenerateId( const string &seed );


	private:
		CEvc &m_sample;
		CDiagramController &m_diagramController;
		vector<CDiagramNode*> m_diagrams;
	};

}
