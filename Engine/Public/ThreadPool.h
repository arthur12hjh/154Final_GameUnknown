#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CThreadPool : public CBase
{
public :


private:
	CThreadPool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CThreadPool() = default;

public:
	HRESULT							Initialize(_uint iNumThread);
	void							Update_Async();
	void							Update_WorkThread();

	// 스레드 풀에 등록하면 등록 번호를 반환함
	// 이걸로 나중에 취소하거나 할수있음
	// Handle 받은거 작업 호출되면 nullptr로 바꿔주세요
	// 작업수행되서 작업리스트에서 빠지면 댕글링 포인터입니다.
	ThreadJobHandle*				Add_jobList(function<void(void *)> function);
	
	void							StopAllThread();

	size_t							GetThreadJobCount();
	_bool							IsWorkThread();

	_bool							IsThreadPoolStop() { return m_bIsThreadStopAll; }
	void							FinishedWorkThread(thread::id ThreadID);

private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };

	_uint							m_iNumThread = {};
	_uint							m_iWorkdThread = {};

	map<thread::id, thread>			m_Threads;
	map<thread::id, THREAD_DESC>	m_DefferdContexts;

	condition_variable				m_cv_Jobs;
	mutex							m_Worker;

	queue<ThreadJob>				m_ThreadJobs;
	queue<ID3D11CommandList*>		m_CommandList;

	_bool							m_bIsThreadStopAll = false;

public:
	static	CThreadPool*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumThread);
	virtual void					Free() override;
	
};
NS_END