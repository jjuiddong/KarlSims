/**
 @filename Genome.h
 
*/
#pragma once


namespace evc
{
	class CCreature;
	namespace genotype_parser { struct SExpr; }

	struct SGenome
	{
		double fitness;
		vector<double> chromo;

		SGenome() : fitness(0) {}
		SGenome( const vector<double> &w, double f ): chromo(w), fitness(f) {}

		//overload '<' used for sorting
		friend bool operator<(const SGenome& lhs, const SGenome& rhs) {
			return (lhs.fitness < rhs.fitness);
		}

		const SGenome& operator=(const SGenome &rhs) {
			if (this != &rhs)
			{
				fitness = rhs.fitness;
				chromo = rhs.chromo;
			}
			return *this;
		}
	};


	bool GetChromo(const CCreature *creature, const genotype_parser::SExpr *pexpr, OUT vector<double> &chromo);
	genotype_parser::SExpr* BuildExpr(const vector<double> &chromo, OUT vector<double> &weights);
	void Crossover(const vector<double> &mum, const vector<double> &dad, OUT vector<double> &baby1, OUT vector<double> &baby2);
	void Mutate(INOUT vector<double> &chromo);
}
