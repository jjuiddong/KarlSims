/**
 @filename Creature.h
 
*/
#pragma once

#include "PhysXSample.h"
#include "../genetic/Genome.h"


class CEvc;
class RenderComposition;
namespace evc
{
	class CJoint;
	class CNode;
	class CNeuralNet;
	namespace genotype_parser { struct SExpr; struct SConnection; }


	DECLARE_TYPE_NAME_SCOPE(evc, CCreature)
	class CCreature : public memmonitor::Monitor<CCreature, TYPE_NAME(CCreature)>
	{
	public:
		CCreature(CEvc &sample);
		virtual ~CCreature();
		void GenerateByGenotype(const string &genotypeScriptFileName, const PxVec3 &initialPos,  const bool isDispSkinning=true);
		void GenerateByGenome(const SGenome &genome, const PxVec3 &initialPos);
		void Move(float dtime);
		const SGenome& GetGenome() const;
		PxVec3 GetPos() const;
		const CNeuralNet* GetBrain() const;


	protected:
		CNode* GenerateByGenotype( CNode* parentNode, const genotype_parser::SExpr *pexpr, const int recursiveCnt, 
			const PxVec3 &initialPos, const PxVec3 &randPos=PxVec3(0,0,0), const float dimensionRate=1.f, 
			const PxVec3 &parentDim=PxVec3(0,0,0), const bool IsTerminal=false);
		CNode* GenerateTerminalNode( CNode *parentNode, const genotype_parser::SExpr *pexpr, 
			const PxVec3 &initialPos, const float dimensionRate, const PxVec3 &parentDim );

		void CreateJoint(CNode *parentNode, CNode *childNode, genotype_parser::SConnection *connect, const PxVec3 &conPos );
		CNode* CreateSensor(CNode *parentNode, genotype_parser::SConnection *connect, const PxVec3 &initialPos, const bool IsTerminal=false );

		MaterialIndex GetMaterialType(const string &materialStr);
		void GenerateRenderComposition( CNode *node );
		PxVec3 RandVec3( const PxVec3 &vec, const float rate );
		PxVec3 MaximumVec3( const PxVec3 &vec0, const PxVec3 &vec1 );

	private:
		CEvc &m_Sample;
		CNode *m_pRoot;
		vector<CNode*> m_Nodes;
		SGenome m_Genome;
		PxVec3 m_InitialPos;
		vector<PxTransform> m_TmPalette;
	};


	inline const SGenome& CCreature::GetGenome() const { return m_Genome; }
}
