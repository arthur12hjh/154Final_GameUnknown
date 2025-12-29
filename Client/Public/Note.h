#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CCollider;
class CShader;
NS_END

NS_BEGIN(Client)
class CRimLight;

class CNote final: public CGameObject
{
public:
	enum class NOTE_TYPE { HIT, OVERLAP,END };

	typedef struct Note_Desc : public GAMEOBJECT_DESC
	{
		_float3				vTargetPoint;
		_float				fNoteSpeed;
	}NOTE_DESC;

	typedef struct Note_Data_Desc
	{
		NOTE_TYPE				NoteType;
		DIRECTION				eDirection;
		_float2					vBoundAnimRatio = { 0.f ,1.f };
	}NOTE_DATA_DESC;

protected:
	CNote(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNote(const CNote& Prototype);
	virtual ~CNote() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;

	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;

	virtual HRESULT					Render() override;
	const NOTE_DATA_DESC&			GetNoteData() { return m_NoteData; }

private :
	CModel*							m_pModelCom = {nullptr};
	CCollider*						m_pColliderCom = { nullptr };
	CShader*						m_pShaderCom = { nullptr };
	
	CRimLight*						m_pRimLight = { nullptr };
	RIMLIGHT_DESC					m_RimLightDesc = {};

	NOTE_DATA_DESC					m_NoteData = {};
	_float3							m_vTargetPoint = {};
	_float							m_fNoteSpeed = { 2.f };

private :
	HRESULT							Ready_Components();
	HRESULT							Bind_ShaderResources();

public:
	static		CNote*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual		CGameObject*		Clone(void* pArg) override;
	virtual		void				Free() override;

};
NS_END