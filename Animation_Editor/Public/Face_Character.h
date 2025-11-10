#pragma once

#include "Animation_Editor_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CBone;
NS_END

NS_BEGIN(Animation_Editor)

class CFace_Character final : public CPartObject
{

public:
	typedef struct tagFace_Character_Desc : public CPartObject::PARTOBJECT_DESC
	{
		const _uint*	pParentState = { nullptr };
		void*			pBodyPtr = { nullptr };
	}FACE_CHARACTER_DESC;
private:
	CFace_Character(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFace_Character(const CFace_Character& Prototype);
	virtual ~CFace_Character() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };

private:
	const				_uint*		m_pParentState = { nullptr };

	map<_char*, CBone*>			m_mapBodyBones;


private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	HRESULT Bind_BoneToPartBody(void* pArg);

public:
	static CFace_Character* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END