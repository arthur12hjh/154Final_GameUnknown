#include "pch.h"
#include "PlayerCCTQueryFilterCallback.h"

CPlayerCCTQueryFilterCallback::CPlayerCCTQueryFilterCallback()
	: CCTQueryFilterCallback {}
{
}

CPlayerCCTQueryFilterCallback* CPlayerCCTQueryFilterCallback::Create()
{
	return new CPlayerCCTQueryFilterCallback();
}

void CPlayerCCTQueryFilterCallback::Free()
{
	__super::Free();
}
