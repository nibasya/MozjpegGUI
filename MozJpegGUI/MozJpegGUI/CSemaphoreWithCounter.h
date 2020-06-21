#pragma once
#include <afxmt.h>

class CSemaphoreWithCounter :
	public CSemaphore
{
public:
	CSemaphoreWithCounter(LONG lInitialCount = 1, LONG lmaxCount = 1, LPCTSTR pstrName = NULL, LPSECURITY_ATTRIBUTES lpsaAttributes = NULL);
	BOOL Lock(DWORD dwTimeout = INFINITE);
	BOOL Unlock();
	BOOL Unlock(LONG lCount, LPLONG lpPrevCount = NULL);
	unsigned long GetCount();
	unsigned long AddCount();
private:
	unsigned long m_count;
	CCriticalSection m_cs;
};



