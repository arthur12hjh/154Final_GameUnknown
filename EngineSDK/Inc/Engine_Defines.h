#ifndef Engine_Define_h__
#define Engine_Define_h__

//WIN
#include <Windows.h>
#include <assert.h>
#include <iostream>
#include <wincodec.h>

//FILE Input
#include <fstream>
#include <sstream>

//STL
#include <set>
#include <unordered_set>

#include <stack>
#include <vector>
#include <list>
#include <unordered_map>
#include <map>
#include <queue>
#include <functional>
#include <string>
#include <bitset>

#include <thread>
#include <condition_variable>
#include <mutex>

using namespace std;

//Dx11
#include <d3d11.h>
#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>

#pragma warning(disable : 4251)
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <d3dcompiler.h>

//Dx11 Texture Headers
#include "DirectXTK/DDSTextureLoader.h"
#include "DirectXTK/WICTextureLoader.h"

//Dx11 Shader
#include "DirectXTK/Effects.h"
#include "FX11/d3dx11effect.h"

//Dx11 Collision Debug
#include "DirectXTK/VertexTypes.h"
#include "DirectXTK/PrimitiveBatch.h"

//Dx11 Font
#include "DirectXTK/SpriteBatch.h"
#include "DirectXTK/SpriteFont.h"

#include "DirectXTK/ScreenGrab.h"

//Assimp
#include "FX11/d3dx11effect.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/Importer.hpp"
using namespace DirectX;

//FMode
#include "FMOD/fmod.h"
#include "FMOD/fmod.hpp"
#include "FMOD/fmod_errors.h"
#include <io.h>

//TinyXML
#include "tinyxml2.h"

#define XML tinyxml2

#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"

#include "Engine_Math.h"

namespace Engine
{
	const static wstring g_strTransformTag = { TEXT("Com_Transform")};
}

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif


using namespace Engine;


#endif // Engine_Define_h__
