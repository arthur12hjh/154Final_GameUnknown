#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class CSphereCollider;

class ENGINE_DLL CAISenceComponent final : public CComponent
{
public:
	typedef struct AiSenceComDesc
	{
		_float					fAiSearchRadius;
		_float					fAiTargetSearchDistance;
		_float					m_fAiTargetLostTime;

	}AI_SENCE_COMPONENT_DESC;

private:
	CAISenceComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAISenceComponent(const CAISenceComponent& Prototype);
	virtual ~CAISenceComponent() = default;

public:
	virtual HRESULT						Initialize_Prototype() override;
	virtual HRESULT						Initialize(void* pArg) override;
	virtual void						SetOwner(CGameObject* pGameObject);

	void								UpdatSenceComponent(_float fDeletaTime);
	const list<CGameObject*>*			GetSearchAllObject() { return &m_pSearchList; }
	_bool								IsTagetEmpty();
	void								Add_SenceTargetObject(CGameObject* pSenceObject);

	// 이벤트 바인딩 함수들
	void								Bind_TargetSearch(function<void(CGameObject*)> Func);
	void								Bind_TargetLost(function<void(CGameObject*)> Func);
	void								Bind_TargetDetected(function<void(CGameObject*)> Func);

	void								SetTraceHitType(HIT_TYPE eHitType);
	void								ADD_SenceIgnoreTraceObject(HIT_TYPE eHitType);
	void								ADD_SenceOnlyTraceObject(HIT_TYPE eHitType);

	_float								GetSenceRadiusDegree() { return XMConvertToDegrees(m_fAiSearchRadius); }
	_float								GetSenceRadiusRadian() { return m_fAiSearchRadius; }
#ifdef _DEBUG
	void								Update_Debuge();
#endif

private:
	CSphereCollider*					m_pTargetSearchCol = nullptr;

	// 현재프레임에 시야안에 들어와있는 오브젝트들
	set<CGameObject*>					m_pCurSearchList = {};

	// 이전프레임에 시야안에 들어왔던 오브젝트들
	unordered_map<CGameObject*, _float>			m_pPreSearchList = {};
	list<CGameObject*>							m_pSearchList = {};

	// 이건 이벤트로 받아서 쏴줄 녀석들
	function<void(CGameObject*)>		m_SearchFunc = nullptr;
	function<void(CGameObject*)>		m_TargetLostFunc = nullptr;
	function<void(CGameObject*)>		m_TargetDetectedFunc = nullptr;

	// 시야센서를 이루는 정보들
	_float								m_fAiSearchRadius = {};
	_float								m_fAiTargetSearchDistance = {};
	_float								m_fAiTargetLostTime = {};

private:
	HRESULT								ADD_Components();
	void								OverlapEvent(_float3 vDir, CGameObject* pHitObject);

public:
	static		CAISenceComponent*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CComponent*				Clone(void* pArg) override;
	virtual		void					Free() override;

};
NS_END