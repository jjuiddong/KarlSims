/**
 @filename Creature.h
 
*/
#pragma once

#include "PhysXSample.h"


class CEvc;
namespace evc
{
	class CJoint;
	class CNode;
	class CNeuralNet;
	namespace genotype_parser { struct SExpr; }

	DECLARE_TYPE_NAME_SCOPE(evc, CCreature)
	class CCreature : public memmonitor::Monitor<CCreature, TYPE_NAME(CCreature)>
	{
	public:
		CCreature(CEvc &sample);
		virtual ~CCreature();
		void GenerateByGenotype(const string &genotypeScriptFileName);
		void Move(float dtime);


	protected:
		CNode* GenerateByGenotype( const genotype_parser::SExpr *pexpr, const int recursiveCnt );
		MaterialIndex GetMaterialType(const string &materialStr);


	private:
		CEvc &m_Sample;
		CNode *m_pRoot;
		vector<CNode*> m_Nodes;
		// sensor list
		// output neuron
	};
}
