
#include "stdafx.h"
#include "GenotypeParser.h"


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
 expression -> id ( id, vec3, id, quat, vec3, expression-list )
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
		pexpr->isIdOnly = false;
		pexpr->id = id();
		Match(LPAREN);
		pexpr->shape = id();
		Match(COMMA);
		pexpr->dimension = vec3();
		Match(COMMA);
		pexpr->joint = id();
		Match(COMMA);
		pexpr->orient = quat();
		Match(COMMA);
		pexpr->pos = vec3();
		pexpr->connection = NULL;

		if (m_SymTable.find(pexpr->id) == m_SymTable.end())
		{
			m_SymTable[ pexpr->id] = pexpr;
		}
		else
		{
			SyntaxError( "already exist expression = '%s' ", pexpr->id.c_str() );
		}

		if (COMMA == m_Token)
		{
			Match(COMMA);
			pexpr->connection = expression_list();
		}

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
			//m_SymTable[ Id]->refCount++;
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
		if (m_pScan->GetTokenStringQ(0) == "vec3")
		{
			Match(ID);
			Match(LPAREN);
			v.x = atof(number().c_str());
			Match(COMMA);
			v.y = atof(number().c_str());
			Match(COMMA);
			v.z = atof(number().c_str());
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


//quat -> quat(num, vec3);
SQuat genotype_parser::CGenotypeParser::quat()
{
	SQuat quat;
	quat.angle = 0;

	if (ID == m_Token)
	{
		if (m_pScan->GetTokenStringQ(0) == "quat")
		{
			Match(ID);
			Match(LPAREN);
			quat.angle = atof(number().c_str());
			Match(COMMA);
			quat.dir = vec3();
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
	SExprList *pnode = pexpr->connection;
	while (pnode)
	{
		Build(pnode->expr);
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
