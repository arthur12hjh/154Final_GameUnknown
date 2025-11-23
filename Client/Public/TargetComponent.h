#pragma once
#include "Client_Defines.h"
#include "Component.h"

NS_BEGIN(Client)
class CTargetComponent final : public CComponent
{
public :
	typedef struct TargetComponentDesc
	{
		_float				fRadius;
		_uint				iNumPoints;
	}TARGET_COMPONENT_DESC;

private : 
	CTargetComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTargetComponent() = default;

public :
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg);

	void							Target_Search(const list<CGameObject*>* pList, function<_bool(CGameObject*, CGameObject*)> Func = nullptr);
	CGameObject*					GetTarget() { return m_pTarget; }

	vector<_float3>*				GetPathFinding() { return &m_vPoints; }

private :
	CGameObject*					m_pTarget = { nullptr };

	_uint							m_iNumPoints = {};
	_float							m_fRadius = {};
	vector<_float3>					m_vPoints;

public :
	static		CTargetComponent*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CComponent*			Clone(void* pArg) override;
	virtual		void				Free() override;

};
NS_END