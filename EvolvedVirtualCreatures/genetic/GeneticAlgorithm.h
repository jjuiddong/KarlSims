/**
 @filename GeneticAlgorithm.h
 
 
*/
#pragma once


namespace evc
{
	namespace genotype_parser { struct SExpr; }
	
	vector<double> ConvertChromo(genotype_parser::SExpr *pexpr);
	void Mutate(vector<double> &chromo);
	


}
