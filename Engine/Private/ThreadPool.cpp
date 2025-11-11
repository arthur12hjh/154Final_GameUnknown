#include "ThreadPool.h"

CThreadPool::CThreadPool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	m_pDevice(pDevice),
	m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CThreadPool::Initialize(_uint iNumThread)
{
	m_iNumThread = iNumThread;

	m_bIsThreadStopAll = false;
	for (_uint i = 0; i < m_iNumThread; ++i)
	{
		thread WorkThread([&]() { Update_WorkThread(); });
		m_Threads.emplace(WorkThread.get_id(), move(WorkThread));

		THREAD_DESC Desc;
		m_pDevice->CreateDeferredContext(0, &Desc.pContext);
		m_DefferdContexts.emplace(WorkThread.get_id(), Desc);
	}

	return S_OK;
}

void CThreadPool::Update_Async()
{
	while (!m_CommandList.empty())
	{
		auto pCommandList = m_CommandList.front();
		m_CommandList.pop();

		m_pContext->ExecuteCommandList(pCommandList, FALSE);
	}

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
				if (0 < m_iWorkdThread)
				{
					FinishedWorkThread(this_thread::get_id());
					m_iWorkdThread--;
				}
				return true;
			}
			return false;
		});

		if (m_bIsThreadStopAll && m_ThreadJobs.empty())
		{
			for (auto Desc : m_DefferdContexts)
				Safe_Release(Desc.second.pContext);
			return;
		}

		// 맨 앞의 job 을 뺀다.
		THREAD_JOB job = move(m_ThreadJobs.front());
		m_ThreadJobs.pop();
		lock.unlock();

		// 등록된 함수를 수행
		if (false == job.bIsCanceled)
		{
			m_iWorkdThread++;
			THREAD_DESC Desc = m_DefferdContexts[this_thread::get_id()];
			job.JobFunction(&Desc);
		}
	}
}

ThreadJobHandle* CThreadPool::Add_jobList(function<void(void*)> function)
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
		pThread.second.join();

}

size_t CThreadPool::GetThreadJobCount()
{
	return m_ThreadJobs.size();
}

_bool CThreadPool::IsWorkThread()
{
	return 0 < m_iWorkdThread;
}

void CThreadPool::FinishedWorkThread(thread::id ThreadID)
{
	auto Desc = m_DefferdContexts[ThreadID];
	ID3D11CommandList* pCommandList = nullptr;

	Desc.pContext->FinishCommandList(FALSE, &pCommandList);
	m_CommandList.push(pCommandList);
}

CThreadPool* CThreadPool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iNumThread)
{
	CThreadPool* pThreadPool = new CThreadPool(pDevice, pDeviceContext);
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
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
