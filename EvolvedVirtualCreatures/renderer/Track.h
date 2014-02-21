//-----------------------------------------------------------------------------//
// 2005-10-04  programer: jjuiddong ┏(⊙д⊙)┛
// 
// 2007-12-25 Chrismas에 코딩하다.
//		- 에니메이션 계산 최적화(매 루프마다 에니메이션 배열을 검색했던것을 수정함)
//		- 에니메이션 도중 다른 에니메이션을 바뀔때 보간기능 추가
//
// 2014-02-21 코드 정리.
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

	// 에니메이션의 다음 Key index를 나타낸다.
	int m_nKeyPos;
	int m_nKeyRot;
	int m_nKeyScale;
	// 에니메이션에서 현재 Key와 다음 Key를 저장한다.
	SKeyPos *m_pKeyPos[ 2];
	SKeyRot *m_pKeyRot[ 2];
	SKeyScale *m_pKeyScale[ 2];

	// 새 에니메이션과 보간을 위해 필요함
	SKeyPos m_TempPos[ 2];
	SKeyRot m_TempRot[ 2];
	SKeyScale m_TempScale[ 2];
};
