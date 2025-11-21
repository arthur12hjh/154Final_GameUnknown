#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Client)
class CNayitbaPartBody final : public CPartObject
{
public :
	typedef struct NayitbaPartBodyDesc : public PARTOBJECT_DESC
	{
		const WCHAR*				szBodyModel;
	}NAYITBA_PART_BODY_DESC;

private:
	CNayitbaPartBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNayitbaPartBody(const CNayitbaPartBody& Prototype);
	virtual ~CNayitbaPartBody() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;

	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;

	virtual HRESULT					Render() override;
	virtual HRESULT					Render_Shadow() override;

private:
	CCollider*						m_pColliderCom = { nullptr };
	_bool							m_isAnimFinish = { false };

private:
	HRESULT							Ready_Components(const NAYITBA_PART_BODY_DESC& pDesc);
	HRESULT							Bind_ShaderResources();

public:
	static		CNayitbaPartBody*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CGameObject*		Clone(void* pArg) override;
	virtual		void				Free() override;

};
NS_END