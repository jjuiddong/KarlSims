// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#define BOOST_LIB_DIAGNOSTIC


#include "../../../Common/Common.h"
#include "../wxMemMonitorLib/wxMemMonitor.h"
#include <boost/algorithm/string.hpp>
#include <set>

using std::list;
using std::vector;
using std::map;
using std::set;
using std::string;
using std::stringstream;



///////////////////////////////////////////////////////////////////////////////
// PhysX Sample Renderer
#include <Renderer.h>
#include <RendererVertexBuffer.h>
#include <RendererVertexBufferDesc.h>
#include <RendererIndexBuffer.h>
#include <RendererIndexBufferDesc.h>
#include <RendererMesh.h>
#include <RendererMeshDesc.h>
#include <RendererMemoryMacros.h>
#include <RendererShape.h>
#include <RaycastCCD.h>
#include <RenderMaterial.h>
#include "RenderBoxActor.h"
#include "RenderSphereActor.h"
#include "Picking.h"




///////////////////////////////////////////////////////////////////////////////
// PhysX
#include "PxPhysicsAPI.h"
#include "extensions/PxExtensionsAPI.h"

using namespace physx::shdfnd;
using namespace physx;




///////////////////////////////////////////////////////////////////////////////
// Global Variabl
#include "Configure.h"
extern SDbgConfig *g_pDbgConfig;



///////////////////////////////////////////////////////////////////////////////
// Global Function
#include "utility/Utility.h"
#include "utility/FileLoader.h"


