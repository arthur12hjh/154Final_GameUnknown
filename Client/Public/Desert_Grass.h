#pragma once

#include "Client_Defines.h"
#include "ClientStruct.h"
#include "Actor.h"

NS_BEGIN(Engine)
class CVIBuffer_Instance_Model;
NS_END

NS_BEGIN(Client)

/* 기가스맵 마른 풀 */

class CDesert_Grass final : public CActor
{
private:
	CDesert_Grass(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDesert_Grass(const CDesert_Grass& Prototype);
	virtual ~CDesert_Grass() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CVIBuffer_Instance_Model* m_pModelCom = { nullptr };

private:
	HRESULT Ready_Components(const _tchar* pComponentTag);
	HRESULT Bind_ShaderResources();

public:
	static CDesert_Grass* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END