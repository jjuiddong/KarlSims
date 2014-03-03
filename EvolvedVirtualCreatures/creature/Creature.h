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
	class CPhenotypeNode;
	class CNeuralNet;
	namespace genotype_parser { struct SExpr; struct SConnection; }


	DECLARE_TYPE_NAME_SCOPE(evc, CCreature)
	class CCreature : public memmonitor::Monitor<CCreature, TYPE_NAME(CCreature)>
	{
	public:
		CCreature(CEvc &sample);
		virtual ~CCreature();
		void GenerateImmediate(const string &genotypeScriptFileName, const PxVec3 &initialPos, const PxVec3 *linVel, const int recursiveCount=2, const bool isDispSkinning=true);
		void GenerateImmediate(genotype_parser::SExpr *expr, const PxVec3 &initialPos, const PxVec3 *linVel, const int recursiveCount=2, const bool isDispSkinning=true);
		void GenerateByGenome(const SGenome &genome, const PxVec3 &initialPos);
		void GenerateProgressive(const string &genotypeScriptFileName, const PxVec3 &initialPos, const PxVec3 *linVel, const bool isDispSkinning=true);
		void GenerateProgressive(genotype_parser::SExpr *expr, const PxVec3 &initialPos, const PxVec3 *linVel, const bool isDispSkinning=true);
		void Move(float dtime);
		const SGenome& GetGenome() const;
		PxVec3 GetPos() const;
		void SetPos(const PxVec3 &pos);
		const CNeuralNet* GetBrain() const;
		void SetGravity(const PxVec3 &centerOfGravity);
		genotype_parser::SExpr* GetGenotype();
		void SetMaxGrowCount(const int maxGrowCount);


	protected:
		void FirstStepToGenerateByGenotype(genotype_parser::SExpr *expr, const PxVec3 &initialPos, const PxVec3 *linVel,
			const int recursiveCount=2, const bool isDispSkinning=true);
		CPhenotypeNode* GenerateByGenotype( CPhenotypeNode* parentNode, const genotype_parser::SExpr *pexpr, const int recursiveCnt, 
			const PxVec3 &initialPos, const PxVec3 *linVel, const bool isGenerateBody=true, const PxVec3 &randPos=PxVec3(0,0,0), 
			const float dimensionRate=1.f, const PxVec3 &parentDim=PxVec3(0,0,0), const bool IsTerminal=false);
		CPhenotypeNode* GenerateTerminalNode( CPhenotypeNode *parentNode, const genotype_parser::SExpr *pexpr, 
			const PxVec3 &initialPos, const PxVec3 *linVel, const float dimensionRate, const PxVec3 &parentDim );
		void GenerateProgressive( CPhenotypeNode *currentNode, const genotype_parser::SExpr *expr );

		CPhenotypeNode* CreateBody(const genotype_parser::SExpr *expr, const PxVec3 &initialPos, const PxVec3 *linVel, const PxVec3 &randPos, 
			const float dimensionRate, const PxVec3 &parentDim, OUT PxVec3 &outDimension, const bool isTerminal=false);
		void CreateJoint(CPhenotypeNode *parentNode, CPhenotypeNode *childNode, genotype_parser::SConnection *connect, const PxVec3 &conPos );
		CPhenotypeNode* CreateSensor(CPhenotypeNode *parentNode, genotype_parser::SConnection *connect, const PxVec3 &initialPos, const bool IsTerminal=false );

		void GenerateSkinningMesh();
		void GenerateRenderComposition( CPhenotypeNode *node );
		//PxVec3 RandVec3( const PxVec3 &vec, const float rate );
		//PxVec3 MaximumVec3( const PxVec3 &vec0, const PxVec3 &vec1 );
		const genotype_parser::SExpr* FindExpr( const string &name );
		void MakeExprSymbol( const genotype_parser::SExpr *expr, OUT map<string, const genotype_parser::SExpr*> &symbols );
		bool HasTerminalNode(const genotype_parser::SExpr *expr) const;


	private:
		CEvc &m_sample;
		CPhenotypeNode *m_pRoot;
		vector<CPhenotypeNode*> m_nodes;
		SGenome m_genome;
		PxVec3 m_initialPos;
		vector<PxTransform> m_tmPalette;

		int m_GrowCount;
		int m_maxGrowcount;
		float m_IncreaseTime;
		genotype_parser::SExpr *m_pGenotypeExpr;
		map<string, const genotype_parser::SExpr*> m_GenotypeSymbols;
		bool m_IsDispSkinning;
	};


	inline const SGenome& CCreature::GetGenome() const { return m_genome; }
	inline genotype_parser::SExpr* CCreature::GetGenotype() { return m_pGenotypeExpr; }
	inline void CCreature::SetMaxGrowCount(const int maxGrowCount) { m_maxGrowcount = maxGrowCount; }
}
