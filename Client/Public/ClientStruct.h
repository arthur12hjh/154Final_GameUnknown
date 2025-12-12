#pragma once

static const char*		g_szVisibility[2] = {  "VISIBLE", "HIDDEN" };

typedef struct RuinComponentDesc
{
	const wchar_t* pComponentTag;

} RuinComponentDesc;

typedef struct SocketMatrixDesc
{
	const _float4x4* pSocketMatrix = { nullptr };
	const _float4x4* pParentTransformMatrix = { nullptr };
} SOCKETMATRIX_DESC;