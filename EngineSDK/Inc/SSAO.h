#pragma once

#include "Deferred.h"

NS_BEGIN(Engine)

/*

Screen Space Ambient Occlusion.
스크린 공간 상의 차폐를 계산하여 음영 계산을 더 확실하게 해줌.
이 과정을 통해 조금 더 사실적인 음영을 표현할 수 있게 해준다.
깊이 값이랑, 조명 연산 이전에 찍힌 화면의 정보들이 필요하다.

주변에 비해 얼마나 차폐되어있는지를 구해야 해서,
노말 벡터와 뎁스 벡터가 필요하다.

그리고 시야를 기준으로 정렬된 반구를 기준으로, 주변의 랜덤한 점들과 체크해야돼서 
뷰 스페이스로 끌어 올려야할 필요가 있다. -> 뷰, 투영 행렬과 투영 행렬의 역행렬이 필요하다.

*/

class CSSAO final : public CDeferred
{
private:
	CSSAO(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CSSAO() = default;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Add_RenderObject(class CGameObject* pRenderObject) override { return S_OK; }
	virtual HRESULT Render(class CVIBuffer_Rect* pVIBuffer) override { return S_OK; }
	virtual HRESULT Render(class CVIBuffer_Rect* pVIBuffer, const _wstring& strDepthRTTag, const _wstring& strNormalRTTag, const _wstring& strReturnRTTag);

	virtual HRESULT Bind_RenderTarget(class CShader* pShader, const _char* pConstantName) override;

	void Create_RandomNoise();
#ifdef _DEBUG
	virtual HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY) override;
	virtual HRESULT Render_Debug(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader) override;
#endif

private:
	_float3 m_vRandomNoise[16] = {};

public:
	static CSSAO* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END