struct VsConstants
{
	float4x4 world;
	float4x4 viewProj;
	float4x4 worldViewProjDer;
	float3 direction;
	float magnitude;
	float offset;
	float2 uvSize;
};
ConstantBuffer<VsConstants> vsConstants : register(b0);

SamplerState heightmapSampler : register(s0);
Texture2D<float4> heightmapTex : register(t0);

struct PsInput
{
	float4 hPos : SV_Position;
	float4 wPos : Output0;
	float2 sVelocity : Output1;
	float3 wNormal : Output2;
	float2 texCoord : TEXCOORD0;
	float3 wTangent : Output4;
	float3 wBitangent : Output5;
#ifdef HAS_COLOR
	float4 color : Output3;
#endif
};


struct Derivatives
{
	float z;
	float2 grad;
};


float2 CalculateDuv(Texture2D tex, float2 texCoord)
{
	float width;
	float height;
	uint numLevels;
	tex.GetDimensions(0, width, height, numLevels);
	return float2(1.0f / width, 1.0f / height) * 0.5f;
}


Derivatives SampleHeightmap(Texture2D tex, SamplerState samp, float2 texCoord, float2 uvSize)
{
	Derivatives result;
	result.z = vsConstants.magnitude * heightmapTex.SampleLevel(heightmapSampler, texCoord, 0);
	
	float2 duv = CalculateDuv(tex, texCoord);
	
	float2 dz = float2(
		vsConstants.magnitude * heightmapTex.SampleLevel(heightmapSampler, texCoord + float2(duv.x, 0), 0).x - result.z,
		vsConstants.magnitude * heightmapTex.SampleLevel(heightmapSampler, texCoord + float2(0, duv.y), 0).x - result.z
	);
	
	float2 duvDxy = float2(1.0f, 1.0f) / uvSize;
	
	float2 dzDxy = dz / duv * duvDxy;
	
	result.grad = dzDxy;
	return result;
}


float3 DisplacementAdjustedNormal(float3 normal, float3 tangent, float3 bitangent, float2 dnDtb)
{
	float3 gradT = tangent + normal * dnDtb.x;
	float3 gradB = bitangent + normal * dnDtb.y;
	return cross(gradT, gradB);
}


PsInput VSMain(float4 lPos : POSITION,
				 float3 lNormal : NORMAL,
				 float3 lTangent : TANGENT,
				 float2 texCoord : TEX_COORD
#ifdef HAS_COLOR
				 , float4 color : COLOR
#endif
#ifdef HAS_BITANGENT
				 , float3 lBitangent : BITANGENT
#endif
)
{
	// Calculate bitangent if not provided.
#if !HAS_BITANGENT
	float3 lBitangent = cross(lNormal, lTangent);
#endif
	
	PsInput output;
		
	Derivatives derivatives = SampleHeightmap(heightmapTex, heightmapSampler, texCoord, vsConstants.uvSize);
	
	float4 modifiedPos = lPos + float4(vsConstants.direction * (derivatives.z + vsConstants.offset), 0);
	float3 modifiedNormal = DisplacementAdjustedNormal(lNormal, lTangent, lBitangent, derivatives.grad);

	// Transform position.
	output.sVelocity = mul(modifiedPos, vsConstants.worldViewProjDer).xy;
	output.wPos = mul(modifiedPos, vsConstants.world);
	output.hPos = mul(output.wPos, vsConstants.viewProj);
	
	// Write through texCoord.
	output.texCoord = texCoord;
	
	// Transform through normal.
	float3x3 worldRotation = (float3x3) vsConstants.world;
	output.wNormal = normalize(mul(modifiedNormal, worldRotation));
	
	// Transform through tangent.
	output.wTangent = mul(lTangent, worldRotation);
	
	// Transform through bitangent.
	output.wBitangent = mul(lBitangent, worldRotation);
	
#ifdef HAS_COLOR
	output.color = color;
#endif

	return output;
}



struct PsConstants
{
	float3 lightDir;
	float3 lightColor;
};
ConstantBuffer<PsConstants> psConstants : register(b100);


struct PsOutput
{
	float4 color : SV_Target0;
};


// Surface params
static float3 g_wPosition;
static float3 g_wNormal;
static float3 g_wTangent;
static float3 g_wBitangent;
static float4 g_vertexColor;
static float2 g_texCoord;


// GBuffer output
static float4 go_color = float4(1, 0, 0, 1);


// Implementations
#define NORMAL_IMPLEMENTATION return g_wNormal;
#define TANGENT_IMPLEMENTATION return g_wTangent;
#define BITANGENT_IMPLEMENTATION return g_wBitangent;
#define COLOR_IMPLEMENTATION return g_vertexColor;
#define TEXCOORD_IMPLEMENTATION return g_texCoord;

#define LIGHTCOUNT_IMPLEMENTATION return 1;
#define LIGHTDIR_IMPLEMENTATION return psConstants.lightDir;
#define LIGHTCOLOR_IMPLEMENTATION return psConstants.lightColor;

#define SINK_IMPLEMENTATION go_color = color;


#define main main_sf
#include "SurfaceInputsFunc.hlsl"
#define main main_li
#include "LightInputs.hlsl"
#define main main_tp
#include "Types.hlsl"
#undef main


//MATERIAL_CODE_INCLUDE
#ifdef VERTEX_SHADER
void MtlMain() {}
#endif


PsOutput PSMain(PsInput input)
{
	g_wPosition = input.wPos;
	g_wNormal = normalize(input.wNormal);
	g_wTangent = normalize(input.wTangent);
	g_wBitangent = normalize(input.wBitangent);
	g_texCoord = input.texCoord;

#ifdef HAS_COLOR
	g_vertexColor = input.color;
#endif

	PsOutput output;

	MtlMain();

	output.color = go_color;
	return output;
}