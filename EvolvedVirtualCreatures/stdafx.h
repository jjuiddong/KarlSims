// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

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
