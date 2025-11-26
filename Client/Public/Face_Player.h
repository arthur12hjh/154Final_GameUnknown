#pragma once

#include "Client_Defines.h"
#include "Player_Parts.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CBone;
NS_END

NS_BEGIN(Client)

class CFace_Player final : public CPlayer_Parts
{
public:
	typedef struct tagFace_Player_Desc : public CPartObject::PARTOBJECT_DESC
	{
		void*			pBodyPtr = { nullptr };
	} FACE_PLAYER_DESC;

private:
	CFace_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFace_Player(const CFace_Player& Prototype);
	virtual ~CFace_Player() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

private:
	CModel* m_pBodyModelCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	HRESULT Bind_BoneToPartBody(void* pArg);

public:
	static CFace_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END