
/*
	Script Parser 클래스

	2003-03-11	programer: jjuiddong
	2003-09-19	template 클래스로 수정
	2005-01-11	Token 분리하기 쉽게 수정 (macro 추가)

	2007-12-26	주석기능 추가 '$'
	
	2010-06-20 : vs2005로 컴파일 할 수있게 수정했다.

	2011-02-23 : 메모리 누수 제거 DeleteParseList()에 버그가 있었음

	2012-12-06 : SDftData 오버플로 버그 문제 처리

*/

#if !defined(__PARSER_H__)
#define __PARSER_H__

#include <windows.h>
#include <stdio.h>


/////////////////////////////////////////////////////////////////////////
// 판별함수

// 명령어 리스트 "{}"
static char* is_comlist( char c )
{
	return strchr( (char*)"{}", c );
}

// 주석 판별 '//'
static char* is_comment( char c )
{
	return strchr( (char*)"/", c );
}

// 공백 문자 판별, 공백, tab, carriage return, line feed를 판별
static char* is_white( char c )
{
	return strchr( (char*)" \t\n\r", c );
}

// 수치 자료 판별
static char* is_digit( char c )
{
	return strchr( (char*)"0123456789.", c );
}

// 명령어와 변수명을 판별을 위한 문자 판별 함수
static char* is_alpha( char c )
{
	return strchr( (char*)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz#_", c );
}

// 콤마 판별
static char* is_comma( char c )
{
	return strchr( (char*)",", c );
}

// 문자열 판별함수
static char* is_string( char c )
{
	return strchr( (char*)"'", c );
}

// 값설정 판별 '='
static char* is_assign( char c )
{
	return strchr( (char*)"=", c );
}

// 오퍼레이터 판별
static char* is_operator( char c )
{
	return strchr( (char*)"[]#*", c );
}


//////////////////////////////////////////////////////////////
// Class
template< class T >
class CParser
{
public:
	CParser(): m_pRoot(NULL) { }
	virtual ~CParser() { DeleteParseList(m_pRoot); m_pRoot = NULL; }
public:
	// DataList
	typedef struct _tagSParseList
	{
		T *t;
		_tagSParseList *pNext;
		_tagSParseList *pPrev;
		_tagSParseList *pParent;
		_tagSParseList *pChild;
		_tagSParseList() : t(NULL),pNext(NULL), pPrev(NULL), pParent(NULL), pChild(NULL) { }
	} SParseTree;
protected:
	SParseTree* m_pRoot;
public:
	//-----------------------------------------------------------------------------//
	// Name: Script Load
	// Date: (이재정 2003-03-11 16:3)
	//-----------------------------------------------------------------------------//
	SParseTree* OpenScriptFile( const char *szFileName )
	{
		FILE *fp;
		fopen_s( &fp, szFileName, "r" );
		if( !fp )
			return NULL;

		DeleteParseList( m_pRoot );
		if( fgetc(fp) == 'G' && fgetc(fp) == 'P' && fgetc(fp) == 'J' )
		{
			m_pRoot = new SParseTree;
			m_pRoot->pChild = ParseRec( fp, NULL, NULL );
		}
		fclose( fp );
		return m_pRoot;
	}

	//-----------------------------------------------------------------------------//
	// Token 을찾아서 포인터를 리턴한다.
	//-----------------------------------------------------------------------------//
	static SParseTree* FindToken( SParseTree *pParseList, const T &t, BOOL bSearchSub=FALSE ) // bSearchSub = FALSE
	{
		SParseTree *pNode = pParseList;
		while( pNode )
		{
			if( t == *pNode->t )
				break;
			// 하위트리 검색
			if( bSearchSub )
			{
				if( pNode->pChild )
				{
					CParser<T>::SParseTree *p = FindToken( pNode->pChild, t, bSearchSub );
					if( p ) return p;
				}
			}
			pNode = pNode->pNext;
		}
		return pNode;
	}

	//-----------------------------------------------------------------------------//
	// 자식노드 개수를 리턴한다.
	//-----------------------------------------------------------------------------//
	static int GetChildCount( SParseTree *pList )
	{
		if( !pList ) return 0;
		if( !pList->pChild ) return 0;
		int count = 0;
		SParseTree *pNode = pList->pChild;
		while( pNode )
		{
			++count;
			pNode = pNode->pNext;
		}
		return count;
	}

	//-----------------------------------------------------------------------------//
	// opt = 1 : 전체검사 (자식까지 들어가지 않는다.)
	// opt = 2 : pList만 검사한다.
	//-----------------------------------------------------------------------------//
	static int GetNodeCount( SParseTree *pList, const T &t, int opt=2 )
	{
		if( !pList ) return 0;
		int count = 0;
		SParseTree *pNode = pList;
		while( pNode )
		{
			if( t == *pNode->t )
			{
				++count;
			}
			else
			{
				if( 1 == opt )
				{
					// 다음 Node 검사
				}				
				else if( 2 == opt )
					break;
			}
			pNode = pNode->pNext;
		}

		return count;
	}

	//-----------------------------------------------------------------------------//
	// Date: (이재정 2005-01-13 5:38)
	//-----------------------------------------------------------------------------//
	static void DeleteParseList( SParseTree *pScrList )
	{
		if (!pScrList) return;
		if (pScrList->t) delete pScrList->t;
		DeleteParseList(pScrList->pChild);
		DeleteParseList(pScrList->pNext);
		delete pScrList;
	}

protected:
	//-----------------------------------------------------------------------------//
	// Name: Script Parsing
	// Date: (이재정 2003-03-11 16:4)
	//-----------------------------------------------------------------------------//
	SParseTree* ParseRec( FILE *fp, SParseTree *pParent, SParseTree *pPrev )
	{
		char c;
		BOOL bLoop=TRUE;
		SParseTree *pList = NULL;

		do
		{
			c = fgetc(fp);
			switch( c )
			{
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				break;
			case '}':
				bLoop = FALSE;
				break;

			case '{':
				if( pList )
				{
					pList->pChild = ParseRec( fp, pList, NULL );
					pList->pNext = ParseRec( fp, pParent, pList );
				}
				bLoop = FALSE;
				break;
			default:
				UngetNextChar( fp );
				if( !pList )
				{
					T *t = ParseData( fp );
					if (t)
					{
						pList = new SParseTree;
						pList->t = t;
						pList->pParent = pParent;
						pList->pPrev = pPrev;
					}
				}
				else
				{
					pList->pNext = ParseRec( fp, pParent, pList );
					UngetNextChar( fp );
				}
				break;
			}

		} while (bLoop && (EOF != c));

		return pList;
	}

	//-----------------------------------------------------------------------------//
	// Name: Script Parsing
	// Date: (이재정 2003-03-11 16:9)
	//-----------------------------------------------------------------------------//
	T* ParseData( FILE *fp )
	{
		int state=0;
		char c;
		BOOL bLoop=TRUE;
		char str[ 128] = {0,};
		T* t = NULL;

		do
		{
			c = fgetc(fp);
			switch( c )
			{
			case ' ':
			case '\t':
			case '\r':
			case ',':
			case '(': // 3DMax Script 때문에 추가됨
			case ')': // 3DMax Script 때문에 추가됨
			case '[': // 3DMax Script 때문에 추가됨
			case ']': // 3DMax Script 때문에 추가됨
				break;

			// 주석
			case '$':
				{
					char tmp[ 128];
					fgets( tmp, 128, fp );
					bLoop = FALSE;
				}
				break;

			// the end
			case '{':
			case '}':
				UngetNextChar( fp );
				bLoop = FALSE;
				break;

			case '\n':
				bLoop = FALSE;
				break;

			case '"':
				{
					if( !t ) 
						t = new T;
					int cnt=1;
					str[ 0] = c;

					while( EOF != (c = fgetc(fp)) && '"' != c && '\n' != c && '\r' != c )
						str[ cnt++] = c;
					str[ cnt] = '"';
					str[ ++cnt] = NULL;
					*t << str;

					if( EOF == c )
					{
						bLoop = FALSE;
					}
				}
				break;

			default:
				{
					if( !t )
						t = new T;
					int cnt=1;
					str[ 0]=c;
					
					if (is_assign(c))
					{
						fgetc(fp); // 앞으로 한칸 더 가야함, 밑에 UngetNextChar() 함수가 있기때문임
					}
					else
					{
						BOOL bFloat = FALSE; // float형일경우 '+-' 도 읽어야한다. ex "-1.62921e-007"
						while( EOF != (c = fgetc(fp)) &&
							(is_alpha(c) || is_digit(c) || is_operator(c) || 
							(bFloat && (c == '-')) || (bFloat && (c == '+')) || (bFloat && (c == 'e')) ) )
						{
							if( '.' == c ) bFloat = TRUE; // 소수점이 나오면 float형이 된다.
							str[ cnt++] = c;
						}
					}
					str[ cnt] = NULL;

					*t << str;

					if (EOF == c) break;
					else UngetNextChar( fp );
				}
				break;
			}

		} while (bLoop && (EOF != c));

		return t;
	}

	//-----------------------------------------------------------------------------//
	// File Pointer를 뒤로 한칸
	// Date: (이재정 2003-02-12 11:43)
	//-----------------------------------------------------------------------------//
	void UngetNextChar( FILE *fp )
	{
		fseek( fp, ftell(fp)-1, SEEK_SET );
	}
};



///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
// Base Token Data
typedef struct _SBaseToken
{
	int nType;
	int nCount;
	_SBaseToken():nType(0),nCount(0) {}
	_SBaseToken& operator << ( char *str ) { }
	BOOL operator== ( const _SBaseToken& rhs ) const { return FALSE; }

} SBaseToken;


///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
// Script Parsing Macro

#define PARSEBEGINE(type) \
	type& type::operator<<( char *c ) { \
		switch( nType ) {
#define PARSEEND() \
		case -1: break;\
		} return *this;\
	}
#define PARSETYPEBEGIN(type) \
		case type:{\
			switch( ++nCount ){
#define PARSETYPEEND() \
		case -1: break; }\
		} break;

#define PARSE_FLOAT_VALUE(nOrder,a) case nOrder: a=(float)atof(c); break;
#define PARSE_INT_VALUE(nOrder,a) case nOrder: a=atoi(c); break;
#define PARSE_STRING_VALUE(nOrder,a) case nOrder: strcpy( a, c ); break;

/*
///////////////////////////////////////////////////////////////////////////////////
// sample code

PARSEBEGINE(_tagSPspData)

	PARSETYPEBEGIN( PSP_ZIPSWITCH )
		PARSE_INT_VALUE(1,U.SZip::nFlag)
	PARSETYPEEND()

	PARSETYPEBEGIN( PSP_FTPINFO )
		PARSE_STRING_VALUE(1,U.SFtpInfo::szFtpName)
		PARSE_STRING_VALUE(2,U.SFtpInfo::szFtpID)
		PARSE_STRING_VALUE(3,U.SFtpInfo::szFtpPass)
	PARSETYPEEND()

PARSEEND()
/**/

#endif // __PARSER_H__
