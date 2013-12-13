/**
 @filename GeneticAlgorithm.h
 
 Mat Buckland 2002 Idea
*/
#pragma once

#include "genome.h"


namespace evc
{
	class CGeneticAlgorithm : public common::CSingleton<CGeneticAlgorithm>
	{
	public:
		CGeneticAlgorithm();
		virtual ~CGeneticAlgorithm();
		void InitGenome();
		void AddGenome(const SGenome &genome);
		const vector<SGenome>& GetGenomes() const;
		void Epoch();


	protected:
		SGenome GetChromoRoulette();


	private:
		vector<SGenome> m_Genomes;
	};


	inline const vector<SGenome>& CGeneticAlgorithm::GetGenomes() const { return m_Genomes; }
}
