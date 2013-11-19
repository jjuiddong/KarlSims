
#include "stdafx.h"
#include "RandNoDuplicate.h"
#include <assert.h>
#include <mmSystem.h>

namespace common
{
	enum { MAX_RAND_COUNT = 1024};
	int n_randNumber[ MAX_RAND_COUNT];
	int n_randCount = MAX_RAND_COUNT;
	bool n_IsInitRand = false;			// InitRandNoDuplicate() �Լ��� ȣ��Ǹ� true�� �ȴ�.


	//------------------------------------------------------------------------
	// �ʱ�ȭ ���Ŀ� RandNoDuplicate() �Լ��� ȣ���ؾ� �Ѵ�.
	//------------------------------------------------------------------------
	void InitRandNoDuplicate()
	{
		n_IsInitRand = true;
		srand(timeGetTime());
		for(int i=0; i < MAX_RAND_COUNT; ++i)
			n_randNumber[ i] = i+10; // �������� 10������ �����Ѵ�. (����� ������)
	}

	//------------------------------------------------------------------------
	// �ߺ����� �ʴ� ������ �����Ѵ�. �ִ� ���������� MAX_RAND_COUNT ��.
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
	// �ߺ����� �ʴ� Id�� �����Ѵ�.
	//------------------------------------------------------------------------
	int GenerateId()
	{ 
		if (!n_IsInitRand) 
			InitRandNoDuplicate();
		return RandNoDuplicate();
	}
}
