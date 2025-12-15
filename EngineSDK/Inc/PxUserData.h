#pragma once
#include "Engine_Defines.h"
/* 피직스에 사용할 유저 데이터. */
/* 현재는 식별을 위한 액터 태그만 존재. */

NS_BEGIN(Engine)

typedef struct tagPxUserData {
	wstring szActorTag = {};
	_bool isCCT = { false };
	_bool isActive = { true };
	void* pHitActor = { nullptr };
	class CTransform* pActorTransform = { nullptr };

	void* pWord0 = { nullptr };
	void* pWord1 = { nullptr };
} PxUserData;

NS_END