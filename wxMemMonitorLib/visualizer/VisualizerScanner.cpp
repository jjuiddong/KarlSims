
#include "stdafx.h"
#include "VisualizerScanner.h"

using namespace visualizer;
using namespace visualizer::parser;

namespace visualizer
{
	Tokentype reservedLookup( char *s );

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
		{"#vert", VERTICAL},
		{"#horz", HORIZONTAL},
		{"children", CHILDREN},
		{"graph", GRAPH},
		{"preview", PREVIEW},
		{"stringview", STRINGVIEW},
		{"#", SHARP_BRACKET}, // 가장 낮은 우선순위로 검색해야한다.

	};
	const int g_rsvSize = sizeof(reservedWords) / sizeof(SReservedWord);

}

Tokentype visualizer::reservedLookup( char *s )
{
	int i=0;
	for( i=0; i < g_rsvSize; ++i )
	{
		if( !_stricmp(s, reservedWords[ i].str) )
			return reservedWords[ i].tok;
	}
	return ID;
}

CScanner::CScanner() :
	m_pFileMem(NULL)
{
}

CScanner::~CScanner()
{
	Clear();
}

//----------------------------------------------------------------------------
// 
//----------------------------------------------------------------------------
BOOL CScanner::LoadFile( const char *szFileName, BOOL bTrace ) // bTrace=FALSE
{
	OFSTRUCT of;

	HFILE hFile = OpenFile(szFileName, &of, OF_READ);
	if (hFile != EOF)
	{
		const int fileSize = GetFileSize((HANDLE)hFile, NULL);
		if (fileSize <= 0)
			return FALSE;

		if (m_pFileMem)
			delete[] m_pFileMem;

		DWORD readSize = 0;
		m_pFileMem = new BYTE[ fileSize];
		ReadFile((HANDLE)hFile, m_pFileMem, fileSize, &readSize, NULL);
		CloseHandle((HANDLE)hFile);

		m_nMemSize = fileSize;
		m_bTrace = bTrace;
		Init();
		return TRUE;
	}

	printf( "[%s] 파일이 없습니다.\n", szFileName );
	return FALSE;
}


//----------------------------------------------------------------------------
// 패키지 파일을 읽어서 복사해서 사용한다.
//----------------------------------------------------------------------------
BOOL CScanner::LoadPackageFile( BYTE *pFileMem, int nFileSize )
{
	if (m_pFileMem)
		delete[] m_pFileMem;

	m_nMemSize = nFileSize;
	m_pFileMem = new BYTE[ nFileSize];
	memcpy(m_pFileMem, pFileMem, nFileSize);

	Init();

	return TRUE;
}


//----------------------------------------------------------------------------
// 메모리의 끝을 가르키거나, 로드되지 않았다면 true를 리턴한다.
//----------------------------------------------------------------------------
BOOL CScanner::IsEnd()
{
	if (!m_pFileMem) return TRUE;
	if (m_pCurrentMemPoint >= m_nMemSize) return TRUE;

	return FALSE;
}


Tokentype CScanner::GetToken()
{
	if (!m_pFileMem) return ENDFILE;

	if( m_TokQ.size() == 0 )
	{
		for( int i=0; i < MAX_QSIZE; ++i )
		{
			STokDat d;
			char buf[ MAX_TOKENLEN];
			d.tok = _GetToken( buf );
			d.str = buf;
			m_TokQ.push_back( d );
		}
	}
	else
	{
		STokDat d;
		char buf[ MAX_TOKENLEN];
		d.tok = _GetToken( buf );
		d.str = buf;
		m_TokQ.push_back( d );
		m_TokQ.pop_front();
	}

	return m_TokQ[ 0].tok;
}


Tokentype CScanner::GetTokenQ( int nIdx )
{
	if (!m_pFileMem) return ENDFILE;
	if (m_TokQ.size() <= (size_t)nIdx) return ENDFILE;
	return m_TokQ[ nIdx].tok;
}


char* CScanner::GetTokenStringQ( int nIdx )
{
	if (!m_pFileMem) return NULL;
	return 	(char*)m_TokQ[ nIdx].str.c_str();
}


char* CScanner::CopyTokenStringQ( int nIdx )
{
	if (!m_pFileMem) return NULL;

	int len = m_TokQ[ nIdx].str.size();
	char *p = new char[ len + 1];
	strcpy_s( p, len+1, m_TokQ[ nIdx].str.c_str() );
	return p;
}


char CScanner::GetNextChar()
{
	if( m_nLinePos >= m_nBufSize )
	{
		++m_nLineNo;
//		if( fgets(m_Buf, MAX_BUFF-1, m_fp) )
		if (GetString(m_Buf, MAX_BUFF))
		{
			m_nLinePos = 0;
			m_nBufSize = strlen( m_Buf );
 			if( m_bTrace )
 				printf( "%4d: %s", m_nLineNo, m_Buf );
		}
		else 
		{
			return EOF;
		}
	}

	return m_Buf[ m_nLinePos++];
}


//----------------------------------------------------------------------------
// fgets() 함수와 비슷한 일을 한다. 파일에 있는 데이타가 아니라, 메모리에
// 있는 데이타를 가져온다는 것이 다르다.
//----------------------------------------------------------------------------
BOOL CScanner::GetString(char *receiveBuffer, int maxBufferLength)
{
	if (m_pCurrentMemPoint >= m_nMemSize)
		return FALSE;

	// 메모리는 바이너리 형식으로 얻어오기 때문에, 
	// 개행문자 만큼 데이타를 얻어와야 한다.
	int i=0;
	for (i=0; (i < maxBufferLength) && (m_pCurrentMemPoint < m_nMemSize); ++i)
	{
		const int memPoint = m_pCurrentMemPoint;

		// 개행문자?
		if ( '\r' == m_pFileMem[ memPoint] ) // '\n' == m_pFileMem[idx]
		{
			m_pCurrentMemPoint += 2;
			receiveBuffer[ i++] = '\n';
			break;
		}
		else
		{
			receiveBuffer[ i] = m_pFileMem[ memPoint];
			++m_pCurrentMemPoint;
		}
	}

	if (i < maxBufferLength)
		receiveBuffer[ i] = NULL;
	else
		receiveBuffer[ maxBufferLength-1] = NULL;

	return TRUE;
}


void CScanner::UngetNextChar()
{
	--m_nLinePos;
}


Tokentype CScanner::_GetToken( char *pToken )
{
	BOOL bFloat = FALSE;
	Tokentype currentToken;
	StateType state = START;

	int nTok = 0;
	while( DONE != state )
	{
		char c = GetNextChar();
		BOOL save = TRUE;
		switch( state )
		{
		case START:
			if( isdigit(c) )
				state = INNUM;
			else if( isalpha(c) || '@' == c || '_' == c || '#' == c || '$' == c)
				state = INID;
			else if( '=' == c )
			{
				state = INEQ;
				save = FALSE;
			}
			else if( '!' == c )
			{
				state = INNEQ;
				save = FALSE;
			}
			else if( '|' == c )
			{
				state = INOR;
				save = FALSE;
			}
			else if( '&' == c )
			{
				state = INAND;
				save = FALSE;
			}
			else if( '"' == c )
			{
				state = INSTR;
				save = FALSE;
			}
			else if( '/' == c )
			{
				state = INDIV;
				save = FALSE;
			}
// 			else if( '$' == c )
// 			{
// 				state = INCOMMENT;
// 				save = FALSE;
// 			}
			else if('#' == c)
			{
				state = INSHARP;
				save = FALSE;
			}
			else if('+' == c)
			{
				state = INPLUS;
				save = FALSE;
			}
			else if ( '-' == c)
			{
				state = INARROW;
				save = FALSE;
			}
			else if ( '<' == c)
			{
				state = INLTEQ;
				save = FALSE;
			}
			else if ( '>' == c)
			{
				state = INRTEQ;
				save = FALSE;
			}
			else if (':' == c)
			{
				state = INSCOPE;
				save = FALSE;
			}
			else if( (' ' == c) || ('\t' == c) || ('\n' == c) )
				save = FALSE;
			else
			{
				state = DONE;
				switch( c )
				{
				case EOF:
					save = FALSE;
					currentToken = ENDFILE;
					//					fclose( m_fp );
					//					m_fp = NULL;
					break;
				case '*':
					currentToken = TIMES;
					break;
				case '%':
					currentToken = REMAINDER;
					break;
				case '(':
					currentToken = LPAREN;
					break;
				case ')':
					currentToken = RPAREN;
					break;
				case '{':
					currentToken = LBRACE;
					break;
				case '}':
					currentToken = RBRACE;
					break;
				case '[':
					currentToken = LBRACKET;
					break;
				case ']':
					currentToken = RBRACKET;
					break;
				case '.':
					currentToken = DOT;
					break;
				case ',':
					currentToken = COMMA;
					break;
				case ';':
					currentToken = SEMICOLON;
					break;
				default:
					currentToken = _ERROR;
					break;
				}
			}
			break;

		case INCOMMENT:
			save = FALSE;
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
			if( !isdigit(c) && '.' != c )
			{
				UngetNextChar();
				save = FALSE;
				state = DONE;
				currentToken = NUM;
			}
			break;
		case INID:
			if( !isalpha(c) && !isdigit(c) && ('_' != c) && ('@' != c) )
			{
				UngetNextChar();
				save = FALSE;
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
			save = FALSE;
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
			save = FALSE;
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
			save = FALSE;
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
			save = FALSE;
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
			save = FALSE;
			state = DONE;
			break;
		case INSTR:
			if( '"' == c )
			{
				save = FALSE;
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
			save = FALSE;
			state = DONE;
			break;

		case INPLUS:
			if ('+' == c)
			{
				currentToken = INC;
			}
			else
			{
				UngetNextChar();
				currentToken = PLUS;
			}
			save = FALSE;
			state = DONE;
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
			save = FALSE;
			state = DONE;
			break;

		case INRTEQ:
			if( '=' == c )
			{
				currentToken = RTEQ;
			}
			else
			{
				UngetNextChar();
				currentToken = RT;
			}
			save = FALSE;
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
				save = FALSE;
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

		if( (save) && (nTok < MAX_TOKENLEN) )
		{
			//			if( (INNUM==state) && ('.' == c) )
			//				bFloat = TRUE;

			pToken[ nTok++] = c;
		}
		else if (nTok >= MAX_TOKENLEN)
		{
			state = DONE;
			--nTok;
		}

		if( DONE == state )
		{
			pToken[ nTok] = '\0';
			if( ID == currentToken )
				currentToken = reservedLookup( pToken );
		}
	}

	if( m_bTrace )
	{
		printf( "    %d: ", m_nLineNo );
		//printToken( currentToken, pString );
	}

	return currentToken;
}


void CScanner::Init()
{
	m_nLineNo = 0;
	m_nLinePos = 0;
	m_nBufSize = 0;
	m_pCurrentMemPoint = 0;

}

void CScanner::Clear()
{

	if (m_pFileMem)
	{
		delete[] m_pFileMem;
		m_pFileMem = NULL;
	}

}
