#pragma once

#include "Client_Defines.h"
#include "Player_Parts.h"


NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
NS_END

NS_BEGIN(Client)

class CCameraBone_Player final : public CPlayer_Parts
{
public:
	typedef struct tagCameraBone_Player_Desc : public CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrix = { nullptr };
		class CCharacter* pCharacter = { nullptr };
	}CAMERABONE_DESC;
private:
	CCameraBone_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCameraBone_Player(const CCameraBone_Player& Prototype);
	virtual ~CCameraBone_Player() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;

	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;

	virtual void Set_Animation (const _char* szAnimationTag,
		_bool isLoop = true,
		_float fAnimationPlayRate = 1.f,
		_float fLerpDuration = 0.12f,
		_bool bIsRestart = FALSE,
		_float fEndTrackPosition = -1.f,
		_float fStartTrackPosition = 0.f,
		_bool isResetTrackPosition = TRUE)override;

private:
	CCollider* m_pColliderCom = { nullptr };

private:
	const _float4x4* m_pSocketMatrix = { nullptr };
	class CCharacter* m_pCharacter = { nullptr };

	_float						m_isAnimationChanged = 1.f;
	_int						m_iAnimationIndex = 0;


private:
	HRESULT						Ready_Components();
public:
	static CCameraBone_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END