
#include "stdafx.h"
#include "fastmemloader.h"
#include <sys/stat.h>


using namespace std;


typedef struct _tagSBaseType
{
	char name[ 32];
	char size;
} SBaseType;
static SBaseType g_BaseType[] = 
{
	{ "int", sizeof(int) }, 
	{ "float", sizeof(float) },
	{ "char", sizeof(char) },
	{ "short", sizeof(short) },
	{ "size", sizeof(int) },
	{ "dummy", sizeof(int) }
};
const int g_BaseTypeSize = sizeof(g_BaseType) / sizeof(SBaseType);

SBaseType* GetBaseType( const char *pTypeName )
{
	for( int i=0; i < g_BaseTypeSize; ++i )
	{
		if( !strcmp(g_BaseType[ i].name, pTypeName) )
			return &g_BaseType[ i];
	}
	return NULL;
}


///////////////////////////////////////////////////////////////////////////////////
// Default DataList
void CFastMemLoader::SScriptDataKeyValue::AddValue( char *str )
{
	std::string s = str;
	size_t n1 = s.find( '"' );
	if (std::string::npos != n1) // 스트링은 따옴표를 걸러내야한다.
	{
		size_t n2 = s.find('"', n1+1);
		std::string addStr = s.substr(n1+1, n2-n1-1);
		array.push_back( addStr );
	}
	else
	{
		array.push_back( str );
	}

}

CFastMemLoader::_SScriptData::_SScriptData() 
{
	state = NameParse;
}
CFastMemLoader::_SScriptData::_SScriptData( const char *str ) 
{ 
	if( !str ) return;
	name = str;
	state = KeyValueParse;
}

// SScriptData 에서 Key로 사용되는 스트링이라면 true를 리턴한다.
// 문자로 시작하는 스트링이라면 Key로 인식한다.
bool IsKey( char *str )
{
	if (strlen(str) <= 0)
		return false;
	return (is_alpha(str[0])? true : false);
}

//------------------------------------------------------------------------
// 데이타 삽입
//------------------------------------------------------------------------
CFastMemLoader::_SScriptData& CFastMemLoader::_SScriptData::operator << ( char *str ) 
{
	switch (state)
	{
	case NameParse:
		name = str;
		state = KeyValueParse;
		break;

	case KeyValueParse:
		{
			if (std::string(str) == "=") // key에 할당하는 토큰일때는 무시한다.
				return *this;

			if (IsKey(str))
			{
				keyValueList.push_back(SScriptDataKeyValue());

				SScriptDataKeyValue &value = keyValueList.back();
				value.key = str;
			}
			else
			{
				if (!keyValueList.empty())
				{
					SScriptDataKeyValue &value = keyValueList.back();
					value.AddValue( str );
				}
			}
		}
		break;
	}

	return *this;
}

//------------------------------------------------------------------------
// 비교 연산
//------------------------------------------------------------------------
BOOL CFastMemLoader::_SScriptData::operator == ( const _SScriptData& rhs ) const
{
	return name == rhs.name;
}

//------------------------------------------------------------------------
// 멤버변수중에 key값과 같은 멤버타입을 리턴한다. (자식의 자식은 찾지 않는다.)
//------------------------------------------------------------------------
BOOL CFastMemLoader::SDataStructure::FindMemberKey( const std::string &key, OUT SMemberType *pType) const
{
	if (!pMember)
		return FALSE;
	MemberItor it = find( pMember->begin(), pMember->end(), SMemberType("", key));
	if (pMember->end() == it)
		return FALSE;
	*pType = *it;
	return TRUE;
}

//------------------------------------------------------------------------
// 멤버변수중에 type값과 같은 멤버타입을 리턴한다. (자식의 자식은 찾지 않는다.)
//------------------------------------------------------------------------
BOOL CFastMemLoader::SDataStructure::FindMemberType( const std::string &type, OUT SMemberType *pType) const
{
	if (!pMember)
		return FALSE;
	MemberItor it = find( pMember->begin(), pMember->end(), SMemberType(type, ""));
	if (pMember->end() == it)
		return FALSE;
	*pType = *it;
	return TRUE;
}

//------------------------------------------------------------------------
// 멤버변수중에 type값과 같은 멤버타입을 리턴한다. (자식의 자식은 찾지 않는다.)
// MemberItor it 부터 검사를 한다.
//------------------------------------------------------------------------
CFastMemLoader::MemberItor CFastMemLoader::SDataStructure::FindMemberType( MemberItor beginIt, 
								const std::string &type, OUT SMemberType *pType) const
{
	MemberItor it = find( beginIt, pMember->end(), SMemberType(type, ""));
	if (pMember->end() == it)
		return pMember->end();
	*pType = *it;
	return it;
}
///////////////////////////////////////////////////////////////////////////////////




CFastMemLoader::CFastMemLoader()
{
	CreateDefaultType();
}

CFastMemLoader::CFastMemLoader( const char *szDataStructureFileName )
{
	CreateDefaultType();
	if( szDataStructureFileName )
		LoadDataStructureFile( szDataStructureFileName );
}


CFastMemLoader::~CFastMemLoader()
{
	Clear();
}

void CFastMemLoader::Clear()
{
	TokenItor i = m_DataStructureMap.begin();
	while( m_DataStructureMap.end() != i )
	{
		SDataStructure type = i++->second;
		if( type.pMember )
			delete type.pMember;
	}
	m_DataStructureMap.clear();
}


//-----------------------------------------------------------------------------//
// 기본적인건 미리 만들어 놓는다.
//-----------------------------------------------------------------------------//
void CFastMemLoader::CreateDefaultType()
{
	for( int i=0; i < g_BaseTypeSize; ++i )
		AddType( g_BaseType[ i].name, g_BaseType[ i].size, 0 );
}


//-----------------------------------------------------------------------------//
// Token file 로드
// opt: 0 중복된 데이타가 발생하면 에러를 발생한다.
//		1 중복된 데이타가 발생하면 덮어 씌운다.
//-----------------------------------------------------------------------------//
BOOL CFastMemLoader::LoadDataStructureFile( const char *szFileName, int opt )
{
	SDftDataParser parser;
	SScriptParseTree *pRoot = parser.OpenScriptFile( szFileName );
	if( !pRoot ) return NULL;

	SScriptParseTree *pNode = pRoot->pChild;
	LoadDataStructureFileRec( pRoot->pChild, NULL, opt );

	return TRUE;
}


//-----------------------------------------------------------------------------//
// Token을 생성한다.
// opt: 0 임 m_DataStructureMap에 있는 타입 일경우 다시 저장하지 않는다.
//		1 중복된 데이타가 발생하면 덮어 씌운다.
//-----------------------------------------------------------------------------//
BOOL CFastMemLoader::LoadDataStructureFileRec( SScriptParseTree *pParseTree, MemberList *pMemberList, int opt ) // opt=0
{
	if( !pParseTree ) return FALSE;

	TokenItor itor = m_DataStructureMap.find( pParseTree->t->name );

	if( itor != m_DataStructureMap.end() ) // 이미 등록된 데이타 타입이라면,
	{
		// 같은 데이타가 중복되어 선언되어 있다면 opt값에 따라 흐름이 결정된다.
		if( 0 == opt )
		{
			return TRUE;
		}
		else if( 1 == opt )
		{
			SDataStructure type = itor->second;
			type.pMember->clear();
			if (!LoadMemberList( pParseTree->pChild, type.pMember ))
				return FALSE;
			itor->second.size = GetStructSize( type.pMember );
		}
	}
	else
	{
		list<SMemberType> *pNewMemberList = new list<SMemberType>;
		if (!LoadMemberList( pParseTree->pChild, pNewMemberList ))
			return FALSE;
		SDataStructure type( pNewMemberList, GetStructSize(pNewMemberList), IsPointer(pNewMemberList) );
		m_DataStructureMap.insert( DataStructureMap::value_type(pParseTree->t->name, type) );
	}

	if (!LoadDataStructureFileRec( pParseTree->pNext, pMemberList, opt ))
		return FALSE;

	return TRUE;
}


//-----------------------------------------------------------------------------//
// Custom struct의 멤버 타입을 pMemberList에 저장한다.
//-----------------------------------------------------------------------------//
BOOL CFastMemLoader::LoadMemberList( SScriptParseTree *pParseTree, MemberList *pMemberList )
{
	SScriptParseTree *pNode = pParseTree;

	int offset = 0; // 멤버변수가 구조체내에서 위치해될 주소를 저장한다.
	while( pNode )
	{
		TokenItor itor = m_DataStructureMap.find( pNode->t->name );
		if (itor == m_DataStructureMap.end())
		{
			// 배열
			if (string::npos != pNode->t->name.find('['))
			{
				const size_t n1 = pNode->t->name.find('[');
				const size_t n2 = pNode->t->name.find(']'); 
				const std::string sizeStr = pNode->t->name.substr(n1+1, n2-n1-1);
				int nSize = atoi(sizeStr.c_str());
 				const std::string typeName = pNode->t->name.substr(0, n1);

				std::string valueName;
				if (!pNode->t->keyValueList.empty())
					valueName = pNode->t->keyValueList.front().key;

				TokenItor typeIt = m_DataStructureMap.find( typeName.c_str() );
				if( typeIt == m_DataStructureMap.end() )
					return FALSE;	// error

				nSize *= typeIt->second.size; // Array Total size
				pMemberList->push_back( SMemberType(typeName, valueName, nSize, typeIt->second.size, offset, TYPE_ARRAY) );

				offset += nSize;
			}
			// 포인터
			else if (string::npos != pNode->t->name.find('*'))
			{
				const size_t n1 = pNode->t->name.find('*');
				const string typeName = pNode->t->name.substr(0, n1);

				std::string valueName;
				if (!pNode->t->keyValueList.empty())
					valueName = pNode->t->keyValueList.front().key;

				TokenItor typeIt = m_DataStructureMap.find( typeName.c_str() );
				if( typeIt == m_DataStructureMap.end() )
					return FALSE;	// error

				const int nSize = sizeof(void*);
				pMemberList->push_back( SMemberType(typeName, valueName, nSize, typeIt->second.size, offset, TYPE_POINTER) );

				offset += nSize;
			}
			else
			{
				// 멤버리스트에 들어가는 타입은 이미 정의된 상태여야 한다. 없다면 Error
				break;
			}
		}
		else
		{
			std::string valueName;
			if (!pNode->t->keyValueList.empty())
				valueName = pNode->t->keyValueList.front().key;

			SBaseType *p = GetBaseType( pNode->t->name.c_str() );
			if( p )
			{
				pMemberList->push_back( SMemberType(p->name, valueName, p->size, p->size, offset, TYPE_DATA) );
				offset += p->size;
			}
			else
			{
				// custome structure 일경우
				SDataStructure type = itor->second;
				pMemberList->push_back( SMemberType(pNode->t->name, valueName, type.size, type.size, offset, TYPE_STRUCT) );

				offset += type.size;
			}
		}

		pNode = pNode->pNext;
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// 구조체 pStruct 정보를 szTokenName 타입으로 szFileName 파일에 쓴다.
// szFileName : Write할 파일이름
// pStruct: 정보를 저장하고있는 메모리
// szTokenName : 구조체 타입
//-----------------------------------------------------------------------------//
BOOL CFastMemLoader::WriteBin( const char *szFileName, void *pStruct, const char *typeName )
{
	if( !pStruct ) return FALSE;

	FILE *fp;
	fopen_s( &fp, szFileName, "wb" );
	if( !fp ) return FALSE;

	WriteBin( fp, pStruct, typeName );
	fclose( fp );

	return TRUE;
}
BOOL CFastMemLoader::WriteScript( const char *szFileName, void *pStruct, const char *typeName )
{
	if( !pStruct ) return FALSE;

	FILE *fp;
	fopen_s( &fp, szFileName, "w" );
	if( !fp ) return FALSE;
	fprintf( fp, "GPJ\n" );

	WriteScript( fp, pStruct, typeName, 0 );
	fclose( fp );

	return TRUE;
}

//-----------------------------------------------------------------------------//
// 구조체 pStruct 정보를 szTokenName 타입으로 파일포인터 fp에 쓴다.
// fp : Dest파일 포인터
// pStruct: 정보를 저장하고있는 메모리
// typeName : 구조체 타입
//-----------------------------------------------------------------------------//
BOOL CFastMemLoader::WriteBin( FILE *fp, void *pStruct, const char *typeName )
{
	queue<SWsp> wspQueue;
	wspQueue.push( SWsp(typeName, TYPE_DATA, 0, 0, (BYTE*)pStruct) );
	int nFileStartPos = ftell( fp );

	while( !wspQueue.empty() )
	{
		SWsp wsp = wspQueue.front(); 
		wspQueue.pop();

		TokenItor it = m_DataStructureMap.find( wsp.typeName );
		if( m_DataStructureMap.end() == it ) 
			return FALSE;

		SDataStructure type = it->second;

		// file에 쓰기
		switch( wsp.eType )
		{
		case TYPE_ARRAY:
		case TYPE_DATA:
		case TYPE_STRUCT:
			{
				fwrite( wsp.pStruct, type.size, 1, fp );

				// struct에서 pointer 와 filepos 얻음
				if (!CollectPointerRec( ftell(fp) - type.size, (BYTE*)wsp.pStruct, type.pMember, &wspQueue ))
					return FALSE;
			}
			break;

		case TYPE_POINTER:
			{
				// 현재 filePos를 저장한다.
				DWORD curPos = ftell( fp );
				DWORD pointerOffset = curPos - nFileStartPos;
				fseek( fp, wsp.nFilePos, SEEK_SET ); // pointer값이 Update될곳으로 이동
				fwrite( &pointerOffset, sizeof(DWORD), 1, fp );
				fseek( fp, curPos, SEEK_SET ); // 다시 돌아옴
				fwrite( wsp.pStruct, type.size, wsp.nPointerSize, fp ); // writeSize = type.size * pointerSize

				// struct에서 pointer 와 filepos 얻음
				//curPos = ftell( fp ) - (type.size * wsp.nPointerSize);
				for( int i=0; i < wsp.nPointerSize; ++i )
					if (!CollectPointerRec( curPos + (type.size*i), wsp.pStruct+(type.size*i), type.pMember, &wspQueue ))
						return FALSE;
			}
			break;
		}
	}

	return TRUE;
}

//-----------------------------------------------------------------------------//
// 구조체포인터 pStruct를 szTokenName포맷에 맞게 스크립트로 저장한다.
//-----------------------------------------------------------------------------//
BOOL CFastMemLoader::WriteScript( FILE *fp, void *pStruct, const char *typeName, int tab )
{
	TokenItor titor = m_DataStructureMap.find( typeName );
	if( m_DataStructureMap.end() == titor ) return FALSE; // error !!
	SDataStructure structureType = titor->second;

	WriteTab( fp, tab );
	fprintf( fp, "%s ", typeName );

	BYTE *psrc = (BYTE*)pStruct;
	BOOL bchild = FALSE;

	MemberItor it = structureType.pMember->begin();
	while( structureType.pMember->end() != it )
	{
		SMemberType type = *it++;
		switch( type.eType )
		{
		case TYPE_NULL:
			break;

		case TYPE_DATA:
		case TYPE_ARRAY:
			{
				if (GetBaseType((char*)type.typeName.c_str()))
				{
					WriteTextData( fp, psrc, &type );
					fprintf( fp, " " );
				}
				else
				{
					if( !bchild )
					{
						fprintf( fp, "\n" );
						WriteTab( fp, tab );
						fprintf( fp, "{" );
						fprintf( fp, "\n" );
						bchild = TRUE;
					}

					TokenItor titor = m_DataStructureMap.find( type.typeName );
					if( m_DataStructureMap.end() == titor )
						return FALSE; // error !!

					SDataStructure structType = titor->second;
					const int size = type.nSize / structType.size;
					for( int i=0; i < size; ++i )	
						if (!WriteScript( fp, (psrc+(structType.size*i)), (char*)type.typeName.c_str(), tab+1 ))
							return FALSE;
				}

				psrc += type.nSize;
			}
			break;

		case TYPE_STRUCT:
			{
				if( !bchild )
				{
					fprintf( fp, "\n" );
					WriteTab( fp, tab );
					fprintf( fp, "{" );
					fprintf( fp, "\n" );
					bchild = TRUE;
				}

				if (!WriteScript( fp, psrc, (char*)type.typeName.c_str(), tab+1 ))
					return FALSE;

				psrc += type.nSize;
			}
			break;

		case TYPE_POINTER:
			{
				if( !bchild )
				{
					fprintf( fp, "\n" );
					WriteTab( fp, tab );
					fprintf( fp, "{" );
					fprintf( fp, "\n" );
					bchild = TRUE;
				}

				// ** pointer type이 나오기전에 먼저 size값이 저장되어있다.
				int *psize = (int*)(psrc - sizeof(void*)); // pointersize 를 얻는다.
				BYTE* pPointerAdress = (BYTE*)*(DWORD*)psrc;

				TokenItor titor = m_DataStructureMap.find( type.typeName );
				if( m_DataStructureMap.end() == titor )
					return FALSE; // error !!

				SDataStructure structType = titor->second;
				for( int i=0; i < *psize; ++i )				
					if (!WriteScript( fp, (pPointerAdress+(structType.size*i)), (char*)type.typeName.c_str(), tab+1 ))
						return FALSE;

				psrc += type.nSize;
			}
			break;
		}
	}

	if( bchild )
	{
		WriteTab( fp, tab );
		fprintf( fp, "}" );
	}

	fprintf( fp, "\n" );

	return TRUE;
}


//-----------------------------------------------------------------------------//
// binary file로된 szFileName 파일을 typeName 타입으로 읽는다.
// 읽은 데이타는 BYTE* 리턴된다. (메모리제거는 직접해줘야함)
//-----------------------------------------------------------------------------//
BYTE* CFastMemLoader::ReadBin( const char *szFileName, const char *typeName )
{
	FILE *fp;
	fopen_s( &fp, szFileName, "rb" );
	if( !fp ) return NULL;

	struct _stat buf;
	_stat( szFileName, &buf );
	int filesize = buf.st_size;	// 64 bit 

	BYTE *pRead = new BYTE[ filesize];
	fread( pRead, 1, filesize, fp );
	fclose( fp );

	if( !ReadBinMem(pRead, typeName) )
	{
		delete[] pRead;
		pRead = NULL;
	}

	return pRead;
}


//-----------------------------------------------------------------------------//
// pReadMem 에 저장된 데이타를 typeName 타입으로 읽는다.
// pReadMem : Write() 함수로 쓰여진 file 정보
// typeName : 타입 이름
// **읽은 byte수를 리턴할려고 했지만 속도최적화를 이유로 뺐다.**
// 함수가 올바로 작동하지 못했다면 0을 리턴한다.
//-----------------------------------------------------------------------------//
int CFastMemLoader::ReadBinMem( BYTE *pReadMem, const char *typeName )
{
	TokenItor itor = m_DataStructureMap.find( typeName );
	if( m_DataStructureMap.end() == itor ) return 0;

	// pointer offset값 보정
	if (!ReadRec( (DWORD)pReadMem, pReadMem, itor->second.pMember ))
		return 0;

	return 1;
}


//-----------------------------------------------------------------------------//
// Member변수 size를 구한다.
//-----------------------------------------------------------------------------//
int CFastMemLoader::GetStructSize( MemberList *pList )
{
	int nSize = 0;
	MemberItor itor = pList->begin();
	while( itor != pList->end() )
	{
		nSize += itor->nSize;
		++itor;
	}
	return nSize;
}


//-----------------------------------------------------------------------------//
// Member중 pointer가 있는지 검사한다.
//-----------------------------------------------------------------------------//
BOOL CFastMemLoader::IsPointer( MemberList *pList )
{
	MemberItor itor = pList->begin();
	while( itor != pList->end() )
	{
		if( TYPE_POINTER == itor->eType )
			return TRUE;
		else if( TYPE_STRUCT == itor->eType )
		{
			TokenItor titor = m_DataStructureMap.find( itor->typeName );
			if( m_DataStructureMap.end() == titor ) return FALSE; // error !!
			if( titor->second.pointer )
				return TRUE;
		}
		++itor;
	}
	return FALSE;
}


//-----------------------------------------------------------------------------//
// token 추가
// szName: type name
// nSize : type의 byte수
// szParentName : parent type name (이경우 parent의 child로 추가된다.)
//-----------------------------------------------------------------------------//
BOOL CFastMemLoader::AddType( const char *szTypeName, int nSize, int nOffset, const char *szParentName ) // szParentName = NULL
{
	if( szParentName )
	{
		TokenItor it = m_DataStructureMap.find( szParentName );
		if( m_DataStructureMap.end() == it ) return FALSE; // error!!
		it->second.pMember->push_back( SMemberType(szTypeName, "__root", nSize, nSize, nOffset, TYPE_DATA) );
	}
	else
	{
		list<SMemberType> *pNewList = new list<SMemberType>;
		pNewList->push_back( SMemberType(szTypeName, "__root", nSize, nSize, 0, TYPE_DATA) );
		m_DataStructureMap.insert( DataStructureMap::value_type(szTypeName, SDataStructure(pNewList,nSize,FALSE)) );
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// pointer & filepos 찾기
// curFilePos : pStruct 데이타가 저장된 filepointer
// pStruct : File에 쓰여진 Data Pointer
// pMList : pStruct 의 멤버리스트
//-----------------------------------------------------------------------------//
BOOL CFastMemLoader::CollectPointerRec( DWORD curFilePos, BYTE *pStruct, MemberList *pMList, queue<SWsp> *pWspList )
{
	BOOL result = TRUE;
	DWORD filePos = curFilePos;
	MemberItor it = pMList->begin();
	while( it != pMList->end() )
	{
		SMemberType t = *it;

		if( TYPE_POINTER == it->eType )
		{
			// ** pointer type이 나오기전에 먼저 size값이 저장되어 있어야한다.
			int *pSize = (int*)(pStruct - sizeof(void*)); // pointersize 를 얻는다.
			if( 0 < *pSize )
			{
				DWORD dwAddress = *(DWORD*)pStruct;
				pWspList->push( SWsp(it->typeName, TYPE_POINTER, filePos, *pSize, (BYTE*)dwAddress ) );
			}
		}
		else if( TYPE_ARRAY == it->eType )
		{
			TokenItor titor = m_DataStructureMap.find( it->typeName );
			if( m_DataStructureMap.end() == titor ) 
			{ // error!!
				result = FALSE; 
				break; 
			} 

			// member변수중 pointer가 있을때만
			SDataStructure type = titor->second;
			if( type.pointer ) // Pointer를 포함하고 있다면 true
			{
				int nArraySize = it->nSize / type.size; // MemberSize는 배열개수만큼 곱해진 상태다.
				for( int i=0; i < nArraySize; ++i )
				{
					if (!CollectPointerRec( filePos + (type.size*i), (pStruct + (type.size*i)), type.pMember, pWspList ))
					{
						result = FALSE;
						break;
					}
				}
				if (!result)
					break; // 함수 종료
			}
		}
		else if( TYPE_STRUCT == it->eType )
		{
			TokenItor titor = m_DataStructureMap.find( it->typeName );
			if( m_DataStructureMap.end() == titor ) 
			{ // error!!
				result = FALSE;
				break;
			}

			// member변수중 pointer가 있을때만
			SDataStructure type = titor->second;
			if( type.pointer )
			{
				if (!CollectPointerRec( filePos, pStruct, type.pMember, pWspList ))
				{ // error!!
					result = FALSE;
					break;
				}
			}
		}

		filePos += it->nSize;
		pStruct += it->nSize;
		++it;
	}

	return result;
}


//-----------------------------------------------------------------------------//
// pointer offset값 보정
// member변수를 하나씩검사해서 type이 pointer일경우 dwOffset값만큼 보정한다.
//-----------------------------------------------------------------------------//
BOOL CFastMemLoader::ReadRec( DWORD dwOffset, BYTE *pStruct, MemberList *pMList )
{
	MemberItor mitor = pMList->begin();
	while( mitor != pMList->end() )
	{
		SMemberType st = *mitor;

		if( TYPE_POINTER == mitor->eType )
		{
			// size값이 0 보다 클때만 보정함
			int *pSize = (int*)(pStruct - sizeof(void*)); // pointersize 를 얻는다.
 			if( 0 < *pSize )
			{
				*(DWORD*)pStruct += dwOffset; // Poiner값 보정
				BYTE* pPointerAdress = (BYTE*)*(DWORD*)pStruct;

				TokenItor titor = m_DataStructureMap.find( mitor->typeName );
				if( m_DataStructureMap.end() == titor ) 
					return FALSE; // error !!

				SDataStructure type = titor->second;
				if( type.pointer )
				{
					for( int i=0; i < *pSize; ++i )
						ReadRec( dwOffset, (pPointerAdress+(type.size*i)), type.pMember );
				}
			}
		}
		else if( TYPE_ARRAY == mitor->eType )
		{
			TokenItor titor = m_DataStructureMap.find( mitor->typeName );
			if( m_DataStructureMap.end() == titor ) 
				return FALSE; // error !!

			SDataStructure type = titor->second;
			if( type.pointer )
			{
				int nArraySize = mitor->nSize / type.size;
				for( int i=0; i < nArraySize; ++i )
					ReadRec( dwOffset, pStruct+(type.size*i), type.pMember );
			}
		}
		else if( TYPE_STRUCT == mitor->eType )
		{
			TokenItor titor = m_DataStructureMap.find( mitor->typeName );
			if( m_DataStructureMap.end() == titor ) 
				return FALSE; // error !!

			SDataStructure type = titor->second;
			if( type.pointer )
				ReadRec( dwOffset, pStruct, type.pMember );
		}

		pStruct += mitor->nSize;
		++mitor;
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// szFileName 파일을 typeName 타입으로 파싱해서 메모리에 저장한다.
// szFileName : 스크립트 파일 경로
// typeName : 변환될 포맷
// szFileName 스크립트 파일을 열어 타입정보를 분석하면서 메모리로 변환 시킨다.
// 포인터가 포함된 타입일 경우 동적으로 메모리를 생성하기 때문에, 제거할 때 
// 포인터는 delete를 호출해 주어야 한다.
//-----------------------------------------------------------------------------//
BYTE* CFastMemLoader::ReadScript( const char *szFileName, const char *typeName )
{
	SDftDataParser parser;
	SScriptParseTree *pRoot = parser.OpenScriptFile( szFileName );
	if( !pRoot ) return NULL;
	SScriptParseTree *pParseTree = CParser<SScriptData>::FindToken( pRoot->pChild, SScriptData(typeName) );
	if( !pParseTree ) return NULL;

	TokenItor titor = m_DataStructureMap.find( typeName );
	if( m_DataStructureMap.end() == titor ) return NULL;

	SDataStructure type = titor->second;
	BYTE *pStruct = new BYTE[ type.size];
	ZeroMemory( pStruct, type.size );

	if (!ReadScriptRec( pStruct, pParseTree, &type ))
	{
		delete pStruct;
		pStruct = NULL;
	}

	return pStruct;
}


//-----------------------------------------------------------------------------//
// 스크립트 로딩 재귀
//-----------------------------------------------------------------------------//
BOOL CFastMemLoader::ReadScriptRec( BYTE *pStruct, SScriptParseTree *pParseTree, const SDataStructure *pDataStructure )
{
	if( !pParseTree || !pStruct || !pDataStructure ) 
		return FALSE;

	// 구조체 선언 라인 파싱
	std::list<SScriptDataKeyValue>::iterator it = pParseTree->t->keyValueList.begin();
	while (pParseTree->t->keyValueList.end() != it)
	{
		SScriptDataKeyValue keyValue = *it++;
		
		SMemberType type;
		if (!pDataStructure->FindMemberKey(keyValue.key, &type)) // key에 해당하는 멤버변수를 리턴한다.
		{
			ErrorReport( keyValue.key + " Not Found" );
			return FALSE;
		}

		if (TYPE_POINTER == type.eType) // pointer는 따로 처리해줘야한다.
		{
			const int nSize = (int)keyValue.array.size() * type.nElementSize;
			if (nSize > 0)
			{
				BYTE *pSubMem = new BYTE[ nSize];
				int offset = 0;
				std::list<std::string>::iterator ait = keyValue.array.begin();
				while (keyValue.array.end() != ait)
				{
					// 새메모리에 써야하기 때문에 nOffset은 0이 되어야 한다. 그래서 빼줌
					ParseData( pSubMem+offset-type.nOffset, type, ait->c_str() );
					offset += type.nElementSize;
					++ait;
				}
				// memory주소값 설정
				*(DWORD*)(pStruct+type.nOffset) = (DWORD)pSubMem;
				// pointer size 값 설정
				*(int*)(pStruct+type.nOffset-sizeof(void*)) = (int)keyValue.array.size();
			}
		}
		else
		{
			int offset = 0;
			std::list<std::string>::iterator ait = keyValue.array.begin();
			while (keyValue.array.end() != ait)
			{
				ParseData( pStruct+offset, type, ait->c_str() );
				offset += type.nElementSize;
				++ait;
			}
		}
	}

	// 자식 파싱
	pParseTree = pParseTree->pChild;
	MemberItor memberIt = pDataStructure->pMember->begin();
	while (pParseTree)
	{
 		TokenItor titor = m_DataStructureMap.find( pParseTree->t->name );
		if( m_DataStructureMap.end() == titor )
		{
			ErrorReport( pParseTree->t->name + " Not Found" );
			return FALSE; // error !!
		}
		const SDataStructure curDataStructure = titor->second;

		SMemberType type;
		memberIt = pDataStructure->FindMemberType(memberIt, pParseTree->t->name, &type);
		if (pDataStructure->pMember->end() == memberIt)
		{
			ErrorReport( pParseTree->t->name + " Not Found" );
			return FALSE;
		}

		switch (type.eType)
		{
		case TYPE_NULL:
		case TYPE_DATA:
			// 자식 노드에서는 데이타가 없으며, size데이타는 자동으로 설정되게 된다.
			pParseTree = pParseTree->pNext;
			break;

		case TYPE_STRUCT:
		case TYPE_ARRAY:
			{
				const int arraySize = type.nSize / type.nElementSize;
				for (int i=0; (i < arraySize) && pParseTree; ++i)
				{
					if (!ReadScriptRec( pStruct+type.nOffset+(type.nElementSize*i), pParseTree, &curDataStructure))
						return FALSE;
					pParseTree = pParseTree->pNext;
				}
			}
			break;

		case TYPE_POINTER:
			{
				const int nCount = CParser<SScriptData>::GetNodeCount( pParseTree, SScriptData(type.typeName.c_str()) );
				if( 0 < nCount )
				{
					BYTE *pSubMem = new BYTE[ nCount * type.nElementSize];
					ZeroMemory( pSubMem, nCount * type.nElementSize );

					for( int i=0; (i < nCount) && pParseTree; ++i )
					{
						if (!ReadScriptRec( pSubMem + (i*type.nElementSize), pParseTree, &curDataStructure ))
							return FALSE;
						pParseTree = pParseTree->pNext;
					}

					// memory주소값 설정
					*(DWORD*)(pStruct+type.nOffset) = (DWORD)pSubMem;
					// pointer size 값 설정
					*(int*)(pStruct+type.nOffset-sizeof(void*)) = nCount;
				}
			}
			break;
		}
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// token parse
//-----------------------------------------------------------------------------//
int CFastMemLoader::ParseData( BYTE *pStruct, const SMemberType &type, const char *szToken, SScriptParseTree *pParseTree, int idx )
{
	if (TYPE_NULL == type.eType || TYPE_STRUCT == type.eType)
		return 0;

	if( type.typeName == "int" )
	{
		*(int*)(pStruct+type.nOffset) = atoi( szToken );
	}
	else if( type.typeName == "float" )
	{
		*(float*)(pStruct+type.nOffset) = (float)atof( szToken );
	}
	else if( type.typeName == "short" )
	{
		*(short*)(pStruct+type.nOffset) = (short)atoi( szToken );
	}
	else if( type.typeName == "char" )
	{
		if( TYPE_ARRAY == type.eType )
		{
			strcpy_s( (char*)(pStruct+type.nOffset), strlen(szToken)+1, szToken );
		}
		else
		{
			*(char*)(pStruct+type.nOffset) = *szToken;
		}
	}
	else if( type.typeName == "size" )
	{
		// script를 읽을때 size값은 자동으로 설정된다.
	}
	else if( type.typeName == "dummy" )
	{
		// 아무일도 없다.
	}

	return ++idx;
}


//-----------------------------------------------------------------------------//
// 파일포인터 fp에 토큰타입에 맞게 텍스트형태로 저장한다.
//-----------------------------------------------------------------------------//
void CFastMemLoader::WriteTextData( FILE *fp, BYTE *pSrc, SMemberType *pTok )
{
	if( pTok->typeName == "int" )
	{
		if( TYPE_ARRAY == pTok->eType )
		{
			fprintf( fp, "%s=", pTok->valueName.c_str());
			int size = pTok->nSize;
			while( 0 < size )
			{
				fprintf( fp, "%d ", *(int*)pSrc );
				size -= sizeof(int);
				pSrc += sizeof(int);
			}
		}
		else
		{
			fprintf( fp, "%s=%d", pTok->valueName.c_str(), *(int*)pSrc );
		}
	}
	else if( pTok->typeName == "float" )
	{
		fprintf( fp, "%s=%f", pTok->valueName.c_str(), *(float*)pSrc );
	}
	else if( pTok->typeName == "short" )
	{
		fprintf( fp, "%s=%d", pTok->valueName.c_str(), *(short*)pSrc );
	}
	else if( pTok->typeName == "char" )
	{
		if( TYPE_ARRAY == pTok->eType )
		{
			fprintf( fp, "%s= \"%s\"", pTok->valueName.c_str(), (char*)pSrc );
		}
		else
		{
			fprintf( fp, "%s=%c", pTok->valueName.c_str(), *(char*)pSrc );
		}
	}
 	else if( pTok->typeName == "size" )
	{
//		fprintf( fp, "%d", *(int*)pSrc ); Size는 제외
	}
	else if( pTok->typeName == "dummy" )
	{
		// 아무일도 없다.
	}

}


//-----------------------------------------------------------------------------//
// Tab 개수만큼 이동
//-----------------------------------------------------------------------------//
void CFastMemLoader::WriteTab( FILE *fp, int tab )
{
	for( int t=0; t < tab; ++t )
		fprintf( fp, "\t" );
}
