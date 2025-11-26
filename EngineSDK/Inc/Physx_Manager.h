#pragma once

#include "Base.h"

/*
뉴네오피직스매니저

여기 내용물은 거의 건드릴거 없음 최적화 하고싶으면 갠디 ㄱㄱ
*/

NS_BEGIN(Engine)

class CPhysx_Manager : public CBase
{
private:
	CPhysx_Manager();
	virtual ~CPhysx_Manager() = default;

public: 
	HRESULT		Initialize();
	void		Update(_float fTimeDelta);

public:
	void		TestSetting();
	void		Clear();

public:
	/* 두 싱글턴은 제공 가능하게 세팅해주자. */
	PxControllerManager* Get_PxCCTManager() { return m_pPxCCTManager; }
	PxPhysics*	Get_PxPhysics() { return m_PxPhysics; }
	PxScene*	Get_PxScene() { return m_PxScene; }
	/* CCT는 생성한 순간 씬에 들어가므로, 피직스에서 충돌쌍만 들고 있게 한다. (쓸지도 모름) */
	HRESULT		Add_CCT_ToPhysx(class CGameObject* pGameObject, class CCharacterController* pCCT);
	HRESULT		Add_RigidBody_ToPhysx(class CGameObject* pGameObject, class CRigidBody* pRigidBody);
	// 객체 생성하고 터레인 버퍼 집어넣기만 하면 됩니다.
	HRESULT		Add_Terrain_ToPhysx(class CVIBuffer_Terrain* pTerrainVIBuffer);

	/* 피직스 트랜스폼 변환함수. 어지간하면 건드리기 ㄴㄴ */
	PxTransform Convert_Matrix_ToPxTransform(_matrix WorldMatrix);
	/* 피직스 트랜스폼 변환함수. 어지간하면 건드리기 ㄴㄴ */
	_matrix		Convert_PxTransform_ToMatrix(PxTransform Transform);

private:
	/* 모든 Physx 모듈을 사용하려면 필요한 인스턴스. 다렉의 Device나 현재 프레임워크의 GameInstance 같은 녀석.*/
	PxPvdTransport* m_PxTransport = { nullptr };
	PxFoundation* m_PxFoundation = { nullptr };

	/* Foundation을 생성하기 위해 필요한 녀석 */
	PxDefaultAllocator m_DefaultAllocator = {};
	PxDefaultErrorCallback m_DefaultErrorCallback = {};

	/* Scene 내 모든 녀석들에게 영향을 미치는 녀석이라고 한다. (중력 같은거?)*/
	PxPhysics* m_PxPhysics = { nullptr };

	/* 말그대로 씬의 개념. 액터들을 모아놓고 액터끼리의 상호작용을 수행한다. */
	PxScene* m_PxScene = { nullptr };

	/* Physx Visual Debugger를 사용하기 위해 필요한 녀석 */
	PxPvd* m_Pvd = { nullptr };
	/* CPU 멀티 스레딩을 사용하기 위한 녀석 */
	PxDefaultCpuDispatcher* m_PxDispatcher = { nullptr };
	PxMaterial* m_pTestMaterial = { nullptr };


	/* 구워놓은 터레인 보관용. */
	vector<PxRigidStatic*> m_pTerrains = {};

	/* 캐릭터 컨트롤러 매니저 */
	PxControllerManager* m_pPxCCTManager = { nullptr };

	/* 캐릭터 컨트롤러들 보관용. 매니저에서 뽑아와도 좋겠지만 일단 들고있게끔 처리.  */
	vector<pair<class CGameObject*, CCharacterController*>> m_CCTs = {};

	/* 리지드 바디 객체들 보관용 */
	vector<pair<class CGameObject*, CRigidBody*>> m_RigidBodies = {};
	class CGameInstance* m_pGameInstance = { nullptr };
public:
	static CPhysx_Manager* Create();
	virtual void Free() override;
};

NS_END