#pragma once
#ifdef _DEBUG

#include "Base.h"

/* 디버그용 콜라이더 렌더러. 로직 분리용으로 클래스 작성 */
NS_BEGIN(Engine)

class CColliderRenderer : public CBase
{
private:
	CColliderRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CColliderRenderer() = default;

public:
	unique_ptr<GeometricPrimitive> CreateHemisphere(ID3D11DeviceContext* pContext, _float fRadius, _int iTessellation, _bool isTop);
	HRESULT Add_DebugComponent(class CComponent* pDebugCom);
	HRESULT Add_PhysxGeometry(class PxRigidActor* pActor, class PxShape* pShape);

	void Set_DebugColliderVisible(_bool bIsVisible);

public:
	HRESULT Initialize();
	void	Render(class CShader* pShader);

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device*		 m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	list<pair<class PxRigidActor*, class PxShape*>> m_PxShapes = {};
	class CShader* m_pPhysxDebugShader = { nullptr };
	list<class CComponent*>							m_DebugComponents = {};
	BasicEffect* m_pEffect = {};
	ID3D11InputLayout* m_pInputLayout = {};

	// 외부 제공 모듈이라 어쩔수 없이 unique ptr 써야함..
	unique_ptr<GeometricPrimitive>		m_pBoxShape = { nullptr };
	unique_ptr<GeometricPrimitive>		m_pSphereShape = { nullptr };
	unique_ptr<GeometricPrimitive>		m_pCapsuleCylinderShape = { nullptr };
	unique_ptr<GeometricPrimitive>		m_pCapsuleHemiSphereShape = { nullptr };

	_bool								m_isColliderVisible = { false };

private:
	void	Render_DebugCollider(class CShader* pShader);
	void	Render_DebugPhysxCollider(class CShader* pShader);
	_float4 Convert_PxColor_ToVector(PxU32 iColor);

public:
	static CColliderRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END

#endif