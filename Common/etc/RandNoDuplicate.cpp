
#include "stdafx.h"
#include "RandNoDuplicate.h"
#include <assert.h>
#include <mmSystem.h>

namespace common
{
	enum { MAX_RAND_COUNT = 1024};
	int n_randNumber[ MAX_RAND_COUNT];
	int n_randCount = MAX_RAND_COUNT;
	bool n_IsInitRand = false;			// InitRandNoDuplicate() 함수가 호출되면 true가 된다.


	//------------------------------------------------------------------------
	// 초기화 한후에 RandNoDuplicate() 함수를 호출해야 한다.
	//------------------------------------------------------------------------
	void InitRandNoDuplicate()
	{
		n_IsInitRand = true;
		srand(timeGetTime());
		for(int i=0; i < MAX_RAND_COUNT; ++i)
			n_randNumber[ i] = i+10; // 랜덤값은 10번부터 시작한다. (예약어 때문에)
	}

	//------------------------------------------------------------------------
	// 중복되지 않는 난수를 리턴한다. 최대 난수개수는 MAX_RAND_COUNT 다.
	//------------------------------------------------------------------------
	int RandNoDuplicate()
	{
		if (n_randCount <= 0)
		{
			assert(0);
			return rand();
		}
		
		const int idx = rand()%n_randCount;
		const int r = n_randNumber[ idx];
		std::swap(n_randNumber[idx], n_randNumber[ n_randCount-1]);
		--n_randCount;		

		return r;
	}

	//------------------------------------------------------------------------
	// 중복되지 않는 Id를 생성한다.
	//------------------------------------------------------------------------
	int GenerateId()
	{ 
		if (!n_IsInitRand) 
			InitRandNoDuplicate();
		return RandNoDuplicate();
	}
}
