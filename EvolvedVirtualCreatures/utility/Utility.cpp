
#include "stdafx.h"
#include "Utility.h"
#include "RendererProjection.h"


using namespace utility;


void utility::convertD3D9(D3DMATRIX &dxmat, const SampleRenderer::RendererProjection &mat)
{
	float temp[16];
	mat.getColumnMajor44(temp);
	for(PxU32 r=0; r<4; r++)
		for(PxU32 c=0; c<4; c++)
		{
			dxmat.m[r][c] = temp[c*4+r];
		}
}


// 보간.
void utility::lerp(OUT PxVec3 &out, const PxVec3 &a, const PxVec3 &b, float t)
{
	out.x = a.x + (b.x-a.x) * t;
	out.y = a.y + (b.y-a.y) * t;
	out.z = a.z + (b.z-a.z) * t;
}


// 베지어 곡선을 리턴한다.
// point[ 0 ~ 3]
// t : 0 ~ 1
void utility::bezier(OUT PxVec3 &out, const vector<PxVec3> &points, const float t)
{
	PxVec3 ab,bc,cd,abbc,bccd;
	utility::lerp(ab, points[ 0], points[ 1], t);
	utility::lerp(bc, points[ 1], points[ 2], t);
	utility::lerp(cd, points[ 2], points[ 3], t);
	utility::lerp(abbc, ab, bc, t);
	utility::lerp(bccd, bc, cd, t);
	utility::lerp(out, abbc, bccd, t);
}
