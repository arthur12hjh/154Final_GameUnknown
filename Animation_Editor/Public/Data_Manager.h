#pragma once
#include "Animation_Editor_Defines.h"


#include "Base.h"


NS_BEGIN(Animation_Editor)
class CData_Manager final : public CBase
{
private:
	CData_Manager();
	virtual ~CData_Manager() = default;

	HRESULT									Initalize();

public:
	const ANIM_NOTIFY* Find_AnimationNotifyData(_wstring szAnimationTag);

private:
	// 캐릭터 구조체는 이거 하나만있으도 될거같아서 픽스
	// map<_uint, CHARACTER_NETWORK_DESC>			m_pTextures = {};

	//스킬 데이터
	map<_uint, ANIM_NOTIFY>		m_pAnimationNotifyDatas = {};


private:
	HRESULT									LoadAnimNotifyData(void* pArg = nullptr);
public:
	static CData_Manager* Create();
	virtual void							Free() override;

};
NS_END