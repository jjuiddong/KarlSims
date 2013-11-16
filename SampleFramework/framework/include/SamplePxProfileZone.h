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

#ifndef SAMPLE_PROFILE_ZONE_H
#define SAMPLE_PROFILE_ZONE_H


#ifdef PHYSX_PROFILE_SDK

#include "physxprofilesdk/PxProfileScopedEvent.h"
#include "physxprofilesdk/PxProfileEventSender.h"
#include "physxprofilesdk/PxProfileEventNames.h"
#include "physxprofilesdk/PxProfileZone.h"

namespace physx { namespace profile {
#define AG_PERFMON_EVENT_DEFINITION_HEADER "AgPerfMonEventDefs.h"
#include "SampleProfileDeclareAgPerfmonEventInfo.h"
#undef AG_PERFMON_EVENT_DEFINITION_HEADER
}}

struct SampleProfileEventNameProvider : public physx::PxProfileNameProvider
{
	virtual physx::PxProfileNames getProfileNames() const;
};

#ifdef SAMPLE_PERF_SCOPE
#undef SAMPLE_PERF_SCOPE
#endif

#ifdef SAMPLE_PERF_SCOPE_PERF_DSCOPE
#undef SAMPLE_PERF_SCOPE_PERF_DSCOPE
#endif

#ifdef SAMPLE_PERF_SCOPE_PERF_SCOPE_STOP
#undef SAMPLE_PERF_SCOPE_PERF_SCOPE_STOP
#endif

#define SAMPLE_PERF_SCOPE(name)							   physx::profile::ScopedEventWithContext<true,physx::PxProfileEventSender,physx::profile::AgPerfmonEventIds::name> __scope(getEventBuffer(),0);
#define SAMPLE_PERF_SCOPE_PERF_DSCOPE(name, data)          physx::profile::eventValue( true,getEventBuffer(),physx::PxProfileEventSender,physx::profile::AgPerfmonEventIds::name,0,static_cast<PxI64>(data)); \
														   SAMPLE_PERF_SCOPE(name)
#define SAMPLE_PERF_SCOPE_PERF_SCOPE_STOP(data)            //unsupported at this time

#else

#define SAMPLE_PERF_SCOPE(name)
#define SAMPLE_PERF_SCOPE_PERF_DSCOPE(name, data)
#define SAMPLE_PERF_SCOPE_PERF_SCOPE_STOP(data)

#endif //PHYSX_PROFILE_SDK
 
#endif //SAMPLE_PROFILE_SDK_H
