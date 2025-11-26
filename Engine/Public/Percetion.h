#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class CCollider;
class CGameObject;

class ENGINE_DLL CPerception : public CComponent
{
public:
	typedef struct PercetionDesc
	{
		_float					fSearchRadius;
		_float					fTargetLostDistance;

		//_float					fTargetLostTime;
	}PERCETION_COMPONENT_DESC;

	// 탐색 결과값을 넣어줄 구조체
	typedef struct PercetionOutDesc
	{
		// 발견한 오브젝트 방향
		_float3					vDir;

		// 타겟이 사라진 위치 및 발견된 위치
		_float3					vReturnPoint;

		// 발견한 오브젝트 및 잃어버린 녀석
		CGameObject*			pObject;
	}PERCETION_OUT_DESC;

private:
	CPerception(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPerception(const CPerception& Prototype);
	virtual ~CPerception() = default;

public:
	virtual HRESULT						Initialize_Prototype() override;
	virtual HRESULT						Initialize(void* pArg) override;

	// 이걸로 타겟 감지되는 콜리전 
	// 위치 조정 가능합니다.
	void								Updat_Component(_float fDeletaTime, const _float4x4* pSearchMatrix = nullptr);

	// 구조체로 리턴합니다.
	// PERCETION_OUT_DESC로 캐스팅해주세요
	void								Bind_TargetSearch(function<void(void*)> Func);
	
	// 구조체로 리턴합니다.
	// PERCETION_OUT_DESC로 캐스팅해주세요
	void								Bind_TargetLost(function<void(void*)> Func);
	
	// 구조체로 리턴합니다.
	// PERCETION_OUT_DESC로 캐스팅해주세요
	void								Bind_TargetDetected(function<void(void*)> Func);

#ifdef _DEBUG
	// 색상 넣으면 그색상으로 범위 출력됩니다.
	void								Render_Percetion(_float4 vColor = {});
#endif 

private:
	CCollider*							m_pTargetSearchCol = nullptr;

	list<CGameObject*>					m_pSearchList = {};
	list<CGameObject*>					m_pOldSearchList = {};

	function<void(void*)>				m_SearchFunc = nullptr;
	function<void(void*)>				m_TargetLostFunc = nullptr;
	function<void(void*)>				m_TargetDetectedFunc = nullptr;

private:
	_float								m_fSearchRadius = { 1.f };
	_float								m_fTargetLostDistance = { 2.f };

	set<CGameObject*>					m_pPercetionObjects;
	PERCETION_OUT_DESC					m_PercetionOutDesc = {};

private:
	HRESULT								ADD_Components();

	void								TargetSearch(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor);
	void								TargetDetected(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor);
	void								TargetLost(_float3 vHitPoint, _float3 vHitDir, CGameObject* pHitActor);

public:
	static		CPerception*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CComponent*				Clone(void* pArg) override;
	virtual		void					Free() override;

};
NS_END