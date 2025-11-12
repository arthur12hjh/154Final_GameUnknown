#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CMaterial final : public CBase
{
private:
	CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMaterial() = default;

public:
	HRESULT Initialize(const _char* pModelFilePath, const binMaterial* pBinMaterial);
	HRESULT Bind_SRV(class CShader* pShader , const _char* pConstantName, aiTextureType eType, _uint iTextureIndex);
private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };

	class CGameInstance*			m_pGameInstance = { nullptr };

	_uint									m_iNumSRVs = {};
	vector<ID3D11ShaderResourceView*>		m_SRVs[AI_TEXTURE_TYPE_MAX];

private:
	HRESULT BindDefaultTexture(CShader* pShader, _uint eType);

public:
	static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const binMaterial* pBinMaterial);
	virtual void Free() override;	
};

NS_END