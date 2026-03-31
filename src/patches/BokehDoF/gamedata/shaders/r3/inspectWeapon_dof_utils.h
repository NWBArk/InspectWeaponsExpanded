#include "common.h"
#include "inspectWeapon_dof_defines.h"

uniform float4 shader_param_8;

bool IsUsingNVG()
{
	return floor(shader_param_8.x) > 0.0f;
}

float4 GetPosition(float2 tc, uint iSample : SV_SAMPLEINDEX)
{
#ifndef USE_MSAA
	return s_position.SampleLevel(smp_nofilter, tc, 0);
#else
	return s_position.Load(int3(tc * screen_res.xy, 0), iSample);
#endif
}

float GetDepth(float2 tc, uint iSample: SV_SAMPLEINDEX)
{
    return GetPosition(tc, iSample).z;
}

float GetLuminance(float3 fragment)
{
    return dot(fragment, LumaCoeff);
}

float CalculateCoC(float depth, float distance, float range, float radius)
{
	// CoC < 0: pixel in foreground DoF, CoC > 0: pixel in background DoF.
	return clamp((depth - distance) / range, -1.0f, 1.0f) * radius;
}

float SampleWeightFromCoC(float coc, float radius)
{
	return saturate((coc - radius + 2.0f) / 2.0f);
}

// Taken from CinematicDOF reshade shader.
float3 ConeOverlap(float3 fragment)
{
    float k = 0.4 * 0.33;
    float2 f = float2(1-2 * k, k);
    float3x3 m = float3x3(f.xyy, f.yxy, f.yyx);
    return mul(fragment, m);
}

float3 ConeOverlapInverse(float3 fragment)
{
    float k = 0.4 * 0.33;
    float2 f = float2(k-1, k) * rcp(3 * k-1);
    float3x3 m = float3x3(f.xyy, f.yxy, f.yyx);
    return mul(fragment, m);
}

float3 AccentuateWhites(float3 fragment, float gamma, float highlightBoost)
{
    fragment = pow(abs(ConeOverlap(fragment)), gamma);
    return fragment / max((1.001 - (highlightBoost * fragment)), 0.001);
}

float3 CorrectForWhiteAccentuation(float3 fragment, float gamma, float highlightBoost)
{
    float3 toReturn = fragment / (1.001 + (highlightBoost * fragment));
    return ConeOverlapInverse(pow(abs(toReturn), 1.0/ gamma));
}