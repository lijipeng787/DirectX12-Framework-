#include "../GraphicsUtility/Lightfunctions.hlsl"
#include "../GraphicsUtility/GraphicsUtility.hlsl"
cbuffer SceneConstantBuffer : register(b0)
{
    Camera camera;
};
cbuffer RootConstant : register(b1)
{
    float lod;
};
TextureCube cube : register(t0);
SamplerState g_sampler : register(s0);
struct PSInput
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
    float3 viewdir : VIEW;
};
float3 ToneMapACES(float3 hdr)
{
    const float A = 2.51, B = 0.03, C = 2.43, D = 0.59, E = 0.14;
    return saturate((hdr * (A * hdr + B)) / (hdr * (C * hdr + D) + E));
}
PSInput VSMain(uint id : SV_VertexID)
{
	PSInput result;
	result.uv = float2((id << 1) & 2, id & 2);
	result.position = float4(result.uv  * float2(2, -2) + float2(-1, 1), 0, 1);


    float4 projcoord;
    projcoord.xy = result.uv;
    projcoord.y = 1.0 - projcoord.y;
    projcoord.xy = projcoord.xy * 2.0f - 1.0f;
    projcoord.z = 1.0f;
    projcoord.w = 1.0f;

    float4 vpos = mul(camera.projinverse, projcoord);
    vpos.xyzw /= vpos.w;


    //// fov 90 degree special case and ratio is one
    
    
    ////top setting
    //vpos.x = projcoord.x * camera.ratio;
    //vpos.y = 1.0;
    //vpos.z = -projcoord.y;
    //vpos.w = 1.0f;


    ////front setting
    /*vpos.x = projcoord.x * camera.ratio;
    vpos.y = projcoord.y;
    vpos.z = -1.0;
    vpos.w = 1.0f;*/

   vpos = mul(camera.viewinverse, vpos);
    result.viewdir = vpos.xyz;

	return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{

  
    float tlod = min(10.0f, lod);
    float3 final = ToneMapACES(cube.SampleLevel(g_sampler, input.viewdir, tlod).xyz);
    final = pow(final, 1.0f / 2.2f);
   // return float4(final,0.0f);
   return float4(cube.SampleLevel(g_sampler, input.viewdir, tlod));
}
