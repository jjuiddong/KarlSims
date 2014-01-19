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
		void GenerateByGenotype(const string &genotypeScriptFileName, const PxVec3 &initialPos, const int recursiveCount=2, const bool isDispSkinning=true);
		void GenerateByGenome(const SGenome &genome, const PxVec3 &initialPos);
		void GenerateProgressive(const string &genotypeScriptFileName, const PxVec3 &initialPos,  const bool isDispSkinning=true);
		void Move(float dtime);
		const SGenome& GetGenome() const;
		PxVec3 GetPos() const;
		const CNeuralNet* GetBrain() const;


	protected:
		CNode* GenerateByGenotype( CNode* parentNode, const genotype_parser::SExpr *pexpr, const int recursiveCnt, 
			const PxVec3 &initialPos, const bool isGenerateBody=true, const PxVec3 &randPos=PxVec3(0,0,0), const float dimensionRate=1.f, 
			const PxVec3 &parentDim=PxVec3(0,0,0), const bool IsTerminal=false);
		CNode* GenerateTerminalNode( CNode *parentNode, const genotype_parser::SExpr *pexpr, 
			const PxVec3 &initialPos, const float dimensionRate, const PxVec3 &parentDim );
		void GenerateProgressive( CNode *currentNode, const genotype_parser::SExpr *expr );

		CNode* CreateBody(const genotype_parser::SExpr *expr, const PxVec3 &initialPos, const PxVec3 &randPos, 
			const float dimensionRate, const PxVec3 &parentDim, OUT PxVec3 &outDimension);
		void CreateJoint(CNode *parentNode, CNode *childNode, genotype_parser::SConnection *connect, const PxVec3 &conPos );
		CNode* CreateSensor(CNode *parentNode, genotype_parser::SConnection *connect, const PxVec3 &initialPos, const bool IsTerminal=false );

		void GenerateSkinningMesh();
		void GenerateRenderComposition( CNode *node );
		PxVec3 RandVec3( const PxVec3 &vec, const float rate );
		PxVec3 MaximumVec3( const PxVec3 &vec0, const PxVec3 &vec1 );
		const genotype_parser::SExpr* FindExpr( const string &name );
		void MakeExprSymbol( const genotype_parser::SExpr *expr, OUT map<string, const genotype_parser::SExpr*> &symbols );
		MaterialIndex GetMaterialType(const string &materialStr);


	private:
		CEvc &m_Sample;
		CNode *m_pRoot;
		vector<CNode*> m_Nodes;
		SGenome m_Genome;
		PxVec3 m_InitialPos;
		vector<PxTransform> m_TmPalette;

		int m_GrowCount;
		float m_IncreaseTime;
		genotype_parser::SExpr *m_pGenotypeExpr;
		map<string, const genotype_parser::SExpr*> m_GenotypeSymbols;
		bool m_IsDispSkinning;

	};


	inline const SGenome& CCreature::GetGenome() const { return m_Genome; }
}
