#include "ThreadPool.h"

HRESULT CThreadPool::Initialize(_uint iNumThread)
{
	m_iNumThread = iNumThread;

	m_bIsThreadStopAll = false;
	m_Threads.reserve(m_iNumThread);

	for (_uint i = 0; i < m_iNumThread; ++i)
		m_Threads.emplace_back([&]() { Update_WorkThread(); });

	return S_OK;
}

void CThreadPool::Update_WorkThread()
{
	while (true)
	{
		unique_lock<mutex> lock(m_Worker);
		m_cv_Jobs.wait(lock, [this]()
		{
			if (!m_ThreadJobs.empty() || m_bIsThreadStopAll)
			{
				if(0 < m_iWorkdThread)
					m_iWorkdThread--;

				return true;
			}
			return false;
		});

		if (m_bIsThreadStopAll && m_ThreadJobs.empty())
			return;

		// 맨 앞의 job 을 뺀다.
		THREAD_JOB job = move(m_ThreadJobs.front());
		m_ThreadJobs.pop();
		lock.unlock();

		// 등록된 함수를 수행
		if (false == job.bIsCanceled)
		{
			m_iWorkdThread++;
			job.JobFunction();
		}
	}
}

ThreadJobHandle* CThreadPool::Add_jobList(function<void()> function)
{
	ThreadJobHandle Handle = {};
	Handle.iJobID = _uint(m_ThreadJobs.size() + 1);
	Handle.JobFunction = function;
	Handle.bIsCanceled = false;

	m_ThreadJobs.push(Handle);
	m_cv_Jobs.notify_one();
	return &m_ThreadJobs.back();
}

void CThreadPool::StopAllThread()
{
	m_bIsThreadStopAll = true;
	while (!m_ThreadJobs.empty())
		m_ThreadJobs.pop();

	m_cv_Jobs.notify_all();

	for (auto& pThread : m_Threads)
		pThread.join();
}

size_t CThreadPool::GetThreadJobCount()
{
	return m_ThreadJobs.size();
}

CThreadPool* CThreadPool::Create(_uint iNumThread)
{
	CThreadPool* pThreadPool = new CThreadPool();
	if (FAILED(pThreadPool->Initialize(iNumThread)))
	{
		Safe_Release(pThreadPool);
		MSG_BOX("Create Fail : Thread Pool");
	}
	return pThreadPool;
}

void CThreadPool::Free()
{
	__super::Free();
	StopAllThread();
}
