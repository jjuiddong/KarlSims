//-----------------------------------------------------------------------------//
// 2005-10-04  programer: jjuiddong ��(���բ�)��
// 
// 2007-12-24 Chrismas Eve �� �ڵ��ϴ�.
//		- BoneNode Ŭ���� �߰�
//		- BoneTree�� CBone Ŭ�������� �����Ҽ� �ֵ��� ��
//
// 2014-02-21 �ڵ� ����
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
	int m_ObjId;			// ObjID�� CBone_ Ŭ�������� ��� BoneNode���� �����ϴ�.
	char m_pName[ 32];		// BoneNode �̸�
	CTrack *m_pTrack;
	Matrix44 *m_pPalette;
	Matrix44 m_matTM;		// Local��ȯ ���
	Matrix44 m_matAccTM;	// ������ TM
	Matrix44 m_matOffset;	// inverse( m_matWorld )
	Matrix44 m_matAni;		// Animation�� ����� ���
	Matrix44 m_matLocal;
	int m_AniStart;			// ������ ���۽ð�
	int m_AniEnd;			// ������ ����ð�
	int m_AniTime;			// ���ϸ��̼� �� �ð�
	int m_LoopTime;			// ���ϸ��̼� �ѽð�
	int m_AniIncFrame;		// ���ϸ��̼� �� �� �ð�
	int m_AniFrame;			// ���� ���ϸ��̼� ������ (AniEnd�� ������ 0���� �ʱ�ȭ�ȴ�.)
	Box *m_pBox;

	BOOL m_bAni;			// TRUE�ϰ�츸 ���ϸ��̼��� �ȴ�.
	BOOL m_bLoop;			// ���ϸ��̼� �ݺ� ����
	BOOL m_bQuick;			// ���� ���ϸ��̼ǿ��� �������� ����
	BOOL m_bSmooth;			// ���ϸ��̼ǰ��� ������ �߻��Ҷ� TRUE�� �ȴ�.
	int m_nTempEndTime;		// ������ ������ EndTime�� ������Ʈ��Ű�� ���Ѻ���

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
// Skinning ���ϸ��̼ǿ� �ʿ��� physiq����� �ϴ� ���̷��� Ʈ����.
// ���������� CBoneNode�� Ʈ���� ������ ���ϸ��̼Ǹ��� ��������� Loop�� ���Ƽ�
// ��� BoneNode�� ���ϸ��̼� ����� ����ϰ� Palette�� ������Ʈ�Ѵ�.
//-----------------------------------------------------------------------------//
class CBone_
{
public:
	CBone_();
	virtual ~CBone_();

protected:
	int m_ObjId;			// CModel ID�� �����ϴ�.
	CBoneNode *m_pRoot;
	Matrix44 *m_pPalette;
	Matrix44 m_matWorld;	// world ���
	Matrix44 *m_pmatAniTM;	// ���ϸ��̼� ��� (Bip01)

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

