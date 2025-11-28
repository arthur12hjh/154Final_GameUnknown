#pragma once

#include "MapTool_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Terrain;
class CNavigation;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Tool_Map)

class CTerrain_Desert final : public CGameObject
{
public:
	class CNavigation* Get_NavigationComponent() const {
		return m_pNavigationCom; // 이미 멤버 변수로 가지고 있으니 바로 리턴
	}

	void Set_MapTool(class CMapTool_Desert* pMapTool) { m_pMaptool = pMapTool; }

private:
	CTerrain_Desert(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain_Desert(const CTerrain_Desert& Prototype);
	virtual ~CTerrain_Desert() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Change_Height_Rect(_vector PickingPos, _float fHeight, _float fRadius);
	void Change_Height_Flat(_vector PickingPos, _float fHeight, _float fRadius);
	void Change_Height_Sculpt(_vector vPickingPos, _float fAmount, _float fRadius, _float fMaxHeight);
	void Change_Height_Smooth(_vector vPickingPos, _float fFactor, _float fRadius);

	_float Get_Height_In_World_Space(_float fWorldX, _float fWorldZ);

private:
	CVIBuffer_Terrain* m_pVIBufferCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };

	class CMapTool_Desert* m_pMaptool = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CTerrain_Desert* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END