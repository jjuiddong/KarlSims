
#include "stdafx.h"
#include "GeneticAlgorithm.h"
#include "../genoype/GenotypeParser.h"


using namespace evc;



CGeneticAlgorithm::CGeneticAlgorithm()
{
	m_Genomes.reserve(100);
}

CGeneticAlgorithm::~CGeneticAlgorithm()
{

}


/**
 @brief 
 @date 2013-12-13
*/
void CGeneticAlgorithm::InitGenome()
{
	m_Genomes.clear();
}


/**
 @brief 
 @date 2013-12-13
*/
void CGeneticAlgorithm::AddGenome(const SGenome &genome)
{
	m_Genomes.push_back(genome);
}


/**
 @brief 
 @date 2013-12-13
*/
void CGeneticAlgorithm::Epoch()
{
	const size_t population = m_Genomes.size();

	//sort the population (for scaling and elitism)
	sort(m_Genomes.begin(), m_Genomes.end());

	//create a temporary vector to store new chromosones
	vector<SGenome> vecNewPop;
	vecNewPop.reserve(population);
	
	//now we enter the GA loop

	//repeat until a new population is generated
	while (vecNewPop.size() < population)
	{
		//grab two chromosones
		SGenome mum = GetChromoRoulette();
		SGenome dad = GetChromoRoulette();

		//create some offspring via crossover
		vector<double> baby1, baby2;

		Crossover(mum.chromo, dad.chromo, baby1, baby2);

		//now we mutate
		Mutate(baby1);
		Mutate(baby2);

		//now copy into vecNewPop population
		vecNewPop.push_back(SGenome(baby1, 0));
		vecNewPop.push_back(SGenome(baby2, 0));
	}

	m_Genomes = vecNewPop;
}


/**
 @brief 
 @date 2013-12-13
*/
SGenome CGeneticAlgorithm::GetChromoRoulette()
{
	return m_Genomes[ 0];
}
