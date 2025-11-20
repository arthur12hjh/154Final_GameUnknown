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
	HRESULT Save_Data();


	const vector<ANIM_NOTIFY>* Find_AnimationNotifyData(const _wstring& szAnimationTag);

	unordered_map<_wstring, vector<ANIM_NOTIFY>>* Get_AnimationEventMapPtr() { return &m_AnimationNotifyDatas; }

private:
	// 고민이 크다..
	// 9조
	//	- 모든 애니메이션 관련 이벤트를 담당하는 ANIM_NOTIFY
	//  - 를 담고 있는 vector<ANIM_NOTIFY>
	//  - 들을 애니메이션 태그(_char*)로 구분짓는 map

	// 애니메이션 노티파이(이벤트) 데이터
	unordered_map<_wstring, vector<ANIM_NOTIFY>>		m_AnimationNotifyDatas = {};


private:
	HRESULT									LoadAnimNotifyData(void* pArg = nullptr);

public:
	static CData_Manager* Create();
	virtual void							Free() override;

};
NS_END