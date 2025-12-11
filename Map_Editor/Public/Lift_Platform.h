#pragma once

#include "Maptool_Defines.h"
#include "DesertObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Tool_Map)

class CLift_Platform final : public CDesertObject
{
public:
	typedef struct Lift_Platform_Desc : public DESERT_OBJECT_DESC
	{
		_uint	iPlatFormID = 0;
		_float	fMoveDistance = 0.f;
	}LIFT_PLATFORM_DESC;

public:
	_uint Get_PlatformID() const { return m_iPlatformId; }
	_float Get_MoveDistance() const { return m_fMoveDistance; }

private:
	CLift_Platform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLift_Platform(const CLift_Platform& Prototype);
	virtual ~CLift_Platform() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_uint	m_iPlatformId = 0;
	_float	m_fMoveDistance = 0.f;

private:
	HRESULT Ready_Components(const _tchar* pComponentTag);
	HRESULT Bind_ShaderResources();

public:
	static CLift_Platform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CDesertObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END