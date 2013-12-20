/**
 @filename Creature.h
 
*/
#pragma once

#include "PhysXSample.h"
#include "../genetic/Genome.h"


class CEvc;
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
		void GenerateByGenotype(const string &genotypeScriptFileName, const PxVec3 &initialPos);
		void GenerateByGenome(const SGenome &genome, const PxVec3 &initialPos);
		void Move(float dtime);
		const SGenome& GetGenome() const;
		PxVec3 GetPos() const;
		const CNeuralNet* GetBrain() const;


	protected:
		CNode* GenerateByGenotype( const genotype_parser::SExpr *pexpr, const int recursiveCnt, const PxVec3 &initialPos );
		void CreateJoint(CNode *parentNode, CNode *childNode, genotype_parser::SConnection *connect);
		CNode* CreateSensor(CNode *parentNode, genotype_parser::SConnection *connect, const PxVec3 &initialPos );
		MaterialIndex GetMaterialType(const string &materialStr);


	private:
		CEvc &m_Sample;
		CNode *m_pRoot;
		vector<CNode*> m_Nodes;
		SGenome m_Genome;
		PxVec3 m_InitialPos;
	};


	inline const SGenome& CCreature::GetGenome() const { return m_Genome; }
}
