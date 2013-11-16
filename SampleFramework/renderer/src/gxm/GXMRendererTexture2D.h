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
#ifndef GXM_RENDERER_TEXTURE_2D_H
#define GXM_RENDERER_TEXTURE_2D_H

#include <RendererConfig.h>

#if defined(RENDERER_ENABLE_LIBGXM)

#include <RendererTexture2D.h>
#include <gxm/constants.h>
#include <scetypes.h>
#include "GXMRenderer.h"

class SceGxmTexture;

namespace SampleRenderer
{	
	class GXMRendererTexture2D : public RendererTexture2D
	{
	public:
		GXMRendererTexture2D(const RendererTexture2DDesc &desc, GXMRenderer& renderer);
		virtual ~GXMRendererTexture2D(void);

	public:
		virtual void *lockLevel(PxU32 level, PxU32 &pitch);
		virtual void  unlockLevel(PxU32 level);

		void bind(PxU32 textureUnit);

		virtual	void	select(PxU32 stageIndex)
		{
			bind(stageIndex);
		}

	private:
		static void GetGXMTextureFormat(RendererTexture2D::Format f, SceGxmTextureFormat& format);
		static SceGxmTextureAddrMode GetAddressMode(RendererTexture2D::Addressing add);

	private:
		GXMRenderer&		m_Renderer;
		SceGxmTexture*		m_Texture;
		SceUInt8**			m_Data;
		PxU32				m_NumLevels;
		SceUID*				m_TextureMemoryId;
		SceGxmTextureFormat	m_Format;
		SceGxmTextureAddrMode m_UAddrsMode;
		SceGxmTextureAddrMode m_VAddrsMode;
	};

} // namespace SampleRenderer

#endif // #if defined(RENDERER_ENABLE_LIBGXM)
#endif
