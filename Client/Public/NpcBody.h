#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Client)

class CNpcBody final : public CPartObject
{
public:
	typedef struct NpcPartBodyDesc : public PARTOBJECT_DESC
	{
		const WCHAR* szBodyModel;
	}NPC_PART_BODY_DESC;

private:
	CNpcBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNpcBody(const CNpcBody& Prototype);
	virtual ~CNpcBody() = default;

public:
	virtual HRESULT										Initialize_Prototype() override;
	virtual HRESULT										Initialize(void* pArg) override;

	virtual void										Priority_Update(_float fTimeDelta) override;
	virtual void										Update(_float fTimeDelta) override;
	virtual void										Late_Update(_float fTimeDelta) override;

	virtual HRESULT										Render() override;
	virtual HRESULT										Render_Shadow() override;

private:
	HRESULT												Ready_Components(const NPC_PART_BODY_DESC& pDesc);
	HRESULT												Bind_ShaderResources();

public:
	static		CNpcBody*								Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CGameObject*							Clone(void* pArg) override;
	virtual		void									Free() override;

};
NS_END