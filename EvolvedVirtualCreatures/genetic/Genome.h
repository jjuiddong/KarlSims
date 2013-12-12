/**
 @filename Genome.h
 
*/
#pragma once


namespace evc
{
	namespace genotype_parser { struct SExpr; }

	struct SGenome
	{
		double fitness;
		vector<double> chromo;
	};


	bool GetChromo(const genotype_parser::SExpr *pexpr, OUT vector<double> &chromo);

	genotype_parser::SExpr* BuildExpr(const vector<double> &chromo);

	void Mutate(INOUT vector<double> &chromo);
}
