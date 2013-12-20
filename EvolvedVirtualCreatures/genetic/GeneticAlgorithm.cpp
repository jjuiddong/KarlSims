
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


//-------------------------GrabNBest----------------------------------
//
//	This works like an advanced form of elitism by inserting NumCopies
//  copies of the NBest most fittest genomes into a population vector
//--------------------------------------------------------------------
void CGeneticAlgorithm::GrabNBest(int NBest, const int NumCopies, vector<SGenome> &Pop)
{
	const int size = m_Genomes.size();
	if (NBest > size)
		NBest = size;
	//add the required amount of copies of the n most fittest 
	//to the supplied vector
	while(NBest--)
	{
		for (int i=0; i<NumCopies; ++i)
		{
			//Pop.push_back(m_vecPop[(m_iPopSize - 1) - NBest]);
			Pop.push_back(m_Genomes[(size - 1) - NBest]);
		}
	}
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
	
	//Now to add a little elitism we shall add in some copies of the fittest genomes.
	GrabNBest(4, 1, vecNewPop);

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
	const int idx = (int)(m_Genomes.size() * RandFloat());
	return m_Genomes[ idx];
}
