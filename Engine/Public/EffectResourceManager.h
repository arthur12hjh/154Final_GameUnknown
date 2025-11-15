#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CComponent;
class CGameInstance;

class CEffectResourceManager : public CBase
{
private:
	CEffectResourceManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CEffectResourceManager() = default;

	HRESULT									Initalize();

public :
	void									ADD_TextureResource(const WCHAR* TextureTag, CTexture* pTexture);
	void									ADD_ShaderResource(const WCHAR* ShaderTag, CShader* pShader);
	void									ADD_VIBufferResource(const WCHAR* VIBufferTag, CComponent* pVIBuffer);

	CTexture*								GetTextureResource(const WCHAR* TextureTag);
	CShader*								GetShaderResource(const WCHAR* ShaderTag);
	CComponent*								GetVIBufferResource(const WCHAR* VIBufferTag);

#pragma region _DEBUG
	map<const _wstring, CTexture*>*			GetALLTextureResource() { return &m_pTextures; }
	map<const _wstring, CShader*>*			GetALLShaderResource() { return &m_pShaders; }
	map<const _wstring, CComponent*>*		GetALLVIBufferResource() { return &m_pVIBuffers; }
#pragma endregion

private:
	ID3D11Device*							m_pDevice = nullptr;
	ID3D11DeviceContext*					m_pContext = nullptr;
	CGameInstance*							m_pGameInstance = nullptr;

	map<const _wstring, CTexture*>			m_pTextures = {};
	map<const _wstring, CComponent*>		m_pVIBuffers = {};
	map<const _wstring, CShader*>			m_pShaders = {};

private :
	HRESULT									LoadTexture();
	HRESULT									LoadVIBuffer(void* pArg);

	//  이펙트 구현 되면 이거 하나 풀어서 사용할 예정
	//	폴더에 이펙트 데이터 넣고 내가 읽어서 파싱
	//HRESULT							LoadPartEffect();
	//HRESULT							LoadContainerEffect();
public:
	static CEffectResourceManager*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void							Free() override;
};
NS_END