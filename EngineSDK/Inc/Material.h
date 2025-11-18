#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CMaterial final : public CBase
{
private:
	CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMaterial() = default;

public:
	const _char* Get_Name() const {
		return m_szName;
	}
public:
	HRESULT Initialize(const _char* pModelFilePath, const binMaterial* pBinMaterial);
	HRESULT Bind_SRV(class CShader* pShader , const _char* pConstantName, aiTextureType eType, _uint iTextureIndex);

	HRESULT Import_Texture(aiTextureType eType, ID3D11ShaderResourceView* pSRV);
private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };

	class CGameInstance*			m_pGameInstance = { nullptr };

	_char									m_szName[MAX_PATH] = {};

	_uint									m_iNumSRVs = {};
	vector<ID3D11ShaderResourceView*>		m_SRVs[AI_TEXTURE_TYPE_MAX];

private:
	HRESULT BindDefaultTexture(CShader* pShader, _uint eType);

public:
	static CMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const binMaterial* pBinMaterial);
	virtual void Free() override;	
};

NS_END