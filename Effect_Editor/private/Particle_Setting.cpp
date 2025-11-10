#include "pch.h"
#include "GameObject.h"
#include "Particle_Setting.h"
#include "GameInstance.h"

CParticle_Setting::CParticle_Setting(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CImgBase{ pDevice, pContext } {}

CParticle_Setting* CParticle_Setting::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    auto p = new CParticle_Setting(pDevice, pContext);
    if (FAILED(p->Initialize()))
    {
        Safe_Release(p);
        MSG_BOX("CREATE FAIL : CParticle_Setting");
        return nullptr;
    }
    return p;
}

HRESULT CParticle_Setting::Initialize()
{
    m_pShaderCom = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointParticle.hlsl"), VTX_POS_INSTANCE_PARTICLE::Elements, VTX_POS_INSTANCE_PARTICLE::iNumElements);
    m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0, 0, 0, 1));


    m_iNumInstance = 100;
    m_fCenter = _float3(0.0f, 0.f, 0.f);
    m_fRange = _float3(1.f, 1.f, 1.f);
    m_fSize = _float2(0.05f, 0.1f);
    m_fLifeTime = _float2(0.5f, 1.f);
    m_fSpeed = _float2(1.f, 0.5f);
    m_bisLoop = true;

    CVIBuffer_Point_Instance::POINT_INSTANCE_DESC		desc{};
    desc.iNumInstance = m_iNumInstance;
    desc.vCenter = m_fCenter;
    desc.vRange = m_fRange;
    desc.vSize = m_fSize;
    desc.vLifeTime = m_fLifeTime;
    desc.vSpeed = m_fSpeed;
    desc.isLoop = m_bisLoop;
    m_fColor = { 0,0,0,1 };
    m_pVIBufferCom = CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, &desc);
    m_pVIBufferCom->Initialize(nullptr);



    m_ImageFiles.clear();

    vector<string> ImageFiles;
    char pattern[MAX_PATH] = {};
    strcpy_s(pattern, MAX_PATH, "../Bin/Resources/Textures/*.dds");

    WIN32_FIND_DATAA fd{};
    HANDLE h = FindFirstFileA(pattern, &fd);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                char szFilePath[MAX_PATH] = {};
                strcpy_s(szFilePath, MAX_PATH, "../Bin/Resources/Textures/");
                strcat_s(szFilePath, MAX_PATH, fd.cFileName);
                ImageFiles.emplace_back(szFilePath);
                m_ImageFiles.emplace_back(fd.cFileName);
            }
        } while (FindNextFileA(h, &fd));
        FindClose(h);
    }
    for (auto Texture : m_pTextures) {
        Safe_Release(Texture);
    }
    m_pTextures.clear();
    m_SRVs.clear();
    for (auto ImageFile : ImageFiles) {
        _tchar szPath[256] = { 0, };
        MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, ImageFile.c_str(), strlen(ImageFile.c_str()), szPath, 256);
        m_pTextures.push_back(CTexture::Create(m_pDevice, m_pContext, szPath, 1));

        _tchar			szEXT[MAX_PATH] = {};

        _wsplitpath_s(szPath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

        _tchar			szFullPath[MAX_PATH] = {};

        ID3D11ShaderResourceView* pSRV = { nullptr };

        HRESULT			hr = { };
        if (false == lstrcmp(szEXT, TEXT(".dds")))
            hr = CreateDDSTextureFromFile(m_pDevice, szPath, nullptr, &pSRV);
        else if (false == lstrcmp(szEXT, TEXT(".tga")))
            hr = E_FAIL;
        else
            hr = CreateWICTextureFromFile(m_pDevice, szPath, nullptr, &pSRV);
        if (FAILED(hr))
            return E_FAIL;
        m_SRVs.push_back(pSRV);
    }

    for (_uint i = 0; i < 3; ++i) {
        m_pTexture[i] = m_pTextures[0];
        szFile[i] = m_ImageFiles[0];
    }
    return S_OK;
}

HRESULT CParticle_Setting::Save_Binary(const _tchar* pFilePath)
{
    return S_OK;
}

HRESULT CParticle_Setting::Load_Binary(const _tchar* pFilePath)
{
    return S_OK;
}

void CParticle_Setting::Update(_float fTimeDelta)
{
    ImGui::SetNextWindowSizeConstraints(
        ImVec2(100, 100),
        ImVec2(400, 600) 
    );
    ImGui::Begin("Tools", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
    ImVec2 btn = { 120, ImGui::GetFrameHeight() };
    if (ImGui::Button("Particle", btn)) {
        m_iLevel = 0;
    }
    ImGui::SameLine();
    if (ImGui::Button("Shader", btn)) {
        m_iLevel = 1;
    }

    switch (m_iLevel)
    {
    case 0:
    {
        _float3 f;

        static _float fTime = 10;
        static _uint  iSelect = 0;
        static vector<DiagramData> value;
        static _float values[100] = {};

        CVIBuffer_Point_Instance::POINT_INSTANCE_DESC		desc{};
        desc.iNumInstance = m_iNumInstance;
        desc.vCenter = m_fCenter;
        desc.vRange = m_fRange;
        desc.vSize = m_fSize;
        desc.vLifeTime = m_fLifeTime;
        desc.vSpeed = m_fSpeed;
        desc.isLoop = m_bisLoop;
        ImGui::DragInt("NumInstance", &m_iNumInstance, 1, 0, 0);
        ImGui::DragFloat3("Center", reinterpret_cast<_float*>(&m_fCenter), 0.1f, -100.f, 100.f);
        ImGui::DragFloat3("Range", reinterpret_cast<_float*>(&m_fRange), 0.1f, -100.f, 100.f);
        ImGui::DragFloat2("Size", reinterpret_cast<_float*>(&m_fSize), 0.1f, 0.f, 100.f);
        ImGui::DragFloat2("LifeTime", reinterpret_cast<_float*>(&m_fLifeTime), 0.1f, 0.f, 100.f);
        ImGui::DragFloat2("Speed", reinterpret_cast<_float*>(&m_fSpeed), 0.1f, 0.f, 100.f);
        ImGui::Checkbox("Loop", &m_bisLoop);
        if (desc.iNumInstance != m_iNumInstance ||
            desc.vCenter.x != m_fCenter.x ||
            desc.vCenter.y != m_fCenter.y ||
            desc.vCenter.z != m_fCenter.z ||
            desc.vRange.x != m_fRange.x ||
            desc.vRange.y != m_fRange.y ||
            desc.vRange.z != m_fRange.z ||
            desc.vSize.x != m_fSize.x ||
            desc.vSize.y != m_fSize.y ||
            desc.vLifeTime.x != m_fLifeTime.x ||
            desc.vLifeTime.y != m_fLifeTime.y ||
            desc.vSpeed.x != m_fSpeed.x ||
            desc.vSpeed.y != m_fSpeed.y ||
            desc.isLoop != m_bisLoop) {
            desc.iNumInstance = m_iNumInstance;
            desc.vCenter = m_fCenter;
            desc.vRange = m_fRange;
            desc.vSize = m_fSize;
            desc.vLifeTime = m_fLifeTime;
            desc.vSpeed = m_fSpeed;
            desc.isLoop = m_bisLoop;
            Safe_Release(m_pVIBufferCom);
            m_pVIBufferCom = CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, &desc);
            m_pVIBufferCom->Initialize(nullptr);
        }

        _float3 pos;
        XMStoreFloat3(&pos, m_pTransformCom->Get_State(STATE::POSITION));
        ImGui::DragFloat3("Position", reinterpret_cast<_float*>(&pos), 0.1f, -1000.f, 1000.f);
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat3(&pos));
        ImGui::SetNextItemWidth(180);

        _float pitch, yaw, roll;
        _vector vRight = XMVector4Normalize(m_pTransformCom->Get_State(STATE::RIGHT) / m_pTransformCom->Get_Scale().x);
        _vector vUp = XMVector4Normalize(m_pTransformCom->Get_State(STATE::UP) / m_pTransformCom->Get_Scale().y);
        _vector vLook = XMVector4Normalize(m_pTransformCom->Get_State(STATE::LOOK) / m_pTransformCom->Get_Scale().z);
        _float _11 = XMVectorGetX(vRight);
        _float _12 = XMVectorGetY(vRight);
        _float _13 = XMVectorGetZ(vRight);

        _float _21 = XMVectorGetX(vUp);
        _float _22 = XMVectorGetY(vUp);
        _float _23 = XMVectorGetZ(vUp);

        _float _31 = XMVectorGetX(vLook);
        _float _32 = XMVectorGetY(vLook);
        _float _33 = XMVectorGetZ(vLook);

        pitch = asinf(-_32);
        if (cosf(pitch) > 0.0001f)
        {
            yaw = atan2f(_31, _33);
            roll = atan2f(_12, _22);
        }
        else
        {
            yaw = atan2f(-_13, _11);
            roll = 0.0f;
        }
        _float3 rot = { XMConvertToDegrees(pitch),XMConvertToDegrees(yaw), XMConvertToDegrees(roll) };
        ImGui::DragFloat3("Rotation", reinterpret_cast<_float*>(&rot), 1.f, 0.f, 360.f);
        m_pTransformCom->Rotation(XMConvertToRadians(rot.x), XMConvertToRadians(rot.y), XMConvertToRadians(rot.z));


        if (iSelect < value.size()) {
            _float time = 1.f / 100;
            _float fMax = 5;
            for (_uint i = 0; i < 100; ++i) {
                DiagramData* in = {};
                DiagramData* out = {};
                for (_uint j = 0; j < (_uint)value.size(); ++j) {
                    if (value[j].fTime <= time * i) {
                        in = &value[j];
                    }
                    if (value[j].fTime > time * i) {
                        out = &value[j];
                        break;
                    }
                }
                if (nullptr == out)
                    values[i] = in->fValue;
                else {
                    _float t = (time * i - in->fTime) / (out->fTime - in->fTime);
                    if (fabsf(in->fX) >= 90.f || fabsf(out->fX) >= 90.f) {
                        values[i] = in->fValue;
                    }
                    else {
                        values[i] = (2 * powf(t, 3) - 3 * powf(t, 2) + 1) * in->fValue
                            + (powf(t, 3) - 2 * powf(t, 2) + t) * (tanf(XMConvertToRadians(in->fX)) * (out->fTime - in->fTime) * 100)
                            + (-2 * powf(t, 3) + 3 * powf(t, 2)) * out->fValue
                            + (powf(t, 3) - powf(t, 2)) * (tanf(XMConvertToRadians(out->fX)) * (out->fTime - in->fTime) * 100);
                    }
                }
                fMax = max(fMax, fabsf(values[i]));
            }
            ImGui::PlotLines("Sine Wave", values, IM_ARRAYSIZE(values), 0,
                "sample data", -fMax, fMax, ImVec2(0, 100));


            char str[10];
            snprintf(str, sizeof(str), "%d. %.2f", iSelect + 1, value[iSelect].fTime);

            if (ImGui::BeginCombo("TimeValue", str))
            {
                for (_uint i = 0; i < (_uint)value.size(); ++i) {
                    _bool sel = i == iSelect;
                    snprintf(str, sizeof(str), "%d. %.2f", i + 1, value[i].fTime);
                    if (ImGui::Selectable(str, sel))
                        iSelect = i;
                    if (sel)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }
        if (ImGui::Button("AddTime", btn)) {
            if (0 < value.size()) {
                DiagramData val = value.back();
                val.fX = 0;
                value.push_back(val);
            }
            else {
                DiagramData val = { 0,0,0 };
                value.push_back(val);
            }
            iSelect = value.size() - 1;
        }
        ImGui::SameLine();
        if (ImGui::Button("DeleteTime", btn)) {
            _uint iCount = 0;
            for (auto i = value.begin(); i != value.end();) {
                if (iCount == iSelect) {
                    value.erase(i);
                    break;
                }
                ++i;
                ++iCount;
            }
            iSelect = max(iSelect - 1, 0);
        }

        if (iSelect < value.size()) {
            if (0 < iSelect)
                ImGui::DragFloat("Time", &value[iSelect].fTime, 0.01f, value[iSelect - 1].fTime, iSelect != value.size() - 1 ? value[iSelect + 1].fTime : 1);
            ImGui::DragFloat("fx", &value[iSelect].fX, 1.f, -90.f, 90.f);
            ImGui::DragFloat("fValue", &value[iSelect].fValue, 0.01f, -100.f, 100.f);
        }
    }
    break;
    case 1:
    {
        _float4 fColor = m_fColor;
        ImGui::ColorPicker4("MyColor", (_float*)&m_fColor, ImGuiColorEditFlags_PickerHueWheel);

        if (ImGui::Button("Refresh", btn)) {
            m_ImageFiles.clear();

            vector<string> ImageFiles;
            char pattern[MAX_PATH] = {};
            strcpy_s(pattern, MAX_PATH, "../Bin/Resources/Textures/*.dds");

            WIN32_FIND_DATAA fd{};
            HANDLE h = FindFirstFileA(pattern, &fd);
            if (h != INVALID_HANDLE_VALUE) {
                do {
                    if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                        char szFilePath[MAX_PATH] = {};
                        strcpy_s(szFilePath, MAX_PATH, "../Bin/Resources/Textures/");
                        strcat_s(szFilePath, MAX_PATH, fd.cFileName);
                        ImageFiles.emplace_back(szFilePath);
                        m_ImageFiles.emplace_back(fd.cFileName);
                    }
                } while (FindNextFileA(h, &fd));
                FindClose(h);
            }
            for (auto Texture : m_pTextures) {
                Safe_Release(Texture);
            }
            m_pTextures.clear();
            m_SRVs.clear();
            for (_uint i = 0; i < 3; ++i) {
                m_pTexture[i] = nullptr;
            }
            _uint i = 0;
            for (auto ImageFile : ImageFiles) {
                _tchar szPath[256] = { 0, };
                MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, ImageFile.c_str(), strlen(ImageFile.c_str()), szPath, 256);
                m_pTextures.push_back(CTexture::Create(m_pDevice, m_pContext, szPath, 1));

                _tchar			szEXT[MAX_PATH] = {};

                _wsplitpath_s(szPath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

                _tchar			szFullPath[MAX_PATH] = {};

                ID3D11ShaderResourceView* pSRV = { nullptr };

                HRESULT			hr = { };
                if (false == lstrcmp(szEXT, TEXT(".dds")))
                    hr = CreateDDSTextureFromFile(m_pDevice, szPath, nullptr, &pSRV);
                else if (false == lstrcmp(szEXT, TEXT(".tga")))
                    hr = E_FAIL;
                else
                    hr = CreateWICTextureFromFile(m_pDevice, szPath, nullptr, &pSRV);
                if (FAILED(hr))
                    return;
                m_SRVs.push_back(pSRV);

                for (_uint j = 0; j < 3; ++j) {
                    if (szFile[j] == m_ImageFiles[i]) {
                        m_pTexture[j] = m_pTextures[i];
                    }
                }
                ++i;
            }

            for (_uint j = 0; j < 3; ++j) {
                if (nullptr == m_pTexture[i]) {
                    szFile[j] = m_ImageFiles[0];
                    m_pTexture[j] = m_pTextures[0];
                }
            }
        }

        if (ImGui::Button("Refresh Shader", btn)) {
            Safe_Release(m_pShaderCom);
            m_pShaderCom = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointParticle.hlsl"), VTX_POS_INSTANCE_PARTICLE::Elements, VTX_POS_INSTANCE_PARTICLE::iNumElements);
        }
        ImGui::InputInt("Shader Begine", &m_iShaderBegine);

        if (ImGui::BeginCombo("ImageType", m_iImageType == 0 ? "Mask" : m_iImageType == 1 ? "Diffuse" : "Dissolve"))
        {
            for (_uint i = 0; i < 3; ++i) {
                _bool sel = i == m_iImageType;
                if (ImGui::Selectable(i == 0 ? "Mask" : i == 1 ? "Diffuse" : "Dissolve", sel))
                    m_iImageType = i;
                if (sel)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::Separator();

        ImVec2 child_size = ImVec2(250, 200);
        ImGui::BeginChild("ImageScroll", child_size, true);
        if (0 < m_SRVs.size()) {
            _uint i = 0;
            for (auto SRV : m_SRVs) {
                if (ImGui::ImageButton(m_ImageFiles[i].c_str(), (ImTextureRef)SRV, ImVec2(100, 100))) {
                    m_pTexture[m_iImageType] = m_pTextures[i];
                    szFile[m_iImageType] = m_ImageFiles[i];
                }
                if(1 == ++i % 2)
                    ImGui::SameLine();
            }
        }
        ImGui::EndChild();
    }
    break;
    }

    ImGui::End();

    m_pVIBufferCom->Spread(fTimeDelta);
}

HRESULT CParticle_Setting::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float3))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_fColor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pTexture[0]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
        return E_FAIL;

    if (FAILED(m_pTexture[1]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
        return E_FAIL;

    if (FAILED(m_pTexture[2]->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture", 0)))
        return E_FAIL;


    m_pShaderCom->Begin(m_iShaderBegine);


    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();
    return S_OK;
}

void CParticle_Setting::Free()
{
    __super::Free();
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTransformCom);
    for (auto texture : m_pTextures)
        Safe_Release(texture);
    for (auto SRV : m_SRVs)
        Safe_Release(SRV);
    for (int i = 0; i < 3; ++i)
        Safe_Release(m_pTexture[i]);
}
