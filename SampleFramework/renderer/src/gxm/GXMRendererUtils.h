// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2013 NVIDIA Corporation. All rights reserved.
#ifndef GXM_RENDERER_UTILS_H
#define GXM_RENDERER_UTILS_H

#include <RendererConfig.h>
#include <RendererColor.h>

#if defined(RENDERER_ENABLE_LIBGXM)

static void convertToSce(sce::Vectormath::Scalar::Aos::Vector4 &scecolor, const SampleRenderer::RendererColor &color)
{
	const float inv255 = 1.0f / 255.0f;
	scecolor = sce::Vectormath::Scalar::Aos::Vector4(color.r*inv255, color.g*inv255, color.b*inv255, color.a*inv255);
}

static void convertToSce(float *scevec, const physx::PxVec3 &vec)
{
	scevec[0] = vec.x;
	scevec[1] = vec.y;
	scevec[2] = vec.z;
	scevec[3] = 0.0f;
}

static void convertToSce(sce::Vectormath::Scalar::Aos::Matrix4 &dxmat, const physx::PxMat44 &mat)
{
	physx::PxMat44 mat44 = mat.getTranspose();
	memcpy((physx::PxF32*)&dxmat, mat44.front(), 4 * 4 * sizeof (float));	
}

static void convertToSce(sce::Vectormath::Scalar::Aos::Matrix4 &dxmat, const SampleRenderer::RendererProjection &mat)
{
	float temp[16];
	mat.getColumnMajor44(temp);
	for(physx::PxU32 r=0; r<4; r++)
		for(physx::PxU32 c=0; c<4; c++)
		{
			dxmat[r][c] = temp[c*4+r];
		}
}

#endif

#endif
