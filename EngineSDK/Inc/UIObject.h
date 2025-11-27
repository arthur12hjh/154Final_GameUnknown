#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIObject abstract : public CGameObject
{
public:
	enum class DRAW_TYPE { WORLD, SCREEN, END };

	typedef struct tagUIObjectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float		fX, fY, fSizeX, fSizeY;
	}UIOBJECT_DESC;

protected:
	CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIObject(const CUIObject& Prototype);
	virtual ~CUIObject() = default;

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(void* pArg);
	virtual void			Priority_Update(_float fTimeDelta);
	virtual void			Update(_float fTimeDelta);
	virtual void			Late_Update(_float fTimeDelta);

	virtual HRESULT			Render();	

	// ZOrder 라는 것을 통해서 UI의 우선순위를 관리한다.
	void					SetZOrder(_uint iZOrder);
	_uint					GetZOrder() { return m_iZOrder; }

	const _float4x4*		GetCombinedMatrixPtr() { return &m_CombinedMatrix; }
	const RECT&				GetScreenSize() { return m_ScreenSize; }

protected:
	CGameObject*			m_pParent = { nullptr };
	DRAW_TYPE				m_eDrawType = { DRAW_TYPE::SCREEN };
	RECT					m_ScreenSize = {};

	_uint					m_iZOrder = {};
	_float4x4				m_ProjMatrix = {};
	_float4x4				m_CombinedMatrix = {};

protected :
	// 변경 되어야 하는 포지션
	void					ComputeTransform(_vector vPosition);

public:	
	virtual CGameObject*		Clone(void* pArg) = 0;
	virtual void				Free() override;

};

NS_END