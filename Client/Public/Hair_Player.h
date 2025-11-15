#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CBone;
NS_END

NS_BEGIN(Client)

class CHair_Player final : public CPartObject
{

public:
	typedef struct tagHair_Player_Desc : public CPartObject::PARTOBJECT_DESC
	{
		const _uint* pParentState = { nullptr };
		void* pBodyPtr = { nullptr };
	}HAIR_PLAYER_DESC;
private:
	CHair_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHair_Player(const CHair_Player& Prototype);
	virtual ~CHair_Player() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

private:
	const				_uint* m_pParentState = { nullptr };

	map<_char*, CBone*>			m_mapBodyBones;


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	HRESULT Bind_BoneToPartBody(void* pArg);

public:
	static CHair_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END