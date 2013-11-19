//------------------------------------------------------------------------
// Name:    RandNoDuplicate.h
// Author:  jjuiddong
// Date:    12/22/2012
// 
// 중복되지 않는 난수를 생성한다.
// ** 쓰레드에 안전하지 않다. ** 
//------------------------------------------------------------------------
#pragma once

namespace common
{
	void InitRandNoDuplicate();
	int RandNoDuplicate();
	int GenerateId();

}
