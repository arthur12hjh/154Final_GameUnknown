#pragma once

#include "ContainerObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CEntity abstract : public CContainerObject
{
public:

protected:
	CEntity(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEntity(const CEntity& Prototype);
	virtual ~CEntity() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	/// * * * CAUTION  존 나 주 의 * * * * *
	/// CEntity는 무조건 "Part_Body"와
	///				     "Com_Model" 태그인 CModel 컴포넌트가 존재해야합니다.
	/// * * * * * * * * * * * * * * * * * *

	_bool	Play_Animation(_float fTimeDelta, class CTransform* pTargetTransform = nullptr, _float fRootMotionRatio = 0.f);
	_bool	IsAnmiationFinished();
	void Set_AnimationIndex(_int iAnimIndex, _bool isLoop = true);

	void Set_Animation(const _char* szAnimationTag, _bool isLoop = true, _float fAnimationPlayRate = 1.f);

protected:
	class CModel* m_pBodyModelCom = { nullptr };

protected:
	HRESULT Import_ModelPtr();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END