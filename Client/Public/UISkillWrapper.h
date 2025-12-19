#pragma once

#include "Client_Defines.h"
#include "UIBase.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class CUISkillWrapper final : public CUIBase
{
private:
	CUISkillWrapper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUISkillWrapper(const CUISkillWrapper& Prototype);
	virtual ~CUISkillWrapper() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	_uint Get_SkillID(_uint iIdx) const { return *m_iSkillID[iIdx]; }

protected:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;

	virtual HRESULT Execute(const UI_EVENT_DESC& EventDesc) override;
	virtual void CallbackEvent(void* pArg) override;

private:
	_float* m_fMaxRushCoolTime = nullptr;
	_float* m_fCurrentRushCoolTime = nullptr;

	SKILL_STATE* m_eRushState = nullptr;
	SKILL_STATE m_ePrevRushState = SKILL_STATE::DEFAULT;

	SKILL_STATE* m_eSkillState[4];
	SKILL_STATE m_ePrevSkillState[4];
	_uint* m_iSkillID[4];
	LONGLONG* m_iCurrentBeta{ nullptr };
	/*SKILL_STATE* m_eSkillState[4];
	SKILL_STATE m_ePrevSkillState[4];*/

public:
	static CUISkillWrapper* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END

/*
여기서 스킬 4개, Rush 컨트롤
*/