#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CThreadPool : public CBase
{
private:
	CThreadPool() = default;
	virtual ~CThreadPool() = default;

public:
	HRESULT							Initialize(_uint iNumThread);

	void							Update_WorkThread();

	// 스레드 풀에 등록하면 등록 번호를 반환함
	// 이걸로 나중에 취소하거나 할수있음
	// Handle 받은거 작업 호출되면 nullptr로 바꿔주세요
	// 작업수행되서 작업리스트에서 빠지면 댕글링 포인터입니다.
	ThreadJobHandle*				Add_jobList(function<void()> function);
	
	void							StopAllThread();

	size_t							GetThreadJobCount();
	_bool							IsThreadPoolStop() { return m_bIsThreadStopAll; }

private:
	_uint							m_iNumThread = {};
	_uint							m_iWorkdThread = {};
	vector<thread>					m_Threads;

	condition_variable				m_cv_Jobs;
	mutex							m_Worker;

	queue<ThreadJob>				m_ThreadJobs;
	_bool							m_bIsThreadStopAll = false;

public:
	static	CThreadPool*			Create(_uint iNumThread);
	virtual void					Free() override;
	
};
NS_END