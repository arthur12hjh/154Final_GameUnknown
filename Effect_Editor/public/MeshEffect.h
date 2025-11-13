#pragma once

#include "Tool_Effect_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Tool_Effect)

class CMeshEffect final : public CGameObject
{
public:
	typedef struct MeshEffectData
	{
		const _float4x4* pParentMat = { nullptr };
		_int	iBegin = {};
		_uint	iSelectRender = {};
		_float4 fColor = {};
		_float3 fScale = {};
		_float4 fPosition = { 0,0,0,1 };
		_float3 fRotation = {};
	}MESH_DATA;

private:
	CMeshEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeshEffect(const CMeshEffect& Prototype);
	virtual ~CMeshEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void	Set_Components(MESH_DATA tData);
	void	Update(MESH_DATA tData);
	MESH_DATA	Get_Data() { return m_tData; }
	string	Get_TextureName(_int iIndex) { return m_szFile[iIndex]; }
	void	Set_Texture(_int iIndex, CTexture* pTexture, string szFile) { m_pTexture[iIndex] = pTexture; m_szFile[iIndex] = szFile; }
	void	Set_Model(_wstring szMode);
private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTexture[3] = {};
	string    m_szFile[3];
	_float		m_fTime = {};
	MESH_DATA	m_tData;
	_uint	m_iModelCount = {};
	_float4x4		m_CombinedWorldMatrix = {};

private:
	HRESULT							Ready_Components();
	HRESULT							Bind_ShaderResources();

public:
	static CMeshEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void					Free() override;
};

NS_END