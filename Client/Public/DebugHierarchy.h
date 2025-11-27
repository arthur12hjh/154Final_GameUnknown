#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CGameObject;
class CComponent;
NS_END

NS_BEGIN(Client)
class CDebugHierarchy : public CGameObject
{
private:
	CDebugHierarchy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CDebugHierarchy() = default;

public:
	HRESULT						Initialize();

	void						Update(_float fTimeDeleta);
	HRESULT						Render();

	void						UpdateTransform(_float3 vPosition);


private:
	ImGuiTreeNodeFlags			m_TreeNodeFlag = {};

	char						m_szLayerPreview[MAX_PATH] = {};
	char						m_szLayerView[MAX_PATH] = {};
	char						m_szObjectTag[MAX_PATH] = {};

	list<CGameObject*>*			m_pSelectLayerObject = {};
	list<CGameObject*>			m_pSelectList = {};

	CComponent*					m_pSelectComponent = { nullptr };

#pragma region ColDebug
	_float						m_vColliderRotation[4] = {};
	_float						m_vColliderSize[3] = {};
#pragma endregion


private :
	void						DarwLayerSelect();
	void						DrawObjectInfo(CGameObject* pDrawObject);

	void						DrawEditorCollider(CCollider* pCollider);

public:
	static	CDebugHierarchy*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;

};
NS_END