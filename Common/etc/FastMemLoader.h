//-----------------------------------------------------------------------------//
// 2005-10-23  programer: jaejung ┏(⊙д⊙)┛
//
// 2005-12-05 write, read 함수 기능추가
// 바이너리 메모리 (structure memory)를 TokenData에 맞게 binary, text file로 저장한다. (write)
// write함수로 작성된 바이너리파일을 structure memory로 변환한다. (read)
//
// 2008-02-05 
//		script 데이타 로드&쓰기 기능 추가
//
// 2009-01-03
//		int 배열 로드&쓰기 기능 추가, Clear함수 추가
//
// 2012-12-06
//		리팩토링, 모호한 구조체 이름, 변수 이름을 정리.
//		에러처리, 중간에 문제가 생기면 FALSE를 리턴하게 함
//		WriteScript() 버그 해결
//		LinearMemLoader -> FastMemLoader 로 이름 변경
//		<key, value> 형태의 스크립트로 수정
//		** 모든 기능이 잘 돌아가게 수정 완료함 ** 
//
//
// FastMemLoader로 생성한 메모리를 자동으로 제거해주는 함수 추가하자.
// 스크립트에서 중괄호 안에서 속성값을 설정할 수 있게 하자.
//-----------------------------------------------------------------------------//

#if !defined(__FASTMEMLOADER_H__)
#define __FASTMEMLOADER_H__


#pragma warning (disable: 4786)
#include <windows.h>
#include <string>
#include <list>
#include <queue>
#include <map>
#include "parser.h"


//-----------------------------------------------------------------------------//
// CFastMemLoader
//-----------------------------------------------------------------------------//
class CFastMemLoader
{
public:
	CFastMemLoader();
	CFastMemLoader( const char *szDataStructureFileName );
	virtual ~CFastMemLoader();

protected:
	///////////////////////////////////////////////////////////////////////////////////
	// SScriptData

	typedef struct _SScriptDataKeyValue
	{
		std::string key;
		std::list<std::string> array;

		_SScriptDataKeyValue() {}
		void AddValue( char *str );
	} SScriptDataKeyValue;

	typedef struct _SScriptData : public _SBaseToken
	{
		_SScriptData();
		_SScriptData( const char *str );

		enum STATE { NameParse, KeyValueParse};
		STATE state;
		std::string name;
		std::list<SScriptDataKeyValue> keyValueList;

		_SScriptData& operator << ( char *str );
		BOOL operator == ( const _SScriptData& rhs ) const;
	} SScriptData;

	typedef CParser<SScriptData> SDftDataParser;
	typedef CParser<SScriptData>::SParseTree SScriptParseTree;
	///////////////////////////////////////////////////////////////////////////////////


	enum VALUETYPE { TYPE_NULL, TYPE_DATA, TYPE_POINTER, TYPE_ARRAY, TYPE_STRUCT };

	typedef struct _tagSMemberType
	{
		_tagSMemberType():nSize(0) {}
		_tagSMemberType( const std::string &tName, const std::string &vName ) :
			typeName(tName), valueName(vName) {}
		_tagSMemberType( const std::string &tName, const std::string &vName, int size, int elemtSize,
			int offset, VALUETYPE type ):
			nSize(size), nElementSize(elemtSize), nOffset(offset), typeName(tName), valueName(vName), eType(type) {}
		VALUETYPE eType;
		std::string typeName;
		std::string valueName;
		int nOffset;			// 구조체에서 어느 위치에 저장되어야 하는지를 저장한다.
		int nSize; // (type_data, (type_pointer = sizeof(void*)) (type_array = typesize * arraysize)
		int nElementSize;		// 배열이나, 포인터에 상관없이 타입의 크기를 저장한다.

		BOOL operator==(const _tagSMemberType &rhs)
			{ 
				if (rhs.typeName.empty())
					return valueName == rhs.valueName; 
				else
					return typeName == rhs.typeName;
			}

	} SMemberType;

	typedef std::list<SMemberType> MemberList;
	typedef MemberList::iterator MemberItor;

	typedef struct _tagSDataStructure
	{
		MemberList *pMember;	// 멤버 변수 타입 리스트
		int size;				// 타입 사이즈 (바이트단위)
		BOOL pointer;			// 멤버에 포인터가 존재하면 TRUE 그렇지않다면 FALSE
		_tagSDataStructure() : pMember(NULL) {}
		_tagSDataStructure( MemberList *p, int s, BOOL pt ) : pMember(p), size(s),pointer(pt) {}
		BOOL FindMemberKey( const std::string &key, OUT SMemberType *pType ) const;
		BOOL FindMemberType( const std::string &type, OUT SMemberType *pType ) const;
		MemberItor FindMemberType( MemberItor beginIt, const std::string &type, OUT SMemberType *pType ) const;

	} SDataStructure;

	typedef std::map<std::string, SDataStructure > DataStructureMap;
	typedef DataStructureMap::iterator TokenItor;

	typedef struct _tagSWsp	// Write Struct Pointer
	{
		_tagSWsp() {}
		_tagSWsp( std::string tok, VALUETYPE type, int fp, int size, BYTE *p ) : typeName(tok), eType(type), nFilePos(fp), nPointerSize(size), pStruct(p) {}
		std::string typeName;
		VALUETYPE eType;
		int nFilePos;		// Update해야될 file pointer position
		int nPointerSize;	// (TYPE_POINTER = pointersize) (그외 = 0)
		BYTE *pStruct;
	} SWsp;



public:
	DataStructureMap	m_DataStructureMap;
	std::string			m_ErrString;			// 가장 최근에 발생한 에러정보를 저장한다.

public:
	BOOL WriteBin( const char *szFileName, void *pStruct, const char *typeName );
	BOOL WriteBin( FILE *fp, void *pStruct, const char *typeName );
	BOOL WriteScript( const char *szFileName, void *pStruct, const char *typeName );
	BOOL WriteScript( FILE *fp, void *pStruct, const char *typeName, int tab );
	BYTE* ReadBin( const char *szFileName, const char *typeName );
	int ReadBinMem( BYTE *pReadMem, const char *typeName );
	BYTE* ReadScript( const char *szFileName, const char *typeName );
	BOOL LoadDataStructureFile( const char *szFileName, int opt=0 );
	BOOL AddType( const char *typeName, int nSize, int nOffset, const char *szParentName=NULL );
	void Clear();

protected:
	void CreateDefaultType();
	BOOL LoadDataStructureFileRec( SScriptParseTree *pParseTree, MemberList *pMemberList, int opt );
	BOOL LoadMemberList( SScriptParseTree *pParseTree, MemberList *pMemberList );

	BOOL ReadRec( DWORD dwOffset, BYTE *pStruct, MemberList *pMList );
	BOOL ReadScriptRec( BYTE *pStruct, SScriptParseTree *pParseTree, const SDataStructure *pDataStructure );
	BOOL CollectPointerRec( DWORD dwCurFilePos, BYTE *pStruct, MemberList *pMList, std::queue<SWsp> *pWspList );
	int ParseData( BYTE *pStruct, const SMemberType &type, const char *szToken, SScriptParseTree *pParseTree=NULL, int idx=0 );
	int GetStructSize( MemberList *pList );
	BOOL IsPointer( MemberList *pList );
	void WriteTextData( FILE *fp, BYTE *pSrc, SMemberType *pTok );
	void WriteTab( FILE *fp, int tab );

	void ErrorReport( const std::string &errStr ) { m_ErrString = errStr; }

};

#endif // __LINEARMEMLOADER_H__
