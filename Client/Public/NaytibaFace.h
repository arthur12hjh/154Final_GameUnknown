#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
NS_END

NS_BEGIN(Client)

class CNaytibaFace final : public CPartObject
{
public :
	typedef struct NaytibaFaceDesc : public PARTOBJECT_DESC
	{
		class CModel*		pBodyModelCom = nullptr;
		WCHAR				szFaceJsonDataName[MAX_PATH];
	}NAYTIBA_FACE_DESC;

private:
	CNaytibaFace(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNaytibaFace(const CNaytibaFace& Prototype);
	virtual ~CNaytibaFace() = default;

public:
	HRESULT								Mapping_Shader_Material(_uint iIdx);


public:
	virtual HRESULT						Initialize_Prototype() override;
	virtual HRESULT						Initialize(void* pArg) override;

	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;

	virtual HRESULT						Render() override;
	virtual HRESULT						Render_Shadow() override;
	virtual HRESULT						Render_MotionBlur() override;

private:
	CModel*								m_pBodyModelCom = { nullptr };
	const _float4x4*					m_pSocketMatrix = { nullptr };

private:
	HRESULT								Ready_Components();
	HRESULT								Bind_ShaderResources();

	HRESULT								Bind_BoneToPartBody(class CModel* pArg);

public:
	static CNaytibaFace*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END