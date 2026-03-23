#include "inspectWeapon_dof_defines.h"
#include "inspectWeapon_dof_utils.h"

// x: min focus length, y: max focus length, z: focus distance, w: blur amount
//uniform float4 ssfx_wpn_dof_1;
//uniform float ssfx_wpn_dof_2;

float3 Inspect_DOF(float2 tc, float3 depth, float3 img)
{
	// Background bokeh DoF.
	float highlightGain = saturate(INSPECT_DOF_HIGHLIGHT_GAIN);

	float3 blurBackground = 0;
	float weight = 0;
	float coc = CalculateCoC(depth.z, ssfx_wpn_dof_1.z, ssfx_wpn_dof_1.y - ssfx_wpn_dof_1.x, ssfx_wpn_dof_1.w);

	[loop]
	for (int i = 0; i < SampleCount; i++)
	{
		// Adjust offset based on radius and screen resolution.
		float2 offset = Kernel[i] * ssfx_wpn_dof_1.w * ssfx_pixel_size * INSPECT_DOF_RADIUS * (screen_res.x / 3840);
		float4 sample = s_image.SampleLevel(smp_nofilter, tc + offset, 0);
		float3 sampleColor = AccentuateWhites(sample.rgb, GammaFactor, highlightGain);
		blurBackground += sampleColor;

		float sampleWeight = SampleWeightFromCoC(max(0, min(sample.a, coc)), length(offset));
		weight += sampleWeight;
	}
	blurBackground *= 1 / (weight + (weight == 0));
	blurBackground = CorrectForWhiteAccentuation(blurBackground, GammaFactor, highlightGain);

	// Use Depth to adjust blur intensity
	float blurAmountForeground = lerp(ssfx_wpn_dof_1.w, 0, smoothstep(ssfx_wpn_dof_1.x, ssfx_wpn_dof_1.y, depth.z ) );	
	blurAmountForeground *= depth.z > SKY_EPS; // Don't apply to the sky ( Sky depth = float(0.001) )

	float edgeBlur = 0;

	// Peripheral vision blur
	if (ssfx_wpn_dof_2 > 0)
	{
		// Vignette to calc blur
		float2 mid_uv = tc - float2(0.5f, 0.5f);
 		edgeBlur = pow(smoothstep(0.0f, saturate(1.0f - ssfx_wpn_dof_2), length(mid_uv)), 1.5f) * 1.33f;

		blurAmountForeground = saturate(blurAmountForeground + edgeBlur) * ssfx_wpn_dof_1.w;
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
		img = Wpn_Blur / 16;

		// Peripheral vision blur with extra help from s_blur_2
		img = lerp(img, blurBackground, saturate(0.4f - (1.0f - edgeBlur)));
	}
	
	// Far blur ( Reload, Inventory and PDA )
	if (ssfx_wpn_dof_1.z > 0)
	{
		img = lerp(img, blurBackground, saturate(smoothstep(0.1f, 1.0f, coc) + int(depth.z <= SKY_EPS)) * ssfx_wpn_dof_1.z);
	}

	return img;
}