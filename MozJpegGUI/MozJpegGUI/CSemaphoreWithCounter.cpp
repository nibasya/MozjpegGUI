#include "pch.h"
#include "CSemaphoreWithCounter.h"

CSemaphoreWithCounter::CSemaphoreWithCounter(LONG lInitialCount, LONG lmaxCount, LPCTSTR pstrName, LPSECURITY_ATTRIBUTES lpsaAttributes)
	:CSemaphore(lInitialCount, lmaxCount, pstrName, lpsaAttributes), m_count(0)
{

}

// this function is not called by CMultiLock due to Microsoft's MFC implementation
BOOL CSemaphoreWithCounter::Lock(DWORD dwTimeout)
{
	BOOL ret;
	ret = CSemaphore::Lock(dwTimeout);
	if (ret != -1 && ret != WAIT_TIMEOUT) {
		m_cs.Lock();
		m_count++;
		m_cs.Unlock();
	}
	return ret;
}


BOOL CSemaphoreWithCounter::Unlock()
{
	BOOL ret;
	ret = CSemaphore::Unlock(1, NULL);
	if (ret != 0) {
		m_cs.Lock();
		ASSERT(m_count > 0);
		m_count--;
		m_cs.Unlock();
	}
	return ret;
}


BOOL CSemaphoreWithCounter::Unlock(LONG lCount, LPLONG lpPrevCount)
{
	BOOL ret;
	ret = CSemaphore::Unlock(lCount, lpPrevCount);
	if (ret != 0) {
		m_cs.Lock();
		ASSERT(m_count > 0);
		m_count--;
		m_cs.Unlock();
	}
	return ret;
}

unsigned long CSemaphoreWithCounter::GetCount() {
	return m_count;
}


unsigned long CSemaphoreWithCounter::AddCount() {
	m_cs.Lock();
	m_count++;
	m_cs.Unlock();
	return m_count;
}


