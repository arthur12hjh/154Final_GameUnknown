#pragma once

#include "Base.h"

NS_BEGIN(Engine)
/*
클라쪽에서 화면 셰이더 효과 예약할 수 있게 넘겨주는 객체.

람다안씀? -> ㅇㅇ 클라쪽에서 받아올 변수 값이 계속 바뀔수도 있는데
이거 계속 바꿔주면서 람다 세팅 하려면 골아픔 (Color 등)
*/

class ENGINE_DLL CReserveDeferred abstract : public CBase
{
public:
	typedef struct tagScreenShader
	{
		_int	iPass;
		_float	fEndTime;
	}EFFECT_SCREEN_DESC;
protected:
	CReserveDeferred(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CReserveDeferred() = default;

public:
	virtual HRESULT Initialize(void* pArg) = 0;
	void Render(class CVIBuffer* pVIBuffer = nullptr);

public:
	//외부에서 인자 세팅해주는 함수
	virtual void Set_Desc(void* pArg) = 0;
	//하위 객체단에서 다형성으로 처리.
	virtual void Bind_Resources(const _wstring& strRTTag) = 0;
	void Set_ShaderPassIdx(_uint iIdx) { m_iShaderPassIdx = iIdx; }
	void Set_Active(_bool bFlag) { m_isActive = bFlag; if (m_isActive) m_fTime = 0; }
	_bool	Update(_float fTimeDelta);
	_bool Get_Active() { return m_isActive; }
	_float Get_Time() { return m_fTime; }
protected:
	ID3D11DeviceContext* m_pContext = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };

	class CGameInstance*	m_pGameInstance = { nullptr };
	class CShader*			m_pShaderCom = { nullptr };
	_uint					m_iShaderPassIdx = { 0 };
	_bool					m_isActive = { false };
	_float					m_fTime = { 0 };
	_float					m_fEndTime = { 0 };

public:
	virtual void Free() override;
};

NS_END
