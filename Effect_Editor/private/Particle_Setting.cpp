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
    m_tParticleData.iNumInstance = 100;
    m_tParticleData.iBegin = 0;
    m_tParticleData.fCenter = _float3(0.0f, 0.f, 0.f);
    m_tParticleData.fPivot = _float3(0.0f, 0.f, 0.0f);
    m_tParticleData.fRange = _float3(1.f, 1.f, 1.f);
    m_tParticleData.fSize = _float2(0.5f, 1.f);
    m_tParticleData.fLifeTime = _float2(0.5f, 1.f);
    m_tParticleData.fSpeed = _float2(1.f, 0.5f);
    m_tParticleData.bisLoop = true;
    m_tParticleData.fGravityDiagram = _float4(0, 0, 0, 0);
    m_tParticleData.iSelectRender = 3;
    m_tParticleData.fColor = { 0,0,0,1 };
    m_tParticleData.szCS = "CS";
    m_tParticleData.fSizeDiagrams.clear();
    m_tParticleData.fSizeDiagrams.push_back(_float3(0,1,0));
    m_tParticleData.fSizeDiagrams.push_back(_float3(1, 1, 0));
    m_szCS = "CS";

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
    for (auto SRV : m_SRVs) {
        Safe_Release(SRV);
    }
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

    strcpy_s(pattern, MAX_PATH, "../Bin/Resources/Models/*.bin");

    h = FindFirstFileA(pattern, &fd);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {

                char szFilePath[MAX_PATH] = {};
                strcpy_s(szFilePath, MAX_PATH, "../Bin/Resources/Models/");
                strcat_s(szFilePath, MAX_PATH, fd.cFileName);
                m_ModelFilePaths.push_back(fd.cFileName);
                m_ModelFiles.push_back(szFilePath);
            }
        } while (FindNextFileA(h, &fd));
        FindClose(h);
    }



    CVIBuffer_Point_Instance::POINT_INSTANCE_DESC		Desc{};

    Desc.iNumInstance = m_tParticleData.iNumInstance;
    Desc.vCenter = m_tParticleData.fCenter;
    Desc.vPivot = m_tParticleData.fPivot;
    Desc.vRange = m_tParticleData.fRange;
    Desc.vSize = m_tParticleData.fSize;
    Desc.vLifeTime = m_tParticleData.fLifeTime;
    Desc.vSpeed = m_tParticleData.fSpeed;
    Desc.isLoop = m_tParticleData.bisLoop;

    m_pParticles.clear();
    CParticle* pParticle = CParticle::Create(m_pDevice, m_pContext);
    pParticle->Initialize(nullptr);

    pParticle->Set_Components(m_tParticleData);

    for (_uint i = 0; i < 3; ++i) {
        pParticle->Set_Texture(i, m_pTextures[0], m_ImageFiles[0]);
    }
    m_iSelectParticle = 0;
    m_pParticles.push_back(pParticle);

    CParticle* pParticle2 = CParticle::Create(m_pDevice, m_pContext);
    pParticle2->Initialize(nullptr);

    pParticle2->Set_Components(m_tParticleData);

    for (_uint i = 0; i < 3; ++i) {
        pParticle2->Set_Texture(i, m_pTextures[0], m_ImageFiles[0]);
    }

    m_iSelectParticle = 0;
    m_pParticles.push_back(pParticle2);

    CMeshEffect::MeshEffectData		MeshDesc{};
    MeshDesc.fColor = { 0,0,0,1 };
    MeshDesc.iBegin = 0;
    MeshDesc.iSelectRender = 3;
    MeshDesc.fScale = { 1,1,1 };


    CMeshEffect* pMeshEffect = CMeshEffect::Create(m_pDevice, m_pContext);
    pMeshEffect->Initialize(nullptr);

    pMeshEffect->Set_Components(MeshDesc);
    _tchar szPath[256] = { 0, };
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_ModelFiles[0].c_str(), strlen(m_ModelFiles[0].c_str()), szPath, 256);
    pMeshEffect->Set_Model(szPath);

    for (_uint i = 0; i < 3; ++i) {
        pMeshEffect->Set_Texture(i, m_pTextures[0], m_ImageFiles[0]);
    }

    m_iSelectMesh = 0;
    m_pMeshs.push_back(pMeshEffect);
    m_tMeshData = MeshDesc;
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

    if (ImGui::BeginCombo("Effect Type", m_iSelectMeshParticle == 0 ? "Particle" : "MeshEffect"))
    {
        for (_uint i = 0; i < 2; ++i) {
            _bool sel = i == m_iSelectMeshParticle;
            if (ImGui::Selectable(i == 0 ? "Particle" : "MeshEffect", sel)) {
                m_iSelectMeshParticle = i;
            }
            if (sel)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImVec2 btn = { 120, ImGui::GetFrameHeight() };
    switch (m_iSelectMeshParticle)
    {
    case 0: {
        if (ImGui::Button("Effect", btn)) {
            m_iLevel = 0;
        }
        ImGui::SameLine();
        if (ImGui::Button("Shader", btn)) {
            m_iLevel = 1;
        }
        char str[3];
        snprintf(str, sizeof(str), "%d", m_iSelectParticle);
        if (ImGui::BeginCombo("Particles", str))
        {
            for (_uint i = 0; i < m_pParticles.size(); ++i) {
                _bool sel = i == m_iSelectParticle;
                snprintf(str, sizeof(str), "%d", i);
                if (ImGui::Selectable(str, sel)) {
                    m_iSelectParticle = i;
                    m_tParticleData = m_pParticles[m_iSelectParticle]->Get_Data();
                }
                if (sel)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        if (ImGui::Button("All Replay", btn)) {
            for (_uint i = 0; i < m_pParticles.size(); ++i) {
                m_pParticles[i]->Set_Components(m_pParticles[i]->Get_Data());
            }
            for (_uint i = 0; i < m_pMeshs.size(); ++i) {
                m_pMeshs[i]->Set_Components(m_pMeshs[i]->Get_Data());
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Replay", btn)) {
            m_pParticles[m_iSelectParticle]->Set_Components(m_pParticles[m_iSelectParticle]->Get_Data());
        }
        m_pParticles[m_iSelectParticle]->Update(m_tParticleData);


        ImGui::BeginChild("Menu", ImVec2(300, 500), true);
        switch (m_iLevel)
        {
        case 0:
        {
            m_tParticleData = m_pParticles[m_iSelectParticle]->Get_Data();
            CVIBuffer_Point_Instance::POINT_INSTANCE_DESC		Desc{};
            Desc.iNumInstance = m_tParticleData.iNumInstance;
            Desc.vCenter = m_tParticleData.fCenter;
            Desc.vPivot = m_tParticleData.fPivot;
            Desc.vRange = m_tParticleData.fRange;
            Desc.vSize = m_tParticleData.fSize;
            Desc.vLifeTime = m_tParticleData.fLifeTime;
            Desc.vSpeed = m_tParticleData.fSpeed;
            Desc.isLoop = m_tParticleData.bisLoop;
            _float4 fGravity = m_tParticleData.fGravityDiagram;
            vector<_float3> fSizeDiagrams = m_tParticleData.fSizeDiagrams;

            ImGui::DragInt("NumInstance", &m_tParticleData.iNumInstance, 1, 0, 0);
            ImGui::DragFloat3("Center", reinterpret_cast<_float*>(&m_tParticleData.fCenter), 0.1f, -100.f, 100.f);
            ImGui::DragFloat3("Pivot", reinterpret_cast<_float*>(&m_tParticleData.fPivot), 0.1f, -100.f, 100.f);
            ImGui::DragFloat3("Range", reinterpret_cast<_float*>(&m_tParticleData.fRange), 0.1f, -100.f, 100.f);
            ImGui::DragFloat2("Size", reinterpret_cast<_float*>(&m_tParticleData.fSize), 0.1f, 0.f, 100.f);

            _float time = 1.f / 100;
            _float fMax = 5;
            //for (_uint i = 0; i < 100; ++i) {
            //    _float t = time * i;
            //    if (fabsf(m_tParticleData.fSizeDiagram.y) >= 90.f || fabsf(m_tParticleData.fSizeDiagram.w) >= 90.f) {
            //        values[i] = m_tParticleData.fSizeDiagram.x;
            //    }
            //    else {
            //
            //        values[i] = (2 * powf(t, 3) - 3 * powf(t, 2) + 1) * m_tParticleData.fSizeDiagram.x
            //            + (powf(t, 3) - 2 * powf(t, 2) + t) * tanf(XMConvertToRadians(m_tParticleData.fSizeDiagram.y)) * 100
            //            + (-2 * powf(t, 3) + 3 * powf(t, 2)) * m_tParticleData.fSizeDiagram.z
            //            + (powf(t, 3) - powf(t, 2)) * tanf(XMConvertToRadians(m_tParticleData.fSizeDiagram.w)) * 100;
            //    }
            //    fMax = max(fMax, fabsf(values[i]));
            //}
            //ImGui::PlotLines("Size Wave", values, IM_ARRAYSIZE(values), 1,
            //    "Size Data", -fMax, fMax, ImVec2(0, 100));
            //
            //ImGui::DragFloat("Start Size", &m_tParticleData.fSizeDiagram.x, 0.01f, 0.f, 100.f);
            //ImGui::DragFloat("Start Size fx", &m_tParticleData.fSizeDiagram.y, 1.f, -90.f, 90.f);
            //ImGui::DragFloat("End Size", &m_tParticleData.fSizeDiagram.z, 0.01f, 0.f, 100.f);
            //ImGui::DragFloat("End Size fx", &m_tParticleData.fSizeDiagram.w, 1.f, -90.f, 90.f);




            float values[100] = {};
            if (m_iSelectSize < m_tParticleData.fSizeDiagrams.size()) {
                _float time = 1.f / 100;
                _float fMax = 5;
                for (_uint i = 0; i < 100; ++i) {
                    _float3* in = {};
                    _float3* out = {};
                    for (_uint j = 0; j < (_uint)m_tParticleData.fSizeDiagrams.size(); ++j) {
                        if (m_tParticleData.fSizeDiagrams[j].x <= time * i) {
                            in = &m_tParticleData.fSizeDiagrams[j];
                        }
                        if (m_tParticleData.fSizeDiagrams[j].x > time * i) {
                            out = &m_tParticleData.fSizeDiagrams[j];
                            break;
                        }
                    }
                    if (nullptr == out)
                        values[i] = in->y;
                    else {
                        _float t = (time * i - in->x) / (out->x - in->x);
                        if (fabsf(in->z) >= 90.f || fabsf(out->z) >= 90.f) {
                            values[i] = in->y;
                        }
                        else {
                            values[i] = (2 * powf(t, 3) - 3 * powf(t, 2) + 1) * in->y
                                + (powf(t, 3) - 2 * powf(t, 2) + t) * (tanf(XMConvertToRadians(in->z)) * (out->x - in->x) * 100)
                                + (-2 * powf(t, 3) + 3 * powf(t, 2)) * out->y
                                + (powf(t, 3) - powf(t, 2)) * (tanf(XMConvertToRadians(out->z)) * (out->x - in->x) * 100);
                        }
                    }
                    fMax = max(fMax, fabsf(values[i]));
                }
                ImGui::PlotLines("Size Wave", values, IM_ARRAYSIZE(values), 0,
                    "Size data", -fMax, fMax, ImVec2(0, 100));


                char str[10];
                snprintf(str, sizeof(str), "%d. %.2f", m_iSelectSize + 1, m_tParticleData.fSizeDiagrams[m_iSelectSize].x);

                if (ImGui::BeginCombo("TimeValue", str))
                {
                    for (_uint i = 0; i < (_uint)m_tParticleData.fSizeDiagrams.size(); ++i) {
                        _bool sel = i == m_iSelectSize;
                        snprintf(str, sizeof(str), "%d. %.2f", i + 1, m_tParticleData.fSizeDiagrams[i].x);
                        if (ImGui::Selectable(str, sel))
                            m_iSelectSize = i;
                        if (sel)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
            }
            if (ImGui::Button("AddTime", btn)) {
                if (0 < m_tParticleData.fSizeDiagrams.size()) {
                    _float3 val = m_tParticleData.fSizeDiagrams.back();
                    val.y = 0;
                    m_tParticleData.fSizeDiagrams.push_back(val);
                }
                else {
                    _float3 val = { 0,0,0 };
                    m_tParticleData.fSizeDiagrams.push_back(val);
                }
                m_iSelectSize = m_tParticleData.fSizeDiagrams.size() - 1;
            }
            ImGui::SameLine();
            if (ImGui::Button("DeleteTime", btn)) {
                _uint iCount = 0;
                for (auto i = m_tParticleData.fSizeDiagrams.begin(); i != m_tParticleData.fSizeDiagrams.end();) {
                    if (iCount == m_iSelectSize) {
                        m_tParticleData.fSizeDiagrams.erase(i);
                        break;
                    }
                    ++i;
                    ++iCount;
                }
                m_iSelectSize = max(m_iSelectSize - 1, 0);
            }

            if (m_iSelectSize < m_tParticleData.fSizeDiagrams.size()) {
                if (0 < m_iSelectSize)
                    ImGui::DragFloat("Size Time", &m_tParticleData.fSizeDiagrams[m_iSelectSize].x, 0.01f, m_tParticleData.fSizeDiagrams[m_iSelectSize - 1].x, m_iSelectSize != m_tParticleData.fSizeDiagrams.size() - 1 ? m_tParticleData.fSizeDiagrams[m_iSelectSize + 1].x : 1);
                ImGui::DragFloat("Size fx", &m_tParticleData.fSizeDiagrams[m_iSelectSize].z, 1.f, -90.f, 90.f);
                ImGui::DragFloat("Size", &m_tParticleData.fSizeDiagrams[m_iSelectSize].y, 0.01f, -100.f, 100.f);
            }







            ImGui::DragFloat2("LifeTime", reinterpret_cast<_float*>(&m_tParticleData.fLifeTime), 0.1f, 0.f, 100.f);
            ImGui::DragFloat2("Speed", reinterpret_cast<_float*>(&m_tParticleData.fSpeed), 0.1f, 0.f, 100.f);
            ImGui::Checkbox("Loop", &m_tParticleData.bisLoop);

            _float3 pos;
            XMStoreFloat3(&pos, m_pParticles[m_iSelectParticle]->GetTransform()->Get_State(STATE::POSITION));
            ImGui::DragFloat3("Position", reinterpret_cast<_float*>(&pos), 0.1f, -1000.f, 1000.f);
            m_pParticles[m_iSelectParticle]->GetTransform()->Set_State(STATE::POSITION, XMLoadFloat3(&pos));
            ImGui::SetNextItemWidth(180);

            _float pitch, yaw, roll;
            _vector vRight = XMVector4Normalize(m_pParticles[m_iSelectParticle]->GetTransform()->Get_State(STATE::RIGHT) / m_pParticles[m_iSelectParticle]->GetTransform()->Get_Scale().x);
            _vector vUp = XMVector4Normalize(m_pParticles[m_iSelectParticle]->GetTransform()->Get_State(STATE::UP) / m_pParticles[m_iSelectParticle]->GetTransform()->Get_Scale().y);
            _vector vLook = XMVector4Normalize(m_pParticles[m_iSelectParticle]->GetTransform()->Get_State(STATE::LOOK) / m_pParticles[m_iSelectParticle]->GetTransform()->Get_Scale().z);
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
            m_pParticles[m_iSelectParticle]->GetTransform()->Rotation(XMConvertToRadians(rot.x), XMConvertToRadians(rot.y), XMConvertToRadians(rot.z));


            for (_uint i = 0; i < 100; ++i) {
                _float t = time * i;
                if (fabsf(m_tParticleData.fGravityDiagram.y) >= 90.f || fabsf(m_tParticleData.fGravityDiagram.w) >= 90.f) {
                    values[i] = m_tParticleData.fGravityDiagram.x;
                }
                else {

                    values[i] = (2 * powf(t, 3) - 3 * powf(t, 2) + 1) * m_tParticleData.fGravityDiagram.x
                        + (powf(t, 3) - 2 * powf(t, 2) + t) * tanf(XMConvertToRadians(m_tParticleData.fGravityDiagram.y)) * 100
                        + (-2 * powf(t, 3) + 3 * powf(t, 2)) * m_tParticleData.fGravityDiagram.z
                        + (powf(t, 3) - powf(t, 2)) * tanf(XMConvertToRadians(m_tParticleData.fGravityDiagram.w)) * 100;
                }
                fMax = max(fMax, fabsf(values[i]));
            }
            ImGui::PlotLines("Gravity Wave", values, IM_ARRAYSIZE(values), 0,
                "Gravity Data", -fMax, fMax, ImVec2(0, 100));

            ImGui::DragFloat("Start Gravity", &m_tParticleData.fGravityDiagram.x, 0.01f, -100.f, 100.f);
            ImGui::DragFloat("Start Gravity fx", &m_tParticleData.fGravityDiagram.y, 1.f, -90.f, 90.f);
            ImGui::DragFloat("End Gravity", &m_tParticleData.fGravityDiagram.z, 0.01f, -100.f, 100.f);
            ImGui::DragFloat("End Gravity fx", &m_tParticleData.fGravityDiagram.w, 1.f, -90.f, 90.f);


            if (Desc.iNumInstance != m_tParticleData.iNumInstance ||
                Desc.vCenter.x != m_tParticleData.fCenter.x ||
                Desc.vCenter.y != m_tParticleData.fCenter.y ||
                Desc.vCenter.z != m_tParticleData.fCenter.z ||
                Desc.vPivot.x != m_tParticleData.fPivot.x ||
                Desc.vPivot.y != m_tParticleData.fPivot.y ||
                Desc.vPivot.z != m_tParticleData.fPivot.z ||
                Desc.vRange.x != m_tParticleData.fRange.x ||
                Desc.vRange.y != m_tParticleData.fRange.y ||
                Desc.vRange.z != m_tParticleData.fRange.z ||
                Desc.vSize.x != m_tParticleData.fSize.x ||
                Desc.vSize.y != m_tParticleData.fSize.y ||
                Desc.vLifeTime.x != m_tParticleData.fLifeTime.x ||
                Desc.vLifeTime.y != m_tParticleData.fLifeTime.y ||
                Desc.vSpeed.x != m_tParticleData.fSpeed.x ||
                Desc.vSpeed.y != m_tParticleData.fSpeed.y ||
                Desc.isLoop != m_tParticleData.bisLoop ||
                fGravity.x != m_tParticleData.fGravityDiagram.x ||
                fGravity.y != m_tParticleData.fGravityDiagram.y ||
                fGravity.z != m_tParticleData.fGravityDiagram.z ||
                fGravity.w != m_tParticleData.fGravityDiagram.w ||
                fSizeDiagrams.size() != m_tParticleData.fSizeDiagrams.size()) {
                Desc.iNumInstance = m_tParticleData.iNumInstance;
                Desc.vCenter = m_tParticleData.fCenter;
                Desc.vPivot = m_tParticleData.fPivot;
                Desc.vRange = m_tParticleData.fRange;
                Desc.vSize = m_tParticleData.fSize;
                Desc.vLifeTime = m_tParticleData.fLifeTime;
                Desc.vSpeed = m_tParticleData.fSpeed;
                Desc.isLoop = m_tParticleData.bisLoop;
                //m_tParticleData.fSizeDiagrams = fSizeDiagrams;
                m_pParticles[m_iSelectParticle]->Set_Components(m_tParticleData);
            }
            for (_uint i = 0; i < fSizeDiagrams.size(); ++i) {
                if (fSizeDiagrams[i].x != m_tParticleData.fSizeDiagrams[i].x ||
                    fSizeDiagrams[i].y != m_tParticleData.fSizeDiagrams[i].y ||
                    fSizeDiagrams[i].z != m_tParticleData.fSizeDiagrams[i].z) {
                    //m_tParticleData.fSizeDiagrams = fSizeDiagrams;
                    m_pParticles[m_iSelectParticle]->Set_Components(m_tParticleData);
                }
            }
        }
        break;
        case 1:
        {
            ImGui::ColorPicker4("MyColor", (_float*)&m_tParticleData.fColor, ImGuiColorEditFlags_PickerHueWheel);

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
                for (auto SRV : m_SRVs) {
                    Safe_Release(SRV);
                }
                m_SRVs.clear();
                _uint i = 0;
                _bool bisFlag = true;
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
                    if (bisFlag) {
                        for (_uint j = 0; j < m_pParticles.size(); ++j) {
                            for (_uint k = 0; k < 3; ++k) {
                                m_pParticles[j]->Set_Texture(k, m_pTextures[0], m_ImageFiles[0]);
                            }
                        }
                        for (_uint j = 0; j < m_pMeshs.size(); ++j) {
                            for (_uint k = 0; k < 3; ++k) {
                                m_pMeshs[j]->Set_Texture(k, m_pTextures[0], m_ImageFiles[0]);
                            }
                        }
                        bisFlag = false;
                    }
                    for (_uint j = 0; j < m_pParticles.size(); ++j) {
                        for (_uint k = 0; k < 3; ++k) {
                            if (m_pParticles[j]->Get_TextureName(k) == m_ImageFiles[i]) {
                                m_pParticles[j]->Set_Texture(k, m_pTextures[i], m_ImageFiles[i]);
                            }
                        }
                    }
                    for (_uint j = 0; j < m_pMeshs.size(); ++j) {
                        for (_uint k = 0; k < 3; ++k) {
                            if (m_pMeshs[j]->Get_TextureName(k) == m_ImageFiles[i]) {
                                m_pMeshs[j]->Set_Texture(k, m_pTextures[i], m_ImageFiles[i]);
                            }
                        }
                    }
                    ++i;
                }
            }

            string szRender;
            switch (m_tParticleData.iSelectRender)
            {
            case 2:
                szRender = "NONBLEND";
                break;
            case 3:
                szRender = "NONLIGHT";
                break;
            case 4:
                szRender = "BLUR";
                break;
            case 5:
                szRender = "DISTORTION";
                break;
            }
            if (ImGui::BeginCombo("RenderType", szRender.c_str()))
            {
                for (_uint i = 2; i < 6; ++i) {
                    _bool sel = i == m_tParticleData.iSelectRender;
                    switch (i)
                    {
                    case 2:
                        szRender = "NONBLEND";
                        break;
                    case 3:
                        szRender = "NONLIGHT";
                        break;
                    case 4:
                        szRender = "BLUR";
                        break;
                    case 5:
                        szRender = "DISTORTION";
                        break;
                    }
                    if (ImGui::Selectable(szRender.c_str(), sel))
                        m_tParticleData.iSelectRender = i;
                    if (sel)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            char szName[30] = {};
            strncpy_s(szName, sizeof(szName), m_szCS.c_str(), _TRUNCATE);
            if (ImGui::InputText("File", szName, sizeof(szName))) {
                m_szCS = szName;
            }
            if (ImGui::Button("Refresh Shader", btn)) {
                m_tParticleData.szCS = m_szCS;
                m_pParticles[m_iSelectParticle]->Set_Components(m_tParticleData);
            }
            ImGui::InputInt("Shader Begine", &m_tParticleData.iBegin);
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

            ImGui::BeginChild("ImageScroll", ImVec2(300, 200), true);
            if (0 < m_SRVs.size()) {
                _uint i = 0;
                for (auto SRV : m_SRVs) {
                    if (ImGui::ImageButton(m_ImageFiles[i].c_str(), (ImTextureRef)SRV, ImVec2(100, 100))) {
                        m_pParticles[m_iSelectParticle]->Set_Texture(m_iImageType, m_pTextures[i], m_ImageFiles[i]);
                    }
                    if (1 == ++i % 2)
                        ImGui::SameLine();
                }
            }
            ImGui::EndChild();
        }
        break;
        }
        ImGui::EndChild();


    }
        break;
    case 1: {
        char str[3];
        
        if (ImGui::BeginCombo("Models", m_ModelFilePaths[m_iSelectModel].c_str()))
        {
            for (_uint i = 0; i < m_ModelFilePaths.size(); ++i) {
                _bool sel = i == m_iSelectModel;
                if (ImGui::Selectable(m_ModelFilePaths[i].c_str(), sel)) {
                    m_iSelectModel = i;
                    _tchar szPath[256] = { 0, };
                    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_ModelFiles[m_iSelectModel].c_str(), strlen(m_ModelFiles[m_iSelectModel].c_str()), szPath, 256);
                    m_pMeshs[m_iSelectMesh]->Set_Model(szPath);
                }
                if (sel)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        snprintf(str, sizeof(str), "%d", m_iSelectMesh);
        if (ImGui::BeginCombo("Meshs", str))
        {
            for (_uint i = 0; i < m_pMeshs.size(); ++i) {
                _bool sel = i == m_iSelectMesh;
                snprintf(str, sizeof(str), "%d", i);
                if (ImGui::Selectable(str, sel)) {
                    m_iSelectMesh = i;
                    m_tMeshData = m_pMeshs[m_iSelectMesh]->Get_Data();
                }
                if (sel)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        if (ImGui::Button("All Replay", btn)) {
            for (_uint i = 0; i < m_pParticles.size(); ++i) {
                m_pParticles[i]->Set_Components(m_pParticles[i]->Get_Data());
            }
            for (_uint i = 0; i < m_pMeshs.size(); ++i) {
                m_pMeshs[i]->Set_Components(m_pMeshs[i]->Get_Data());
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Replay", btn)) {
            m_pMeshs[m_iSelectMesh]->Set_Components(m_pMeshs[m_iSelectMesh]->Get_Data());
        }
        m_pMeshs[m_iSelectMesh]->Update(m_tMeshData);



        ImGui::ColorPicker4("MyColor", (_float*)&m_tMeshData.fColor, ImGuiColorEditFlags_PickerHueWheel);

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
            for (auto SRV : m_SRVs) {
                Safe_Release(SRV);
            }
            m_SRVs.clear();
            _uint i = 0;
            _bool bisFlag = true;
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

                if (bisFlag) {
                    for (_uint j = 0; j < m_pParticles.size(); ++j) {
                        for (_uint k = 0; k < 3; ++k) {
                            m_pParticles[j]->Set_Texture(k, m_pTextures[0], m_ImageFiles[0]);
                        }
                    }
                    for (_uint j = 0; j < m_pMeshs.size(); ++j) {
                        for (_uint k = 0; k < 3; ++k) {
                            m_pMeshs[j]->Set_Texture(k, m_pTextures[0], m_ImageFiles[0]);
                        }
                    }
                    bisFlag = false;
                }
                for (_uint j = 0; j < m_pParticles.size(); ++j) {
                    for (_uint k = 0; k < 3; ++k) {
                        if (m_pParticles[j]->Get_TextureName(k) == m_ImageFiles[i]) {
                            m_pParticles[j]->Set_Texture(k, m_pTextures[i], m_ImageFiles[i]);
                        }
                    }
                }
                for (_uint j = 0; j < m_pMeshs.size(); ++j) {
                    for (_uint k = 0; k < 3; ++k) {
                        if (m_pMeshs[j]->Get_TextureName(k) == m_ImageFiles[i]) {
                            m_pMeshs[j]->Set_Texture(k, m_pTextures[i], m_ImageFiles[i]);
                        }
                    }
                }
                ++i;
            }
        }

        ImGui::DragFloat3("Scale", reinterpret_cast<_float*>(&m_tMeshData.fScale), 0.1f, 0.f, 100.f);

        string szRender;
        switch (m_tMeshData.iSelectRender)
        {
        case 2:
            szRender = "NONBLEND";
            break;
        case 3:
            szRender = "NONLIGHT";
            break;
        case 4:
            szRender = "BLUR";
            break;
        case 5:
            szRender = "DISTORTION";
            break;
        }
        if (ImGui::BeginCombo("RenderType", szRender.c_str()))
        {
            for (_uint i = 2; i < 6; ++i) {
                _bool sel = i == m_tMeshData.iSelectRender;
                switch (i)
                {
                case 2:
                    szRender = "NONBLEND";
                    break;
                case 3:
                    szRender = "NONLIGHT";
                    break;
                case 4:
                    szRender = "BLUR";
                    break;
                case 5:
                    szRender = "DISTORTION";
                    break;
                }
                if (ImGui::Selectable(szRender.c_str(), sel))
                    m_tMeshData.iSelectRender = i;
                if (sel)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        if (ImGui::Button("Refresh Shader", btn)) {
            m_pMeshs[m_iSelectMesh]->Set_Components(m_tMeshData);
        }
        ImGui::InputInt("Shader Begine", &m_tMeshData.iBegin);
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

        ImGui::BeginChild("ImageScroll", ImVec2(300, 200), true);
        if (0 < m_SRVs.size()) {
            _uint i = 0;
            for (auto SRV : m_SRVs) {
                if (ImGui::ImageButton(m_ImageFiles[i].c_str(), (ImTextureRef)SRV, ImVec2(100, 100))) {
                    m_pMeshs[m_iSelectMesh]->Set_Texture(m_iImageType, m_pTextures[i], m_ImageFiles[i]);
                }
                if (1 == ++i % 2)
                    ImGui::SameLine();
            }
        }
        ImGui::EndChild();

    }
        break;
    }


    
    ImGui::End();

    for (_uint i = 0; i < m_pParticles.size(); ++i) {
        m_pParticles[i]->Priority_Update(fTimeDelta);
        m_pParticles[i]->Update(fTimeDelta);
        m_pParticles[i]->Late_Update(fTimeDelta);
    }

    for (_uint i = 0; i < m_pMeshs.size(); ++i) {
        m_pMeshs[i]->Priority_Update(fTimeDelta);
        m_pMeshs[i]->Update(fTimeDelta);
        m_pMeshs[i]->Late_Update(fTimeDelta);
    }
    //m_pVIBufferCom->Spread(fTimeDelta);
}

HRESULT CParticle_Setting::Render()
{


    //m_pShaderCom->Begin(m_iShaderBegine);
    //
    //
    //m_pVIBufferCom->Bind_Resources();
    //
    //m_pVIBufferCom->Render();
    return S_OK;
}

void CParticle_Setting::Free()
{
    __super::Free();
    for (auto texture : m_pTextures)
        Safe_Release(texture);
    m_pTextures.clear();
    for (auto SRV : m_SRVs)
        Safe_Release(SRV);
    m_SRVs.clear();
    for(auto pParticle : m_pParticles)
        Safe_Release(pParticle);
    m_pParticles.clear();
    for (auto pMesh : m_pMeshs)
        Safe_Release(pMesh);
    m_pParticles.clear();
}
