#pragma once
#include "Maptool_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CCollider;
class CShader;
NS_END

NS_BEGIN(Tool_Map)
class CSoundTriggerBox : public CGameObject
{
public:
	typedef struct TriggerBoxDesc : public GAMEOBJECT_DESC
	{
		GROUND_SOUND_TYPE		eSoundBoxType;
	}SOUNDTRIGGER_BOX_DESC;

private:
	CSoundTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSoundTriggerBox(const CSoundTriggerBox& Prototype);
	virtual ~CSoundTriggerBox() = default;

public:
	GROUND_SOUND_TYPE		Get_GroundSoundType() const { return m_eGroundSoundType; }

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;

	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;

	virtual HRESULT			Render() override;

private:
	CCollider*				m_pCollider = { nullptr };
	GROUND_SOUND_TYPE		m_eGroundSoundType = {};

private:
	HRESULT					Ready_Components();

public:
	static CSoundTriggerBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END