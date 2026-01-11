#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
NS_END

NS_BEGIN(Client)

class CCinematicPartFacial final : public CPartObject
{
public:
	typedef struct CinematicPartFacialDesc : public PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrix = nullptr;
		_wstring szModelTag;
		_bool isAnim = TRUE;
		_bool isSetTransform = FALSE;
		_float3 vPartPosition = {};
		_float3 vPartRotation = {};
		_float3 vPartScale = { 1.f, 1.f, 1.f };
		CModel* pBody;
	}FACIAL_CINEMATIC_DESC;

private:
	CCinematicPartFacial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCinematicPartFacial(const CCinematicPartFacial& Prototype);
	virtual ~CCinematicPartFacial() = default;

public:
	HRESULT								Mapping_Shader_Material(_uint iIdx);

public:
	virtual HRESULT										Initialize_Prototype() override;
	virtual HRESULT										Initialize(void* pArg) override;

	virtual void										Priority_Update(_float fTimeDelta) override;
	virtual void										Update(_float fTimeDelta) override;
	virtual void										Late_Update(_float fTimeDelta) override;

	virtual HRESULT										Render() override;
	virtual HRESULT										Render_Shadow() override;
	virtual HRESULT										Render_MotionBlur() override;

	void												Reset_SocketMatrix(_float4x4* pSocketMatrix = nullptr);
	void												Set_Render(_bool isActive) { SetActive(isActive); }

private:
	CModel*												m_pBodyModelCom = { nullptr };
	_wstring											m_szModelTag;
	_float4x4*											m_pSocketMatrix = { nullptr };

	_bool												m_bIsAnim = TRUE;

private:
	HRESULT												Bind_ShaderResources();
	HRESULT												Ready_Components();

	HRESULT												Bind_BoneToPartBody(CModel* pArg);

public:
	static		CCinematicPartFacial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CGameObject* Clone(void* pArg) override;
	virtual		void									Free() override;

};
NS_END