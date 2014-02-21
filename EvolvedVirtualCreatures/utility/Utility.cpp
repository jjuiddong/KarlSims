
#include "stdafx.h"
#include "Utility.h"
#include "RendererProjection.h"



void convertD3D9(D3DMATRIX &dxmat, const SampleRenderer::RendererProjection &mat)
{
	float temp[16];
	mat.getColumnMajor44(temp);
	for(PxU32 r=0; r<4; r++)
		for(PxU32 c=0; c<4; c++)
		{
			dxmat.m[r][c] = temp[c*4+r];
		}
}
