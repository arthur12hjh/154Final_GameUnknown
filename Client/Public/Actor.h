#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer;
class CRigidBody;
class CModel;
NS_END

NS_BEGIN(Client)
class CActor abstract : public CGameObject
{
public :
	typedef struct ActorDesc : public GAMEOBJECT_DESC
	{
		const WCHAR*	szVIBuffer_PrototypeName;
		_uint			iObjectID = 0;
	}ACTOR_DESC;

protected :
	CActor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActor(const CActor& Prototype);
	virtual ~CActor() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;

	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;

	virtual HRESULT				Render() override;

	const _tchar*				Get_ComponentTag() const { return m_ComponentTag; }


protected :
	CShader*					m_pShaderCom = { nullptr };

	// 충돌용 메시입니다 다른 모델 넣듯이 똑같이 추가해주시면 돼요 진성햄
	// 재훈이쪽에서 던져준 fbx binx로 변환해서 넣어주기만 하면 돼요.
	// 텍스쳐같은건 신경 안쓰셔도됩니다. 어차피 렌더링 안되고 그냥 충돌처리만 할 메시라서 
	CModel*						m_pColModelCom = { nullptr };
	CRigidBody*					m_pRigidBody = { nullptr };
	_tchar						m_ComponentTag[256] = {};

public:
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;

};
NS_END