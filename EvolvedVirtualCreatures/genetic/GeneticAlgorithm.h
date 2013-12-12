/**
 @filename GeneticAlgorithm.h
 
 
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




	protected:
		

	private:
		//vector<SGenome> m_Genomes;
	};
}
