#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CPartObject abstract : public CGameObject
{
public:
	typedef struct tagPartObjectDesc : public GAMEOBJECT_DESC
	{
		class CTransform* pParentTransform = { nullptr };
	}PARTOBJECT_DESC;

protected:
	CPartObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CPartObject(const CPartObject& Prototype);
	virtual ~CPartObject() = default;

public:
	void Update_PreCombinedMatrix();
public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(void* pArg);

	virtual void			Priority_Update(_float fTimeDelta);
	virtual void			Update(_float fTimeDelta);
	virtual void			Late_Update(_float fTimeDelta);

	virtual HRESULT			Render();

	const	_float4x4*		Get_BoneMatrixPtr(const _char* pBoneName) const;
	const	_float4x4*		Get_CombinedMatrixPtr() const;

	virtual void			Active_SFX(const _wstring& strObjectTag, ANIM_NOTIFY& NotifyReference) {};

protected:	
	class CModel*			m_pModelCom = { nullptr };
	class CShader*			m_pShaderCom = { nullptr };

	class CTransform*		m_pParentTransformCom = { nullptr };
	_float4x4				m_CombinedWorldMatrix = {};
	_float4x4				m_PreCombinedWorldMatrix = {};

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END

