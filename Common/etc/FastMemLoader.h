//-----------------------------------------------------------------------------//
// 2005-10-23  programer: jaejung ��(���բ�)��
//
// 2005-12-05 write, read �Լ� ����߰�
// ���̳ʸ� �޸� (structure memory)�� TokenData�� �°� binary, text file�� �����Ѵ�. (write)
// write�Լ��� �ۼ��� ���̳ʸ������� structure memory�� ��ȯ�Ѵ�. (read)
//
// 2008-02-05 
//		script ����Ÿ �ε�&���� ��� �߰�
//
// 2009-01-03
//		int �迭 �ε�&���� ��� �߰�, Clear�Լ� �߰�
//
// 2012-12-06
//		�����丵, ��ȣ�� ����ü �̸�, ���� �̸��� ����.
//		����ó��, �߰��� ������ ����� FALSE�� �����ϰ� ��
//		WriteScript() ���� �ذ�
//		LinearMemLoader -> FastMemLoader �� �̸� ����
//		<key, value> ������ ��ũ��Ʈ�� ����
//		** ��� ����� �� ���ư��� ���� �Ϸ��� ** 
//
//
// FastMemLoader�� ������ �޸𸮸� �ڵ����� �������ִ� �Լ� �߰�����.
// ��ũ��Ʈ���� �߰�ȣ �ȿ��� �Ӽ����� ������ �� �ְ� ����.
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
		int nOffset;			// ����ü���� ��� ��ġ�� ����Ǿ�� �ϴ����� �����Ѵ�.
		int nSize; // (type_data, (type_pointer = sizeof(void*)) (type_array = typesize * arraysize)
		int nElementSize;		// �迭�̳�, �����Ϳ� ������� Ÿ���� ũ�⸦ �����Ѵ�.

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
		MemberList *pMember;	// ��� ���� Ÿ�� ����Ʈ
		int size;				// Ÿ�� ������ (����Ʈ����)
		BOOL pointer;			// ����� �����Ͱ� �����ϸ� TRUE �׷����ʴٸ� FALSE
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
		int nFilePos;		// Update�ؾߵ� file pointer position
		int nPointerSize;	// (TYPE_POINTER = pointersize) (�׿� = 0)
		BYTE *pStruct;
	} SWsp;



public:
	DataStructureMap	m_DataStructureMap;
	std::string			m_ErrString;			// ���� �ֱٿ� �߻��� ���������� �����Ѵ�.

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
