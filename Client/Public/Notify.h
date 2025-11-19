#pragma once

#include "Client_Defines.h"
#include "Component.h"

NS_BEGIN(Engine)
class CModel;
NS_END

NS_BEGIN(Client)

class CNotify final : public CComponent
{
public:
    typedef struct tagNotifyDesc
    {
        class CModel* pModel;
    }NOTIFY_DESC;
private:
	CNotify(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNotify(const CNotify& Prototype);
	virtual ~CNotify() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	void Update(_float fTimeDelta);

    void AnimationChanged(const _wstring& szAnimationTag);

private:
	class CGameManager* m_pGameManager = nullptr;

    class CModel* m_pModelCom = nullptr;
    priority_queue<ANIM_NOTIFY> m_NotifyQueue;


public:
	static CNotify* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free();
};

NS_END

/*
ContainerObject
 ├─ HeadPartObject
     ├─ TransformComponent
     ├─ ModelComponent
     ├─ NotifyComponent
 ├─ BodyPartObject
     ├─ TransformComponent
     ├─ ModelComponent
     ├─ NotifyComponent
 ├─ WeaponPartObject
     ├─ TransformComponent
     ├─ ModelComponent
     ├─ NotifyComponent
 ├─ TransformComponent
 ├─ BehaviorTreeComponent

  NotifyComponent에서 애니메이션 노티파이들을 점검하고 실행해줄 것이다.
  -> NotifyComponent에서 필요한 것 : 애니메이션 컴포넌트, 모델 컴포넌트, 현재 실행되는 애니메이션, 애니메이션 노티파이 리스트
  -> NotifyComponent에서 할 일 : 지금 모델에서 실행중인 애니메이션, 키프레임 정보, 대상 GameObject를 받아와서,
            애니메이션 노티파이 리스트를 돌면서 현재 키프레임에 해당하는 노티파이가 있으면 실행시켜준다.
                -> 어떻게 한 번씩만 호출해줄 것인가?
					| - > 애니메이션 노티파이를 priority_queue형태로 복사생성하여 관리한다.
  -> 문제들
    1. NotifyComponent가 현재 애니메이션을 어떻게 알고, 어떻게 받아와야하는가?
		-> 모델 컴포넌트에 std::function으로 애니메이션 갱신 때마다 NotifyComponent에 알려주는 콜백함수를 등록해준다.
            
    2. 지금 애니메이션들은 행동트리에서 제어해주고 있다. 이 제어들이 NotifyComponent에 영향을 미칠까?
    3. NotifyQueue가 진행되다가, 피격이상 혹은 캔슬로 씹혀야하는 Notify들은 어떻게 처리할 것인가?
		-> NotifyQueue를 애니메이션 바꿔줄 때마다 비워준다

    애니메이션 노티파이 맵
	-> DataManager가 들고있기 ㅇㅇ
	-> 나는 NotifyQueue만 들고 있고, 애니메이션이 바뀔 때마다 DataManager에서 애니메이션 노티파이 리스트를 받아와서 NotifyQueue를 채워준다.

    해야할 것
	-> Data_Manager에서 애니메이션 노티파이 데이터 로드하고 저장하는 기능 만들기
	-> Model에서 애니메이션 변경 콜백 함수 만들기






*/