#pragma once

#include "Base.h"
/* 1. 레벨교체하는 기능을 제공 (현재 게임내에 보여줘야하는 레벨의 주소를 보관한다 + 기존 레벨을 파괴하는 작업) */
/* 2. 기존 레벨용 자원을 정리한다. */

NS_BEGIN(Engine)

class CLevel_Manager final : public CBase
{
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT Change_Level(class CLevel* pNewLevel);
	void Update(_float fTimeDelta);
	HRESULT Render();

private:
	class CLevel*			m_pCurrentLevel = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };

public:
	static CLevel_Manager* Create();
	virtual void Free() override;
};

NS_END