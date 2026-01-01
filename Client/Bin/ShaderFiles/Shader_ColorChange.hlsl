#include "Client_Shader_Utils.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
vector g_vColor;
float g_fTime;

texture2D g_SceneTexture, g_GlassTexture;

struct VS_IN_DEFERRED
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT_DEFERRED
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_IN_DEFERRED
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT_DEFERRED
{
    float4 vBackBuffer : SV_TARGET0;
};

VS_OUT_DEFERRED VS_MAIN(VS_IN_DEFERRED In)
{
    VS_OUT_DEFERRED Out;
  
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

PS_OUT_DEFERRED PS_BREAK(PS_IN_DEFERRED In)
{
    PS_OUT_DEFERRED Out;
    
    Out.vBackBuffer = g_SceneTexture.Sample(DefaultSampler, In.vTexcoord);
    
    float2 dir = In.vTexcoord - float2(0.5, 0.5);
    float len = length(dir);
    
    float crackMask = g_GlassTexture.Sample(DefaultSampler, In.vTexcoord).r;
    
    float strength = pow(len * 1.5, 2);
    float2 distortedUV = In.vTexcoord + normalize(-dir) * strength * crackMask * saturate(sin(g_fTime * 5));
    Out.vBackBuffer = g_SceneTexture.Sample(ClampSampler, distortedUV);
    Out.vBackBuffer.rgb = Out.vBackBuffer.rgb * (1 - (g_GlassTexture.Sample(DefaultSampler, In.vTexcoord).r * strength * saturate(sin(g_fTime * 5)))) + g_GlassTexture.Sample(DefaultSampler, In.vTexcoord).rgb * strength * float3(1, 55.f / 255, 1) * saturate(sin(g_fTime * 5));
    return Out;   
}

PS_OUT_DEFERRED PS_POWER_BREAK(PS_IN_DEFERRED In)
{
    PS_OUT_DEFERRED Out;
    
    Out.vBackBuffer = g_SceneTexture.Sample(DefaultSampler, In.vTexcoord);
    float fTime = g_fTime * 5;
    if (fTime < 1.57)
    {
        float2 dir = In.vTexcoord - float2(0.5, 0.5);
        float len = length(dir);
    
        float crackMask = g_GlassTexture.Sample(DefaultSampler, In.vTexcoord).r;
    
        float strength = pow(len * 1.5, 2);
        float2 distortedUV = In.vTexcoord + normalize(-dir) * strength * crackMask * saturate(sin(fTime));
        Out.vBackBuffer = g_SceneTexture.Sample(ClampSampler, distortedUV);
        strength = pow(len * 2, 3);
        Out.vBackBuffer.rgb = Out.vBackBuffer.rgb * (1 - (g_GlassTexture.Sample(DefaultSampler, In.vTexcoord).r * strength * saturate(sin(fTime)))) + g_GlassTexture.Sample(DefaultSampler, In.vTexcoord).rgb * strength * float3(1, 105.f / 255, 1) * saturate(sin(fTime));
    }
    else if (fTime < 3.14)
    {
        fTime -= 1.57;
        fTime *= 2;
        float2 dir = In.vTexcoord - float2(0.5, 0.5);
        float len = length(dir);
    
        float crackMask = g_GlassTexture.Sample(DefaultSampler, In.vTexcoord).r;
    
        float strength = pow(len * 1.5, 2);
        float2 distortedUV = In.vTexcoord + normalize(-dir) * strength * crackMask * saturate(sin(1.57));
        Out.vBackBuffer = g_SceneTexture.Sample(ClampSampler, distortedUV);
        strength = pow(len * 2, 3);
        Out.vBackBuffer.rgb = Out.vBackBuffer.rgb * (1 - (g_GlassTexture.Sample(DefaultSampler, In.vTexcoord).r * strength * (sin(1.57) + saturate(sin(fTime))))) + g_GlassTexture.Sample(DefaultSampler, In.vTexcoord).rgb * strength * float3(1, 105.f / 255, 1) * sin(1.57) + g_GlassTexture.Sample(DefaultSampler, In.vTexcoord).rgb * strength * float3(1, 0.2f, 0.4f) * saturate(sin(fTime)) * 3;
    }
    else
    {
        fTime -= 1.57;
        float2 dir = In.vTexcoord - float2(0.5, 0.5);
        float len = length(dir);
    
        float crackMask = g_GlassTexture.Sample(DefaultSampler, In.vTexcoord).r;
    
        float strength = pow(len * 1.5, 2);
        float2 distortedUV = In.vTexcoord + normalize(-dir) * strength * crackMask * abs(sin(fTime));
        Out.vBackBuffer = g_SceneTexture.Sample(ClampSampler, distortedUV);
        strength = pow(len * 2, 3);
        Out.vBackBuffer.rgb = Out.vBackBuffer.rgb * (1 - (g_GlassTexture.Sample(DefaultSampler, In.vTexcoord).r * strength * abs(sin(fTime)))) + g_GlassTexture.Sample(DefaultSampler, In.vTexcoord).rgb * strength * float3(1, 105.f / 255, 1) * abs(sin(fTime));
    }
    
    
    //Out.vBackBuffer = lerp(Out.vBackBuffer, 1 - Out.vBackBuffer, saturate(pow(saturate(sin(g_fTime * 5)) * 2, 1.2) * 0.5));
    return Out;
}

PS_OUT_DEFERRED PS_SLASH(PS_IN_DEFERRED In)
{
    PS_OUT_DEFERRED Out;
    float2 uv = In.vTexcoord;
    float fLine = uv.y + uv.x * 0.2;
    float fTime = min(g_fTime, 1.57);
    float fLineTime = min(g_fTime * 1.2, 1.57);
    float fSmallLineTime = min(g_fTime * 1.5, 1.57);
    float width = pow(cos(fSmallLineTime), 3) * 0.1;
    float edge = saturate(pow(smoothstep(width, 0, abs(fLine - 0.6)), 5));
    float BigWidth = pow(cos(fLineTime), 3) * 0.5;
    float BigEdge = smoothstep(BigWidth, 0, abs(fLine - 0.6));
    
    if (In.vTexcoord.y + In.vTexcoord.x * 0.2 < 0.6)
    {
        uv.x += saturate(pow(saturate(cos(max(fTime - 0.5, 0) * 3)) * 2, 1.2) * 0.5) * (1 - In.vTexcoord.x) * 0.3;
        uv.y -= saturate(pow(saturate(cos(max(fTime - 0.5, 0) * 3)) * 2, 1.2) * 0.5) * (In.vTexcoord.y) * 0.065;
    }
    else
    {
        uv.x -= saturate(pow(saturate(cos(max(fTime - 0.5, 0) * 3)) * 2, 1.2) * 0.5) * (In.vTexcoord.x) * 0.3;
        uv.y += saturate(pow(saturate(cos(max(fTime - 0.5, 0) * 3)) * 2, 1.2) * 0.5) * (1 - In.vTexcoord.y) * 0.065;
    }
    
    Out.vBackBuffer = g_SceneTexture.Sample(DefaultSampler, uv);
    Out.vBackBuffer = Out.vBackBuffer * max(1 - BigEdge, 0) + float4(1, 65.f / 255, 150.f / 255, 1) * BigEdge;
    Out.vBackBuffer = Out.vBackBuffer * max(1 - (edge * 5), 0) + float4(1, 55.f / 255, 85.f / 255, 1) * edge * 5;
    //Out.vBackBuffer = lerp(Out.vBackBuffer, float4(1, 15.f / 255, 120.f / 255, 1), edge * 5);
    fTime = min(g_fTime * 2, 1.57);
    Out.vBackBuffer = lerp(Out.vBackBuffer, 1 - Out.vBackBuffer, sin(fTime * 2));
    
    return Out;
}

technique11 DefaultTechnique
{
    // idx 0
    pass Break
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BREAK();
    }
    // idx 1
    pass PowerBreak
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_POWER_BREAK();
    }
    // idx 2
    pass Slash
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_None, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SLASH();
    }
}