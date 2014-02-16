//-----------------------------------------------------------------------------//
// 2005-10-04  programer: jaejung ┏(⊙д⊙)┛
// 
// 2007-12-24 Chrismas Eve 에 코딩하다.
//		- Model, Animation 로딩시 자동으로 바이너리파일 저장, 로딩가능하게 구현함
//
// 2014-02-16 코드 정리
//
// CFileLoader
//-----------------------------------------------------------------------------//
#pragma once


#include "d3dx9.h"
#include "structdef.h"

class CFileLoader : public common::CSingleton<CFileLoader>
{
public:
	CFileLoader();
	virtual ~CFileLoader();

	void Init( IDirect3DDevice9* pDevice );
	SBMMLoader* LoadModel( const string &szFileName );
	SAniLoader* LoadAnimation( char *szFileName );
	IDirect3DTexture9* LoadTexture( char *szFileName );
	//static int LoadCombo( char *pFileName, SCombo *pReval[] );
	SScene* LoadUI( char *pFileName );
	BOOL WriteScript2BinaryModel( char *szSrcFileName, char *szDstFileName );
	BOOL WriteScript2BinaryAni( char *szSrcFileName, char *szDstFileName );
	BOOL WriteScript2BinaryModel( SBMMLoader *pModelLoader, char *szDstFileName );
	BOOL WriteScript2BinaryAni( SAniLoader *pAniLoader, char *szDstFileName );
	void Clear();


protected:
	SBMMLoader* LoadBMM_GPJ( const string &szFileName );
	SBMMLoader* LoadBMM_Bin( char *szFileName );
	SAniLoader* LoadAni_GPJ( char *szFileName );
	SKeyGroupLoader* LoadAni_Bin( char *szFileName );
	BOOL ConvertAniListToAniform( char *szSrcFileName, char *szDstFileName );
	void ModifyTextureFilename( SMaterialGroupLoader *pLoader );
	BOOL CreateIndexBuffer( SBMMLoader *pLoader );
	BOOL CreateVertexBuffer( SBMMLoader *pLoader );


private:
	enum MEMTYPE { MT_TREE, MT_LINEAR };
	typedef struct _tagSMapItem
	{
		_tagSMapItem() {}
		_tagSMapItem( MEMTYPE t, BYTE *p ) : eType(t), pItem(p) {}
		MEMTYPE eType;
		BYTE *pItem;
	} SMapItem;

	typedef struct _tagModelData
	{
		SBMMLoader *pBmm;
		IDirect3DVertexBuffer9 *pVtxBuff;
		IDirect3DIndexBuffer9 *pIdxBuff;
	} ModelData;

	typedef map<string, SMapItem> ModelMap;
	typedef map<string, SMapItem> KeyMap;
	typedef map<string, IDirect3DTexture9*> TextureMap;

	IDirect3DDevice9 *m_pDevice;
	ModelMap m_ModelMap;
	KeyMap m_KeyMap;
	TextureMap m_TextureMap;
	CFastMemLoader m_MemLoader;
	int m_ObjId;						// unique한 오브젝트 아이디를 생성한다.
};

