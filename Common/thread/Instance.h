//------------------------------------------------------------------------
// Name:    SyncNode.h
// Author:  jjuiddong
// Date:    2012-12-03
// 
// ��Ƽ������ �ý����� ���� ���������.
// ���� �ٸ� �����尣�� ���̴� ��ü�ϰ�� �޸� ���Ÿ� �Ϲ�ȭ�ϱ� ���� 
// ���������.
//------------------------------------------------------------------------

#pragma once


namespace common
{
	class CInstance
	{
	public:
		CInstance() {}
		virtual ~CInstance() {}

	public:
		// Overriding
		virtual void	Release() {}
	};
}
