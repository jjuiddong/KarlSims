//-----------------------------------------------------------------------------//
// 2005-10-04  programer: jjuiddong ��(���բ�)��
// 
// 2007-12-25 Chrismas�� �ڵ��ϴ�.
//		- ���ϸ��̼� ��� ����ȭ(�� �������� ���ϸ��̼� �迭�� �˻��ߴ����� ������)
//		- ���ϸ��̼� ���� �ٸ� ���ϸ��̼��� �ٲ� ������� �߰�
//
// 2014-02-21 �ڵ� ����.
//
//-----------------------------------------------------------------------------//
#pragma once


// CTrack
class CTrack
{
public:
	CTrack();
	virtual ~CTrack();

	BOOL Load( SKeyLoader *pLoader, BOOL bContinue=FALSE, BOOL bSmooth=FALSE, int nSmoothTime=0 );
	void InitAnimate();
	void Animate( int frame, Matrix44 *pMat );


protected:
	BOOL GetPosKey( int frame, Vector3 *pVec );
	BOOL GetRotKey( int frame, Quaternion *pQuat );
	BOOL GetScaleKey( int frame, Vector3 *pVec );
	float GetAlpha( float f1, float f2, float frame ) { return ( (frame-f1) / (f2-f1) ); }


protected:
	SKeyLoader *m_pKeys;
	int m_nCurFrame;

	// ���ϸ��̼��� ���� Key index�� ��Ÿ����.
	int m_nKeyPos;
	int m_nKeyRot;
	int m_nKeyScale;
	// ���ϸ��̼ǿ��� ���� Key�� ���� Key�� �����Ѵ�.
	SKeyPos *m_pKeyPos[ 2];
	SKeyRot *m_pKeyRot[ 2];
	SKeyScale *m_pKeyScale[ 2];

	// �� ���ϸ��̼ǰ� ������ ���� �ʿ���
	SKeyPos m_TempPos[ 2];
	SKeyRot m_TempRot[ 2];
	SKeyScale m_TempScale[ 2];
};
