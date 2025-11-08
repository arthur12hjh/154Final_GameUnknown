#pragma once

#include "Base.h"

NS_BEGIN(Engine)
class CHUDLayer;
class CGameInstance;
class CGameObject;

class ENGINE_DLL CGameHUD abstract  : public CBase
{
protected:
	CGameHUD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CGameHUD() = default;

public:
	virtual HRESULT				Initialize();
	virtual void				Update(_float fDeletaTime);

	HRESULT						Add_UserInterface(_uint iLevel , const  WCHAR* PrototypeTag, const  WCHAR* LayerTag, const  WCHAR* UITag, CGameObject** ppOut = nullptr, void* pArg = nullptr);
	HRESULT						Remove_UserInterface(const WCHAR* LayerTag, const WCHAR* UITag);

	CHUDLayer*					Find_Layer(const WCHAR* LayerTag);
	void						Clear_HUD();

protected:
	unordered_map<_wstring, CHUDLayer*>				m_pLayers;

	//기본 변수들 바꾸지마라
	CGameInstance*									m_pGameInstance = nullptr;
	ID3D11Device*									m_pDevice = nullptr;
	ID3D11DeviceContext*							m_pContext = nullptr;

public:
	virtual void									Free() override;

};
NS_END

/* [구조]
<HUD>
	<Layer LayerTag="">
		<Wrapper>
			<Img></Img>

			<FlexBox>
				<Button>Button</Button>
				<Button>Button</Button>
				<Button>Button</Button>
				<Button>Button</Button>
			</FlexBox>
		</Wrapper>
	</Layer>
</HUD>


*/