#include "pch.h"
#include "TrailData.h"
#include "Trail.h"
#include "TrailEffect.h"

#include "GameInstance.h"

CTrailData::CTrailData(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CTrailData::CTrailData(const CTrailData& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CTrailData::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTrailData::Initialize(void* pArg)
{
	m_pTrail = static_cast<CTrailEffect*>(pArg);
	m_pContext->OMGetRenderTargets(1, nullptr, &m_pOriginalDSV);
	m_pOriginalDSV->GetResource(&m_resourse);
	return S_OK;
}

void CTrailData::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(m_eRender, this);
	m_iRenderCount = 0;
}

HRESULT CTrailData::Render()
{
	m_pTrail->Render(this);
	m_iRenderCount++;
	return S_OK;
}

HRESULT CTrailData::Render_MotionBlur()
{
	m_pTrail->Render(this);
	m_iRenderCount++;
	return S_OK;
}

void CTrailData::Set_Components(TRAIL_DATA tData)
{
	m_tData = tData;
	Set_Texture(0, m_tData.szMaskTexture.c_str());
	Set_Texture(1, m_tData.szDiffuseTexture.c_str());
	Set_Texture(2, m_tData.szDissolveTexture.c_str());

	/*
	
                case 0:
                    szRender = "NONBLEND";
                    break;
                case 1:
                    szRender = "NONLIGHT";
                    break;
                case 2:
                    szRender = "BLEND";
                    break;
                case 3:
                    szRender = "BLUR";
                    break;
                case 4:
                    szRender = "GLOW";
                    break;
                case 5:
                    szRender = "METABALL";
                    break;
                case 6:
                    szRender = "DISTORTION";
                    break;
	*/
	switch (m_tData.iSelectRender)
	{
	case 0:
		m_eRender = RENDER::NONBLEND;
		break;
	case 1:
		m_eRender = RENDER::NONLIGHT;
		break;
	case 2:
		m_eRender = RENDER::BLACKBLEND;
		break;
	case 3:
		m_eRender = RENDER::BLUR;
		break;
	case 4:
		m_eRender = RENDER::GLOW;
		break;
	case 5:
		m_eRender = RENDER::METABALL;
		break;
	case 6:
		m_eRender = RENDER::DISTORTION;
		break;
	case 7:
		m_eRender = RENDER::MOTIONBLUR;
		break;
	}
}

void CTrailData::Update(TRAIL_DATA tData)
{
	m_tData = tData;

	switch (m_tData.iSelectRender)
	{
	case 0:
		m_eRender = RENDER::NONBLEND;
		break;
	case 1:
		m_eRender = RENDER::NONLIGHT;
		break;
	case 2:
		m_eRender = RENDER::BLACKBLEND;
		break;
	case 3:
		m_eRender = RENDER::BLUR;
		break;
	case 4:
		m_eRender = RENDER::GLOW;
		break;
	case 5:
		m_eRender = RENDER::METABALL;
		break;
	case 6:
		m_eRender = RENDER::DISTORTION;
		break;
	case 7:
		m_eRender = RENDER::MOTIONBLUR;
		break;
	}
}

HRESULT CTrailData::Set_Texture(_int iIndex, const char* szPrototype)
{

	Safe_Release(m_pTexture[iIndex]);
	_tchar sztPrototype[256] = { 0, };
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szPrototype, strlen(szPrototype), sztPrototype, 256);

	char pattern[MAX_PATH] = {};
	strcpy_s(pattern, MAX_PATH, "Com_Texture");

	snprintf(pattern, sizeof(pattern), "Com_Texture_%d", m_iCount++);


	_tchar sztPrototype2[256] = { 0, };
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pattern, strlen(pattern), sztPrototype2, 256);

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::TOOL), sztPrototype,
		sztPrototype2, reinterpret_cast<CComponent**>(&m_pTexture[iIndex]))))
		return E_FAIL;
	return S_OK;
}

HRESULT CTrailData::Bind_Texture(CShader* pShader)
{
	//카메라의 여러 정보들을 받아올 수 있어 여기서 fFar 받아올 수 있음.
	//카메라 Far 값을 받아오는 변수는 "g_fFar" 로 세팅해줘. 
	//클라에선 g_fFar 알아서 세팅해주니까 걱정안해도 돼.
	CAMERA_INFO CamInfo = m_pGameInstance->Get_CurrentCamInfo();
	CamInfo.fFar;

	if (FAILED(m_pTexture[0]->Bind_ShaderResource(pShader, "g_MaskTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pTexture[1]->Bind_ShaderResource(pShader, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pTexture[2]->Bind_ShaderResource(pShader, "g_DissolveTexture", 0)))
		return E_FAIL;


	if (RENDER::BLUR == m_eRender || RENDER::METABALL == m_eRender) {
		ID3D11Texture2D* pDepthTexture = nullptr;
		D3D11_TEXTURE2D_DESC texDesc = {};
		ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
		texDesc.Width = 1600;
		texDesc.Height = 900;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		texDesc.SampleDesc.Count = 1;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		texDesc.SampleDesc.Quality = 0;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;
		if (FAILED(m_pDevice->CreateTexture2D(&texDesc, nullptr, &pDepthTexture)))
			return E_FAIL;

		m_pContext->CopyResource(pDepthTexture, m_resourse);

		Safe_Release(m_pRSV);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		if (FAILED(m_pDevice->CreateShaderResourceView(pDepthTexture, &srvDesc, &m_pRSV)))
			return E_FAIL;
		Safe_Release(pDepthTexture);
		pShader->Bind_SRV("g_DepthTexture", m_pRSV);
	}

	if (FAILED(pShader->Begin(m_tData.iBegin + m_iRenderCount)))
		return E_FAIL;
	return S_OK;
}

CTrailData* CTrailData::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrailData* pInstance = new CTrailData(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : pGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTrailData::Clone(void* pArg)
{
	CTrailData* pInstance = new CTrailData(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTrailData");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrailData::Free()
{
	__super::Free();
	for (_uint i = 0; i < 3; ++i)
		Safe_Release(m_pTexture[i]);

	Safe_Release(m_pOriginalDSV);
	Safe_Release(m_resourse);
	Safe_Release(m_pRSV);
}
