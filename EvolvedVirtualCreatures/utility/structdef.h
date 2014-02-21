//-----------------------------------------------------------------------------//
// 2005-10-04  programer: jjuiddong ┏(⊙д⊙)┛
// 
//-----------------------------------------------------------------------------//

#if !defined(__STRUCTDEF_H__)
#define __STRUCTDEF_H__


//#include "d3dx9.h"
//#include "math.h"

////////////////////////////////////////////////////////////////////////////////
enum ANI_TYPE { ANI_RIGID=1, ANI_KEYANI, ANI_SKIN };

#pragma pack(1)

////////////////////////////////////////////////////////////////////////////////
typedef struct _tagSVector3i
{
	int x,y,z;
} SVector3i;
typedef struct _tagSVector3s
{
	short x,y,z;
} SVector3s;
typedef struct _tagSVector4i
{
	int x,y,z,w;
} SVector4i;

////////////////////////////////////////////////////////////////////////////////
typedef struct _tagSMaterialLoader
{
	int id;
	D3DMATERIAL9 mtrl;
	char szFileName[ 128];

} SMaterialLoader;

typedef struct _tagSMaterialGroupLoader
{
	int size;
	SMaterialLoader *pMtrl;

} SMaterialGroupLoader;


////////////////////////////////////////////////////////////////////////////////
typedef struct _tagSVtxDiff
{
	enum { FVF = (D3DFVF_XYZ | D3DFVF_DIFFUSE) };
	Vector3 v;
	DWORD c;
} SVtxDiff;

typedef struct _tagSVtxRhwTex
{
	enum { FVF = D3DFVF_XYZRHW | D3DFVF_TEX1 };
	Vector3 v;
	float rhw;
	float tu,tv;
} SVtxRhwTex;


typedef struct _tagSVtxRhwDiff
{
	enum { FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE };
	Vector3 v;
	float rhw;
	DWORD c;
} SVtxRhwDiff;


typedef struct _tagSVtxNorm
{
	enum { FVF = (D3DFVF_XYZ | D3DFVF_NORMAL) };
	Vector3 v;
	Vector3 n;
} SVtxNorm;

typedef struct _tagSVtxNormTex
{
	enum { FVF = (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1) };
	Vector3 v;
	Vector3 n;
	float tu,tv;
} SVtxNormTex;

////////////////////////////////////////////////////////////////////////////////
typedef struct _tagSVertexNormLoader
{
	int size;
	SVtxNorm *pV;
} SVertexNormLoader;

typedef struct _tagSVertexNormTexLoader
{
	int size;
	SVtxNormTex *pV;
} SVertexNormTexLoader;




////////////////////////////////////////////////////////////////////////////////
typedef struct _tagSIndexLoader
{
	int size;
	SVector3s *pI;
} SIndexLoader;


////////////////////////////////////////////////////////////////////////////////
typedef struct _tagSKeyPos
{
	float frame;
	Vector3 p;
} SKeyPos;
typedef struct _tagSKeyRot
{
	float frame;
	Quaternion q;
} SKeyRot;
typedef struct _tagSKeyScale
{
	float frame;
	Vector3 s;
} SKeyScale;

////////////////////////////////////////////////////////////////////////////////
typedef struct _tagSKeyPosLoader
{
	int size;
	SKeyPos *p;
} SKeyPosLoader;

typedef struct _tagSKeyRotLoader
{
	int size;
	SKeyRot *r;
} SKeyRotLoader;

typedef struct _tagSKeyScaleLoader
{
	int size;
	SKeyScale *s;
} SKeyScaleLoader;


////////////////////////////////////////////////////////////////////////////////
typedef struct _tagSKeyLoader
{
	int linkid;
	float start;
	float end;
	SKeyPosLoader	pos;
	SKeyRotLoader	rot;
	SKeyScaleLoader	scale;

} SKeyLoader;

// key animation
typedef struct _tagSKeyGroupLoader
{
	int id;
	char name[ 32];
	int size;
	SKeyLoader *pKey;

} SKeyGroupLoader;

// animation form
typedef struct _tagSAniLoader
{
	int size;
	SKeyGroupLoader *pAniGroup;

} SAniLoader;

// animation merge info
typedef struct _tagSAniInfo
{
	char name[ 32];
	char path[ 64];

} SAniInfo;

// animation merge
typedef struct _tagSAniList
{
	int size;
	SAniInfo *pAniInfo;

} SAniList;


////////////////////////////////////////////////////////////////////////////////
typedef struct _tagSWeight
{
	int bone;
	float weight;
} SWeight;

typedef struct _tagSPhysique
{
	int vtx;
	int size;
	SWeight *w;
} SPhysique;

typedef struct _tagSPhysiqueLoader
{
	int size;
	SPhysique *p;

} SPhysiqueLoader;


////////////////////////////////////////////////////////////////////////////////

// mesh, meterial 정보
typedef struct _tagSMeshLoader
{
	int id;
	char szName[ 32];	// name
	int parentid;
	int mtrlid;
	SVertexNormLoader vn;
	SVertexNormTexLoader vnt;
	SIndexLoader i;
	SPhysiqueLoader physiq;
	SMaterialLoader *pMtrl;
	IDirect3DVertexBuffer9 *pvtxbuff;
	IDirect3DIndexBuffer9 *pidxbuff;

} SMeshLoader;

// 복수의 mesh를 저장한다.
typedef struct _tagSMeshGroupLoader
{
	int size;
	SMeshLoader *pMesh;

} SMeshGroupLoader;

// Bone 정보를 저장한다.
typedef struct _tagSBoneLoader
{
	int id;
	char szName[ 32];	// name
	int parentid;
	Matrix44 localtm;
	Matrix44 worldtm;

} SBoneLoader;

// 복수의 Bone을 저장한다.
typedef struct _tagSBoneGroupLoader
{
	int size;
	SBoneLoader *pBone;

} SBoneGroupLoader;

typedef struct _tagSBMMLoader
{
	int type;
	SMaterialGroupLoader mtrl;

	SMeshGroupLoader m;
	SBoneGroupLoader b;

} SBMMLoader;



struct SAlpha
{
	int iAlphaRowCount;
	int* pAlpha;
};

struct SAlphaTexture
{
	int iCount;
	SAlpha* pCol;
};

struct STile
{
	int iTileNum;
	char strTextureName[64];
	int iAttribute;
	int iVtxSize;
	SVtxNormTex *sVertex;
	int iBlockSize;
	SVtxNormTex *sBlock;
	SAlphaTexture sAlphaTex;
};

struct STerrainObject
{
	char strName[64];
	float x;
	float y;
	float z;
	
	float qx;
	float qy;
	float qz;
	float qw;
	
	float size;
};

struct SObjectTotal
{
	int iCount;
	STerrainObject* sObject;
};

struct STileForm
{
	int iWidth;
	int iHeight;
	int iDetail;
	int iSize;
	char strTextureName[64];
	int iTileCount;
	STile* pTile;
	SObjectTotal sModel;
};

struct MYINDEX
{
	WORD _0, _1, _2;
};


typedef struct _tagControl
{
	int type;
	int id;
	int visible;
	char file[ 64];
	int x;
	int y;
	int w;
	int h;
	int cw;
	int ch;

} SControl;


typedef struct _tagSScene
{
	int id;
	int csize;
	SControl *pc;
	int ssize;
	_tagSScene *ps;
} SScene;


#pragma pack()



#endif // __STRUCTDEF_H__
