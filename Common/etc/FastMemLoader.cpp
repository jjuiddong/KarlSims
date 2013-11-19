
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
	if (std::string::npos != n1) // ��Ʈ���� ����ǥ�� �ɷ������Ѵ�.
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

// SScriptData ���� Key�� ���Ǵ� ��Ʈ���̶�� true�� �����Ѵ�.
// ���ڷ� �����ϴ� ��Ʈ���̶�� Key�� �ν��Ѵ�.
bool IsKey( char *str )
{
	if (strlen(str) <= 0)
		return false;
	return (is_alpha(str[0])? true : false);
}

//------------------------------------------------------------------------
// ����Ÿ ����
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
			if (std::string(str) == "=") // key�� �Ҵ��ϴ� ��ū�϶��� �����Ѵ�.
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
// �� ����
//------------------------------------------------------------------------
BOOL CFastMemLoader::_SScriptData::operator == ( const _SScriptData& rhs ) const
{
	return name == rhs.name;
}

//------------------------------------------------------------------------
// ��������߿� key���� ���� ���Ÿ���� �����Ѵ�. (�ڽ��� �ڽ��� ã�� �ʴ´�.)
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
// ��������߿� type���� ���� ���Ÿ���� �����Ѵ�. (�ڽ��� �ڽ��� ã�� �ʴ´�.)
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
// ��������߿� type���� ���� ���Ÿ���� �����Ѵ�. (�ڽ��� �ڽ��� ã�� �ʴ´�.)
// MemberItor it ���� �˻縦 �Ѵ�.
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
// �⺻���ΰ� �̸� ����� ���´�.
//-----------------------------------------------------------------------------//
void CFastMemLoader::CreateDefaultType()
{
	for( int i=0; i < g_BaseTypeSize; ++i )
		AddType( g_BaseType[ i].name, g_BaseType[ i].size, 0 );
}


//-----------------------------------------------------------------------------//
// Token file �ε�
// opt: 0 �ߺ��� ����Ÿ�� �߻��ϸ� ������ �߻��Ѵ�.
//		1 �ߺ��� ����Ÿ�� �߻��ϸ� ���� �����.
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
// Token�� �����Ѵ�.
// opt: 0 �� m_DataStructureMap�� �ִ� Ÿ�� �ϰ�� �ٽ� �������� �ʴ´�.
//		1 �ߺ��� ����Ÿ�� �߻��ϸ� ���� �����.
//-----------------------------------------------------------------------------//
BOOL CFastMemLoader::LoadDataStructureFileRec( SScriptParseTree *pParseTree, MemberList *pMemberList, int opt ) // opt=0
{
	if( !pParseTree ) return FALSE;

	TokenItor itor = m_DataStructureMap.find( pParseTree->t->name );

	if( itor != m_DataStructureMap.end() ) // �̹� ��ϵ� ����Ÿ Ÿ���̶��,
	{
		// ���� ����Ÿ�� �ߺ��Ǿ� ����Ǿ� �ִٸ� opt���� ���� �帧�� �����ȴ�.
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
// Custom struct�� ��� Ÿ���� pMemberList�� �����Ѵ�.
//-----------------------------------------------------------------------------//
BOOL CFastMemLoader::LoadMemberList( SScriptParseTree *pParseTree, MemberList *pMemberList )
{
	SScriptParseTree *pNode = pParseTree;

	int offset = 0; // ��������� ����ü������ ��ġ�ص� �ּҸ� �����Ѵ�.
	while( pNode )
	{
		TokenItor itor = m_DataStructureMap.find( pNode->t->name );
		if (itor == m_DataStructureMap.end())
		{
			// �迭
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
			// ������
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
				// �������Ʈ�� ���� Ÿ���� �̹� ���ǵ� ���¿��� �Ѵ�. ���ٸ� Error
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
				// custome structure �ϰ��
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
// ����ü pStruct ������ szTokenName Ÿ������ szFileName ���Ͽ� ����.
// szFileName : Write�� �����̸�
// pStruct: ������ �����ϰ��ִ� �޸�
// szTokenName : ����ü Ÿ��
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
// ����ü pStruct ������ szTokenName Ÿ������ ���������� fp�� ����.
// fp : Dest���� ������
// pStruct: ������ �����ϰ��ִ� �޸�
// typeName : ����ü Ÿ��
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

		// file�� ����
		switch( wsp.eType )
		{
		case TYPE_ARRAY:
		case TYPE_DATA:
		case TYPE_STRUCT:
			{
				fwrite( wsp.pStruct, type.size, 1, fp );

				// struct���� pointer �� filepos ����
				if (!CollectPointerRec( ftell(fp) - type.size, (BYTE*)wsp.pStruct, type.pMember, &wspQueue ))
					return FALSE;
			}
			break;

		case TYPE_POINTER:
			{
				// ���� filePos�� �����Ѵ�.
				DWORD curPos = ftell( fp );
				DWORD pointerOffset = curPos - nFileStartPos;
				fseek( fp, wsp.nFilePos, SEEK_SET ); // pointer���� Update�ɰ����� �̵�
				fwrite( &pointerOffset, sizeof(DWORD), 1, fp );
				fseek( fp, curPos, SEEK_SET ); // �ٽ� ���ƿ�
				fwrite( wsp.pStruct, type.size, wsp.nPointerSize, fp ); // writeSize = type.size * pointerSize

				// struct���� pointer �� filepos ����
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
// ����ü������ pStruct�� szTokenName���˿� �°� ��ũ��Ʈ�� �����Ѵ�.
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

				// ** pointer type�� ���������� ���� size���� ����Ǿ��ִ�.
				int *psize = (int*)(psrc - sizeof(void*)); // pointersize �� ��´�.
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
// binary file�ε� szFileName ������ typeName Ÿ������ �д´�.
// ���� ����Ÿ�� BYTE* ���ϵȴ�. (�޸����Ŵ� �����������)
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
// pReadMem �� ����� ����Ÿ�� typeName Ÿ������ �д´�.
// pReadMem : Write() �Լ��� ������ file ����
// typeName : Ÿ�� �̸�
// **���� byte���� �����ҷ��� ������ �ӵ�����ȭ�� ������ ����.**
// �Լ��� �ùٷ� �۵����� ���ߴٸ� 0�� �����Ѵ�.
//-----------------------------------------------------------------------------//
int CFastMemLoader::ReadBinMem( BYTE *pReadMem, const char *typeName )
{
	TokenItor itor = m_DataStructureMap.find( typeName );
	if( m_DataStructureMap.end() == itor ) return 0;

	// pointer offset�� ����
	if (!ReadRec( (DWORD)pReadMem, pReadMem, itor->second.pMember ))
		return 0;

	return 1;
}


//-----------------------------------------------------------------------------//
// Member���� size�� ���Ѵ�.
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
// Member�� pointer�� �ִ��� �˻��Ѵ�.
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
// token �߰�
// szName: type name
// nSize : type�� byte��
// szParentName : parent type name (�̰�� parent�� child�� �߰��ȴ�.)
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
// pointer & filepos ã��
// curFilePos : pStruct ����Ÿ�� ����� filepointer
// pStruct : File�� ������ Data Pointer
// pMList : pStruct �� �������Ʈ
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
			// ** pointer type�� ���������� ���� size���� ����Ǿ� �־���Ѵ�.
			int *pSize = (int*)(pStruct - sizeof(void*)); // pointersize �� ��´�.
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

			// member������ pointer�� ��������
			SDataStructure type = titor->second;
			if( type.pointer ) // Pointer�� �����ϰ� �ִٸ� true
			{
				int nArraySize = it->nSize / type.size; // MemberSize�� �迭������ŭ ������ ���´�.
				for( int i=0; i < nArraySize; ++i )
				{
					if (!CollectPointerRec( filePos + (type.size*i), (pStruct + (type.size*i)), type.pMember, pWspList ))
					{
						result = FALSE;
						break;
					}
				}
				if (!result)
					break; // �Լ� ����
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

			// member������ pointer�� ��������
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
// pointer offset�� ����
// member������ �ϳ����˻��ؼ� type�� pointer�ϰ�� dwOffset����ŭ �����Ѵ�.
//-----------------------------------------------------------------------------//
BOOL CFastMemLoader::ReadRec( DWORD dwOffset, BYTE *pStruct, MemberList *pMList )
{
	MemberItor mitor = pMList->begin();
	while( mitor != pMList->end() )
	{
		SMemberType st = *mitor;

		if( TYPE_POINTER == mitor->eType )
		{
			// size���� 0 ���� Ŭ���� ������
			int *pSize = (int*)(pStruct - sizeof(void*)); // pointersize �� ��´�.
 			if( 0 < *pSize )
			{
				*(DWORD*)pStruct += dwOffset; // Poiner�� ����
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
// szFileName ������ typeName Ÿ������ �Ľ��ؼ� �޸𸮿� �����Ѵ�.
// szFileName : ��ũ��Ʈ ���� ���
// typeName : ��ȯ�� ����
// szFileName ��ũ��Ʈ ������ ���� Ÿ�������� �м��ϸ鼭 �޸𸮷� ��ȯ ��Ų��.
// �����Ͱ� ���Ե� Ÿ���� ��� �������� �޸𸮸� �����ϱ� ������, ������ �� 
// �����ʹ� delete�� ȣ���� �־�� �Ѵ�.
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
// ��ũ��Ʈ �ε� ���
//-----------------------------------------------------------------------------//
BOOL CFastMemLoader::ReadScriptRec( BYTE *pStruct, SScriptParseTree *pParseTree, const SDataStructure *pDataStructure )
{
	if( !pParseTree || !pStruct || !pDataStructure ) 
		return FALSE;

	// ����ü ���� ���� �Ľ�
	std::list<SScriptDataKeyValue>::iterator it = pParseTree->t->keyValueList.begin();
	while (pParseTree->t->keyValueList.end() != it)
	{
		SScriptDataKeyValue keyValue = *it++;
		
		SMemberType type;
		if (!pDataStructure->FindMemberKey(keyValue.key, &type)) // key�� �ش��ϴ� ��������� �����Ѵ�.
		{
			ErrorReport( keyValue.key + " Not Found" );
			return FALSE;
		}

		if (TYPE_POINTER == type.eType) // pointer�� ���� ó��������Ѵ�.
		{
			const int nSize = keyValue.array.size() * type.nElementSize;
			if (nSize > 0)
			{
				BYTE *pSubMem = new BYTE[ nSize];
				int offset = 0;
				std::list<std::string>::iterator ait = keyValue.array.begin();
				while (keyValue.array.end() != ait)
				{
					// ���޸𸮿� ����ϱ� ������ nOffset�� 0�� �Ǿ�� �Ѵ�. �׷��� ����
					ParseData( pSubMem+offset-type.nOffset, type, ait->c_str() );
					offset += type.nElementSize;
					++ait;
				}
				// memory�ּҰ� ����
				*(DWORD*)(pStruct+type.nOffset) = (DWORD)pSubMem;
				// pointer size �� ����
				*(int*)(pStruct+type.nOffset-sizeof(void*)) = keyValue.array.size();
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

	// �ڽ� �Ľ�
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
			// �ڽ� ��忡���� ����Ÿ�� ������, size����Ÿ�� �ڵ����� �����ǰ� �ȴ�.
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

					// memory�ּҰ� ����
					*(DWORD*)(pStruct+type.nOffset) = (DWORD)pSubMem;
					// pointer size �� ����
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
		// script�� ������ size���� �ڵ����� �����ȴ�.
	}
	else if( type.typeName == "dummy" )
	{
		// �ƹ��ϵ� ����.
	}

	return ++idx;
}


//-----------------------------------------------------------------------------//
// ���������� fp�� ��ūŸ�Կ� �°� �ؽ�Ʈ���·� �����Ѵ�.
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
//		fprintf( fp, "%d", *(int*)pSrc ); Size�� ����
	}
	else if( pTok->typeName == "dummy" )
	{
		// �ƹ��ϵ� ����.
	}

}


//-----------------------------------------------------------------------------//
// Tab ������ŭ �̵�
//-----------------------------------------------------------------------------//
void CFastMemLoader::WriteTab( FILE *fp, int tab )
{
	for( int t=0; t < tab; ++t )
		fprintf( fp, "\t" );
}
