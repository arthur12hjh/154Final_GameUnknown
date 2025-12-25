
#include "Engine_Shader_Defines.hlsli"

matrix g_ViewMatrix, g_ProjMatrix;
vector g_vColor = vector(1.f, 1.f, 1.f, 1.f);
vector g_vSize = vector(1.f, 0.f, 1.f, 0.f);
float g_fTime = 0;
float g_fEndTime = 0;
float2 g_fMaskUV = float2(0, 0);
float2 g_fMaskUVSpeed = float2(0, 0);
float2 g_fMaskUVSize = float2(1, 1);
float2 g_fDiffuseUV = float2(0, 0);
float2 g_fDiffuseUVSpeed = float2(0, 0);
float2 g_fDiffuseUVSize = float2(1, 1);
float2 g_fDissolveUV = float2(0, 0);
float2 g_fDissolveUVSpeed = float2(0, 0);
float2 g_fDissolveUVSize = float2(1, 1);
float g_fFar;

texture2D g_MaskTexture, g_DiffuseTexture, g_DissolveTexture;
struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    float4 pos = mul(vector(In.vPosition, 1.f), g_ViewMatrix);
    Out.vPosition = mul(pos, g_ProjMatrix);
    
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    return Out;
}

struct GS_IN
{
    float4 vPosition : POSITION;
    float2 vLifeTime : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

/* 출력된 정점 위치벡터의 w값으로 모든 성분을 나눈다 -> 투영스페이스로 변환 */ 
/* 정점의 위치에 대해서 뷰포트 변환을 수행한다 */ 
/* 정점의 모든 정보를 보간하여 픽셀을 만든다. -> 래스터라이즈 */ 

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

struct PS_NONLIGHT_OUT
{
    float4 vDiffuse : SV_TARGET0;
};

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_DISTORTION(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float2 MaskTexcoord = float2((1 - (In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x)) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    Out.vDiffuse = g_vColor;
    Out.vDiffuse *= g_MaskTexture.Sample(NoneSampler, MaskTexcoord).r * g_fDissolveUVSize.x;
    return Out;
}

PS_NONLIGHT_OUT PS_SPECTRUM(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float2 MaskTexcoord = float2((1 - (In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x)) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    Out.vDiffuse = g_vColor;
    if (0.2 > (1 - abs(MaskTexcoord.x)) * (1 - abs(1 - MaskTexcoord.y * 2)))
        discard;
    Out.vDiffuse.a *= 1 - MaskTexcoord.x;
    return Out;
}
PS_NONLIGHT_OUT PS_SLASH(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    float2 MaskTexcoord = float2((1 - (In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x)) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    Out.vDiffuse = g_vColor;
    Out.vDiffuse *= g_MaskTexture.Sample(NoneSampler, MaskTexcoord).r;
    if (0.5 > Out.vDiffuse.a)
        discard;
    //Out.vDiffuse.a = 1;
    return Out;
}

PS_NONLIGHT_OUT PS_SLASH_BLOOM(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float depth = In.vProjPos.z / In.vProjPos.w / g_fFar;
    float weight = saturate(pow(1 - depth, 3));
    
    float2 MaskTexcoord = float2((1 - (In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x)) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    
    Out.vDiffuse = g_vColor;
    Out.vDiffuse *= g_MaskTexture.Sample(NoneSampler, MaskTexcoord).r;
    if (0 >= Out.vDiffuse.a)
        discard;
    Out.vDiffuse.rgb *= Out.vDiffuse.a;
    Out.vDiffuse.a = 1;
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_SLASH_PATTERN(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float depth = In.vProjPos.z / In.vProjPos.w / g_fFar;
    float weight = saturate(pow(1 - depth, 3));
    float2 MaskTexcoord = float2((1 - (In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x)) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);

    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    
    Out.vDiffuse = g_vColor;
    vector color = g_DiffuseTexture.Sample(MirrorSampler, float2(DiffuseTexcoord.x, DiffuseTexcoord.y)) * g_vColor;
    color.a = 1 - (1 - abs(MaskTexcoord.x * 1.5)) * abs(MaskTexcoord.y);
    Out.vDiffuse.rgb = Out.vDiffuse.rgb * (1 - color.a) + color.rgb * color.a;
    Out.vDiffuse.a = g_vColor.a * g_MaskTexture.Sample(NoneSampler, float2(MaskTexcoord.x, MaskTexcoord.y)).r;
    if (0.5 > Out.vDiffuse.a)
        discard;
    Out.vDiffuse.a = 1;
    
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_SLASH_PATTERN_BLOOM(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    
    float depth = In.vProjPos.z / In.vProjPos.w / g_fFar;
    float weight = saturate(pow(1 - depth, 3));
    float2 MaskTexcoord = float2((1 - (In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x)) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);

    float2 DiffuseTexcoord = float2((In.vTexcoord.x + g_fDiffuseUV.x + g_fTime * g_fDiffuseUVSpeed.x) * g_fDiffuseUVSize.x, (In.vTexcoord.y + g_fDiffuseUV.y + g_fTime * g_fDiffuseUVSpeed.y) * g_fDiffuseUVSize.y);
    float2 DissolveTexcoord = float2((In.vTexcoord.x + g_fDissolveUV.x + g_fTime * g_fDissolveUVSpeed.x) * g_fDissolveUVSize.x, (In.vTexcoord.y + g_fDissolveUV.y + g_fTime * g_fDissolveUVSpeed.y) * g_fDissolveUVSize.y);
    
    Out.vDiffuse = g_vColor;
    vector color = g_DiffuseTexture.Sample(MirrorSampler, float2(DiffuseTexcoord.x, DiffuseTexcoord.y)) * g_vColor;
    color.a = 1 - (1 - abs(MaskTexcoord.x * 1.5)) * abs(MaskTexcoord.y);
    Out.vDiffuse.rgb = Out.vDiffuse.rgb * (1 - color.a) + color.rgb * color.a;
    Out.vDiffuse.a = g_vColor.a * g_MaskTexture.Sample(NoneSampler, float2(MaskTexcoord.x, MaskTexcoord.y)).r;
    //if (g_DissolveTexture.Sample(MirrorSampler, float2(DissolveTexcoord.x, DissolveTexcoord.y)).r + ((1 - abs(MaskTexcoord.x) * 1.5) * abs(MaskTexcoord.y) * 2)  < abs(MaskTexcoord.x))
    //    discard;
    if (0 >= Out.vDiffuse.a)
        discard;
    Out.vDiffuse.rgb *= Out.vDiffuse.a;
    Out.vDiffuse.a = 1;
    
    return Out;
}

/* 픽셀 쉐이더 : 픽셀의 최종적인 색을 결정하낟. */
PS_NONLIGHT_OUT PS_BLUR_SLASH(PS_IN In)
{
    PS_NONLIGHT_OUT Out;
    float2 MaskTexcoord = float2((1 - (In.vTexcoord.x + g_fMaskUV.x + g_fTime * g_fMaskUVSpeed.x)) * g_fMaskUVSize.x, (In.vTexcoord.y + g_fMaskUV.y + g_fTime * g_fMaskUVSpeed.y) * g_fMaskUVSize.y);
    Out.vDiffuse = g_vColor;
    Out.vDiffuse *= g_MaskTexture.Sample(NoneSampler, MaskTexcoord).r;
    if (0 > Out.vDiffuse.a)
        discard;
    float linearDepth = saturate((0.1 * g_fFar / (g_fFar - (In.vProjPos.z / In.vProjPos.w) * (g_fFar - 0.1))) / g_fFar);
    
    float weight = saturate(exp(-linearDepth * 10));
    Out.vDiffuse.rgb = Out.vDiffuse.rgb * Out.vDiffuse.a * weight * g_fDissolveUVSize.x;
    Out.vDiffuse.a = Out.vDiffuse.a * weight * g_fDissolveUVSize.x;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Distortion
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORTION();
    }
    pass Spectrum
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_BlendAlpha, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SPECTRUM();
    }
    pass Slash
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SLASH();
    }
    pass Slash_Bloom
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SLASH_BLOOM();
    }
    pass Slash_Pattern
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SLASH_PATTERN();
    }
    pass Slash_Pattern_Bloom
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SLASH_PATTERN_BLOOM();
    }
    pass Slash_Blur
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_DepthNonWrite, 0);
        SetBlendState(BS_BlendAlpha, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR_SLASH();
    }

}