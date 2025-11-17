#include "ThreadPool.h"

#include "GameInstance.h"

CThreadPool::CThreadPool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	m_pDevice(pDevice),
	m_pContext(pContext),
	m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CThreadPool::Initialize(_uint iNumThread)
{
	m_iNumThread = iNumThread;
	m_bIsThreadStopAll = false;
	for (_uint i = 0; i < m_iNumThread; ++i)
	{
		thread WorkThread([&]() {
			Update_WorkThread(); }
		);

		thread::id WorkerID = WorkThread.get_id();
		m_Threads.emplace(WorkerID, move(WorkThread));

		THREAD_DESC Desc;
		m_pDevice->CreateDeferredContext(0, &Desc.pContext);
		Desc.OnCompleted = [&](thread::id ThreadID) { FinishedWorkThread(ThreadID); };
		m_DefferdContexts.emplace(WorkerID, Desc);
	}

	return S_OK;
}

void CThreadPool::Update_Async()
{
	while (!m_AddObjectList.empty())
	{
		auto Prototype = m_AddObjectList.front();
		m_AddObjectList.pop();

		m_pGameInstance->Add_Prototype(Prototype.iLevelID, Prototype.szPrototypeName, Prototype.pPrototype);
	}


	/*while (!m_CommandList.empty())
	{
		auto pCommandList = m_CommandList.front();
		m_CommandList.pop();

		m_pContext->ExecuteCommandList(pCommandList, TRUE);
		Safe_Release(pCommandList);
	}*/

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
				return true;
			}
			return false;
		});

		if (m_bIsThreadStopAll && m_ThreadJobs.empty())
		{
		
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
			job.JobFunction(&m_DefferdContexts[this_thread::get_id()]);
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
	auto& Desc = m_DefferdContexts[ThreadID];

	ID3D11CommandList* pCommandList = nullptr;
	Desc.pContext->FinishCommandList(FALSE, &pCommandList);
	m_iWorkdThread--;

	unique_lock<mutex> lock(m_QueueLock);
	m_CommandList.push(pCommandList);

	for (auto& iter : Desc.pAddObejct)
		m_AddObjectList.push(iter);

	Desc.pAddObejct.clear();
	lock.unlock();
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

	for (auto& Desc : m_DefferdContexts)
		Safe_Release(Desc.second.pContext);


	while (!m_CommandList.empty())
	{
		Safe_Release(m_CommandList.front());
		m_CommandList.pop();
	}

	while (!m_AddObjectList.empty())
	{
		auto& Desc = m_AddObjectList.front();
		m_AddObjectList.pop();
		Safe_Release(Desc.pPrototype);
	}

	StopAllThread();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
