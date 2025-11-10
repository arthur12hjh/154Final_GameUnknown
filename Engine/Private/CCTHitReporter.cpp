#include "CCTHitReporter.h"

CCTHitReporter::CCTHitReporter()
{
}

void CCTHitReporter::Free()
{
	__super::Free();

	if(nullptr != m_pController)
		m_pController = nullptr;
}
