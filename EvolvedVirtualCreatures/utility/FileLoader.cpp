
#include "stdafx.h"
#include "fileloader.h"
#include <sys/stat.h>
#include "Shlwapi.h"
#include "../EvolvedVirtualCreatures.h"


#pragma comment(lib, "shlwapi.lib")

const float FRAME = (1000.f / 30.f);

CFileLoader::CFileLoader() :
	m_pDevice(NULL)
{

}

CFileLoader::~CFileLoader()
{
	Clear();
}


//-----------------------------------------------------------------------------//
// 클래스 초기화
// script를 로드하기위한 "def.txt"을 로드한다.
//-----------------------------------------------------------------------------//
void CFileLoader::Init( IDirect3DDevice9* pDevice ) 
{
	m_pDevice = pDevice; 

	static BOOL bLoaded = FALSE;
	if( !bLoaded )
	{
		//m_MemLoader.LoadDataStructureFile( "script//def.txt" );
		m_MemLoader.LoadDataStructureFile( "../../media/SampleRenderer/4/script/def.txt" );
		bLoaded = TRUE;
	}
}


//-----------------------------------------------------------------------------//
// load mesh, bone, material file
//-----------------------------------------------------------------------------//
SBMMLoader* CFileLoader::LoadModel( const string &szFileName )
{
	string fileName = "../../media/SampleRenderer/4/";
	fileName += szFileName;

	auto itor = m_ModelMap.find( fileName );
	if (m_ModelMap.end() != itor)
	{
		SBMMLoader *p = (SBMMLoader*)itor->second.pItem;
		// Skinning Animation의 경우 모델마다 VertexBuffer를 생성해야 한다.
		//if (ANI_SKIN == p->type)
		//	CreateVertexBuffer( (SBMMLoader*)itor->second.pItem );
		return (SBMMLoader*)itor->second.pItem;
	}


	// binary파일이 있다면 그것을 로드한다.
	// binary파일은 같은 파일명에서 확장자만(.bmm) 바뀐다. 
	char binfile[ MAX_PATH];
	strcpy( binfile, fileName.c_str() );
	strcpy( &binfile[ strlen(binfile)-3], "bmm" );

	SMapItem item;
	ZeroMemory( &item, sizeof(item) );
	if (PathFileExistsA(binfile))
	{
		// binary파일이 있다면 그것을 로드한다.
		item.eType = MT_LINEAR;
		item.pItem = (BYTE*)LoadBMM_Bin( binfile );
		if( !item.pItem ) return FALSE;
		m_ModelMap.insert( ModelMap::value_type(fileName,item) );
	}
	else
	{
		// binary파일이 없다면, binary파일을 생성한다.
		item.eType = MT_TREE;
		item.pItem = (BYTE*)LoadBMM_GPJ(fileName);
		if( !item.pItem ) return FALSE;
		m_ModelMap.insert( ModelMap::value_type(fileName,item) );
//		WriteScripttoBinary_Model( (SBMMLoader*)item.pItem, binfile );
	}

	//ModifyTextureFilename( &((SBMMLoader*)item.pItem)->mtrl );

	// VertexBuffer, IndexBuffer를 생성한다.
	//CreateVertexBuffer( (SBMMLoader*)item.pItem );
	//CreateIndexBuffer( (SBMMLoader*)item.pItem );

	return (SBMMLoader*)item.pItem;
}


//-----------------------------------------------------------------------------//
// load script mesh file
// script에 포함된 내용: 
//	material, mesh info(vertex,index,normal,texture,physiq), bone, animation 
//-----------------------------------------------------------------------------//
SBMMLoader* CFileLoader::LoadBMM_GPJ( const string &szFileName )
{
	SBMMLoader *pLoader = (SBMMLoader*)m_MemLoader.ReadScript( szFileName.c_str(), "BMMFORM" );
	if( !pLoader ) return NULL;
	for( int i=0; i < pLoader->m.size; ++i )
	{
		if( -1 == pLoader->m.pMesh[ i].mtrlid )
			pLoader->m.pMesh[ i].pMtrl = NULL;
		else
			pLoader->m.pMesh[ i].pMtrl = &pLoader->mtrl.pMtrl[ pLoader->m.pMesh[ i].mtrlid];
	}

	return pLoader;
}


//-----------------------------------------------------------------------------//
// load binary mesh file
//-----------------------------------------------------------------------------//
SBMMLoader* CFileLoader::LoadBMM_Bin( char *szFileName )
{
	// file size만큼 메모리에 담는다. (header제외)
	FILE *fp = fopen( szFileName, "rb" );
	char temp[3];
	fread( temp, sizeof(char), 3, fp );
	struct _stat buf;
	_stat( szFileName, &buf );
	int filesize = buf.st_size-3; // 64 bit (header제외)
	BYTE *pRead = new BYTE[ filesize];
	fread( pRead, 1, filesize, fp );
	fclose( fp );

	// data parsing
	m_MemLoader.ReadBinMem( pRead, "BMMFORM" );
	SBMMLoader *pLoader = (SBMMLoader*)pRead;

	// material 설정
	for( int i=0; i < pLoader->m.size; ++i )
	{
		if( -1 == pLoader->m.pMesh[ i].mtrlid )
			pLoader->m.pMesh[ i].pMtrl = NULL;
		else
			pLoader->m.pMesh[ i].pMtrl = &pLoader->mtrl.pMtrl[ pLoader->m.pMesh[ i].mtrlid];
	}

	return pLoader;
}


//-----------------------------------------------------------------------------//
// load animation file
//-----------------------------------------------------------------------------//
SAniLoader* CFileLoader::LoadAnimation( char *szFileName )
{
	auto itor = m_KeyMap.find( szFileName );
	if( m_KeyMap.end() != itor ) 
		return (SAniLoader*)itor->second.pItem;

	// binary파일이 있다면 그것을 로드한다.
	// binary파일은 같은 파일명에서 확장자만(.a) 바뀐다. 
	char binfile[ MAX_PATH];
	strcpy( binfile, szFileName );
	strcpy( &binfile[ strlen(binfile)-3], "a" );

	SMapItem item;
	ZeroMemory( &item, sizeof(item) );
	if (PathFileExistsA(binfile))
	{
		// binary파일이 있다면 그것을 로드한다.
		item.eType = MT_LINEAR;
		item.pItem = (BYTE*)LoadAni_Bin( binfile );
		if( item.pItem ) m_KeyMap.insert( KeyMap::value_type(szFileName,item) );
	}
	else
	{
		char srcfilename[ MAX_PATH];
		strcpy( srcfilename, szFileName );
		strcpy( &srcfilename[ strlen(srcfilename)-4], "_.txt" );

		if( !ConvertAniListToAniform(szFileName, srcfilename) )
		{
			// 함수가 실패했다면 원본파일은 szFileName이 되어야 한다.
			strcpy( srcfilename, szFileName );
		}

		// binary파일이 없다면, binary파일을 생성한다.
		item.eType = MT_TREE;
		item.pItem = (BYTE*)LoadAni_GPJ( srcfilename );
		if( !item.pItem ) return NULL; 
		m_KeyMap.insert( KeyMap::value_type(szFileName,item) );
		WriteScript2BinaryAni( (SAniLoader*)item.pItem, binfile );
	}

	return (SAniLoader*)item.pItem;
}


//-----------------------------------------------------------------------------//
// load script animation file
//-----------------------------------------------------------------------------//
SAniLoader* CFileLoader::LoadAni_GPJ( char *szFileName )
{
	SAniLoader *pLoader = (SAniLoader*)m_MemLoader.ReadScript( szFileName, "ANIFORM" );
	
	if( !pLoader ) return NULL;

	int size = pLoader->size;
	for( int m=0; m < size; ++m )
	{
		int grpsize = pLoader->pAniGroup[ m].size;
		for( int i=0; i < grpsize; ++i )
		{
			// Animation Frame 수정
			// 3DMax 에서는 1초에 30 frame 을 기준으로 작업하기 때문에
			// 1초에 1000frame을 사용하는 게임프로그램에 맞게 수정되어야 한다.
			int k=0;
			pLoader->pAniGroup[ m].pKey[ i].start *= FRAME;
			pLoader->pAniGroup[ m].pKey[ i].end *= FRAME;
			int rsize = pLoader->pAniGroup[ m].pKey[ i].rot.size;
			for( k=0; k < rsize; ++k )
				pLoader->pAniGroup[ m].pKey[ i].rot.r[ k].frame *= FRAME;

			int ssize = pLoader->pAniGroup[ m].pKey[ i].scale.size;
			for( k=0; k < ssize; ++k )
				pLoader->pAniGroup[ m].pKey[ i].scale.s[ k].frame *= FRAME; 

			int psize = pLoader->pAniGroup[ m].pKey[ i].pos.size;
			for( k=0; k < psize; ++k )
				pLoader->pAniGroup[ m].pKey[ i].pos.p[ k].frame *= FRAME; 
		}
	}

	return pLoader;
}


//-----------------------------------------------------------------------------//
// load binary animation file
//-----------------------------------------------------------------------------//
SKeyGroupLoader* CFileLoader::LoadAni_Bin( char *szFileName )
{
	// file size만큼 메모리에 담는다. (header제외)
	FILE *fp = fopen( szFileName, "rb" );
	char temp[3];
	fread( temp, sizeof(char), 3, fp );
	struct _stat buf;
	_stat( szFileName, &buf );
	int filesize = buf.st_size-3; // 64 bit (header제외)
	BYTE *pRead = new BYTE[ filesize];
	fread( pRead, 1, filesize, fp );
	fclose( fp );

	// data parsing
	m_MemLoader.ReadBinMem( pRead, "ANIFORM" );
	SKeyGroupLoader *pLoader = (SKeyGroupLoader*)pRead;

	return pLoader;
}


//-----------------------------------------------------------------------------//
// bmm script를 bmm binaryfile로 만든다. (확장자 .bmm)
// szSrcFileName : bmm script file name
// szDstFileName : 생성될 filename
//-----------------------------------------------------------------------------//
BOOL CFileLoader::WriteScript2BinaryModel( char *szSrcFileName, char *szDstFileName )
{
	SBMMLoader *pLoader = LoadModel( szSrcFileName );
	BOOL bResult = WriteScript2BinaryModel( pLoader, szDstFileName );
	return bResult;
}


//-----------------------------------------------------------------------------//
// ANILIST 포맷으로 된 ani파일을 ANIFORM 포맷으로 수정해서 저장한다.
// 즉 여러개의 ANIFORM 파일을 한 파일로 모으는 기능을 한다.
// return value : TRUE = ANILIST 포맷으로 된 ani파일을 변환 성공
//			    : FALSE = 이미 ANIFORM 포맷으로 된 파일이여서 변환 할 필요 없음
//					      혹은 파일이 없어서 변환 불가능 일때
//-----------------------------------------------------------------------------//
BOOL CFileLoader::ConvertAniListToAniform( char *szSrcFileName, char *szDstFileName )
{
	SAniList *pani = (SAniList*)m_MemLoader.ReadScript( szSrcFileName, "ANILIST" );
	if( !pani ) return FALSE;

	FILE *fp = fopen( szDstFileName, "w" );
	if( !fp ) return FALSE;
	fprintf( fp, "GPJ\n" );
	fprintf( fp, "ANIFORM %d\n", pani->size );
	fprintf( fp, "{\n" );

	int cnt = 0;
	for( int i=0; i < pani->size; ++i )
	{
		SAniLoader *ploader = (SAniLoader*)m_MemLoader.ReadScript( pani->pAniInfo[ i].path, "ANIFORM" );
		if( 0 < ploader->size )
		{
			ploader->pAniGroup[ 0].id = cnt++;
			strcpy( ploader->pAniGroup[ 0].name, pani->pAniInfo[ i].name );
			m_MemLoader.WriteScript( fp, ploader->pAniGroup, "ANIMATIONGROUP", 1 );
		}
	}

	fprintf( fp, "}\n" );
	fclose(fp);
	return TRUE;
}


//-----------------------------------------------------------------------------//
// animation script를 binaryfile로 만든다. (확장자 .a)
// szSrcFileName : ani script file name
// szDstFileName : 생성될 filename
//-----------------------------------------------------------------------------//
BOOL CFileLoader::WriteScript2BinaryAni( char *szSrcFileName, char *szDstFileName )
{
	SAniLoader *pLoader = LoadAnimation( szSrcFileName );
	BOOL bResult = WriteScript2BinaryAni( pLoader, szDstFileName );
	return bResult;
}


//-----------------------------------------------------------------------------//
// 구조체 memory에 저장된 데이타를 file에 쓴다.
// pModelLoader : source 구조체 메모리
// szDstFileName : 생성될 filename
//-----------------------------------------------------------------------------//
BOOL CFileLoader::WriteScript2BinaryModel( SBMMLoader *pModelLoader, char *szDstFileName )
{
	FILE *fp = fopen( szDstFileName, "wb" );
	if( !fp ) return FALSE;

	// header 작성
	char szHeader[] = { "bmm" };
	fwrite( szHeader, strlen(szHeader), 1, fp );

	// data 쓰기
	BOOL bResult = m_MemLoader.WriteBin( fp, pModelLoader, "BMMFORM" );

	fclose( fp );

	return bResult;
}


//-----------------------------------------------------------------------------//
// 구조체 memory에 저장된 데이타를 file에 쓴다.
// pAniLoader : source 구조체 메모리
// szDstFileName : 생성될 filename
//-----------------------------------------------------------------------------//
BOOL CFileLoader::WriteScript2BinaryAni( SAniLoader *pAniLoader, char *szDstFileName )
{
	FILE *fp = fopen( szDstFileName, "wb" );
	if( !fp ) return FALSE;

	// header 작성
	char szHeader[] = {'a', 0, 0 };
	fwrite( szHeader, sizeof(szHeader), 1, fp );

	// data 쓰기
	BOOL bResult = m_MemLoader.WriteBin( fp, pAniLoader, "ANIFORM" );

	fclose( fp );

	return bResult;
}


//-----------------------------------------------------------------------------//
// texture filename을 바꿔준다.
//-----------------------------------------------------------------------------//
void CFileLoader::ModifyTextureFilename( SMaterialGroupLoader *pLoader )
{
	for( int i=0; i < pLoader->size; ++i )
	{
		if( pLoader->pMtrl[ i].szFileName[0] )
		{
			// filename 만 뽑아낸다.
			char szTemp[ 128];
			int len = strlen( pLoader->pMtrl[ i].szFileName );
			int k = len-1;
			for( ; k >= 0; --k )
			{
				if( '/' == pLoader->pMtrl[ i].szFileName[ k] || 
					'\\' == pLoader->pMtrl[ i].szFileName[ k] )
				{
					k += 1;
					break;
				}
			}
			strcpy( szTemp, &pLoader->pMtrl[ i].szFileName[ k] );
			ZeroMemory( pLoader->pMtrl[ i].szFileName, sizeof(pLoader->pMtrl[ i].szFileName) );
			sprintf( pLoader->pMtrl[ i].szFileName, "image//%s", szTemp );
		}
	}
}


//-----------------------------------------------------------------------------//
// DirectX IndexBuffer를 생성한다.
// Rigid, KeyAnimation, Skinning Animation 모두 Index Buffer는 한번만 생성하면 된다.
//-----------------------------------------------------------------------------//
BOOL CFileLoader::CreateIndexBuffer( SBMMLoader *pLoader )
{
	RETV(!m_pDevice,FALSE);

	int size = pLoader->m.size;
	for( int i=0; i < size; ++i )
	{
		SMeshLoader *pm = &pLoader->m.pMesh[ i];
		if( 0 < pm->i.size )
		{
			if( FAILED(m_pDevice->CreateIndexBuffer(sizeof(SVector3s)*pm->i.size, 0, D3DFMT_INDEX16, 
													D3DPOOL_MANAGED, &pm->pidxbuff, NULL)) )
				continue;

			SVector3s *i;
			pm->pidxbuff->Lock( 0, pm->i.size, (void**)&i, 0 );
			memcpy( i, pm->i.pI, pm->i.size * sizeof(SVector3s) );
			pm->pidxbuff->Unlock();
		}
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// DirectX용 VertexBuffer 를 생성한다.
// Rigid, KeyAnimation 일경우는 한번만 Vertex Buffer를 생성하면 되지만,
// Skinning Animation은 모델갯수(instance)만큼 생성되어야 한다.
//-----------------------------------------------------------------------------//
BOOL CFileLoader::CreateVertexBuffer( SBMMLoader *pLoader )
{
	RETV(!m_pDevice,FALSE);

	int size = pLoader->m.size;
	for( int i=0; i < size; ++i )
	{
		SMeshLoader *pm = &pLoader->m.pMesh[ i];
		int fvf, stride, vsize;
		BYTE *pv;

		if( 0 < pm->vn.size ) // FVF = D3DFVF_XYZ | D3DFVF_NORMAL
		{
			fvf = SVtxNorm::FVF;
			stride = sizeof( SVtxNorm );
			vsize = pm->vn.size;
			pv = (BYTE*)pm->vn.pV;
		}
		else // FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1
		{
			fvf = SVtxNormTex::FVF;
			stride = sizeof( SVtxNormTex );
			vsize = pm->vnt.size;
			pv = (BYTE*)pm->vnt.pV;
		}

		if( 0 >= vsize ) continue;

		if( FAILED(m_pDevice->CreateVertexBuffer(stride*vsize, 0, fvf, D3DPOOL_MANAGED, &pm->pvtxbuff, NULL)) )
			return FALSE;

		BYTE *v;
		pm->pvtxbuff->Lock( 0, 0, (void**)&v, 0 );
		memcpy( v, pv, vsize * stride );
		pm->pvtxbuff->Unlock();
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// Texture 파일을 로드한다.
// 만약 이미 로드한 texture라면 기존데이타를 리턴한다.
//-----------------------------------------------------------------------------//
IDirect3DTexture9* CFileLoader::LoadTexture( char *szFileName )
{
	RETV(!m_pDevice,NULL);

	auto itor = m_TextureMap.find( szFileName );
	if( m_TextureMap.end() != itor )
		return itor->second;

	IDirect3DTexture9 *pTex = NULL;
	//D3DXCreateTextureFromFileA( s_pDevice, szFileName, &pTex );

	m_TextureMap.insert( TextureMap::value_type(szFileName,pTex) );

	return pTex;
}


//-----------------------------------------------------------------------------//
// 콤보데이타 로드
//-----------------------------------------------------------------------------//
//int CFileLoader::LoadCombo( char *pFileName, SCombo *pReval[] )
//{
//	// 바이너리 파일이 있다면 그것을 로드한다.
//	// 바이너리 파일은 같은 파일명에서 확장자만(.b) 바뀐다.
//	// 바이너리 파일이 없다면 컴파일해서 바이너리파일로 저장한다.
//	char binfile[ MAX_PATH];
//	strcpy( binfile, pFileName );
//	strcpy( &binfile[ strlen(binfile)-3], "b" );
//
////	if( !PathFileExists(binfile) )
//	{
//		char buf[ 128];
//		sprintf( buf, "script\\comboparser.exe 0 < %s", pFileName );
//		system( buf );
//	}
//
//	int size = Read_ComboData( binfile, pReval );
//
//	// Animation Frame 수정
//	// 3DMax 에서는 1초에 30 frame 을 기준으로 작업하기 때문에
//	// 1초에 1000frame을 사용하는 게임프로그램에 맞게 수정되어야 한다.
//	for( int i=0; i < size; ++i )
//	{
//		queue<SActInfo*> que;
//		que.push( &pReval[ i]->act );
//		while( !que.empty() )
//		{
//			SActInfo *pact = que.front(); que.pop();
//			pact->anitime *= FRAME;
//			pact->stime *= FRAME;
//			pact->etime *= FRAME;
//			pact->col_stime *= FRAME;
//			pact->col_etime *= FRAME;
//			for( int k=0; k < pact->nextcount; ++k )
//				que.push( pact->next[ k] );
//		}
//	}
//
//	return size;
//}


//-----------------------------------------------------------------------------//
// UserInterface 정보를 읽어드린다.
//-----------------------------------------------------------------------------//
SScene* CFileLoader::LoadUI( char *pFileName )
{
	// binary파일이 있다면 그것을 로드한다.
	// binary파일은 같은 파일명에서 확장자만(.bmm) 바뀐다. 
	char binfile[ MAX_PATH];
	strcpy( binfile, pFileName );
	strcpy( &binfile[ strlen(binfile)-3], "ui" );

	SScene *pscene = NULL;
//	if( PathFileExists(binfile) )
	{
//		pscene = (SScene*)s_MemLoader.ReadBin( binfile, "SCENE" );
	}
//	else
	{
		pscene = (SScene*)m_MemLoader.ReadScript( pFileName, "SCENE" );
		m_MemLoader.WriteBin( binfile, pscene, "SCENE" );
	}

	return pscene;
}


//-----------------------------------------------------------------------------//
// 저장된데이타 모두 삭제한다.
//-----------------------------------------------------------------------------//
void CFileLoader::Clear()
{
	auto mitor = m_ModelMap.begin();
	while( m_ModelMap.end() != mitor )
	{
		SMapItem item = mitor->second;
		if( MT_TREE == item.eType )
		{
			SBMMLoader *pLoader = (SBMMLoader*)item.pItem;
			for( int i=0; i < pLoader->m.size; ++i )
			{
				if( pLoader->m.pMesh[ i].vn.pV )
					delete[] (BYTE*)pLoader->m.pMesh[ i].vn.pV;
				if( pLoader->m.pMesh[ i].vnt.pV )
					delete[] (BYTE*)pLoader->m.pMesh[ i].vnt.pV;
				if( pLoader->m.pMesh[ i].i.pI )
					delete[] (BYTE*)pLoader->m.pMesh[ i].i.pI;
				for( int k=0; k < pLoader->m.pMesh[ i].physiq.size; ++k )
					delete[] (BYTE*)pLoader->m.pMesh[ i].physiq.p[ k].w;
				if( pLoader->m.pMesh[ i].physiq.p )
					delete[] (BYTE*)pLoader->m.pMesh[ i].physiq.p;

				// Skinnin Animation의 경우 CAniMesh에서 소거된다.
				if( ANI_SKIN != pLoader->type )
					DX_SAFE_RELEASE( pLoader->m.pMesh[ i].pvtxbuff );
				DX_SAFE_RELEASE( pLoader->m.pMesh[ i].pidxbuff );
			}
			if( pLoader->m.pMesh )
				delete[] (BYTE*)pLoader->m.pMesh;
			if( pLoader->b.pBone )
				delete[] (BYTE*)pLoader->b.pBone;
			if( pLoader->mtrl.pMtrl )
				delete[] (BYTE*)pLoader->mtrl.pMtrl;
			delete[] (BYTE*)pLoader;
		}
		else if( MT_LINEAR == item.eType )
		{
			SBMMLoader *pLoader = (SBMMLoader*)item.pItem;
			for( int i=0; i < pLoader->m.size; ++i )
			{
				// Skinnin Animation의 경우 CAniMesh에서 소거된다.
				if( ANI_SKIN != pLoader->type )
					DX_SAFE_RELEASE( pLoader->m.pMesh[ i].pvtxbuff );
				DX_SAFE_RELEASE( pLoader->m.pMesh[ i].pidxbuff );
			}
			delete[] (BYTE*)pLoader;
		}

		++mitor;
	}
	m_ModelMap.clear();

	auto kitor = m_KeyMap.begin();
	while( m_KeyMap.end() != kitor )
	{
		SMapItem item = kitor->second;
		if( MT_TREE == item.eType )
		{
			SAniLoader *pLoader = (SAniLoader*)item.pItem;
			for( int m=0; m < pLoader->size; ++m )
			{
				for( int i=0; i < pLoader->pAniGroup[ m].size; ++i )
				{
					if( pLoader->pAniGroup[ m].pKey[i].pos.p )
						delete[] (BYTE*)pLoader->pAniGroup[ m].pKey[i].pos.p;
					if( pLoader->pAniGroup[ m].pKey[i].rot.r )
						delete[] (BYTE*)pLoader->pAniGroup[ m].pKey[i].rot.r;
					if( pLoader->pAniGroup[ m].pKey[i].scale.s )
						delete[] (BYTE*)pLoader->pAniGroup[ m].pKey[i].scale.s;
				}
				delete[] (BYTE*)pLoader->pAniGroup[ m].pKey;
			}
			delete[] (BYTE*)pLoader->pAniGroup;
			delete (BYTE*)pLoader;
		}
		else
		{
			delete[] (BYTE*)item.pItem;
		}

		++kitor;
	}
	m_KeyMap.clear();

	auto titor = m_TextureMap.begin();
	while( m_TextureMap.end() != titor )
	{
		IDirect3DTexture9 *pTex = titor->second;
		if( pTex ) pTex->Release();
		++titor;
	}
	m_TextureMap.clear();

}
