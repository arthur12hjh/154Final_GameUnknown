#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{

#define ROOTFLAG_RESET		0
#define ROOTFLAG_INIT		(1 << 0)
#define ROOTFLAG_ACTIVE		(1 << 1)

#ifdef _DEBUG
	enum class GAMELOOP_TYPE	{ PRIORITY, UPDATE, LATE_UPDATE, RENDER, COLLISION, END};
#endif // _DEBUG

	enum class PROTOTYPE		{ GAMEOBJECT, COMPONENT };
	enum class COLLIDER			{ AABB, OBB, SPHERE, END };

	// 추가 예정
	enum class HIT_TYPE			{ ALL, OBJECT, STATIC, PLAYER, MONSTER, INTERACTION, END };
	enum class LIGHT_TYPE		{ POINT = 1, SPOT, DIRECTIONAL, END };

	enum class MOUSEKEYSTATE	{ LBUTTON, RBUTTON, WBUTTON, XBUTTON };
	enum class MOUSEMOVESTATE	{ HORIZONTAL, VERTICAL, W };

	enum class WINMODE			{ FULL, WIN, END };
	enum class KEY_INPUT		{ KEYBOARD, MOUSE, END };
								// 이전 프레임 뷰 투영도 추가
	enum class D3DTS			{ VIEW, PROJ, END };
	enum class MODEL_TYPE		{ ANIM, NONANIM, END };
								
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
		EXTRA3 = 11,
		EXTRA4 = 12,
		EXTRA5 = 13,
		EXTRA6 = 14,
		EXTRA7 = 15,
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
		NONBLEND,
		NONLIGHT,
		BLUR,
		GLOW,
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
		FINAL
	};

	
	enum class AXIS				{ HORIZONTAL, VERTICAL, END };

	enum class NAVI_POINT		{ A, B, C, END };
	enum class NAVI_LINE		{ AB, BC, CA, END };

	enum CHANNELID				{ BGM, EFFECT, EFFECT2, END };
}
#endif // Engine_Enum_h__
