#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "UIStruct.h"

NS_BEGIN(Engine)
class CHUDLayer;
class CTexture;
NS_END

NS_BEGIN(Client)
class CUIAnimInstance;
class CUIBase;

class CUIAnimManager final : public CBase
{
private:
	CUIAnimManager();
	virtual ~CUIAnimManager() = default;

public:
	HRESULT Initialize();
	void Update(_float fTimeDelta);

	HRESULT Export_Anim_Prefab(_wstring szAnimTag, void* pDesc);
	HRESULT Import_Anim_Prefab(_wstring szAnimTag, void* pAnimOut);

	HRESULT Load_Anim_Files();
	vector<CUIAnimInstance*> Get_AnimInstances() { return m_AnimInstances; }
	map<_wstring, UI_ANIM_DESC> Get_AnimDatas() { return m_AnimDatas; }
	UI_ANIM_DESC* Get_AnimData(_wstring szAnimTag);

	HRESULT Create_Prefab(_wstring szAnimTag);
	HRESULT Delete_Prefab(_wstring szAnimTag);

	void Anim_Play(CUIBase* pUI, _wstring szAnimTag);
	void Clear_AnimInstances();

	//void Anim_Play(_wstring szLayerTag, _wstring szUITag, _wstring szAnimTag);
	////void Anim_Pause(_wstring szLayerTag, _wstring szUITag, _wstring szAnimTag);
	//void Anim_Stop(_wstring szLayerTag, _wstring szUITag);
	////void Anim_All_Stop();

private:
	vector<CUIAnimInstance*> m_AnimInstances{};
	map<_wstring, UI_ANIM_DESC> m_AnimDatas{};

public:
	static CUIAnimManager* Create();
	virtual void Free() override;
};

NS_END