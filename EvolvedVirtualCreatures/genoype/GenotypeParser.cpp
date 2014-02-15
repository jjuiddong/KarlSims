
#include "stdafx.h"
#include "GenotypeParser.h"
#include <iostream>

using namespace evc;
using namespace evc::genotype_parser;
using namespace std;


genotype_parser::CGenotypeParser::CGenotypeParser() 
{
	m_pScan = new CGenotypeScanner();
	m_IsTrace = false;
	m_IsErrorOccur = false;
}

genotype_parser::CGenotypeParser::~CGenotypeParser()
{
	SAFE_DELETE(m_pScan);

}


/**
 @brief 
 @date 2013-12-05
*/
genotype_parser::SExpr* genotype_parser::CGenotypeParser::Parse( const string &fileName, bool isTrace )
{
	if( !m_pScan->LoadFile(fileName.c_str(), isTrace) )
		return NULL;

	m_fileName = fileName;

	m_Token = m_pScan->GetToken();
	if (ENDFILE == m_Token)
	{
		m_pScan->Clear();
		return NULL;
	}

	{
		// Parsing ~ 
		SExprList *root = start();
		while (root)
		{
			SExprList *tmp = root;
			root = root->next;
			SAFE_DELETE(tmp);		
		}
	}

	if (ENDFILE != m_Token)
	{
		SyntaxError( " code ends before file " );
		m_pScan->Clear();
		return NULL;
	}

	if (m_IsErrorOccur)
		return NULL;

	if (m_SymTable.find("main") == m_SymTable.end())
	{
		SyntaxError( " Not Exist 'main' node" );
		m_pScan->Clear();
		RemoveNotRefExpression();
		return NULL;
	}

	SExpr *pMainExpr = m_SymTable[ "main"];

	m_RefCount.clear();
	Build(pMainExpr);
	RemoveNotRefExpression();

	return pMainExpr;
}


/**
 @brief start -> expression;
 @date 2013-12-05
*/
SExprList* genotype_parser::CGenotypeParser::start()
{
	return expression_list();
}


/**
 @brief 
 expression -> id ( id, vec3, material, mass, [randshape,] [connection-list] )
 	| id;

 @date 2013-12-05
*/
SExpr* genotype_parser::CGenotypeParser::expression()
{
	SExpr *pexpr = NULL;
	if ((ID == m_Token) && (LPAREN == m_pScan->GetTokenQ(1)))
	{
		pexpr = new SExpr;
		pexpr->refCount = 0;
		pexpr->connection = NULL;
		pexpr->randShape = SVec3(0,0,0);

		pexpr->id = id();
		Match(LPAREN);
		pexpr->shape = id();
		Match(COMMA);
		pexpr->dimension = vec3();
		Match(COMMA);
		pexpr->material = material();
		Match(COMMA);
		pexpr->mass = mass();

		if (m_SymTable.find(pexpr->id) == m_SymTable.end())
		{
			m_SymTable[ pexpr->id] = pexpr;
		}
		else
		{
			SyntaxError( "already exist expression = '%s' ", pexpr->id.c_str() );
		}

		if (COMMA == m_Token)
			Match(COMMA);
		pexpr->randShape = randshape();

		if (COMMA == m_Token)
			Match(COMMA);
		pexpr->connection = connection_list();

		Match(RPAREN);
	}
	else if (ID == m_Token)
	{
		const string Id = id();
		if (m_SymTable.find(Id) == m_SymTable.end())
		{
			SyntaxError( "not found expression = '%s' ", Id.c_str() );
		}
		else
		{
			pexpr = m_SymTable[ Id];
		}
	}
	return pexpr;
}


/**
 @brief 
 expression-list -> [ expression {, expression } ];
 @date 2013-12-05
*/
SExprList* genotype_parser::CGenotypeParser::expression_list()
{
	SExpr *pexpr = expression();
	if (!pexpr)
		return NULL;
	SExprList *plist = new SExprList;
	plist->expr = pexpr;

	if (COMMA == m_Token)
		Match(COMMA);
	
	plist->next = expression_list();
	return plist;
}


/**
 @brief connection -> connection( id, quat, vec3, limit, velocity, period, [randpos,] [randorient,]  [terminalonly,] expression )
 @date 2013-12-07
*/
SConnection* genotype_parser::CGenotypeParser::connection()
{
	SConnection *connct = NULL;
	if (ID != m_Token)
		return NULL;

	const string tok =m_pScan->GetTokenStringQ(0);
	if (!boost::iequals(tok, "joint") && !boost::iequals(tok, "sensor"))
	{
		SyntaxError( "must declare %s -> 'joint / sensor' ", tok.c_str() );
		return NULL;
	}

	connct = new SConnection;
	connct->conType = id();	
	connct->terminalOnly = false;

	Match(LPAREN);
	connct->type = id();
	Match(COMMA);
	connct->parentOrient = quat();
	Match(COMMA);
	connct->orient = quat();
	Match(COMMA);
	connct->pos = vec3();
	Match(COMMA);
	connct->limit = limit();
	Match(COMMA);
	connct->velocity = velocity();

	if (COMMA == m_Token)
		Match(COMMA);
	connct->period = period();

	if (COMMA == m_Token)
		Match(COMMA);
	connct->randPos = randField();

	if (COMMA == m_Token)
		Match(COMMA);
	connct->randOrient = randField();

	if (COMMA == m_Token)
		Match(COMMA);
	connct->terminalOnly = terminalonly();

	if (COMMA == m_Token)
		Match(COMMA);
	connct->expr = expression();

	Match(RPAREN);
	return connct;
}


/**
 @brief connection-list -> [connection {, connection}];
 @date 2013-12-07
*/
SConnectionList* genotype_parser::CGenotypeParser::connection_list()
{
	SConnection *connct = connection();
	if (!connct)
		return NULL;

	SConnectionList *plist = new SConnectionList;
	plist->connect = connct;

	if (COMMA == m_Token)
		Match(COMMA);

	plist->next = connection_list();
	return plist;
}


/**
 @brief 
 vec3 -> vec3( num, num, num ) ;
 @date 2013-12-05
*/
SVec3 genotype_parser::CGenotypeParser::vec3()
{
	SVec3 v;
	v.x = v.y = v.z = 0.f;

	if (ID == m_Token)
	{
		const string tok = m_pScan->GetTokenStringQ(0);
		if (boost::iequals(tok, "vec3"))
		{
			Match(ID);
			Match(LPAREN);
			v.x = (float)atof(number().c_str());
			Match(COMMA);
			v.y = (float)atof(number().c_str());
			Match(COMMA);
			v.z = (float)atof(number().c_str());
			Match(RPAREN);
		}
		else
		{
			SyntaxError( "undeclare token %s, must declare 'vec3'\n", m_pScan->GetTokenStringQ(0).c_str() );
		}
	}
	else
	{
		SyntaxError( "vec3 type need id string\n" );
	}

	return v;
}


/**
 @brief quat -> quat(num, vec3)
						| quat()
						;
 @date 2013-12-08
*/
SQuat genotype_parser::CGenotypeParser::quat()
{
	SQuat quat;
	quat.angle = 0;
	quat.dir.x = quat.dir.y = quat.dir.z = 0.f;

	if (ID == m_Token)
	{
		const string tok = m_pScan->GetTokenStringQ(0);
		if (boost::iequals(tok, "quat"))
		{
			Match(ID);
			Match(LPAREN);
			if (RPAREN != m_Token)
			{
				quat.angle = (float)atof(number().c_str());
				Match(COMMA);
				quat.dir = vec3();
			}
			Match(RPAREN);
		}
		else
		{
			SyntaxError( "undeclare token %s, must declare 'quat'\n", m_pScan->GetTokenStringQ(0).c_str() );
		}
	}
	else
	{
		SyntaxError( "quat type need id string\n" );
	}

	return quat;
}


/**
 @brief limit -> limit(num, num, num)
 @date 2013-12-07
*/
SVec3 genotype_parser::CGenotypeParser::limit()
{
	SVec3 v;
	v.x = v.y = v.z = 0.f;

	if (ID != m_Token)
	{
		SyntaxError( "limit type need limit \n" );
		return v;
	}

	const string tok = m_pScan->GetTokenStringQ(0);
	if (boost::iequals(tok, "limit"))
	{
		Match(ID);
		Match(LPAREN);
		v.x = (float)atof(number().c_str());
		Match(COMMA);
		v.y = (float)atof(number().c_str());
		Match(COMMA);
		v.z = (float)atof(number().c_str());
		Match(RPAREN);
	}
	else
	{
		SyntaxError( "undeclare token %s, must declare 'limit'\n", m_pScan->GetTokenStringQ(0).c_str() );
	}

	return v;
}


/**
 @brief material -> material( rgbValue )
 @date 2013-12-07
*/
SVec3 genotype_parser::CGenotypeParser::material()
{
	if (ID != m_Token)
		return SVec3(0.5f, 0.5f, 0.5f);

	SVec3 ret(0.5f, 0.5f, 0.5f);
	const string tok = m_pScan->GetTokenStringQ(0);
	if (boost::iequals(tok, "material"))
	{
		Match(ID);
		Match(LPAREN);
		ret = rgbValue();
		Match(RPAREN);
	}
	else
	{
		SyntaxError( "undeclare token %s, must declare 'material'\n", m_pScan->GetTokenStringQ(0).c_str() );
	}

	return ret;
}


// material_arg -> id | rgb
SVec3 genotype_parser::CGenotypeParser::rgbValue()
{
	SVec3 v;
	v.x = v.y = v.z = 0.f;

	if (ID == m_Token)
	{
		const string tok = m_pScan->GetTokenStringQ(0);
		if (boost::iequals(tok, "rgb"))
		{
			Match(ID);
			Match(LPAREN);
			v.x = (float)atof(number().c_str());
			Match(COMMA);
			v.y = (float)atof(number().c_str());
			Match(COMMA);
			v.z = (float)atof(number().c_str());
			Match(RPAREN);
		}
		else if (boost::iequals(tok, "grey"))
		{
			Match(ID);
			v.x = 0.5f, v.y = 0.5f, v.z = 0.5f;			
		}
		else if (boost::iequals(tok, "red"))
		{
			Match(ID);
			v.x = 0.75f, v.y = 0, v.z = 0;
		}
		else if (boost::iequals(tok, "green"))
		{
			Match(ID);
			v.x = 0, v.y = 0.75f, v.z = 0;
		}
		else if (boost::iequals(tok, "blue"))
		{
			Match(ID);
			v.x = 0, v.y = 0, v.z = 0.75f;
		}
		else if (boost::iequals(tok, "yellow"))
		{
			Match(ID);
			v.x = 0.75f, v.y = 0.75f, v.z = 0;
		}
		else if (boost::iequals(tok, "white"))
		{
			Match(ID);
			v.x = 0.75f, v.y = 0.75f, v.z = 0.75f;
		}
		else
		{
			SyntaxError( "undeclare token %s, must declare 'rgb'\n", m_pScan->GetTokenStringQ(0).c_str() );
		}
	}
	else
	{
		SyntaxError( "vec3 type need id string\n" );
	}

	return v;
}


/**
 @brief mass -> mass(num)
 @date 2013-12-07
*/
float genotype_parser::CGenotypeParser::mass()
{
	if (ID != m_Token)
		return 0.f;

	float ret = 0.f;
	const string tok = m_pScan->GetTokenStringQ(0);
	if (boost::iequals(tok, "mass"))
	{
		Match(ID);
		Match(LPAREN);
		ret = (float)atof(number().c_str());
		Match(RPAREN);
	}
	else
	{
		SyntaxError( "undeclare token %s, must declare 'mass'\n", m_pScan->GetTokenStringQ(0).c_str() );
	}

	return ret;
}


/**
 @brief velocity -> velocity( num )
 @date 2013-12-07
*/
SVec3 genotype_parser::CGenotypeParser::velocity()
{
	SVec3 v;
	v.x = v.y = v.z = 0.f;

	if (ID != m_Token)
		return v;

	const string tok = m_pScan->GetTokenStringQ(0);
	if (boost::iequals(tok, "velocity"))
	{
		Match(ID);
		Match(LPAREN);
		v.x = (float)atof(number().c_str());
		Match(RPAREN);
	}
	else
	{
		// nothing
	}

	return v;
}


// period -> period(num)
float genotype_parser::CGenotypeParser::period()
{
	if (ID != m_Token)
		return 0.f;

	float ret = 0.f;
	const string tok = m_pScan->GetTokenStringQ(0);
	if (boost::iequals(tok, "period"))
	{
		Match(ID);
		Match(LPAREN);
		ret = (float)atof(number().c_str());
		Match(RPAREN);
	}
	else
	{
		// nothing
	}

	return ret;
}


/**
 @brief randfield -> randshape | randpos | randorient
 @date 2014-01-13
*/
SVec3 CGenotypeParser::randField()
{
	SVec3 v(0,0,0);
	
	const string tok = m_pScan->GetTokenStringQ(0);
	if (boost::iequals(tok, "randpos"))
	{
		v = randpos();
	}
	else if (boost::iequals(tok, "randorient"))
	{
		v = randorient();
	}
	else if (boost::iequals(tok, "randshape"))
	{
		v = randshape();
	}

	return v;
}


/**
 @brief randshape -> randshape(num, num, num)
 @date 2014-01-13
*/
SVec3 CGenotypeParser::randshape()
{
	SVec3 v;
	v.x = v.y = v.z = 0.f;

	if (ID != m_Token)
		return v;

	const string tok = m_pScan->GetTokenStringQ(0);
	if (boost::iequals(tok, "randshape"))
	{
		Match(ID);
		Match(LPAREN);
		v.x = (float)atof(number().c_str());
		Match(COMMA);
		v.y = (float)atof(number().c_str());
		Match(COMMA);
		v.z = (float)atof(number().c_str());
		Match(RPAREN);
	}
	else
	{
		// nothing
	}

	return v;
}


/**
 @brief randpos -> randpos(num, num, num)
 @date 2014-01-13
*/
SVec3 genotype_parser::CGenotypeParser::randpos()
{
	SVec3 v;
	v.x = v.y = v.z = 0.f;

	if (ID != m_Token)
		return v;

	const string tok = m_pScan->GetTokenStringQ(0);
	if (boost::iequals(tok, "randpos"))
	{
		Match(ID);
		Match(LPAREN);
		v.x = (float)atof(number().c_str());
		Match(COMMA);
		v.y = (float)atof(number().c_str());
		Match(COMMA);
		v.z = (float)atof(number().c_str());
		Match(RPAREN);
	}
	else
	{
		// nothing
	}

	return v;
}


/**
 @brief randorient -> randorient(num, num, num)
 @date 2014-01-13
*/
SVec3 genotype_parser::CGenotypeParser::randorient()
{
	SVec3 v;
	v.x = v.y = v.z = 0.f;

	if (ID != m_Token)
		return v;

	const string tok = m_pScan->GetTokenStringQ(0);
	if (boost::iequals(tok, "randorient"))
	{
		Match(ID);
		Match(LPAREN);
		v.x = (float)atof(number().c_str());
		Match(COMMA);
		v.y = (float)atof(number().c_str());
		Match(COMMA);
		v.z = (float)atof(number().c_str());
		Match(RPAREN);
	}
	else
	{
		// nothing
	}

	return v;
}


/**
 @brief terminalOnly -> terminalOnly
 @date 2014-01-13
*/
bool CGenotypeParser::terminalonly()
{
	if (ID != m_Token)
		return false;

	const string tok = m_pScan->GetTokenStringQ(0);
	if (boost::iequals(tok, "terminalOnly"))
	{
		Match(ID);
		return true;
	}
	return false;
}


/**
 @brief 
 @date 2013-12-05
*/
string genotype_parser::CGenotypeParser::number()
{
	string str = "";
	str = m_pScan->GetTokenStringQ(0);
	Match(NUM);
	return str;
}


/**
 @brief 
 @date 2013-12-05
*/
int genotype_parser::CGenotypeParser::num()
{
	int n = atoi(m_pScan->GetTokenStringQ(0).c_str());
	Match(NUM);
	return n;
}


/**
 @brief 
 @date 2013-12-05
*/
string genotype_parser::CGenotypeParser::id()
{
	string str = m_pScan->GetTokenStringQ(0);
	Match( ID );
	return str;
}


/**
 @brief 
 @date 2013-12-05
*/
bool genotype_parser::CGenotypeParser::Match( Tokentype t )
{
	if( m_Token == t )
	{
		m_Token = m_pScan->GetToken();
	}
	else
	{
		SyntaxError( "unexpected token -> " );
		printf( "\n" );
	}
	return true;
}


/**
 @brief 
 @date 2013-12-05
*/
void genotype_parser::CGenotypeParser::SyntaxError( char *msg, ... )
{
	m_IsErrorOccur = true;
	char buf[ 256];
	va_list marker;
	va_start(marker, msg);
	vsprintf_s(buf, sizeof(buf), msg, marker);
	va_end(marker);

	stringstream ss;
	ss << "Syntax error at line " << m_fileName << " " << m_pScan->GetLineNo() <<  ": " << buf ;

	std::cout << ss.str() << std::endl;
}


/**
 @brief increase reference counter of SExpr structure using main expression pointer
 @date 2013-12-06
*/
void genotype_parser::CGenotypeParser::Build( SExpr *pexpr )
{
	if (!pexpr)
		return;

	pexpr->refCount++;

	if (m_RefCount.find(pexpr->id) != m_RefCount.end())
		return; // already check

	m_RefCount.insert(pexpr->id);
	SConnectionList *pnode = pexpr->connection;
	while (pnode)
	{
		Build(pnode->connect->expr);
		pnode = pnode->next;
	}
}


/**
 @brief remove not reference expression node
 @date 2013-12-06
*/
void genotype_parser::CGenotypeParser::RemoveNotRefExpression()
{
	set<string> rm;
	BOOST_FOREACH(auto &kv, m_SymTable)
	{
		if (kv.second->refCount <= 0)
		{
			RemoveExpressoin_OnlyExpr(kv.second);
			rm.insert(kv.first);
		}
	}
	BOOST_FOREACH(auto &key, rm)
	{
		cout << "remove not reference expression : " << key << endl;
		m_SymTable.erase(key);
	}
}
