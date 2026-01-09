#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Client)
class CNpcFace final : public CPartObject
{
public:
	typedef struct tagFace_Desc : public CPartObject::PARTOBJECT_DESC
	{
		class CModel*		pBodyModelCom = nullptr;
		WCHAR				szFaceJsonDataName[256];
	}FACE_DESC;

private:
	CNpcFace(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNpcFace(const CNpcFace& Prototype);
	virtual ~CNpcFace() = default;

public:
	virtual HRESULT										Initialize_Prototype() override;
	virtual HRESULT										Initialize(void* pArg) override;

	virtual void										Priority_Update(_float fTimeDelta) override;
	virtual void										Update(_float fTimeDelta) override;
	virtual void										Late_Update(_float fTimeDelta) override;

	virtual HRESULT										Render() override;
	virtual HRESULT										Render_Shadow() override;

private:
	CModel*												m_pBodyModelCom = { nullptr };
	const _float4x4*									m_pSocketMatrix = { nullptr };

private:
	HRESULT												Ready_Components();
	HRESULT												Bind_ShaderResources();
	HRESULT												Mapping_Shader_Material(_uint iIdx);
	HRESULT												Bind_BoneToPartBody(class CModel* pArg);

public:
	static		CNpcFace*								Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CGameObject*							Clone(void* pArg) override;
	virtual		void									Free() override;

};
NS_END