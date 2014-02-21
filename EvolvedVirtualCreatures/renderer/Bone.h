//-----------------------------------------------------------------------------//
// 2005-10-04  programer: jjuiddong ┏(⊙д⊙)┛
// 
// 2007-12-24 Chrismas Eve 에 코딩하다.
//		- BoneNode 클래스 추가
//		- BoneTree를 CBone 클래스에서 관리할수 있도록 함
//
// 2014-02-21 코드 정리
//
//-----------------------------------------------------------------------------//
#pragma once

#include "3dnode.h"

class CTrack;
class CBoneNode : public C3dNode
{
public:
	CBoneNode( int nId );
	virtual ~CBoneNode();
	friend class CBone_;

protected:
	int m_ObjId;			// ObjID는 CBone_ 클래스내의 모든 BoneNode에서 동일하다.
	char m_pName[ 32];		// BoneNode 이름
	CTrack *m_pTrack;
	Matrix44 *m_pPalette;
	Matrix44 m_matTM;		// Local변환 행렬
	Matrix44 m_matAccTM;	// 누적된 TM
	Matrix44 m_matOffset;	// inverse( m_matWorld )
	Matrix44 m_matAni;		// Animation이 적용된 행렬
	Matrix44 m_matLocal;
	int m_AniStart;			// 프래임 시작시간
	int m_AniEnd;			// 프래임 종료시간
	int m_AniTime;			// 에니메이션 될 시간
	int m_LoopTime;			// 에니메이션 총시간
	int m_AniIncFrame;		// 에니메이션 된 총 시간
	int m_AniFrame;			// 현재 에니메이션 프래임 (AniEnd를 지나면 0으로 초기화된다.)
	Box *m_pBox;

	BOOL m_bAni;			// TRUE일경우만 에니메이션이 된다.
	BOOL m_bLoop;			// 에니메이션 반복 여부
	BOOL m_bQuick;			// 다음 에니메이션에서 보간할지 여부
	BOOL m_bSmooth;			// 에니메이션간의 보간이 발생할때 TRUE가 된다.
	int m_nTempEndTime;		// 보간이 끝난후 EndTime을 업데이트시키기 위한변수

public:
	BOOL Load( int ObjId, SBoneLoader *pLoader, Matrix44 *pPalette );
	void SetAnimation( SKeyLoader *pLoader, int nAniTime, BOOL bContinue=FALSE, BOOL bLoop=FALSE, BOOL bQuick=FALSE, BOOL bSmooth=FALSE );
	virtual void Render();
	virtual BOOL Animate( int nDelta );
	void SetBox( Box *pbox );
	void SetTM( Matrix44 *pTM ) { m_matTM = *pTM; }
	void MutliplyTM( Matrix44 *pTM ) { m_matTM *= *pTM; }
	Matrix44* GetAccTM() { return &m_matAccTM; }
	Box* GetCollisionBox() { return m_pBox; }

};


//-----------------------------------------------------------------------------//
// CBone_
// Skinning 에니메이션에 필요한 physiq계산을 하는 스켈레톤 트리다.
// 내부적으로 CBoneNode의 트리를 가지며 에니메이션마다 재귀적으로 Loop를 돌아서
// 모든 BoneNode의 에니메이션 행렬을 계산하고 Palette를 업데이트한다.
//-----------------------------------------------------------------------------//
class CBone_
{
public:
	CBone_();
	virtual ~CBone_();

protected:
	int m_ObjId;			// CModel ID와 동일하다.
	CBoneNode *m_pRoot;
	Matrix44 *m_pPalette;
	Matrix44 m_matWorld;	// world 행렬
	Matrix44 *m_pmatAniTM;	// 에니메이션 행렬 (Bip01)

public:
	BOOL Load( int ObjId, SBoneGroupLoader *pLoader, BOOL bCollision, SMeshGroupLoader *pMeshLoader );
	void SetAnimation( SKeyGroupLoader *pLoader, int nAniTime, BOOL bContinue, BOOL bLoop, BOOL bQuick );
	BOOL Animate( int nDelta );
	void Render();

	void Clear();
	Matrix44* GetPalette() const { return m_pPalette; }
	Matrix44* GetAniTM();
	void SetTM( Matrix44 *pTM );
	void MutliplyTM( Matrix44 *pTM );

	CBoneNode* FindBoneNode( int id );
	CBoneNode* FindBoneNode( char *pName );

	void UpdateCollisionBox();
	void GetCollisionBox( std::list<Box*> *pList );
	BOOL Pick( const Vector2 &pickPos );

	int GetCurrentFrame();

protected:
	void RenderRec( C3dNode *pNode );
	BOOL AnimateRec( C3dNode *pNode, int nDelta );
	void SetAnimationRec( CBoneNode *pNode, SKeyGroupLoader *pLoader, int nAniTime, BOOL bContinue, BOOL bLoop, BOOL bQuick );
	BOOL CreateCollisionBox( SBoneGroupLoader *pLoader, SMeshGroupLoader *pMeshLoader, Box *pReval );
	CBoneNode* FindBoneNodeRec( CBoneNode *pCurNode, char *pName );
	void UpdateCollisionBoxRec( CBoneNode *pCurNode );
	void GetCollisionBoxRec( CBoneNode *pCurNode, std::list<Box*> *pList );
	BOOL PickRec( CBoneNode *pCurNode, const Vector2 &pickPos );
};

