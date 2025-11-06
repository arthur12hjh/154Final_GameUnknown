#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CTimer final : public CBase
{
private:
	CTimer();
	virtual ~CTimer() = default;

public:
	_float	Get_TimeDelta() const { 
		return m_fTimeDelta; }

public:
	HRESULT	Initialize();
	void Update_Timer();

private:
	LARGE_INTEGER		m_FrameTime{};
	LARGE_INTEGER		m_FixTime{};
	LARGE_INTEGER		m_LastTime{};
	LARGE_INTEGER		m_CpuTick{};

	_float				m_fTimeDelta{};

public:
	static CTimer* Create();
	virtual void Free() override;
};

NS_END

