#pragma once
#include "Engine_Defines.h"

/* 피직스에 사용할 유저 데이터. */
/* 현재는 식별을 위한 액터 태그만 존재. */

typedef struct tagPxUserData {
	wstring szActorTag = {};
} PxUserData;