#include "Engine_Shader_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_Texture;
texture2D g_Texture0;
texture2D g_Texture1;
texture2D g_Texture2;
texture2D g_Texture3;
texture2D g_Texture4;
texture2D g_DepthTexture;

vector g_Color = 1.f;

float2 g_UIPosition;
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
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
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
    
    /* Out.vPosition.xy => 시야각에 있는 점들을 90에 맞춰준다 */ 
    /* Out.vPosition.z => n~f사이에 있는 점들의 z를 0 ~ f로 바꿔준다. */     
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

/* 출력된 정점 위치벡터의 w값으로 모든 성분을 나눈다 -> 투영스페이스로 변환 */ 
/* 정점의 위치에 대해서 뷰포트 변환을 수행한다 */ 
/* 정점의 모든 정보를 보간하여 픽셀을 만든다. -> 래스터라이즈 */ 

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
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
    //if (g_bUseTintColor)
    //{
    //    gaugeColor.rgb *= g_vTintColor.rgb;
    //    gaugeColor.a *= g_vTintColor.a;
    //}
  
    float4 CombinedColor = BgColor;
    CombinedColor = lerp(CombinedColor, GaugeColor, GaugeColor.a);
  
    // ----------------------------
    //  Alpha 적용
    // ----------------------------
    Out.vColor = CombinedColor;
    //Out.vColor.a *= g_Alpha;

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
    
    return Out;
}

//PS_OUT PS_SHIELD(PS_IN In)
//{    
//    PS_OUT Out;
//    Out.vColor = float4(0, 0, 0, 0);

//    float2 uv = In.vTexcoord;

//    float rightFill = saturate((g_fFillAmount - 0.5) * g_GroupCount);
//    float leftFill = saturate(g_fFillAmount * g_GroupCount);
    
//    float leftEnd = 0.5 - g_UVGap * 0.5;
//    float rightStart = 0.5 + g_UVGap * 0.5;

//    // GAP → 완전 투명
//    if (uv.x > leftEnd && uv.x < rightStart)
//        return Out;

//    //-----------------------------------------------
//    // LEFT BLOCK (배경 FULL + 전경만 Clip)
//    //-----------------------------------------------
//    if (uv.x < leftEnd)
//    {
//        float localX = uv.x / leftEnd;

//        // ---- 타일 반복 UV ----
//        float2 tileUV;
//        tileUV.x = frac(localX * g_UVScale.x);
//        tileUV.y = frac(uv.y * g_UVScale.y);

//        // 1) 배경은 항상 FULL
//        float4 bgColor = g_Texture0.Sample(DefaultSampler, tileUV);

//        // 2) 전경은 FillAmount 기준으로 잘라냄
//        float4 fgColor = float4(0, 0, 0, 0);

//        if (localX <= leftFill)        // 조건 만족할 때만 FG를 그린다
//            fgColor = g_Texture1.Sample(DefaultSampler, tileUV);

//        fgColor *= g_vTintColor;
        
//        // 최종 색 = BG + FG
//        Out.vColor = lerp(bgColor, fgColor, fgColor.a);
//        return Out;
//    }


//    //-----------------------------------------------
//    // RIGHT BLOCK (배경 FULL + 전경만 Clip)
//    //-----------------------------------------------
//    if (uv.x > rightStart)
//    {
//        float localX = (uv.x - rightStart) / (1.0 - rightStart);

//        float2 tileUV;
//        tileUV.x = frac(localX * g_UVScale.x);
//        tileUV.y = frac(uv.y * g_UVScale.y);

//        float4 bgColor = g_Texture0.Sample(DefaultSampler, tileUV);

//        float4 fgColor = float4(0, 0, 0, 0);

//        if (localX <= rightFill)
//            fgColor = g_Texture1.Sample(DefaultSampler, tileUV);

//        fgColor *= g_vTintColor;
        
//        Out.vColor = lerp(bgColor, fgColor, fgColor.a);
//        return Out;
//    }

//    return Out;
//}

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
    return Out;
}

/*------------------[E_BETA]----------------*/

/*------------------[S_SKILL_SLOT]----------------*/
PS_OUT PS_SKILL_SLOT(PS_IN In)
{
    PS_OUT Out;
    
    float4 TintColor = 1.f;
    
    float4 shadow = g_Texture0.Sample(DefaultSampler, In.vTexcoord);
    float4 frame = g_Texture1.Sample(DefaultSampler, In.vTexcoord);
    float4 icon = 0.f;
    float4 cover = 0.f;
    
    if(g_bUseTintColor)
        TintColor = g_vTintColor;
    
    if(g_isUseable)
    {   
        icon = g_Texture2.Sample(DefaultSampler, In.vTexcoord);
        icon *= TintColor;
    }
    
    if (g_UseCover)
    {   
        cover = g_Texture3.Sample(DefaultSampler, In.vTexcoord) * g_Alpha;
    }
    
    float4 result = shadow;
    result = lerp(result, frame, frame.a);
    result = lerp(result, icon, icon.a);
    result = lerp(result, cover, cover.a);
    
    Out.vColor = result;
    
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
    
    return Out;
}

/*------------------[E_RUSH_SLOT]----------------*/

/*------------------[S_RUSH_SLOT_GLOW]----------------*/

PS_OUT PS_RUSH_SLOT_GLOW(PS_IN In)
{
    //PS_OUT Out;
  
    //float2 uv = In.vTexcoord;

    ////float4 CoolTimeColor = float4(0.6235294118, 0.6823529412, 0.7882352941, 1.0);
    
    //float4 glow0 = g_Texture0.Sample(DefaultSampler, uv);
    
    //if(g_bUseGlow)
    //{
    //    glow0.rgb += glow0.rgb * g_GlowIntensity;
    //    glow0.rgb *= glow0.a * 0.5;
    //}
    
    //float4 result = glow0;
    
    //Out.vColor = result;
    
    //return Out;
    
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
    
    return Out;
}

/*------------------[E_SKILL_WRAPPER_ON_FX]----------------*/

technique11 DefaultTechnique
{
    pass UI // 0
    {
        SetRasterizerState(RS_Cull_None);
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
    
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
    
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BETA();
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

    pass SKILL_WRAPPER_ON_FX // 13
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Additive, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SKILL_WRAPPER_ON_FX();
    }
}