#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Client)
//texture2D g_MaskTexture, g_DiffuseTexture, g_DissolveTexture;
class CMeshEffect final : public CGameObject
{
public:
	typedef struct MeshEffectData
	{
		string	szModel = {};
		string	szMaskTexture = {};
		string	szDiffuseTexture = {};
		string	szDissolveTexture = {};
		vector<_float3>		fSizeDiagrams;
		_float4 fColor = {};
		_float4 fPosition = { 0,0,0,1 };
		_float3 fScale = {};
		_float3 fRotation = {};


		_float2				fMaskUV;
		_float2				fMaskUVSpeed;
		_float2				fMaskUVSize;
		_float2				fDiffuseUV;
		_float2				fDiffuseUVSpeed;
		_float2				fDiffuseUVSize;
		_float2				fDissolveUV;
		_float2				fDissolveUVSpeed;
		_float2				fDissolveUVSize;

		_float				fDelayTime;
		_float				fEndTime;

		_int	iBegin = {};
		_int				iSelectRender;
	}MESH_EFFECT_DATA;

private:
	CMeshEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeshEffect(const CMeshEffect& Prototype);
	virtual ~CMeshEffect() = default;

public:
	virtual HRESULT Initialize_Prototype(const MESH_EFFECT_DATA* pEffectData);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void	Set_ParentMat(const _float4x4* pParentMat) { m_pParentMat = pParentMat; }
private:
	const _float4x4* m_pParentMat = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTexture[3] = {};
	_float		m_fTime = {};
	_uint			m_iRenderCount = {};
	MESH_EFFECT_DATA	m_tData;
	_float4x4		m_CombinedWorldMatrix = {};
	ID3D11ShaderResourceView* m_pSizeDiagramSRV = { nullptr };
	RENDER			m_eRender;

private:
	HRESULT							Ready_Components();
	HRESULT							Bind_ShaderResources();

public:
	static CMeshEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const MESH_EFFECT_DATA* pEffectData);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void					Free() override;
};

NS_END