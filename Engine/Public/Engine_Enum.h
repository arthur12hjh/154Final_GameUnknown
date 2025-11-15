#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
#ifdef _DEBUG
	enum class GAMELOOP_TYPE	{ PRIORITY, UPDATE, LATE_UPDATE, RENDER, COLLISION, END};
#endif // _DEBUG

	enum class PROTOTYPE		{ GAMEOBJECT, COMPONENT };
	enum class COLLIDER			{ AABB, OBB, SPHERE, END };

	// 추가 예정
	enum class HIT_TYPE			{ ALL, OBJECT, END };
	enum class LIGHT_TYPE		{ POINT = 1, SPOT, DIRECTIONAL, END };

	enum class MOUSEKEYSTATE	{ LBUTTON, RBUTTON, WBUTTON, XBUTTON };
	enum class MOUSEMOVESTATE	{ HORIZONTAL, VERTICAL, W };

	enum class WINMODE			{ FULL, WIN, END };
	enum class KEY_INPUT		{ KEYBOARD, MOUSE, END };
								
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
		HEIGHT = 5,
		NORMAL = 6,
		SHININESS = 7,
		OPACITY = 8,
		LIGHTMAP = 10,
		REFLECTION = 11,
		BASE_COLOR = 12,
		NORMAL_CAMERA = 13,
		EMISSION_COLOR = 14,
		METALNESS = 15,
		ROUGHNESS = 16,
		OCCLUSION = 17,
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
		//여기까진 조명처리
		DEBUG, DIRECTIONAL, POINT, COMBINED,
		DEFERRED,
		SCREEN_RADIAL_BLUR, FINAL
	};

	
	enum class AXIS				{ HORIZONTAL, VERTICAL, END };

	enum class NAVI_POINT		{ A, B, C, END };
	enum class NAVI_LINE		{ AB, BC, CA, END };

	enum CHANNELID				{ BGM, EFFECT, EFFECT2, END };
}
#endif // Engine_Enum_h__
