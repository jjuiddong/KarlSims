
#include "stdafx.h"
#include "GenotypeScanner.h"

using namespace evc;
using namespace evc::genotype_parser;

namespace evc
{
	Tokentype reservedLookup( const string &s );

	// lookup table of reserved words
	typedef struct _SReservedWord
	{
		char *str;
		Tokentype tok;
	} SReservedWord;

	SReservedWord reservedWords[] =
	{
		{"#if", IF },
		{"#elif", IF },
		{"#else", ELSE },
		{"#array", ARRAY },
		{"#list", LIST },
		{"#tree", TREE},
		{"children", CHILDREN},
		{"preview", PREVIEW},
		{"stringview", STRINGVIEW},
		{"#", SHARP_BRACKET}, // ���� ���� �켱������ �˻��ؾ��Ѵ�.

	};
	const int g_rsvSize = sizeof(reservedWords) / sizeof(SReservedWord);

}

Tokentype evc::reservedLookup( const string &s )
{
	int i=0;
	for( i=0; i < g_rsvSize; ++i )
	{
		if( !_stricmp(s.c_str(), reservedWords[ i].str) )
			return reservedWords[ i].tok;
	}
	return ID;
}


CGenotypeScanner::CGenotypeScanner() 
{
}

CGenotypeScanner::~CGenotypeScanner()
{
	Clear();
}

//----------------------------------------------------------------------------
// 
//----------------------------------------------------------------------------
bool CGenotypeScanner::LoadFile( const string &fileName, bool bTrace ) // bTrace=FALSE
{
	OFSTRUCT of;

	HFILE hFile = OpenFile(fileName.c_str(), &of, OF_READ);
	if (hFile != EOF)
	{
		const int fileSize = GetFileSize((HANDLE)hFile, NULL);
		if (fileSize <= 0)
			return false;

		DWORD readSize = 0;
		BYTE *mem = new BYTE[ fileSize+1];
		ReadFile((HANDLE)hFile, mem, fileSize, &readSize, NULL);
		CloseHandle((HANDLE)hFile);
		mem[ fileSize] = NULL;
		m_Source = (char*)mem;
		delete[] mem;

		m_memSize = m_Source.length();
		m_IsTrace = bTrace;
		Init();
		return true;
	}

	printf( "[%s] ������ �����ϴ�.\n", fileName.c_str() );
	return false;
}


//----------------------------------------------------------------------------
// ��Ű�� ������ �о �����ؼ� ����Ѵ�.
//----------------------------------------------------------------------------
bool CGenotypeScanner::LoadPackageFile( BYTE *pFileMem, int nFileSize )
{
	m_Source.clear();

	m_Source = (char*)pFileMem;
	m_memSize = m_Source.length();

	Init();

	return true;
}


//----------------------------------------------------------------------------
// �޸��� ���� ����Ű�ų�, �ε���� �ʾҴٸ� true�� �����Ѵ�.
//----------------------------------------------------------------------------
bool CGenotypeScanner::IsEnd()
{
	if (m_Source.empty()) return true;
	if (m_pCurrentMemPoint >= m_memSize) return true;

	return false;
}


Tokentype CGenotypeScanner::GetToken()
{
	if (m_Source.empty())
	{
		return ENDFILE;
	}

	if( m_TokQ.size() == 0 )
	{
		for( int i=0; i < MAX_QSIZE; ++i )
		{
			STokDat d;
			string buf;
			d.tok = _GetToken( buf );
			d.str = buf;
			m_TokQ.push_back( d );
		}
	}
	else
	{
		STokDat d;
		string buf;
		d.tok = _GetToken( buf );
		d.str = buf;
		m_TokQ.push_back( d );
		m_TokQ.pop_front();
	}

	return m_TokQ[ 0].tok;
}


Tokentype CGenotypeScanner::GetTokenQ( int nIdx )
{
	if (m_Source.empty())
	{
		return ENDFILE;
	}
	return m_TokQ[ nIdx].tok;
}


const string& CGenotypeScanner::GetTokenStringQ( int nIdx )
{
	static string emptyString;
	if (m_Source.empty()) return emptyString;
	return m_TokQ[ nIdx].str;
}


//char* CGenotypeScanner::CopyTokenStringQ( int nIdx )
//{
//	if (m_Source.empty()) return NULL;
//
//	int len = m_TokQ[ nIdx].str.size();
//	char *p = new char[ len + 1];
//	strcpy_s( p, len+1, m_TokQ[ nIdx].str.c_str() );
//	return p;
//}


char CGenotypeScanner::GetNextChar()
{
	//if( m_nLinePos >= m_nBufSize )
	//{
	//	++m_nLineNo;
	//	//		if( fgets(m_Buf, MAX_BUFF-1, m_fp) )
	//	if (GetString(m_Buf, MAX_BUFF))
	//	{
	//		m_nLinePos = 0;
	//		m_nBufSize = strlen( m_Buf );
	//		if( m_bTrace )
	//			printf( "%4d: %s", m_nLineNo, m_Buf );
	//	}
	//	else 
	//	{
	//		return EOF;
	//	}
	//}

	//return m_Buf[ m_nLinePos++];
	if (m_linePos >= m_memSize)
		return EOF;

	const short c = m_Source[ m_linePos];

	++m_linePos;
	if (c == '\r')
		++m_lineNo;
	return c;
}


//----------------------------------------------------------------------------
// fgets() �Լ��� ����� ���� �Ѵ�. ���Ͽ� �ִ� ����Ÿ�� �ƴ϶�, �޸𸮿�
// �ִ� ����Ÿ�� �����´ٴ� ���� �ٸ���.
//----------------------------------------------------------------------------
bool CGenotypeScanner::GetString(char *receiveBuffer, int maxBufferLength)
{
	if (m_pCurrentMemPoint >= m_memSize)
		return false;

	// �޸𸮴� ���̳ʸ� �������� ������ ������, 
	// ���๮�� ��ŭ ����Ÿ�� ���;� �Ѵ�.
	ZeroMemory(receiveBuffer, maxBufferLength);
	int i=0;
	for (i=0; (i < maxBufferLength) && (m_pCurrentMemPoint < m_memSize); ++i)
	{
		const int memPoint = m_pCurrentMemPoint;

		// ���๮��?
		if ( '\r' == m_Source[ memPoint] ) // '\n' == m_Source[ memPoint]
		{
			m_pCurrentMemPoint += 2;
			receiveBuffer[ i++] = '\n';
			break;
		}
		else
		{
			receiveBuffer[ i] = m_Source[ memPoint];
			++m_pCurrentMemPoint;
		}
	}

	return true;
}


void CGenotypeScanner::UngetNextChar()
{
	--m_linePos;
	if (m_linePos < 0)
		m_linePos = 0;
}


Tokentype CGenotypeScanner::_GetToken( string &token )
{
	bool bFloat = false;
	Tokentype currentToken;
	StateType state = START;

	int nTok = 0;
	while( DONE != state )
	{
		char c = GetNextChar();
		bool save = true;
		switch( state )
		{
		case START:
			if ((c >= 0) && (c < 255) && isdigit(c))
				state = INNUM;
			else if ( ((c >=0) && (c < 255) && isalpha(c)) || '@' == c || '_' == c || '#' == c || '$' == c)
				state = INID;
			else
			{
				switch (c)
				{
				case '=': state = INEQ; save = false; break;
				case '!': state = INNEQ; save = false; break;
				case  '|': state = INOR; save = false; break;
				case '&': state = INAND; save = false; break;
				case '"': state = INSTR; save = false; break;
				case '/': state = INDIV; save = false; break;
				case '$': state = INCOMMENT; save = false; break;
				case '<': state = INLTEQ; save = false; break;
				case '>': state = INRTEQ; save = false; break;
				case '+': state = INPLUS; save = true; break;
				case '-': state = INMINUS; save = true; break;

				case ' ': // got through
				case '\t':
				case '\n':
				case '\r':
					save = false;
					break;

				default:
					{
						state = DONE;
						switch( c )
						{
						case EOF:
							save = false;
							currentToken = ENDFILE;
							break;
						case '*': currentToken = TIMES; break;
						//case '+': currentToken = PLUS; break;
						//case '-': currentToken = MINUS; break;
						case '%': currentToken = REMAINDER; break;
						//case '<': currentToken = RT; break;
						//case '>': currentToken = LT; break;
						case '(': currentToken = LPAREN; break;
						case ')': currentToken = RPAREN; break;
						case '{': currentToken = LBRACE; break;
						case '}': currentToken = RBRACE; break;
						case '[': currentToken = LBRACKET; break;
						case ']': currentToken = RBRACKET; break;
						case ',': currentToken = COMMA; break;
						//case ':':	currentToken = COLON; break;
						case ';': currentToken = SEMICOLON; break;
						default: currentToken = _ERROR; break;
						}
					}
					break;
				}
			}
			break;

		case INCOMMENT:
			save = false;
			if( '\n' == c ) state = START;
			break;

		case INMULTI_COMMENT:
			if('*' == c)
			{
				state = OUTMULTI_COMMENT;
			}
			break;

		case OUTMULTI_COMMENT:
			if ('/' == c)
			{
				state = START;
			}
			else
			{
				state = INMULTI_COMMENT;
			}
			break;

		case INNUM:
			if( (c >= 0) && (c <= 255) && !isdigit(c) && ('.' != c) && ('-' != c) && ('+' != c))
			{
				UngetNextChar();
				save = false;
				state = DONE;
				currentToken = NUM;
			}
			break;
		case INID:
			if( (c >= 0) && (c <= 255) && !isalpha(c) && !isdigit(c) && 
				('_' != c) && ('.' != c) && ('@' != c) && ('-' != c) && (':' != c) && ('/'!=c) )
			{
				UngetNextChar();
				save = false;
				state = DONE;
				currentToken = ID;
			}
			break;
		case INDIV:
			if( '/' == c )
			{
				state = INCOMMENT;
			}
			else if ('*' == c)
			{
				state = INMULTI_COMMENT;
			}
			else
			{
				UngetNextChar();
				currentToken = DIV;
				state = DONE;
			}
			save = false;
			break;

		case INEQ:
			if( '=' == c )
			{
				currentToken = EQ;
			}
			else
			{
				UngetNextChar();
				currentToken = ASSIGN;
			}
			save = false;
			state = DONE;
			break;
		case INNEQ:
			if( '=' == c )
			{
				currentToken = NEQ;
			}
			else
			{
				currentToken = NEG;
				UngetNextChar();
				// 				currentToken = _ERROR;
			}
			save = false;
			state = DONE;
			break;
		case INOR:
			if( '|' == c )
			{
				currentToken = OR;
			}
			else
			{
				UngetNextChar();
				currentToken = LOGIC_OR;
			}
			save = false;
			state = DONE;
			break;
		case INAND:
			if( '&' == c )
			{
				currentToken = AND;
			}
			else
			{
				currentToken = REF;
				UngetNextChar();
				// 				currentToken = _ERROR;
			}
			save = false;
			state = DONE;
			break;
		case INSTR:
			if( '"' == c )
			{
				save = false;
				state = DONE;
				currentToken = STRING;
			}
			break;

		case INARROW:
			if( '>' == c )
			{
				currentToken = ARROW;
			}
			else if('-' == c)
			{
				currentToken = DEC;
			}
			else
			{
				UngetNextChar();
				currentToken = MINUS;
			}
			save = false;
			state = DONE;
			break;

		case INPLUS:
			if ('+' == c)
			{
				currentToken = INC;
				save = false;
				state = DONE;
			}
			else if ((c >= 0) && (c <= 255) && (isdigit(c) || ('.' == c)))
			{
				//if( (c >= 0) && (c <= 255) && !isdigit(c) && ('.' != c) && ('-' != c) && ('+' != c))
				//{
				//	UngetNextChar();
				//	save = false;
				//	state = DONE;
				//	currentToken = NUM;
				//}
				state = INNUM;
			}
			else
			{
				UngetNextChar();
				currentToken = PLUS;
				save = false;
				state = DONE;
			}
			break;

		case INMINUS:
			if ('-' == c)
			{
				currentToken = DEC;
				save = false;
				state = DONE;
			}
			else if ((c >= 0) && (c <= 255) && (isdigit(c) || ('.' == c)))
			{
				state = INNUM;
				//if( (c >= 0) && (c <= 255) && !isdigit(c) && ('.' != c) && ('-' != c) && ('+' != c))
				//{
				//	UngetNextChar();
				//	save = false;
				//	state = DONE;
					//currentToken = NUM;
				//}
				//currentToken = NUM;
			}
			else
			{
				UngetNextChar();
				currentToken = MINUS;
				save = false;
				state = DONE;
			}
			break;

		case INLTEQ:
			if( '=' == c )
			{
				currentToken = LTEQ;
			}
			else
			{
				UngetNextChar();
				currentToken = LT;
			}
			save = false;
			state = DONE;
			break;

		case INRTEQ:
			if ('=' == c)
			{
				currentToken = RTEQ;
			}
			else
			{
				UngetNextChar();
				currentToken = RT;
			}
			save = false;
			state = DONE;
			break;

		case INSCOPE:
			{
				if (':' == c)
				{
					currentToken = SCOPE;
				}
				else
				{
					UngetNextChar();
					currentToken = COLON;
				}
				save = false;
				state = DONE;
			}
			break;

		case DONE:
		default:
			printf( "scanner bug: state = %d", state );
			state = DONE;
			currentToken = _ERROR;
			break;
		}

		//if( (save) && (nTok < MAX_TOKENLEN) )
		//{
		//	//			if( (INNUM==state) && ('.' == c) )
		//	//				bFloat = TRUE;

		//	pToken[ nTok++] = c;
		//}
		//else if (nTok >= MAX_TOKENLEN)
		//{
		//	state = DONE;
		//	--nTok;
		//}

		//if( DONE == state )
		//{
		//	pToken[ nTok] = '\0';
		//	if( ID == currentToken )
		//		currentToken = reservedLookup( pToken );
		//}
		if (save)
		{
			token += (char)c;
		}
		if (DONE == state)
		{
			if (ID == currentToken)
				currentToken = reservedLookup(token);
		}
		if (EOF == c)
		{
			SetEndOfFile();
		}
	}

	if (m_IsTrace)
	{
		printf( "    %d: ", m_lineNo );
		//printToken( currentToken, pString );
	}

	return currentToken;
}


void CGenotypeScanner::SetEndOfFile()
{
	m_pCurrentMemPoint = m_memSize;
	m_linePos = m_memSize;
}


void CGenotypeScanner::Init()
{
	m_lineNo = 0;
	m_linePos = 0;
	m_bufSize = 0;
	m_pCurrentMemPoint = 0;

}

void CGenotypeScanner::Clear()
{

}


void genotype_parser::RemoveExpressoin_OnlyExpr(SExpr *expr)
{
	if (!expr)
		return;
	SConnectionList *pnode = expr->connection;
	while (pnode)
	{
		SConnectionList *rmNode = pnode;
		pnode = pnode->next;
		SAFE_DELETE(rmNode->connect);
		SAFE_DELETE(rmNode);
	}
	SAFE_DELETE(expr);
}


void genotype_parser::RemoveExpression(SExpr *expr)
{
	if (!expr)
		return;

	map<string,SExpr*> rm;
	std::queue<SExpr*> q;	
	q.push(expr);

	while (!q.empty())
	{
		SExpr *node = q.front(); q.pop();
		if (!node) 
			continue;
		if (rm.find(node->id) != rm.end())
			continue;

		rm[ node->id] = node;
		SConnectionList *con = node->connection;
		while (con)
		{
			q.push(con->connect->expr);
			con = con->next;
		}
	}

	BOOST_FOREACH(auto &kv, rm)
	{
		RemoveExpressoin_OnlyExpr(kv.second);
	}
}


/**
 @brief return Copy Genotype
 @date 2014-02-27
*/
genotype_parser::SExpr* CopyGenotypeRec(const SExpr *expr, map<string,SExpr*> &symbols)
{
	RETV(!expr, NULL);

	auto it = symbols.find(expr->id);
	if (symbols.end() != it)
	{ // already exist
		return it->second;
	}

	SExpr *newExpr = new SExpr;
	*newExpr = *expr;
	symbols[ newExpr->id] = newExpr;

	SConnectionList *srcConnection = expr->connection;
	SConnectionList *currentDestConnection = NULL;
	while (srcConnection)
	{
		if (!currentDestConnection)
		{
			currentDestConnection = new SConnectionList;
			newExpr->connection = currentDestConnection;
		}
		else
		{
			SConnectionList *newCopy = new SConnectionList;
			currentDestConnection->next = newCopy;
			currentDestConnection = newCopy;
		}

		currentDestConnection->connect = new SConnection;
		*currentDestConnection->connect = *srcConnection->connect;
		currentDestConnection->connect->expr = CopyGenotypeRec(srcConnection->connect->expr, symbols);

		srcConnection = srcConnection->next;
	}

	return newExpr;
}


/**
 @brief return Copy Genotype
 @date 2014-02-27
*/
SExpr* genotype_parser::CopyGenotype(const SExpr *expr)
{
	map<string,SExpr*> symbols;
	return CopyGenotypeRec(expr, symbols);
}


/**
 @brief 
 @date 2014-03-05
*/
void AssignGenotypeRec(SExpr *dest, const SExpr *src, set<string> &symbols)
{
	RET(!dest || !src);

	auto it = symbols.find(dest->id);
	if (symbols.end() != it)
	{ // already exist
		return;
	}

	*dest = *src;
	symbols.insert(dest->id);

	SConnectionList *srcConList = src->connection;
	SConnectionList *destConList = dest->connection;
	while (srcConList && destConList)
	{
		*destConList->connect = *srcConList->connect;
		AssignGenotypeRec(destConList->connect->expr, srcConList->connect->expr, symbols);

		srcConList = srcConList->next;
		destConList = destConList->next;
	}
}


/**
 @brief assign deep genotype
 @date 2014-03-05
*/
void genotype_parser::AssignGenotype(SExpr *dest, const SExpr *src)
{
	set<string> symbols;
	AssignGenotypeRec(dest, src, symbols);
}


/**
 @brief FindGenotype
 @date 2014-03-05
*/
SExpr* FindGenotypeRec(SExpr *expr, const string &id, set<string> &symbols)
{
	RETV(!expr, NULL);

	auto it = symbols.find(expr->id);
	if (symbols.end() != it)
	{ // already exist
		return NULL;
	}

	if (boost::iequals(expr->id, id))
		return expr;

	symbols.insert(expr->id);

	SConnectionList *conList = expr->connection;
	while (conList)
	{
		if (SExpr *ret = FindGenotypeRec(conList->connect->expr, id, symbols))
			return ret;
		conList = conList->next;
	}
	return NULL;
}


/**
 @brief 
 @date 2014-03-05
*/
SExpr* genotype_parser::FindGenotype(SExpr *expr, const string &id)
{
	set<string> symbols;
	return FindGenotypeRec(expr, id, symbols);
}
