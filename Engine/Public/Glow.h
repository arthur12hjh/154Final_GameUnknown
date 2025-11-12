#pragma once

#include "Deferred.h"

/*
글로우 & 블러 (그 외에도 여러 후처리 셰이더들) 
다 따로 쓸 수 있게 클래스 분리해놨고, 렌더타겟 세팅 & 객체 기록 다 여기서 하고있어. 
글로우 & 블러 클래스 이 두 개 셰이더 로직은 정환 너가 맘대로 커스텀 해도 돼.

근데 다 건드리고 나면 디코로 채팅 한번 쳐줘 굳이 언급 안하더라도 나중에 확인할게

그리고 이 구조에서 못 돌릴만한 코드 있으면 
그건 나 언급해서 채팅하든 갠디를 하든 직접 말을하든 알려줘 
합의해서 구조 바꿔보자
*/
NS_BEGIN(Engine)

class CGlow final : public CDeferred
{
private:
	CGlow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CGlow() = default;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Add_RenderObject(class CGameObject* pRenderObject) override;
	virtual HRESULT Render(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader) override;
	virtual HRESULT Bind_RenderTarget(class CShader* pShader, const _char* pConstantName) override;
#ifdef _DEBUG
	virtual HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY) override;
	virtual HRESULT Render_Debug(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader) override;
#endif

private:
	list<class CGameObject*> m_GlowObjects = {};

public:
	static CGlow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END