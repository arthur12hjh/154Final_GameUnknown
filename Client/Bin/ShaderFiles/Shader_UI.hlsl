#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_Texture;
Texture2D g_Texture0;
Texture2D g_Texture1;
Texture2D g_Texture2;
Texture2D g_Texture3;
Texture2D g_Texture4;
Texture2D g_Texture5;
Texture2D g_Texture6;
Texture2D g_Texture7;
Texture2D g_DepthTexture;

vector g_Color = 1.f;

float2 g_UIPosition;
float2 g_UIOffset;
float2 g_UISize;
vector g_UIDebugLineColor;

float g_Alpha = 1.f;

float2 g_UVScale = { 1.f, 1.f }; // 반복, 확대/축소
float2 g_UVOffset = { 0.f, 0.f }; // 스크롤, 이동
float2 g_UVGap = { 0.f, 0.f }; // 간격
float g_GroupCount = 1.f; // 그룹 개수

float2 g_TileCount = { 1.f, 1.f }; // 타일 몇개로 쪼갤지

bool g_bUseFillClip = { false }; // 텍스쳐 클리핑
float g_fFillAmount = { 1.f }; // 클리핑 얼마나 할지

bool g_bUseTintColor = { false };
vector g_vTintColor = 1.f;

bool g_bDiscardBlack = { false };

bool g_bUseGlow = { false };
bool g_bUsePulse = { false };
float g_PulseTime = 0.f; // 시간 (sin(g_Time)같은거 하기위해
float g_PulseSpeed = 3.0f; // 반짝이는 시간
float g_GlowIntensity = 0.5f; // 확산 색상 강도 조절
float g_GlowSpread = 1.0f; // 빛이 퍼지는 정도 1 ~ 4 정도 추천

bool g_bScroll = false; // uv사용 여부
float g_ScrollSpeed = 0.5f; // uv에 따라 및이 이동하는 속도

bool g_bUseCoolTime = false;
float g_fCoolAmount = 1.f; // 0~1

bool g_isUseable = false; // 스킬 사용 가능 여부
bool g_UseCover = false;

bool g_bRushActiveOn = false;

bool g_bUseScale = false;
float g_fScale = { 1.f };

bool g_isFinisher = false;
float g_fRotation = 0.f;

bool g_isActive = false;

float2 g_vTransOffset = { 0.f, 0.f };

float2 g_AtlasCount = { 0.f, 0.f };
float2 g_AtlasIndex = { 0.f, 0.f };

float2 g_vWindowSize = { 1600.f, 900.f };

BlendState BS_Additive
{
    BlendEnable[0] = true;

    // RGB Additive
    SrcBlend = ONE;
    DestBlend = ONE;
    BlendOp = Add;

    // Alpha: Src 유지 (보통 UI Glow는 DestAlpha 필요 없음)
    SrcBlendAlpha = ONE;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = Add;

    RenderTargetWriteMask[0] = 0x0F; // RGBA 쓰기 가능
};

/*------------------[S_DEBUG]---------------*/

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
    
    float4 vUVAtlasSize : TEXCOORD1;
    float4 vUVAtlasOffset : TEXCOORD2;
    float4 vAtlasIndex : TEXCOORD3;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vAtlasIndex : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
  
    /* In.vPosition * 월드 * 뷰 * 투영 */    
    //float4x4 == matrix
    matrix matWV, matWVP;
  
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
  
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
  
    Out.vTexcoord = In.vTexcoord;
      
    return Out;
}

VS_OUT VS_INSTANCE_MAIN(VS_IN In)
{
    VS_OUT Out;

    float3 pos = In.vPosition.xyz;
  
    if (In.vUVAtlasSize.z != 0.f && In.vUVAtlasSize.w != 0)
        pos.xy *= In.vUVAtlasSize.zw;
  
    pos.xy += In.vUVAtlasOffset.zw;
  
    // Position    
    float4 worldPos = mul(float4(pos, 1.f), g_WorldMatrix);
    float4 viewPos = mul(worldPos, g_ViewMatrix);
    Out.vPosition = mul(viewPos, g_ProjMatrix);

    // Local UV
    float2 uvScale = In.vUVAtlasSize.xy;
    float2 uvOffset = In.vUVAtlasOffset.xy;

    if (uvScale.x == 0.f && uvScale.y == 0.f)
    {
        uvScale = float2(1.f, 1.f);
        uvOffset = float2(0.f, 0.f);
    }

    float2 localUV = In.vTexcoord * uvScale + uvOffset;

    // Atlas UV
    float2 atlasCount = max(g_AtlasCount.xy, float2(1.f, 1.f));
    float2 tileSize = 1.f / atlasCount;
    float2 tileMin = In.vAtlasIndex.xy * tileSize;
  
    float2 final = tileMin + localUV * tileSize;
  
    Out.vTexcoord = final;
    Out.vAtlasIndex = In.vAtlasIndex;

    return Out;
}


/* 출력된 정점 위치벡터의 w값으로 모든 성분을 나눈다 -> 투영스페이스로 변환 */ 
/* 정점의 위치에 대해서 뷰포트 변환을 수행한다 */ 
/* 정점의 모든 정보를 보간하여 픽셀을 만든다. -> 래스터라이즈 */ 

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vAtlasIndex : TEXCOORD1;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    float2 uv = In.vTexcoord;

    // ----------------------------
    //  UV 조작
    // ----------------------------
    if (g_UVScale.x != 1.f || g_UVScale.y != 1.f ||
        g_UVOffset.x != 0.f || g_UVOffset.y != 0.f)
    {
        uv = uv * g_UVScale + g_UVOffset;

        // 반복
        uv = frac(uv);
    }

    // ----------------------------
    //  텍스처 샘플링
    // ----------------------------
    
    Out.vColor = g_Texture.Sample(DefaultSampler, uv);

    // ----------------------------
    //  Fill Clip (오른쪽부터 잘림)
    // ----------------------------
    if (g_bUseFillClip)
    {
        if (In.vTexcoord.x > g_fFillAmount)
            discard;
    }
    
    // ----------------------------
    //  TintColor
    // ----------------------------
    if (g_bUseTintColor)
    {
        Out.vColor.rgb *= g_vTintColor.rgb;
        Out.vColor.a *= g_vTintColor.a;
    }
    
    // ----------------------------
    //  Alpha 적용
    // ----------------------------
    Out.vColor.a *= g_Alpha;

    if (Out.vColor.a <= 0.0f)
        discard;

    return Out;
}

/*------------------[E_DEFAULT]---------------*/


/*------------------[S_DEBUG]---------------*/

struct VS_IN_DEBUG
{
    float4 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT_DEBUG
{
    float4 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT_DEBUG VS_MAIN_DEBUG(VS_IN_DEBUG In)
{
    VS_OUT_DEBUG Out;
    
    Out.vPosition = float4(g_UIPosition, 0, 1);
    Out.vTexcoord = float2(0, 0); // 필요 없음
    
    return Out;
}

struct GS_IN_DEBUG
{
    float4 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct GS_OUT_DEBUG
{
    float4 vPosition : SV_POSITION;
};

[maxvertexcount(6)]
void GS_MAIN_DEBUG(point GS_IN_DEBUG In[1], inout TriangleStream<GS_OUT_DEBUG> OutStream)
{
    GS_OUT_DEBUG Out[4];
    
    matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matrix matWVP = mul(matWV, g_ProjMatrix);
    
    Out[0].vPosition = mul(float4(-0.5f, 0.5f, 0.f, 1.f), matWVP);
    ///Out[0].vTexcoord = float2(0.f, 0.f);
    
    Out[1].vPosition = mul(float4(0.5f, 0.5f, 0.f, 1.f), matWVP);
    //Out[1].vTexcoord = float2(1.f, 0.f);
    
    Out[2].vPosition = mul(float4(0.5f, -0.5f, 0.f, 1.f), matWVP);
    //Out[2].vTexcoord = float2(1.f, 1.f);
    
    Out[3].vPosition = mul(float4(-0.5f, -0.5f, 0.f, 1.f), matWVP);
    //Out[3].vTexcoord = float2(0.f, 1.f);
    
    OutStream.Append(Out[0]);
    OutStream.Append(Out[1]);
    OutStream.Append(Out[2]);
    OutStream.RestartStrip();
    
    OutStream.Append(Out[0]);
    OutStream.Append(Out[2]);
    OutStream.Append(Out[3]);
    OutStream.RestartStrip();
}

float4 PS_MAIN_DEBUG() : SV_TARGET
{
    return g_UIDebugLineColor;
}

/*------------------[E_DEBUG]---------------*/

/*------------------[S_GLOW]---------------*/
PS_OUT PS_UI_GLOW(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    if (Out.vColor.r <= 0.05f || Out.vColor.g <= 0.05f
        || Out.vColor.b <= 0.05f || Out.vColor.a <= 0.05f)
        discard;

    return Out;
}

/*------------------[E_GLOW]---------------*/

/*------------------[S_FX_GLOW]----------------*/

PS_OUT PS_UI_GLOW_FX(PS_IN In)
{
    PS_OUT Out;

    float2 uv = In.vTexcoord;

    // ----------------------------
    // 1) UV Scroll (선택)
    // ----------------------------
    //if (g_bScroll)
    //{
    //    uv.x += g_PulseTime * g_ScrollSpeed;
    //    uv = frac(uv);
    //}

    // ----------------------------
    //  UV 조작
    // ----------------------------
    //if (g_UVScale.x != 1.f || g_UVScale.y != 1.f ||
    //    g_UVOffset.x != 0.f || g_UVOffset.y != 0.f)
    //{
    //    uv = uv * g_UVScale + g_UVOffset;
    //
    //    // 반복
    //    uv = frac(uv);
    //}

    float4 baseColor = g_Texture.Sample(DefaultSampler, uv);

    // ----------------------------
    // 2) Pulse Alpha (반짝임)
    // ----------------------------
    //if(g_bUsePulse)
    //{
    //    float pulse = (sin(g_PulseTime * g_PulseSpeed) * 0.5f + 0.5f); // 0~1
    //    baseColor.rgb *= pulse;
    //    baseColor.a *= pulse;
    //}

    // ----------------------------
    // 3) Glow 확산 샘플링
    //    좌우로만 퍼져보이는 "레이저 라인" 효과
    // ----------------------------
    
    if(g_bUseGlow)
    {
        float2 texel = float2(g_GlowSpread / 1024.0f, 0); // 텍스처 크기 기반
    
        float4 glow =
            g_Texture.Sample(DefaultSampler, uv + texel) +
            g_Texture.Sample(DefaultSampler, uv - texel) +
            g_Texture.Sample(DefaultSampler, uv + texel * 2) +
            g_Texture.Sample(DefaultSampler, uv - texel * 2);
        
        baseColor.rgb *= glow.rgb * g_GlowIntensity;
        baseColor.a *= g_Alpha;
    }
    
    // ----------------------------
    //  TintColor
    // ----------------------------
    if (g_bUseTintColor)
    {
        baseColor.rgb *= g_vTintColor.rgb;
        baseColor.a *= g_vTintColor.a * g_Alpha;
    }
    
    // 알파 너무 낮으면 버리기
    if (baseColor.a <= 0.05f)
        discard;
    
    Out.vColor = baseColor;
    
    if (Out.vColor.a <= 0.0f)
        discard;
    
    //Out.vColor.a *= g_Alpha;
    //
    //if(Out.vColor.a <= 0.05f)
    //    discard;

    return Out;
}

/*------------------[E_FX_GLOW]----------------*/

/*------------------[S_HP_GAUGE]----------------*/

PS_OUT PS_HP_GAUGE(PS_IN In)
{
    PS_OUT Out;
  
    float2 uv = In.vTexcoord * g_UVScale + g_UVOffset;
    uv = frac(uv);

    // ----------------------------
    //  텍스처 샘플링
    // ----------------------------
  
    float4 BgColor = g_Texture0.Sample(DefaultSampler, uv);
    float4 GaugeColor = g_Texture1.Sample(DefaultSampler, uv);

    // ----------------------------
    //  Fill Clip (오른쪽부터 잘림)
    // ----------------------------
  
    if (In.vTexcoord.x > g_fFillAmount)
    {
        GaugeColor = float4(0, 0, 0, 0);
    }
  
    // ----------------------------
    //  TintColor
    // ----------------------------
    if (g_bUseTintColor)
    {
        GaugeColor.rgb *= g_vTintColor.rgb;
        GaugeColor.a *= g_vTintColor.a;
    }
  
    float4 CombinedColor = BgColor;
    CombinedColor = lerp(CombinedColor, GaugeColor, GaugeColor.a);
  
    // ----------------------------
    //  Alpha 적용
    // ----------------------------
    Out.vColor = CombinedColor;
    //Out.vColor.a *= g_Alpha;

    if (Out.vColor.a <= 0.0f)
        discard;
    
    return Out;
}

/*------------------[E_HP_GAUGE]----------------*/

/*------------------[S_POTION]----------------*/

PS_OUT PS_POTION(PS_IN In)
{
    PS_OUT Out;
        
    float2 uv = (In.vTexcoord) * g_UVScale + g_UVOffset;
    
    uv = frac(uv);

    // ----------------------------
    //  텍스처 샘플링
    // ----------------------------
    
    float4 GaugeColor = g_Texture.Sample(DefaultSampler, uv);
    float4 TintColor = 1.f;
    
    // ----------------------------
    //  Fill Clip (윗쪽부터 잘림)
    // ----------------------------
    
    if (In.vTexcoord.y < 1.f - g_fFillAmount)
    {
        discard;
    }
    
    if(g_bUseTintColor)
        TintColor = g_vTintColor;
    
    // ----------------------------
    //  Alpha 적용
    // ----------------------------
    Out.vColor = GaugeColor *= TintColor;
    //Out.vColor.a *= g_Alpha;
    
    if (Out.vColor.a <= 0.0f)
        discard;

    return Out;
}

/*------------------[E_POTION]----------------*/

/*------------------[S_SHIELD]----------------*/

PS_OUT PS_SHIELD(PS_IN In)
{    
    PS_OUT Out;
    Out.vColor = float4(0, 0, 0, 0);

    float2 uv = In.vTexcoord;

    //// -----------------------------------------
    //// 타입 보정
    //// -----------------------------------------
    int groupCount = (int) floor(g_GroupCount);

    if (groupCount <= 0)
        return Out;

    // -----------------------------------------
    // GAP + BLOCK 레이아웃 계산
    // -----------------------------------------
    float totalGap = g_UVGap * (groupCount - 1);
    float blockWidth = (1.0 - totalGap) / groupCount;
    float totalUnit = blockWidth + g_UVGap;

    float pos = uv.x / totalUnit;
    int blockIndex = (int) floor(pos);

    if (blockIndex < 0 || blockIndex >= groupCount)
        return Out;

    float blockStart = blockIndex * totalUnit;
    float blockEnd = blockStart + blockWidth;

    // GAP 영역
    if (uv.x > blockEnd && uv.x < blockEnd + g_UVGap.x)
        return Out;

    // -----------------------------------------
    // Block 내 local X
    // -----------------------------------------
    float localX = (uv.x - blockStart) / blockWidth;
    localX = saturate(localX);

    // -----------------------------------------
    // FillAmount
    // -----------------------------------------
    float totalFill = g_fFillAmount * groupCount;
    float localFill = saturate(totalFill - blockIndex);

    // -----------------------------------------
    // UV Tile
    // -----------------------------------------
    float2 tileUV = float2(0, 0);
    tileUV.x = frac(localX * g_UVScale.x);
    tileUV.y = frac(uv.y * g_UVScale.y);

    // BG
    float4 bg = g_Texture0.Sample(DefaultSampler, tileUV);

    // FG
    float4 fg = float4(0, 0, 0, 0);
    if (localX <= localFill)
        fg = g_Texture1.Sample(DefaultSampler, tileUV);

    if (g_bUseTintColor)
        fg *= g_vTintColor;

    Out.vColor = lerp(bg, fg, fg.a);
    
    if (Out.vColor.a <= 0.0f)
        discard;
    
    return Out;
}

/*------------------[E_SHIELD]----------------*/

/*------------------[S_BETA]----------------*/

PS_OUT PS_BETA(PS_IN In)
{    
    PS_OUT Out;
    Out.vColor = float4(0, 0, 0, 0);

    float2 uv = In.vTexcoord;

    float groupWidth = (1.0 - g_UVGap * (g_GroupCount - 1)) / g_GroupCount;

    // 현재 uv.x가 어느 묶음에 속하는지 계산
    float x = uv.x;
    float groupIndex = -1;

    float start = 0;
    float end = groupWidth;

    for (int i = 0; i < g_GroupCount; i++)
    {
        if (x >= start && x < end)
        {
            groupIndex = i;
            break;
        }
        // 다음 묶음 시작점 갱신
        start = end + g_UVGap;
        end = start + groupWidth;
    }

    // GAP인 경우 완전 투명 반환
    if (groupIndex < 0)
        return Out;

    // 묶음 내부 local UV (0~1)
    float localX = saturate((x - start) / groupWidth);
    float localY = saturate(uv.y);

    // 2×2 타일 구역
    float tileX = clamp(floor(localX * g_TileCount.x), 0, g_TileCount.x - 1); // 0~1
    float tileY = clamp(floor(localY * g_TileCount.y), 0, g_TileCount.y - 1); // 0~1

    // 타일 순서: (0,0)->(0,1)->(1,0)->(1,1)
    float tileIndexWithinGroup = tileX * 2 + tileY;

    float tilesPerGroup = g_TileCount.x * g_TileCount.y; // 4
    float tileIndex = groupIndex * tilesPerGroup + tileIndexWithinGroup;

    // 전체 타일 20칸 기준 FillAmount 변환
    float totalTiles = tilesPerGroup * g_GroupCount; // 5×4=20
    float filledTiles = g_fFillAmount * totalTiles;

    // 타일 반복 좌표
    float2 tileUV;
    tileUV.x = frac(localX * g_TileCount.x);
    tileUV.y = frac(localY * g_TileCount.y);

    // BG = 항상 FULL
    float4 bg = g_Texture0.Sample(DefaultSampler, tileUV);

    // FG = Fill 조건 만족할 때만 출력
    float4 fg = float4(0, 0, 0, 0);
    if (tileIndex < filledTiles)
        fg = g_Texture1.Sample(DefaultSampler, tileUV);
  
    fg *= g_vTintColor;

    Out.vColor = lerp(bg, fg, fg.a);
    
    if (Out.vColor.a <= 0.0f)
        discard;
    
    return Out;
}

/*------------------[E_BETA]----------------*/

/*------------------[S_BETA_FX]----------------*/

PS_OUT PS_BETA_FX(PS_IN In)
{
    PS_OUT Out;
    
    float2 uv = In.vTexcoord;
    
    float4 result = 0.f;
    
    if(g_isActive)
    {
        float2 Scale2UV = In.vTexcoord;
        Scale2UV -= float2(0.5f, 0.5f);
        Scale2UV /= 2.25f;
        Scale2UV += float2(0.5f, 0.5f);
   
        float4 glow1 = g_Texture1.Sample(ClampSampler, Scale2UV);
        result.rgb = glow1.rgb * float3(0.988f, 1.000f, 1.000f) * g_GlowIntensity;
        //result.rgb = glow1.rgb * float3(0.f, 0.6f, 0.6f) * g_GlowIntensity;
    }
    else
    {
        float2 Center = float2(0.5f, 0.5f);
        float s = sin(g_fRotation);
        float c = cos(g_fRotation);

        float2 rotatedUV;
        float2 d = uv - Center;
        rotatedUV.x = d.x * c - d.y * s;
        rotatedUV.y = d.x * s + d.y * c;
    
        rotatedUV /= g_fScale;
    
        rotatedUV += Center;
        
        float4 glow0 = g_Texture0.Sample(ClampSampler, rotatedUV);
        
        result.rgb = glow0.rgb * float3(0.988f, 1.000f, 1.000f);
        //result.rgb = glow0.rgb * float3(0.f, 0.6f, 0.6f);
    }
    
    Out.vColor.rgb = result.rgb * g_Alpha;
    
    return Out;
}

/*------------------[E_BETA_FX]----------------*/

/*------------------[S_SKILL_SLOT]----------------*/
PS_OUT PS_SKILL_SLOT(PS_IN In)
{
    PS_OUT Out;
    
    float2 uv = In.vTexcoord;
    float4 TintColor = 1.f;
    
    float4 shadow = g_Texture0.Sample(DefaultSampler, uv);
    float4 frame = g_Texture1.Sample(DefaultSampler, uv);
    float4 icon = 0.f;
    float4 cover = 0.f;
    float4 cost = 0.f;
    float4 costDeco = 0.f;
    
    if(g_bUseTintColor)
        TintColor = g_vTintColor;
    
    if(g_isUseable)
    {   
        icon = g_Texture2.Sample(DefaultSampler, uv);
        icon *= TintColor;
        
        float2 vCostScale = uv;
        vCostScale -= float2(0.7f, 0.8f);
        vCostScale /= 0.2f;
        vCostScale += float2(0.7f, 0.8f);
        
        cost = g_Texture4.Sample(ClampSampler, vCostScale);
        float4 costTint = float4(
            0.443136990070343,
            0.49803900718688965,
            0.521569013595581,
            1.0
        );
        
        cost.rgb *= costTint.rgb;
        cost.a *= costTint.a;
        
        float2 vCostDecoScale = uv;
        vCostDecoScale -= float2(0.8f, 0.8f);
        vCostDecoScale /= 0.1f;
        vCostDecoScale += float2(0.8f, 0.8f);
        
        costDeco = g_Texture5.Sample(ClampSampler, vCostDecoScale);
        costDeco.rgb *= costTint.rgb;
        costDeco.a *= costTint.a;
    }
    
    if (g_UseCover)
    {   
        cover = g_Texture3.Sample(DefaultSampler, In.vTexcoord) * g_Alpha;
    }
    
    float4 result = shadow;
    result = lerp(result, frame, frame.a);
    result = lerp(result, icon, icon.a);
    result = lerp(result, cover, cover.a);
    result = lerp(result, cost, cost.a);
    result = lerp(result, costDeco, costDeco.a);
    
    Out.vColor = result;
    if (Out.vColor.a <= 0.0f)
        discard;
    
    return Out;
}

/*------------------[E_SKILL_SLOT]----------------*/

/*------------------[S_SKILL_SLOT_GLOW]----------------*/

PS_OUT PS_SKILL_SLOT_GLOW(PS_IN In)
{
    PS_OUT Out;
  
    float2 uv = In.vTexcoord;

    float4 glow0 = g_Texture0.Sample(DefaultSampler, uv);
    float4 glow1 = g_Texture1.Sample(DefaultSampler, uv);
    
    if(g_bUseGlow)
    {
        glow0.rgb += glow0.rgb * g_GlowIntensity;
        glow0.rgb *= glow0.a * 0.5f;
        glow1.rgb += glow1.rgb * (g_GlowIntensity * 5.f);
        glow1.rgb *= glow1.a * 0.5f;
    }
    
    float4 result = glow0;
    result = lerp(result, glow1, glow1.a);
    
    Out.vColor = result;
    if (Out.vColor.a <= 0.0f)
        discard;
    
    return Out;
}

/*------------------[E_SKILL_SLOT_GLOW]----------------*/

/*------------------[S_RUSH_SLOT]----------------*/
float CoolMask(float2 uv, float amount)
{
    // UV 중심 기준 (-0.5 ~ 0.5)
    float2 centerUV = uv - float2(0.5f, 0.5f);

    centerUV.x = -centerUV.x; // Y축 반전
    
    // atan2(y, x) 로 각도 구하기 (-pi ~ pi)
    float angle = atan2(centerUV.x, centerUV.y);
    
    // 0 ~ 1로 정규화
    float t = (angle + 3.14159265f) / (2.0f * 3.14159265f);

    // 시계 방향 FillAmount보다 크면 가림
    return step(t, amount);
}

PS_OUT PS_RUSH_SLOT(PS_IN In)
{
    PS_OUT Out;
    
    float4 TintColor = 1.f;
    //float4 CoolTimeColor = float4(0.6235294118, 0.6823529412, 0.7882352941, 1.0);
    
    float4 shadow = g_Texture0.Sample(DefaultSampler, In.vTexcoord);
    float4 frame = g_Texture1.Sample(DefaultSampler, In.vTexcoord);
    float4 icon = g_Texture2.Sample(DefaultSampler, In.vTexcoord);
    float4 cooltime = 0.f;
    
    if(g_bUseTintColor)
        TintColor = g_vTintColor;
    
    icon *= TintColor;
        
    if (g_bUseCoolTime)
    {
        cooltime = g_Texture3.Sample(DefaultSampler, In.vTexcoord);
        
        float mask = CoolMask(In.vTexcoord, g_fCoolAmount);
        cooltime *= mask;
        cooltime.a *= 1.25f;
    }
    
    float4 result = shadow;
    result = lerp(result, frame, frame.a);
    result = lerp(result, icon, icon.a);
    result = lerp(result, cooltime, cooltime.a);
    //result += cooltime * CoolTimeColor;
    
    Out.vColor = result;
    
    if (Out.vColor.a <= 0.0f)
        discard;
    
    return Out;
}

/*------------------[E_RUSH_SLOT]----------------*/

/*------------------[S_RUSH_SLOT_GLOW]----------------*/

PS_OUT PS_RUSH_SLOT_GLOW(PS_IN In)
{    
    PS_OUT Out;
  
    float2 uv = In.vTexcoord;

    float4 glow0 = g_Texture0.Sample(DefaultSampler, uv);
    float4 glow1 = g_Texture1.Sample(DefaultSampler, uv);
    float4 glow2 = 0.f;
    float4 glow3 = 0.f;
    
    if (g_bUseGlow)
    {
        glow0.rgb += glow0.rgb * g_GlowIntensity;
        glow0.rgb *= glow0.a * 0.5f;
        glow1.rgb += glow1.rgb * (g_GlowIntensity * 5.f);
        glow1.rgb *= glow1.a * 0.5f;
    }
    
    float4 result = glow0;
    result = lerp(result, glow1, glow1.a);
    
    if (g_bRushActiveOn)
    {
        glow2 = g_Texture2.Sample(DefaultSampler, uv);
        
        glow2.rgb += glow2.rgb * (g_GlowIntensity * 0.5f);
        glow2.rgb *= glow2.a * 0.25f;
        result += glow2;
    }
    
    Out.vColor = result;
    
    if (Out.vColor.a <= 0.0f)
        discard;
    
    return Out;
}

/*------------------[E_RUSH_SLOT_GLOW]----------------*/

/*------------------[S_SKILL_WRAPPER_ON_LINE]----------------*/

PS_OUT PS_SKILL_WRAPPER_ON_LINE(PS_IN In)
{
    PS_OUT Out;
  
    float2 uv = In.vTexcoord;

    float4 glow0 = g_Texture0.Sample(DefaultSampler, uv);
    float4 glow1 = g_Texture1.Sample(DefaultSampler, uv);
    
    glow0.rgb += glow0.rgb * g_GlowIntensity;
    glow0.rgb *= glow0.a * 0.5f;
    glow1.rgb += glow1.rgb * g_GlowIntensity;
    glow1.rgb *= glow1.a * 0.5f;
    
    float4 result = saturate(glow0 + glow1);
    
    Out.vColor = result * g_Alpha;
    
    if (Out.vColor.a <= 0.0f)
        discard;
    
    return Out;
}

/*------------------[E_RUSH_SLOT_GLOW]----------------*/

/*------------------[S_SKILL_WRAPPER_ON_FX]----------------*/

PS_OUT PS_SKILL_WRAPPER_ON_FX(PS_IN In)
{
    PS_OUT Out;
  
    float2 uv = In.vTexcoord;

    float4 glow0 = g_Texture0.Sample(DefaultSampler, uv);
    float mask = g_Texture1.Sample(DefaultSampler, uv).r;
    
    glow0.rgb += glow0.rgb * g_GlowIntensity;
    glow0.rgb *= glow0.a * 0.5f;
    
    float alpha = mask;

    glow0.rgb *= alpha; // Additive 유지하면서 투명하게 만들기
    glow0.a = alpha;
    
    Out.vColor = glow0 * g_Alpha;
    
    if (Out.vColor.a <= 0.0f)
        discard;
    
    return Out;
}

/*------------------[E_SKILL_WRAPPER_ON_FX]----------------*/

/*------------------[S_LOADING_BLUR]----------------*/

PS_OUT PS_LOADING_BLUR(PS_IN In)
{
    PS_OUT Out;
  
    float2 uv = In.vTexcoord;
    
    float2 texel = float2(0.002, 0.002) * 2.f; // Blur 강도 조절

    float4 sum = 0;

    // 9탭 Gaussian Blur
    //sum += g_Texture0.Sample(DefaultSampler, uv + texel * float2(-1, -1)) * 0.05;
    //sum += g_Texture0.Sample(DefaultSampler, uv + texel * float2(0, -1)) * 0.09;
    //sum += g_Texture0.Sample(DefaultSampler, uv + texel * float2(1, -1)) * 0.12;

    //sum += g_Texture0.Sample(DefaultSampler, uv + texel * float2(-1, 0)) * 0.15;
    //sum += g_Texture0.Sample(DefaultSampler, uv + texel * float2(0, 0)) * 0.18;
    //sum += g_Texture0.Sample(DefaultSampler, uv + texel * float2(1, 0)) * 0.15;

    //sum += g_Texture0.Sample(DefaultSampler, uv + texel * float2(-1, 1)) * 0.12;
    //sum += g_Texture0.Sample(DefaultSampler, uv + texel * float2(0, 1)) * 0.09;
    //sum += g_Texture0.Sample(DefaultSampler, uv + texel * float2(1, 1)) * 0.05;
    
    // 1px 주변
    sum += g_Texture0.Sample(DefaultSampler, uv + texel * float2(1, 0)) * 0.15;
    sum += g_Texture0.Sample(DefaultSampler, uv + texel * float2(-1, 0)) * 0.15;
    sum += g_Texture0.Sample(DefaultSampler, uv + texel * float2(0, 1)) * 0.15;
    sum += g_Texture0.Sample(DefaultSampler, uv + texel * float2(0, -1)) * 0.15;

// 대각선 1px
    sum += g_Texture0.Sample(DefaultSampler, uv + texel * float2(1, 1)) * 0.10;
    sum += g_Texture0.Sample(DefaultSampler, uv + texel * float2(-1, 1)) * 0.10;
    sum += g_Texture0.Sample(DefaultSampler, uv + texel * float2(1, -1)) * 0.10;
    sum += g_Texture0.Sample(DefaultSampler, uv + texel * float2(-1, -1)) * 0.10;

// 2px 주변
    sum += g_Texture0.Sample(DefaultSampler, uv + texel * float2(2, 0)) * 0.03;
    sum += g_Texture0.Sample(DefaultSampler, uv + texel * float2(-2, 0)) * 0.03;
    sum += g_Texture0.Sample(DefaultSampler, uv + texel * float2(0, 2)) * 0.03;
    sum += g_Texture0.Sample(DefaultSampler, uv + texel * float2(0, -2)) * 0.03;
  
    Out.vColor = sum;
    
    if(g_bUseGlow)
    {
        Out.vColor.a *= 0.2f;
        Out.vColor.rgb += sum.rgb * g_GlowIntensity;
    }
    
    if (Out.vColor.a <= 0.0f)
        discard;
    
    return Out;
}

/*------------------[E_LOADING_BLUR]----------------*/

/*------------------[S_SIMPLE_KEY]----------------*/

PS_OUT PS_SIMPLE_KEY(PS_IN In)
{
    PS_OUT Out;

    float2 uv = In.vTexcoord;
    float2 KeyUv = In.vTexcoord;
    float baseScale = 0.25f;
    float4 cooltime = 0.f;
    float4 locked = 0.f;
    
    //float4 Color = float4(1.f, 1.f, 1.f, 1.f);
    
    //uv -= float2(0.5f, 0.5f);
    //uv /= baseScale;
    //uv += float2(0.5f, 0.5f);
    
    //float base = g_Texture0.Sample(ClampSampler, uv).r;
    
    //base *= 1.f - g_fScale;
    //Color.a *= base;
    
    if(g_bUseScale)
    {
        KeyUv -= float2(0.5f, 0.5f);
        KeyUv /= g_fScale;
        KeyUv += float2(0.5f, 0.5f);
    }
    
    float4 key = g_Texture0.Sample(ClampSampler, KeyUv);
    
    key.a *= g_Alpha;
    
    if (g_bUseCoolTime)
    {
        cooltime = g_Texture1.Sample(DefaultSampler, In.vTexcoord);
        
        float mask = CoolMask(In.vTexcoord, g_fCoolAmount);
        cooltime *= mask;
        //cooltime.a *= 1.25f;
        //cooltime.a = g_Alpha;
    }
    
    float4 result = key;
    result = lerp(result, cooltime, cooltime.a);
    result.a *= g_Alpha;
    
    Out.vColor = result;
    
    if (Out.vColor.a <= 0.0f)
        discard;
    
    return Out;
}

/*------------------[E_SIMPLE_KEY]----------------*/

/*------------------[S_INTERACTION_FX]----------------*/

PS_OUT PS_INTERACTION_FX(PS_IN In)
{
    PS_OUT Out;

    float2 uv = In.vTexcoord;
    
    float4 base = g_Texture0.Sample(ClampSampler, uv);
    base.a *= g_Alpha * 0.5f;
    
    Out.vColor = base;
    
    return Out;
}

/*------------------[E_INTERACTION_FX]----------------*/

/*------------------[S_INTERACTION_FX_GLOW]----------------*/

PS_OUT PS_INTERACTION_FX_GLOW(PS_IN In)
{
    PS_OUT Out;
    float2 uv = In.vTexcoord;
    float2 ScaleUV = In.vTexcoord;

    ScaleUV -= float2(0.5f, 0.5f);
    ScaleUV /= g_fScale;
    ScaleUV += float2(0.5f, 0.5f);
    
    // GlowMask
    float4 glow = g_Texture0.Sample(ClampSampler, ScaleUV);
    // LightFX
    float4 flare = g_Texture1.Sample(DefaultSampler, uv);

    // --- Additive 합성 ---
    // Additive = rgb끼리 더하고 alpha는 필요 없음
    float3 result = float3(0, 0, 0);

    // GlowMask의 "빛나는 부분"만 add
    glow.a *= g_Alpha * 0.5f;
    result += glow.rgb * glow.a * (g_GlowIntensity * 0.5f);
    
    // LightFX의 "빛나는 부분" 추가
    flare.a *= g_Alpha;
    result += flare.rgb * flare.a * g_GlowIntensity;
    
    Out.vColor = float4(result, 1.f); // additive는 보통 알파 1로 출력
    
    if (Out.vColor.a <= 0.0f)
        discard;
    
    return Out;
}

/*------------------[E_INTERACTION_FX_GLOW]----------------*/

/*------------------[S_STAMINA]----------------*/

PS_OUT PS_STAMINA(PS_IN In)
{
    PS_OUT Out;
    Out.vColor = float4(0, 0, 0, 0);

    float2 uv = In.vTexcoord;

    int tileCount = g_GroupCount;

// 타일 폭 (GAP 반영)
    float tileWidth = (1.0 - g_UVGap * (tileCount - 1)) / tileCount;

// 현재 픽셀의 타일 찾기
    int tileIndex = -1;
    float start = 0;
    float end = tileWidth;

    for (int i = 0; i < tileCount; i++)
    {
        if (uv.x >= start && uv.x < end)
        {
            tileIndex = i;
            break;
        }

        start = end + g_UVGap;
        end = start + tileWidth;
    }

    if (tileIndex < 0)
    {
        Out.vColor = float4(0, 0, 0, 0);
        return Out;
    }

//-----------------------------------------------
// tileUV (GAP 고려된 로컬 UV)
//-----------------------------------------------
    float2 tileUV;
    tileUV.x = (uv.x - start) / tileWidth;
    tileUV.y = uv.y;

//-----------------------------------------------
// BG Only Scale 0.5
//-----------------------------------------------
    float2 bgUV = tileUV;

    bgUV -= float2(0.5f, 0.5f);
    bgUV /= 0.75f;
    bgUV += float2(0.5f, 0.5f);

    float4 bg = g_Texture0.Sample(ClampSampler, bgUV);

//-----------------------------------------------
// FG normal
//-----------------------------------------------
    float filledTiles = g_fFillAmount * tileCount;

    int fullTiles = (int) filledTiles;
    float partial = filledTiles - fullTiles;

    float4 fg = float4(0, 0, 0, 0);

    bool isFull = (tileIndex < fullTiles);
    bool isPart = (tileIndex == fullTiles);

    if (isFull)
    {
        float4 mask = g_Texture1.Sample(DefaultSampler, tileUV);
        float4 tint = g_Texture2.Sample(DefaultSampler, float2(0.5f, 0.5f));
        fg = mask * tint;
    }
    else if (isPart)
    {
        if (tileUV.x < partial)
        {
            float4 mask = g_Texture1.Sample(DefaultSampler, tileUV);
            float4 tint = g_Texture2.Sample(DefaultSampler, float2(0.5f, 0.5f));
            fg = mask * tint;
        }
    }

    Out.vColor = lerp(bg, fg, fg.a);
    
    if (Out.vColor.a <= 0.0f)
        discard;
    
    return Out;
}

/*------------------[E_STAMINA]----------------*/

/*------------------[S_STAMINA_FX]----------------*/

PS_OUT PS_STAMINA_FX(PS_IN In)
{
    PS_OUT Out;

    float2 uv = In.vTexcoord;

    float2 ScaleUV = In.vTexcoord;
    ScaleUV -= float2(0.5f, 0.5f);
    ScaleUV /= g_fScale;
    ScaleUV += float2(0.5f, 0.5f);

    float2 GlowUV = In.vTexcoord;
    GlowUV -= float2(0.5f, 0.5f);
    GlowUV *= 2.f;
    GlowUV += float2(0.5f, 0.5f);

// 텍스쳐 샘플링
    float4 Glow = g_Texture0.Sample(ClampSampler, ScaleUV);
    //float4 OutLine = g_Texture1.Sample(ClampSampler, ScaleUV);

    Glow.rgb += Glow.rgb * (g_GlowIntensity * 2.f);
    Glow.rgb *= Glow.a;

    //OutLine.rgb += OutLine.rgb * g_GlowIntensity;
    //OutLine.rgb *= OutLine.a;

    //float4 result = lerp(Glow, OutLine, OutLine.a);

    //Glow.rgb += OutLine.rgb;
    //Glow.a *= OutLine.a;
    
    Out.vColor.rgb = Glow.rgb * g_Alpha;
    
    return Out;
}

/*------------------[E_STAMINA_FX]----------------*/

/*------------------[S_LOCKON]----------------*/

PS_OUT PS_LOCKON(PS_IN In)
{
    PS_OUT Out;

    float2 uv = In.vTexcoord;
    
    float4 LockOn = 0.f;
    float4 Shadow = 0.f;
    float4 Key = 0.f;
    float4 Ring = 0.f;
    
    float4 Result = 0.f;
    
    float2 DotUV = In.vTexcoord;
    DotUV -= float2(0.5f, 0.5f);
    DotUV /= 0.16f;
    DotUV += float2(0.5f, 0.5f);
        
    LockOn = g_Texture0.Sample(ClampSampler, DotUV);
        
    Result = LockOn;
    
    if (g_isFinisher)
    {
        float2 Center = float2(0.5f, 0.5f);
        float s = sin(g_fRotation);
        float c = cos(g_fRotation);

        float2 rotatedUV;
        float2 d = uv - Center;
        rotatedUV.x = d.x * c - d.y * s;
        rotatedUV.y = d.x * s + d.y * c;
        rotatedUV += Center;
        
        float2 ScaleUV = In.vTexcoord;
        ScaleUV -= float2(0.5f, 0.5f);
        ScaleUV /= g_fScale * 0.75f;
        ScaleUV += float2(0.5f, 0.5f);
        
        float2 ShadowScaleUV = In.vTexcoord;
        ShadowScaleUV -= float2(0.5f, 0.5f);
        ShadowScaleUV /= 1.2f;
        ShadowScaleUV += float2(0.5f, 0.5f);
        
        Shadow = g_Texture1.Sample(ClampSampler, ShadowScaleUV);
        Key = g_Texture2.Sample(ClampSampler, ScaleUV);
        Ring = g_Texture3.Sample(ClampSampler, rotatedUV);
        
        Result = Shadow;
        Result = lerp(Result, Key, Key.a);
        Result = lerp(Result, Ring, Ring.a);
    }
    
    Out.vColor = Result;
    
    if (Out.vColor.a <= 0.0f)
        discard;
    
    return Out;
}

/*------------------[E_LOCKON]----------------*/

/*------------------[S_OWNGOLD]----------------*/

PS_OUT PS_OWNGOLD(PS_IN In)
{
    PS_OUT Out;

    float2 uv = In.vTexcoord;
    
    float2 ScaleUV = In.vTexcoord;
    ScaleUV -= float2(0.f, (0.5f + g_vTransOffset.y));
    ScaleUV.x /= g_fScale;
    ScaleUV += float2(0.f, (0.5f + g_vTransOffset.y));
        
    float4 Icon = g_Texture0.Sample(ClampSampler, ScaleUV);
    float4 Shadow = g_Texture1.Sample(DefaultSampler, uv);
        
    float4 Color = Shadow * g_Alpha;
    Color = lerp(Color, Icon, Icon.a);
    
    Out.vColor = Color * g_Alpha;
    
    if (Out.vColor.a <= 0.0f)
        discard;
    
    return Out;
}

/*------------------[E_OWNGOLD]----------------*/

/*------------------[S_POPUP]----------------*/

PS_OUT PS_POPUP(PS_IN In)
{
    PS_OUT Out;

    float2 uv = In.vTexcoord;
    
    float4 Dim = 0.f;
    float4 Color = 0.f;
    
    //float2 CombineShadowSize = (g_UISize * 1.1f);
    //
    //float2 PopupScaleUV = In.vTexcoord;
    //PopupScaleUV -= float2(((g_UISize.x) + g_UIPosition.x) / g_vWindowSize.x, ((g_UISize.y * 0.5f) + g_UIPosition.y) / g_vWindowSize.y);
    //PopupScaleUV /= CombineShadowSize / g_vWindowSize;
    //PopupScaleUV += float2(((g_UISize.x) + g_UIPosition.x) / g_vWindowSize.x, ((g_UISize.y * 0.5f) + g_UIPosition.y) / g_vWindowSize.y);
      
    //PopupScaleUV -= float2(0.5f, 0.5f);
    //PopupScaleUV /= CombineShadowSize;
    //PopupScaleUV += float2(0.5f, 0.5f);
    
    float4 Popup = g_Texture1.Sample(DefaultSampler, uv);
    
    //if(g_isActive)
    //{
    //    Dim = g_Texture0.Sample(DefaultSampler, uv);
    //    Color = lerp(Color, Dim, Dim.a * 0.25f);
    //}
    
    Color = lerp(Color, Popup, Popup.a * 2.f);
    
    Out.vColor = Color * g_Alpha;
    
    if (Out.vColor.a <= 0.0f)
        discard;
    
    return Out;
}

/*------------------[E_POPUP]----------------*/

/*------------------[S_COSTUME_ANSWER]----------------*/

PS_OUT PS_COSTUME_ANSWER(PS_IN In)
{
    PS_OUT Out;
    
    float4 Icons = g_Texture0.Sample(DefaultSampler, In.vTexcoord);
    
    Out.vColor = Icons;
    
    if (Out.vColor.a <= 0.0f)
        discard;
    
    return Out;
}

/*------------------[E_COSTUME_ANSWER]----------------*/

/*------------------[S_COSTUME_BUTTONS]----------------*/

PS_OUT PS_COSTUME_BUTTONS(PS_IN In)
{
    PS_OUT Out;
    
    float4 Icons = g_Texture0.Sample(DefaultSampler, In.vTexcoord);
    Icons *= g_vTintColor;
    
    if (In.vAtlasIndex.w == 1)
        Icons.rgb = float3(1.f, 1.f, 1.f);
        
    Out.vColor = Icons;
    
    if (Out.vColor.a <= 0.0f)
        discard;
    
    return Out;
}

/*------------------[E_COSTUME_BUTTONS]----------------*/

technique11 DefaultTechnique
{
    pass UI // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass DEBUG // 1
    {
        SetRasterizerState(RS_Wireframe);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_DEBUG();
        GeometryShader = compile gs_5_0 GS_MAIN_DEBUG();
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass GLOW // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_UI_GLOW();
    }
    
    pass GLOWFX // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
    
        SetBlendState(BS_Additive, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_UI_GLOW_FX();
    }

    pass HP_GAUGE // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
    
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HP_GAUGE();
    }

    pass POTION // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
    
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_POTION();
    }

    pass SHIELD // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
    
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SHIELD();
    }

    pass BETA // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
    
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BETA();
    }

    pass BETA_FX // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
    
        SetBlendState(BS_Additive, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BETA_FX();
    }

    pass SKILL_SLOT // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SKILL_SLOT();
    }

    pass SKILL_SLOT_GLOW // 10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Additive, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SKILL_SLOT_GLOW();
    }

    pass RUSH_SLOT // 11
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RUSH_SLOT();
    }

    pass RUSH_SLOT_GLOW // 12
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Additive, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RUSH_SLOT_GLOW();
    }

    pass SKILL_WRAPPER_ON_LINE // 13
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Additive, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SKILL_WRAPPER_ON_LINE();
    }

    pass SKILL_WRAPPER_ON_FX // 14
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Additive, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SKILL_WRAPPER_ON_FX();
    }

    pass LOADING_BLUR // 15
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Additive, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_LOADING_BLUR();
    }

    pass SIMPLE_KEY // 16
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SIMPLE_KEY();
    }

    pass INTERACTION_FX // 17
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_INTERACTION_FX();
    }

    pass INTERACTION_FX_GLOW // 18
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Additive, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_INTERACTION_FX_GLOW();
    }

    pass STAMINA // 19
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_STAMINA();
    }

    pass STAMINA_FX // 20
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Additive, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_STAMINA_FX();
    }

    pass LOCKON // 21
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_LOCKON();
    }

    pass OWNGOLD // 22
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_OWNGOLD();
    }

    pass POPUP // 23
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_POPUP();
    }

    pass COSTUME_ANSWER // 24
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_INSTANCE_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_COSTUME_ANSWER();
    }

    pass COSTUME_BUTTONS // 25
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_INSTANCE_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_COSTUME_BUTTONS();
    }
}