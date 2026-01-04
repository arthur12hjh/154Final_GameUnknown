#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{

#define iFLAG_ROOT_RESET			0
#define iFLAG_ROOT_INIT				(1 << 0)
#define iFLAG_ROOT_ACTIVE			(1 << 1)

#define iFLAG_ANIMATION_RESET		0
#define iFLAG_ANIMATION_PLAY		(1 << 0)
#define iFLAG_ANIMATION_FINISH		(1 << 1)

#define PHYSX_DEFAULT	(1 << 1)
#define PHYSX_CCT		(1 << 2)
#define PHYSX_DYNAMIC	(1 << 3)
#define PHYSX_STATIC	(1 << 4)
#define PHYSX_KINEMATIC (1 << 5)
#define PHYSX_NONTERRAIN  (1 << 6)
#define PHYSX_TERRAIN   (1 << 7)
#define PHYSX_CUSTOM_1  (1 << 8)
#define PHYSX_CUSTOM_2  (1 << 9)
#define PHYSX_CUSTOM_3  (1 << 10)
#define PHYSX_CUSTOM_4  (1 << 11)

#define CASCADE_LEVEL 5

#ifdef _DEBUG
	enum class GAMELOOP_TYPE	{ PRIORITY, UPDATE, LATE_UPDATE, RENDER, COLLISION, END};
#endif // _DEBUG

	enum class PROTOTYPE		{ GAMEOBJECT, COMPONENT };
	enum class COLLIDER			{ AABB, OBB, SPHERE, END };

	// 추가 예정
	enum class HIT_TYPE			{ ALL,
								  OBJECT,
								  STATIC,
								  PLAYER,
								  MONSTER,
								  NPC,
								  SENCE,
								  INTERACTION,
								  END };

	enum class LIGHT_TYPE		{ POINT=1, SPOT, DIRECTIONAL, VOLUMETRIC, END };

	enum class MOUSEKEYSTATE	{ LBUTTON, RBUTTON, WBUTTON, XBUTTON };
	enum class MOUSEMOVESTATE	{ HORIZONTAL, VERTICAL, W };

	enum class WINMODE			{ FULL, WIN, END };
	enum class KEY_INPUT		{ KEYBOARD, MOUSE, END };
								// 이전 프레임 뷰 투영도 추가
	enum class D3DTS			{ VIEW, PROJ, END };
	enum class MODEL_TYPE		{ ANIM, NONANIM, PARTANIM, ANIMONLY,  END };
								
	enum class VISIBILITY		{ VISIBLE, HIDDEN, END };
	enum class STATE			{ RIGHT, UP, LOOK, POSITION, END };

	enum class CAMERASTATE		{ CAMERA_FOV, CAMERA_FAR, CAMERA_NEAR, END };
	enum class OBJECT_TEAM		{ FRIENDLY, ENEMY, NEUTRAL, END };
	enum class DIRECTION		{ LEFT,
								  LEFT_FRONT,
								  LEFT_BACK,
								  RIGHT,
								  RIGHT_FRONT,
								  RIGHT_BACK,
								  FRONT, 
								  BACK, END};

	enum class TEXTURE_TYPE {
		NONE = 0,
		DIFFUSE = 1,
		SPECULAR = 2,
		AMBIENT = 3,
		EMISSIVE = 4,
		ORM = 5,
		NORMAL = 6,
		ORSS = 7,
		MASK = 8,
		EXTRA1 = 9,
		EXTRA2 = 10,
		OPACITY = 11,
		EXTRA3 = 12,
		EXTRA4 = 13,
		EXTRA5 = 14,
		EXTRA6 = 15,
		END
	};

	enum class EFFECT_TEXTURE_TYPE {
		DIFFUSE = 0,
		NORMAL = 1,
		NOISE = 2,
		MASK = 3,
		DISSOLVE = 4,
		END
	};


	enum class RENDER {
		PRIORITY,
		SHADOW,
		MOTIONBLUR,
		OCCLUSION,
		NONBLEND_PRIORITY,
		NONBLEND,
		BLOOM,
		NONLIGHT,
		BLUR,
		GLOW,
		METABALL,
		BLACKBLEND,
		DISTORTION,
		BLEND,
		UI,
		END
	};

	enum class SHADER_DEFERRED_IDX {
		DEBUG, DIRECTIONAL, POINT,
		VOLUMETRIC_DIRECTIONAL, VOLUMETRIC_POINT,
		COMBINED,
		//후처리 합성용
		DEFERRED,
		//톤 매핑
		TONE_MAPPING, 
		//진짜 렌더타겟으로 넘기기
		FINAL,
		SPOT,
		OCCLUSION,
		SHADOW_BLUR_X,
		COMBINE_SHADOW,
		FXAA
	};

	
	enum class AXIS				{ HORIZONTAL, VERTICAL, END };

	enum class NAVI_POINT		{ A, B, C, END };
	enum class NAVI_LINE		{ AB, BC, CA, END };

	enum CHANNELID				{ BGM, EFFECT, EFFECT2, END };

	enum class CAMERA_ROLE		{ MAIN, DEBUG, END };
}
#endif // Engine_Enum_h__
