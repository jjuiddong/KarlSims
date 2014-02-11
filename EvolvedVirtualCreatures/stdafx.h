// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.

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
