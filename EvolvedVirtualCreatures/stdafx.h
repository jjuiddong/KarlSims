// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
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


