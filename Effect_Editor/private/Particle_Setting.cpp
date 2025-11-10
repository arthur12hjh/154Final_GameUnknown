#include "pch.h"
#include "Particle_Setting.h"
#include "GameInstance.h"

//static CVIBuffer_Rect_Instance* pVIBufferCom = { nullptr };
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
    //CVIBuffer_Point_Instance::POINT_INSTANCE_DESC		desc{};
    //desc.iNumInstance = 7;
    //desc.vCenter = _float3(0.0f, 0.f, 0.f);
    //desc.vRange = _float3(0.5f, 0.5f, 1.f);
    //desc.vSize = _float2(0.05f, 0.1f);
    //desc.vLifeTime = _float2(0.1f, 0.4f);
    //desc.vSpeed = _float2(1.f, 2.f);
    //desc.isLoop = false;
    //pVIBufferCom = CVIBuffer_Rect_Instance::Create(m_pDevice, m_pContext, &desc);
    return S_OK;
}

HRESULT CParticle_Setting::Save_Binary(const _tchar* pFilePath)
{
    //HANDLE hFile = CreateFile(pFilePath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    //if (!hFile)
    //    return E_FAIL;
    //
    //DWORD dwByte = 0;
    //
    //_uint iCount = (_uint)m_Placed.size();
    //if (!WriteFile(hFile, &iCount, sizeof(_uint), &dwByte, nullptr)) {
    //    CloseHandle(hFile);
    //    return E_FAIL;
    //}
    //
    //for (const auto& r : m_Placed)
    //{
    //    int protoLenA = 0;
    //    if (!r.protoTag.empty())
    //        protoLenA = WideCharToMultiByte(CP_ACP, 0, r.protoTag.c_str(), (int)r.protoTag.size(), nullptr, 0, nullptr, nullptr);
    //    string protoA;
    //    protoA.resize(protoLenA);
    //    if (protoLenA)
    //        WideCharToMultiByte(CP_ACP, 0, r.protoTag.c_str(), (int)r.protoTag.size(), &protoA[0], protoLenA, nullptr, nullptr);
    //
    //    _uint lenProto = (_uint)protoA.size();
    //    if (!WriteFile(hFile, &lenProto, sizeof(lenProto), &dwByte, nullptr) || dwByte != sizeof(lenProto))
    //    {
    //        CloseHandle(hFile);
    //        return E_FAIL;
    //    }
    //    if (lenProto)
    //    {
    //        if (!WriteFile(hFile, protoA.data(), lenProto, &dwByte, nullptr) || dwByte != lenProto)
    //        {
    //            CloseHandle(hFile);
    //            return E_FAIL;
    //        }
    //    }
    //
    //    // pos/rot/scale
    //    float p[3] = { r.pos.x,     r.pos.y,     r.pos.z };
    //    float rx[3] = { r.rotDeg.x,  r.rotDeg.y,  r.rotDeg.z };
    //    float sc[3] = { r.scale.x,   r.scale.y,   r.scale.z };
    //
    //    if (!WriteFile(hFile, p, sizeof(p), &dwByte, nullptr) || dwByte != sizeof(p))
    //    {
    //        CloseHandle(hFile);
    //        return E_FAIL;
    //    }
    //    if (!WriteFile(hFile, rx, sizeof(rx), &dwByte, nullptr) || dwByte != sizeof(rx))
    //    {
    //        CloseHandle(hFile);
    //        return E_FAIL;
    //    }
    //    if (!WriteFile(hFile, sc, sizeof(sc), &dwByte, nullptr) || dwByte != sizeof(sc))
    //    {
    //        CloseHandle(hFile);
    //        return E_FAIL;
    //    }
    //
    //    int layerLenA = 0;
    //    if (!r.layerTag.empty())
    //        layerLenA = WideCharToMultiByte(CP_ACP, 0, r.layerTag.c_str(), (int)r.layerTag.size(), nullptr, 0, nullptr, nullptr);
    //    string layerA;
    //    layerA.resize(layerLenA);
    //    if (layerLenA)
    //        WideCharToMultiByte(CP_ACP, 0, r.layerTag.c_str(), (int)r.layerTag.size(), &layerA[0], layerLenA, nullptr, nullptr);
    //
    //    _uint lenLayer = (_uint)layerA.size();
    //    if (!WriteFile(hFile, &lenLayer, sizeof(lenLayer), &dwByte, nullptr) || dwByte != sizeof(lenLayer)) {
    //        CloseHandle(hFile);
    //        return E_FAIL;
    //    }
    //    if (lenLayer) {
    //        if (!WriteFile(hFile, layerA.data(), lenLayer, &dwByte, nullptr) || dwByte != lenLayer) {
    //            CloseHandle(hFile);
    //            return E_FAIL;
    //        }
    //    }
    //
    //    if (!WriteFile(hFile, &r.layerLevel, sizeof(r.layerLevel), &dwByte, nullptr) || dwByte != sizeof(r.layerLevel)) {
    //        CloseHandle(hFile);
    //        return E_FAIL;
    //    }
    //    if (!WriteFile(hFile, &r.protoLevel, sizeof(r.protoLevel), &dwByte, nullptr) || dwByte != sizeof(r.protoLevel)) {
    //        CloseHandle(hFile);
    //        return E_FAIL;
    //    }
    //
    //
    //    if (!WriteFile(hFile, &r.cell, sizeof(r.cell), &dwByte, nullptr) || dwByte != sizeof(r.cell)) {
    //        CloseHandle(hFile);
    //        return E_FAIL;
    //    }
    //
    //
    //
    //    layerLenA = 0;
    //    if (!r.NavigaionTag.empty())
    //        layerLenA = WideCharToMultiByte(CP_ACP, 0, r.NavigaionTag.c_str(), (int)r.NavigaionTag.size(), nullptr, 0, nullptr, nullptr);
    //    layerA = {};
    //    layerA.resize(layerLenA);
    //    if (layerLenA)
    //        WideCharToMultiByte(CP_ACP, 0, r.NavigaionTag.c_str(), (int)r.NavigaionTag.size(), &layerA[0], layerLenA, nullptr, nullptr);
    //
    //    lenLayer = (_uint)layerA.size();
    //    if (!WriteFile(hFile, &lenLayer, sizeof(lenLayer), &dwByte, nullptr) || dwByte != sizeof(lenLayer)) {
    //        CloseHandle(hFile);
    //        return E_FAIL;
    //    }
    //    if (lenLayer) {
    //        if (!WriteFile(hFile, layerA.data(), lenLayer, &dwByte, nullptr) || dwByte != lenLayer) {
    //            CloseHandle(hFile);
    //            return E_FAIL;
    //        }
    //    }
    //}
    //
    //CloseHandle(hFile);
    return S_OK;
}

HRESULT CParticle_Setting::Load_Binary(const _tchar* pFilePath)
{
    //HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    //if (hFile == INVALID_HANDLE_VALUE) return E_FAIL;
    //
    //DWORD dwByte = 0;
    //
    //_uint iCount;
    //if (!ReadFile(hFile, &iCount, sizeof(_uint), &dwByte, nullptr)) {
    //    CloseHandle(hFile);
    //    return E_FAIL;
    //}
    //m_pNavigation = nullptr;
    //vector<PlacedRecord> loaded;
    //loaded.reserve(iCount);
    //
    //for (_uint i = 0; i < iCount; ++i)
    //{
    //    _uint lenProto = 0;
    //    if (!ReadFile(hFile, &lenProto, sizeof(lenProto), &dwByte, nullptr) || dwByte != sizeof(lenProto))
    //    {
    //        CloseHandle(hFile);
    //        return E_FAIL;
    //    }
    //    string protoA;
    //    protoA.resize(lenProto);
    //    if (lenProto) {
    //        if (!ReadFile(hFile, &protoA[0], lenProto, &dwByte, nullptr) || dwByte != lenProto)
    //        {
    //            CloseHandle(hFile);
    //            return E_FAIL;
    //        }
    //    }
    //    wstring protoW;
    //    if (lenProto) {
    //        int needW = MultiByteToWideChar(CP_ACP, 0, protoA.data(), (int)protoA.size(), nullptr, 0);
    //        protoW.resize(needW);
    //        if (needW) MultiByteToWideChar(CP_ACP, 0, protoA.data(), (int)protoA.size(), &protoW[0], needW);
    //    }
    //
    //    // pos/rot/scale
    //    float p[3]{}, rx[3]{}, sc[3]{};
    //    if (!ReadFile(hFile, p, sizeof(p), &dwByte, nullptr) || dwByte != sizeof(p))
    //    {
    //        CloseHandle(hFile);
    //        return E_FAIL;
    //    }
    //    if (!ReadFile(hFile, rx, sizeof(rx), &dwByte, nullptr) || dwByte != sizeof(rx))
    //    {
    //        CloseHandle(hFile);
    //        return E_FAIL;
    //    }
    //    if (!ReadFile(hFile, sc, sizeof(sc), &dwByte, nullptr) || dwByte != sizeof(sc))
    //    {
    //        CloseHandle(hFile);
    //        return E_FAIL;
    //    }
    //
    //    _uint lenLayer = 0;
    //    if (!ReadFile(hFile, &lenLayer, sizeof(lenLayer), &dwByte, nullptr) || dwByte != sizeof(lenLayer))
    //    {
    //        CloseHandle(hFile);
    //        return E_FAIL;
    //    }
    //    string layerA;
    //    layerA.resize(lenLayer);
    //    if (lenLayer) {
    //        if (!ReadFile(hFile, &layerA[0], lenLayer, &dwByte, nullptr) || dwByte != lenLayer)
    //        {
    //            CloseHandle(hFile);
    //            return E_FAIL;
    //        }
    //    }
    //    wstring layerW;
    //    if (lenLayer) {
    //        int needW = MultiByteToWideChar(CP_ACP, 0, layerA.data(), (int)layerA.size(), nullptr, 0);
    //        layerW.resize(needW);
    //        if (needW) MultiByteToWideChar(CP_ACP, 0, layerA.data(), (int)layerA.size(), &layerW[0], needW);
    //    }
    //
    //    _uint lv = 0, pv = 0, cell = 0;
    //    if (!ReadFile(hFile, &lv, sizeof(lv), &dwByte, nullptr) || dwByte != sizeof(lv))
    //    {
    //        CloseHandle(hFile);
    //        return E_FAIL;
    //    }
    //    if (!ReadFile(hFile, &pv, sizeof(pv), &dwByte, nullptr) || dwByte != sizeof(pv))
    //    {
    //        CloseHandle(hFile);
    //        return E_FAIL;
    //    }
    //    if (!ReadFile(hFile, &cell, sizeof(cell), &dwByte, nullptr) || dwByte != sizeof(cell))
    //    {
    //        CloseHandle(hFile);
    //        return E_FAIL;
    //    }
    //
    //
    //
    //
    //    _uint lenNavigation = 0;
    //    if (!ReadFile(hFile, &lenNavigation, sizeof(lenNavigation), &dwByte, nullptr) || dwByte != sizeof(lenNavigation))
    //    {
    //        CloseHandle(hFile);
    //        return E_FAIL;
    //    }
    //    string navigationA;
    //    navigationA.resize(lenNavigation);
    //    if (lenNavigation) {
    //        if (!ReadFile(hFile, &navigationA[0], lenNavigation, &dwByte, nullptr) || dwByte != lenNavigation)
    //        {
    //            CloseHandle(hFile);
    //            return E_FAIL;
    //        }
    //    }
    //    wstring navigationW = {};
    //    if (lenNavigation) {
    //        int needW = MultiByteToWideChar(CP_ACP, 0, navigationA.data(), (int)navigationA.size(), nullptr, 0);
    //        navigationW.resize(needW);
    //        if (needW) MultiByteToWideChar(CP_ACP, 0, navigationA.data(), (int)navigationA.size(), &navigationW[0], needW);
    //    }
    //
    //
    //
    //    PlacedRecord r{};
    //    r.protoTag = protoW;
    //    r.pos = _float3{ p[0],  p[1],  p[2] };
    //    r.rotDeg = _float3{ rx[0], rx[1], rx[2] };
    //    r.scale = _float3{ sc[0], sc[1], sc[2] };
    //    r.layerTag = layerW;
    //    r.layerLevel = lv;
    //    r.protoLevel = pv;
    //    r.cell = cell;
    //    r.NavigaionTag = navigationW;
    //
    //    loaded.push_back(r);
    //}
    //
    //CloseHandle(hFile);
    //
    //_bool isMap = true;
    //for (auto& r : loaded) {
    //    _vector P = XMVectorSet(r.pos.x, r.pos.y, r.pos.z, 1.0f);
    //    CGameObject::GAMEOBJECT_DESC desc;
    //    desc.fSpeedPerSec = 0.f;
    //    desc.fRotationPerSec = 0.f;
    //    desc.strName = r.protoTag;
    //    desc.pos = P;
    //    desc.fRot = r.rotDeg;
    //    isMap = r.layerTag == L"Layer_Map";
    //    desc.fScale = isMap ? _float3(r.scale.x * 100, r.scale.y * 100, r.scale.z * 100) : r.scale;
    //    desc.iMapIndex = r.cell;
    //    desc.strNavigation = r.NavigaionTag;
    //
    //    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(r.protoLevel, r.protoTag, r.layerLevel, r.layerTag, &desc))) {
    //        MSG_BOX("faileditem.");
    //    }
    //    else {
    //        m_Placed.push_back(r);
    //    }
    //}

    return S_OK;
}

void CParticle_Setting::Update(_float fTimeDelta)
{

    ImGui::Begin("Tools", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
    ImVec2 btn = { 120, ImGui::GetFrameHeight() };
    if (ImGui::Button("Particle", btn)) {
        m_iLevel = 0;
    }
    ImGui::SameLine();
    if (ImGui::Button("Emission", btn)) {
        m_iLevel = 1;
    }
    ImGui::SameLine();
    if (ImGui::Button("Shape", btn)) {
        m_iLevel = 2;
    }

    static _float fMaxTime = 0;
    struct MyStruct
    {
        _float fTime;
        _float fX;
        _float fValue;
    };
    static vector<MyStruct> value;
    static _float values[100] = {};
    static _int values_offset = 0;
    static _float refresh_time = 0.0f;

    if (ImGui::GetTime() > refresh_time) {
        values[values_offset] = cosf(ImGui::GetTime() * 2.0f);
        values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
        refresh_time = ImGui::GetTime() + 1.0f / 60.0f;
    }

    ImGui::PlotLines("Sine Wave", values, IM_ARRAYSIZE(values), values_offset,
        "sample data", -1.0f, 1.0f, ImVec2(0, 80));
    ImGui::End();
    //pVIBufferCom->Drop(fTimeDelta);
}

HRESULT CParticle_Setting::Render()
{
    return S_OK;
}

void CParticle_Setting::Free()
{
    __super::Free();
}
