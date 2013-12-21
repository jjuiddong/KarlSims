
#include "stdafx.h"
#include "Genome.h"
#include "../genoype/GenotypeParser.h"
#include "../creature/Creature.h"
#include "../creature/NeuralNet.h"


namespace evc 
{

	bool GetChromo_Sub(const genotype_parser::SExpr *pexpr, OUT vector<double> &chromo, 
		INOUT map<string, int> &check);


	genotype_parser::SExpr* BuildChromo_Sub(const vector<double> &chromo, const int startIndex, OUT int &nextIndex,
		INOUT map<int, genotype_parser::SExpr*> &symTable);


	void Mutate_Sub(INOUT vector<double> &chromo, const int startIndex, OUT int &nextIndex, INOUT map<int, genotype_parser::SExpr*> &symTable);
}


using namespace evc;


/**
 @brief 
 @date 2013-12-12
*/
int GetStringTypeIndex(const string strs[], const int size, const string &type)
{
	for (int i=0; i < size; ++i)
	{
		if (boost::iequals(type, strs[ i]))
			return i;
	}
	return 0;
}
string GetTypeString(const string strs[], const int size, const int index)
{
	if (index >= size)
		return strs[ 0];
	else
		return strs[ index];
}


/**
 @brief 
 @date 2013-12-12
*/
const string g_JointTypeStrings[] = {"fixed", "revolute", "spherical" };
const int g_JointTypeStrings_Size = sizeof(g_JointTypeStrings) / sizeof(string);
int GetJointTypeIndex(const string &type) {
	return GetStringTypeIndex(g_JointTypeStrings, g_JointTypeStrings_Size, type);
}
string GetJointType(const int index) {
	return GetTypeString(g_JointTypeStrings, g_JointTypeStrings_Size, index);
}


/**
 @brief 
 @date 2013-12-12
*/
const string g_ConnectTypeStrings[] = {"joint", "sensor"};
const int g_ConnectTypeStrings_Size = sizeof(g_ConnectTypeStrings) / sizeof(string);
int GetConnectTypeIndex(const string &type) {
	return GetStringTypeIndex(g_ConnectTypeStrings, g_ConnectTypeStrings_Size, type);
}
string GetConnectType(const int index) {
	return GetTypeString(g_ConnectTypeStrings, g_ConnectTypeStrings_Size, index);
}


/**
 @brief 
 @date 2013-12-12
*/
const string g_SensorTypeStrings[] = {"photo", "vision" };
const int g_SensorTypeStrings_Size = sizeof(g_SensorTypeStrings) / sizeof(string);
int GetSensorTypeIndex(const string &type) {
	return GetStringTypeIndex(g_SensorTypeStrings, g_SensorTypeStrings_Size, type);
}
string GetSensorType(const int index) {
	return GetTypeString(g_SensorTypeStrings, g_SensorTypeStrings_Size, index);
}


/**
 @brief 
 @date 2013-12-12
*/
const string g_ShapeTypeStrings[] = {"box", "sphere" };
const int g_ShapeTypeStrings_Size = sizeof(g_ShapeTypeStrings) / sizeof(string);
int GetShapeTypeIndex(const string &type) {
	return GetStringTypeIndex(g_ShapeTypeStrings, g_ShapeTypeStrings_Size, type);
}
string GetShapeType(const int index) {
	return GetTypeString(g_ShapeTypeStrings, g_ShapeTypeStrings_Size, index);
}


/**
 @brief 
 @date 2013-12-12
*/
const string g_MaterialTypeStrings[] = {"grey", "red", "green", "blue", "yellow"};
const int g_MaterialTypeStrings_Size = sizeof(g_MaterialTypeStrings) / sizeof(string);
int GetMaterialTypeIndex(const string &type) {
	return GetStringTypeIndex(g_MaterialTypeStrings, g_MaterialTypeStrings_Size, type);
}
string GetMaterialType(const int index) {
	return GetTypeString(g_MaterialTypeStrings, g_MaterialTypeStrings_Size, index);
}




/**
 @brief convert pexpr to chromo
 @date 2013-12-12
*/
bool evc::GetChromo_Sub(const genotype_parser::SExpr *pexpr, OUT vector<double> &chromo, 
	INOUT map<string, int> &check)
{
	using namespace genotype_parser;
	RETV(!pexpr, false);

	if (check.find(pexpr->id) != check.end())
	{
		// write node id, andthen return
		chromo.push_back( check[ pexpr->id] );
		return true; // already check
	}

	const int exprId = check.size();
	check.insert( map<string, int>::value_type(pexpr->id, exprId) );

	// node Id
	chromo.push_back( exprId );

	// shape
	chromo.push_back( GetShapeTypeIndex(pexpr->shape) );

	// dimension
	chromo.push_back( pexpr->dimension.x );
	chromo.push_back( pexpr->dimension.y );
	chromo.push_back( pexpr->dimension.z );

	// mass
	chromo.push_back( pexpr->mass );

	// material
	chromo.push_back( GetMaterialTypeIndex(pexpr->material) );

	// connection count
	int connectCount = 0;
	{
		SConnectionList *pConnect = pexpr->connection;
		while (pConnect)
		{
			++connectCount;
			pConnect = pConnect->next;
		}
	}
	chromo.push_back( connectCount );

	// connection
	SConnectionList *pConnect = pexpr->connection;
	while (pConnect)
	{
		SConnection *pJoint = pConnect->connect;
		if (!pJoint)
			continue;

		// type
		chromo.push_back( (double)GetConnectTypeIndex(pJoint->conType) );
		if (boost::iequals(pJoint->conType, "joint"))
		{
			chromo.push_back( (double)GetJointTypeIndex(pJoint->type) );
		}
		else
		{
			chromo.push_back( (double)GetSensorTypeIndex(pJoint->type) );
		}

		// orient
		chromo.push_back( pJoint->parentOrient.angle );
		chromo.push_back( pJoint->parentOrient.dir.x );
		chromo.push_back( pJoint->parentOrient.dir.y );
		chromo.push_back( pJoint->parentOrient.dir.z );

		chromo.push_back( pJoint->orient.angle );
		chromo.push_back( pJoint->orient.dir.x );
		chromo.push_back( pJoint->orient.dir.y );
		chromo.push_back( pJoint->orient.dir.z );

		// pos
		chromo.push_back( pJoint->pos.x );
		chromo.push_back( pJoint->pos.y );
		chromo.push_back( pJoint->pos.z );

		// limit
		chromo.push_back( pJoint->limit.x );
		chromo.push_back( pJoint->limit.y );
		chromo.push_back( pJoint->limit.z );

		// velocity
		chromo.push_back( pJoint->velocity.x );
		chromo.push_back( pJoint->velocity.y );
		chromo.push_back( pJoint->velocity.z );

		// period
		chromo.push_back( pJoint->period );

		// expr
		GetChromo_Sub( pJoint->expr, chromo, check );

		pConnect = pConnect->next;
	}
	return true;
}


/**
 @brief convert pexpr to chromo
 @date 2013-12-12
*/
bool evc::GetChromo(const CCreature *creature, const genotype_parser::SExpr *pexpr, OUT vector<double> &chromo)
{
	map<string, int> check;
	GetChromo_Sub(pexpr, chromo, check);

	const CNeuralNet *nn = creature->GetBrain();
	RETV(!nn, true);

	vector<double> weights = nn->GetWeights();
	std::copy(weights.begin(), weights.end(), std::back_inserter(chromo));
	chromo.push_back(weights.size()); // save weight size
	return true;
}


/**
 @brief 
 @date 2013-12-12
*/
genotype_parser::SExpr* evc::BuildExpr(const vector<double> &chromo, OUT vector<double> &weights)
{
	map<int, genotype_parser::SExpr*> symTable;
	int next;
	genotype_parser::SExpr *expr = BuildChromo_Sub(chromo, 0, next, symTable);
	std::copy(chromo.begin()+(next+1), chromo.end()-1, std::back_inserter(weights));
	return expr;
}


/**
 @brief 
 @date 2013-12-12
*/
genotype_parser::SExpr* evc::BuildChromo_Sub(const vector<double> &chromo, const int startIndex, 
	OUT int &nextIndex, INOUT map<int, genotype_parser::SExpr*> &symTable)
{
	using namespace genotype_parser;
	if ((int)chromo.size() <= startIndex)
	{
		nextIndex = startIndex;
		return NULL;
	}

	int index = startIndex;

	const int exprId = chromo[ index++];
	auto it = symTable.find(exprId);
	if (symTable.end() != it)
	{ // Already Exist Expression
		nextIndex = startIndex;
		return it->second;
	}

	SExpr *pexpr = new SExpr;
	pexpr->id = common::format("%d", exprId);
	pexpr->connection = NULL;

	symTable[ exprId] = pexpr; // insert Expression

	pexpr->shape = GetShapeType(chromo[ index++]);
	pexpr->dimension.x = chromo[ index++];
	pexpr->dimension.y = chromo[ index++];
	pexpr->dimension.z = chromo[ index++];
	pexpr->mass = chromo[ index++];
	pexpr->material = GetMaterialType(chromo[ index++]);

	SConnectionList *pPrevConnection = NULL;
	const int connectionCount = (int)chromo[ index++];
	for (int i=0; i < connectionCount; ++i)
	{
		SConnectionList *pJList = new SConnectionList;
		pJList->connect = new SConnection;
		pJList->next = NULL;
		SConnection *pJoint = pJList->connect;

		pJoint->conType = GetConnectType(chromo[ index++]);
		if (boost::iequals(pJoint->conType, "joint"))
			pJoint->type = GetJointType(chromo[ index++]);
		else
			pJoint->type = GetSensorType(chromo[ index++]);

		// orient
		pJoint->parentOrient.angle = chromo[ index++];
		pJoint->parentOrient.dir.x = chromo[ index++];
		pJoint->parentOrient.dir.y = chromo[ index++];
		pJoint->parentOrient.dir.z = chromo[ index++];

		pJoint->orient.angle = chromo[ index++];
		pJoint->orient.dir.x = chromo[ index++];
		pJoint->orient.dir.y = chromo[ index++];
		pJoint->orient.dir.z = chromo[ index++];

		// pos
		pJoint->pos.x  = chromo[ index++];
		pJoint->pos.y  = chromo[ index++];
		pJoint->pos.z  = chromo[ index++];

		// limit
		pJoint->limit.x  = chromo[ index++];
		pJoint->limit.y  = chromo[ index++];
		pJoint->limit.z  = chromo[ index++];

		// velocity
		pJoint->velocity.x  = chromo[ index++];
		pJoint->velocity.y = chromo[ index++];
		pJoint->velocity.z  = chromo[ index++];

		// period
		pJoint->period = chromo[ index++];

		int nextIdx;
		pJoint->expr = BuildChromo_Sub(chromo, index, nextIdx, symTable);
		index = nextIdx;

		if (!pexpr->connection)
		{
			pexpr->connection = pJList;
		}
		else
		{
			pPrevConnection->next = pJList;
		}
		pPrevConnection = pJList;
	}

	nextIndex = index;
	return pexpr;
}



/**
 @brief 
 @date 2013-12-12
*/
void evc::Mutate(INOUT vector<double> &chromo)
{
	map<int, genotype_parser::SExpr*> symTable;
	int next=0;
	Mutate_Sub(chromo, 0, next, symTable);
}


/**
 @brief 
 @date 2013-12-12
*/
void evc::Mutate_Sub(INOUT vector<double> &chromo, const int startIndex, OUT int &nextIndex, 
	INOUT map<int, genotype_parser::SExpr*> &symTable)
{
	const int weightSize = chromo.back();
	const int weightStart = chromo.size() - (weightSize+1);

	const int size = chromo.size() - 1;
	for (int i=weightStart; i < size; ++i)
	{
		if (RandFloat() < 0.1f)
			chromo[ i] = (RandomClamped() * 0.3f);
	}	
}


/**
 @brief cross over chromo
 @date 2013-12-13
*/
void evc::Crossover(const vector<double> &mum, const vector<double> &dad, 
	OUT vector<double> &baby1, OUT vector<double> &baby2)
{
	const int mumWeightSize = mum.back();
	const int dadWeightSize = dad.back();
	if ((mumWeightSize > 0) && (mumWeightSize == dadWeightSize))
	{
		baby1.reserve(dad.size());
		baby2.reserve(dad.size());

		std::copy(dad.begin(), dad.end()-(mumWeightSize+1), std::back_inserter(baby1));
		std::copy(mum.begin(), mum.end()-(mumWeightSize+1), std::back_inserter(baby2));

		vector<double> dadW;
		vector<double> mumW;
		dadW.reserve(mumWeightSize);
		mumW.reserve(mumWeightSize);

		const int weightStart = dad.size() - (mumWeightSize+1);
		std::copy(dad.begin()+weightStart, dad.end()-1, std::back_inserter(dadW));
		std::copy(mum.begin()+weightStart, mum.end()-1, std::back_inserter(mumW));

		// crossover dadW, mumW
		int cp = RandInt(0, mumWeightSize - 1);
		for (int i=0; i<cp; ++i)
		{
			baby1.push_back(mumW[i]);
			baby2.push_back(dadW[i]);
		}

		for (int i=cp; i<mumWeightSize; ++i)
		{
			baby1.push_back(dadW[i]);
			baby2.push_back(mumW[i]);
		}

		baby1.push_back(mumWeightSize);
		baby2.push_back(mumWeightSize);
	}
	else
	{
		baby1 = mum;
		baby2 = dad;
	}
}
