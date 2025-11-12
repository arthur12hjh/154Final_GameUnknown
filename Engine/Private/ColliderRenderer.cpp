#include "ColliderRenderer.h"

#include "GameInstance.h"
#include "GameObject.h"

#ifdef _DEBUG

CColliderRenderer::CColliderRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pGameInstance { CGameInstance::GetInstance() }
    , m_pDevice { pDevice } 
    , m_pContext { pContext }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CColliderRenderer::Add_DebugComponent(CComponent* pDebugCom)
{
    if (false == m_isColliderVisible)
        return S_OK;

    m_DebugComponents.push_back(pDebugCom);
    Safe_AddRef(pDebugCom);

    return S_OK;
}

HRESULT CColliderRenderer::Add_PhysxGeometry(PxRigidActor* pActor, PxShape* pShape)
{
    if (false == m_isColliderVisible)
        return S_OK;

    m_PxShapes.push_back(make_pair(pActor, pShape));

    return S_OK;
}

unique_ptr<GeometricPrimitive> CColliderRenderer::CreateHemisphere(ID3D11DeviceContext* pContext, _float fRadius, _int iTessellation, _bool isTop)
{
    GeometricPrimitive::VertexCollection Vertices;
    GeometricPrimitive::IndexCollection Indices;

    // 반구 정점 생성
    for (_int i = 0; i <= iTessellation / 2; ++i)
    {
        _float fPhi = (i / _float(iTessellation / 2)) * XM_PIDIV2; // 0 ~ PI/2
        if (!isTop) // 하단
        {
            fPhi = XM_PIDIV2 + fPhi;
        }

        for (_int j = 0; j <= iTessellation; ++j)
        {
            _float fTheta = j / _float(iTessellation) * XM_2PI;
            _float fX = fRadius * sinf(fPhi) * cosf(fTheta);
            _float fY = fRadius * cosf(fPhi);
            _float fZ = fRadius * sinf(fPhi) * sinf(fTheta);

            Vertices.push_back({ XMFLOAT3(fX, fY, fZ), XMFLOAT3(0,1,0), XMFLOAT2(0,0) });
        }
    }

    // Index 생성 (삼각형)
    for (int i = 0; i < iTessellation / 2; ++i)
    {
        for (int j = 0; j < iTessellation; ++j)
        {
            _uint iIdx0 = i * (iTessellation + 1) + j;
            _uint iIdx1 = iIdx0 + iTessellation + 1;
            _uint iIdx2 = iIdx0 + 1;
            _uint iIdx3 = iIdx1 + 1;

            Indices.push_back(iIdx0); Indices.push_back(iIdx1); Indices.push_back(iIdx2);
            Indices.push_back(iIdx0); Indices.push_back(iIdx2); Indices.push_back(iIdx3);
        }
    }

    return GeometricPrimitive::CreateCustom(pContext, Vertices, Indices);
}

HRESULT CColliderRenderer::Initialize()
{
    m_pPhysxDebugShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Physx_Debug.hlsl"), VTXPOSCOLOR::Elements, VTXPOSCOLOR::iNumElements);
    if (nullptr == m_pPhysxDebugShader)
        return E_FAIL;

    m_pEffect = new BasicEffect(m_pDevice);

    const void* pShaderByteCode = { nullptr };
    size_t		iShaderByteCodeLength = {};

    m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

    if (m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
        pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout))
        return E_FAIL;

    //반지름이 1인 객체로 생성.
    m_pSphereShape = GeometricPrimitive::CreateSphere(m_pContext, 1.f, 6);
    m_pBoxShape = GeometricPrimitive::CreateBox(m_pContext, _float3(0.5f, 0.5f, 0.5f));
    m_pCapsuleCylinderShape = GeometricPrimitive::CreateCylinder(m_pContext, 1.f, 1.f, 6);
    m_pCapsuleHemiSphereShape = CreateHemisphere(m_pContext, 0.5f, 6, true);

    return S_OK;
}

void CColliderRenderer::Render()
{
    if (m_pGameInstance->KeyDown(KEY_INPUT::KEYBOARD, DIK_F3))
        m_isColliderVisible = !m_isColliderVisible;
}

void CColliderRenderer::Render_Debug(class CShader* pShader)
{
    if (false == m_isColliderVisible)
        return;

    m_pGameInstance->Debug_LightRender();
    for (auto& pDebugCom : m_DebugComponents)
    {
        if (nullptr != pDebugCom)
            pDebugCom->Render();

        Safe_Release(pDebugCom);
    }
    m_DebugComponents.clear();

    if (FAILED(pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::VIEW))))
        return;

    if (FAILED(pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Renderer_Matrix(D3DTS::PROJ))))
        return;
}

void CColliderRenderer::Render_PhysxDebug(CShader* pShader)
{
    if (false == m_isColliderVisible)
        return;

    for (auto& Pair : m_PxShapes)
    {
        PxGeometryHolder Geometry = Pair.second->getGeometry();
        PxTransform PhysxTransform = PxShapeExt::getGlobalPose(*Pair.second, *Pair.first);

        m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW));
        m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ));
        m_pEffect->SetColorAndAlpha(XMVectorSet(0.f, 1.0f, 0.f, 1.f));

        if (PxGeometryType::eBOX == Geometry.getType())
        {
            PxBoxGeometry BoxGeom = Geometry.box();
            _matrix ScalingMatrix = XMMatrixScaling(BoxGeom.halfExtents.x, BoxGeom.halfExtents.y, BoxGeom.halfExtents.z);
            _matrix ConvertMatrix = ScalingMatrix * m_pGameInstance->Convert_PxTransform_ToMatrix(PhysxTransform);

            m_pEffect->SetWorld(ConvertMatrix);
            m_pBoxShape->Draw(m_pEffect, m_pInputLayout, false, true);
        }
        else if (PxGeometryType::eSPHERE == Geometry.getType())
        {
            PxSphereGeometry SphereGeom = Geometry.sphere();
            _matrix ScalingMatrix = XMMatrixScaling(SphereGeom.radius * 2.f, SphereGeom.radius * 2.f, SphereGeom.radius * 2.f);
            _matrix ConvertMatrix = m_pGameInstance->Convert_PxTransform_ToMatrix(PhysxTransform);

            m_pEffect->SetWorld(ScalingMatrix * ConvertMatrix);
            m_pSphereShape->Draw(m_pEffect, m_pInputLayout, false, true);
        }
        else if (PxGeometryType::eCAPSULE == Geometry.getType())
        {
            PxCapsuleGeometry CapsuleGeom = Geometry.capsule();
            _matrix ConvertMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-90.f)) * m_pGameInstance->Convert_PxTransform_ToMatrix(PhysxTransform);

            /* 실린더 출력 */
            m_pEffect->SetWorld(ConvertMatrix);
            m_pCapsuleCylinderShape->Draw(m_pEffect, m_pInputLayout, false, true);

            /* 상반구 출력 */
            _vector		vQuternion = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(30.f), 0.f, XMConvertToRadians(-90.f));
            _matrix		RotationMatrix = XMMatrixRotationQuaternion(vQuternion);

            PhysxTransform.p.y += CapsuleGeom.halfHeight;
            ConvertMatrix = RotationMatrix * m_pGameInstance->Convert_PxTransform_ToMatrix(PhysxTransform);
            m_pEffect->SetWorld(ConvertMatrix);
            m_pCapsuleHemiSphereShape->Draw(m_pEffect, m_pInputLayout, false, true);


            /* 하반구 출력*/
            vQuternion = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(30.f), 0.f, XMConvertToRadians(90.f));
            RotationMatrix = XMMatrixRotationQuaternion(vQuternion);

            PhysxTransform.p.y -= 2.f * CapsuleGeom.halfHeight;
            ConvertMatrix = RotationMatrix * m_pGameInstance->Convert_PxTransform_ToMatrix(PhysxTransform);
            m_pEffect->SetWorld(ConvertMatrix);
            m_pCapsuleHemiSphereShape->Draw(m_pEffect, m_pInputLayout, false, true);
        }
        //box, sphere, capsule만 드로우 지원. 나머진 고려해볼게요 ㅎ..
        else
        {
        }
    }

    m_PxShapes.clear();
}

_float4 CColliderRenderer::Convert_PxColor_ToVector(PxU32 iColor)
{
    return _float4();
}

CColliderRenderer* CColliderRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CColliderRenderer* pInstance = new CColliderRenderer(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Create Failed! : DebugRenderer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CColliderRenderer::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

    /* 혹시모를 null 대입. 원본이 스마트 포인터라 따로 해제 안해도 돼요*/
    m_pSphereShape = nullptr;
    m_pBoxShape = nullptr;
    m_pCapsuleCylinderShape = nullptr;
    m_pCapsuleHemiSphereShape = nullptr;

    Safe_Release(m_pPhysxDebugShader);
    Safe_Delete(m_pEffect);
    Safe_Release(m_pInputLayout);

    for (auto& pDebugCom : m_DebugComponents)
        Safe_Release(pDebugCom);
    m_DebugComponents.clear();
}

#endif