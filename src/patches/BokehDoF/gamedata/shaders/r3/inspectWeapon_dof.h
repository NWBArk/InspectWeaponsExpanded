#include "inspectWeapon_dof_settings.h"
#include "inspectWeapon_dof_utils.h"

uniform float4 ssfx_wpn_dof_1;
uniform float ssfx_wpn_dof_2;

/**
	@brief Calculates Depth of Field.

	@param tc TexCoord.
	@param pos Position, xy: viewspace position, z: depth.
	@param image Buffer color.
	@return float4, rgb: pixel color, a: coc.
*/
float4 Inspect_DOF(float2 tc, float3 pos, float3 image)
{
	/*  ----------------------------------------------------------------------------
		ssfx_wpn_dof_1:
		w: blur amount, x: min focus length, y: max focus length, z: focus distance.

		lensProperty:
		x: focus distance, y: focus range, z: blur radius.
		----------------------------------------------------------------------------
	*/
	float focusStart = ssfx_wpn_dof_1.x;
	float focusEnd = ssfx_wpn_dof_1.y;
	float3 lensProperty = float3(ssfx_wpn_dof_1.z, focusEnd - focusStart, ssfx_wpn_dof_1.w);

//  --------------------------------------------------------------------------------
//                              Background Bokeh DoF
//  --------------------------------------------------------------------------------
	float3 blurBackground = 0;
	float blurBackgroundWeight = 0;
	float highlightGain = saturate(INSPECT_DOF_HIGHLIGHT_GAIN);
	float2 radiusToUse = (1 / float2(3840, 2160)) * INSPECT_DOF_RADIUS * lensProperty.z;
	float coc = CalculateCoC(pos.z, lensProperty.x, lensProperty.y, lensProperty.z);

	float weight = 0;
	[unroll]
	for (int i = 0; i < SampleCount; i++)
	{
		// Adjust offset based on radius and screen resolution.
		float2 offset = Kernel[i] * radiusToUse;
		float3 sampleColor = AccentuateWhites(s_image.SampleLevel(smp_nofilter, tc + offset, 0).rgb, GammaFactor, highlightGain);
		float sampleCoc = CalculateCoC(GetDepth(tc + offset, 0), lensProperty.x, lensProperty.y, lensProperty.z);
		// Apply sample weighting based on CoC.
		float sampleWeight = SampleWeightFromCoC(max(0, min(sampleCoc, coc)), length(offset));

		blurBackground += sampleColor * sampleWeight;
		weight += sampleWeight;
	}
	blurBackground *= 1 / (weight + (weight == 0));
	blurBackground = CorrectForWhiteAccentuation(blurBackground, GammaFactor, highlightGain);
	blurBackgroundWeight = saturate(smoothstep(0.1f, 1.0f, coc) + int(pos.z <= SKY_EPS)) * lensProperty.x;

//  --------------------------------------------------------------------------------
//                                  Foreground DoF
//  --------------------------------------------------------------------------------
	// Use Depth to adjust blur intensity
	float blurAmountForeground = lerp(lensProperty.z, 0, smoothstep(focusStart, focusEnd, pos.z)) * (pos.z > SKY_EPS);
	float edgeBlur = 0;

	// Peripheral vision blur
	if (ssfx_wpn_dof_2 > 0)
	{
		// Vignette to calc blur
		float2 mid_uv = tc - float2(0.5f, 0.5f);
 		edgeBlur = pow(smoothstep(0.0f, saturate(1.0f - ssfx_wpn_dof_2), length(mid_uv)), 1.5f) * 1.33f;

		blurAmountForeground = saturate(blurAmountForeground + edgeBlur) * lensProperty.z;
	}

	// Close blur ( Weapon blur ) 
	if (blurAmountForeground > 0)
	{
		float offset_f = 0.001f;
		float ratio = screen_res.y / screen_res.x;
		float2 blur_res = float2(blurAmountForeground, blurAmountForeground) * float2(offset_f * ratio, offset_f);

		// Offset pattern
		float2 blur_Offsets[8] =
		{
			float2( -blur_res.x,-blur_res.y),	// XOX
			float2( blur_res.x,blur_res.y),		// OOO
			float2( blur_res.x,-blur_res.y),	// XOX
			float2(-blur_res.x,blur_res.y),

			float2( 0,-blur_res.y),	// OXO
			float2( blur_res.x,0),	// XOX
			float2( -blur_res.x,0),	// OXO
			float2(0,blur_res.y)
		};

		float3 Wpn_Blur = 0;

		// Create blur
		for (int b = 0; b < 8; b++)
		{
			Wpn_Blur += s_image.SampleLevel(smp_rtlinear, tc + blur_Offsets[b], 0).rgb;
			Wpn_Blur += s_image.SampleLevel(smp_rtlinear, tc + blur_Offsets[b] * 2, 0).rgb;
		}

		// Normalize blur
		image = Wpn_Blur / 16;

		// Peripheral vision blur with extra help from s_blur_2
		image = lerp(image, blurBackground, saturate(0.4f - (1.0f - edgeBlur)));
	}
	
//  --------------------------------------------------------------------------------
//                            Background/Foreground Mix
//  --------------------------------------------------------------------------------
	// Far blur ( Reload, Inventory and PDA )
	if (lensProperty.x > 0)
	{
		image = lerp(image, blurBackground, blurBackgroundWeight);
	}

	return float4(image, coc);
}