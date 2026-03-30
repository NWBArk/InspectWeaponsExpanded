#include "inspectWeapon_dof_settings.h"
#include "inspectWeapon_dof_utils.h"

// DoF buffer.
Texture2D samplero_pepero;
uniform float4 ssfx_wpn_dof_1;

/**
	@brief Denoises Depth of Field.

	@param tc TexCoord.
	@param dof Unfiltered DoF data. rgb: pixel color, a: pixel coc.
	@return float3, Filtered color.
*/
float3 Inspect_DOF_Denoise(float2 tc, float4 dof)
{
    float highlightGain = saturate(INSPECT_DOF_HIGHLIGHT_GAIN);
    // Color we get from the buffer has already applied whiteness accentuation correction, so we need to revert it.
    float3 correctedDof = AccentuateWhites(dof.rgb, GammaFactor, highlightGain);

    float3 filtered = correctedDof;
    float bilateralWeight = 0;
    // 3x3 bilateral filtering with luminance guidence.
    for (int j = -1; j <= 1; j++)
    {
        for (int k = -1; k <= 1; k++)
        {
            float2 offset = float2(j, k) * (1 / float2(1920, 1080));
            float4 sample = samplero_pepero.SampleLevel(smp_nofilter, tc + offset, 0);
            // Same as before.
            float3 correctedSampleColor = AccentuateWhites(sample.rgb, GammaFactor, highlightGain);
            
            // Luminance-based weight to preserve edges in bokeh.
            float lumDiff = abs(GetLuminance(correctedSampleColor) - GetLuminance(correctedDof));
            float lumWeight = exp(-lumDiff * 2.0f);
            
            filtered += correctedSampleColor * lumWeight;
            bilateralWeight += lumWeight;
        }
    }
    filtered /= bilateralWeight;
    filtered = CorrectForWhiteAccentuation(filtered, GammaFactor, highlightGain);

    // Same mix logic as the main DoF pass.
    float coc = dof.a;
    float depth = GetDepth(tc, 0);
    filtered = lerp(dof, filtered, saturate(smoothstep(0.1f, 1.0f, coc) + int(depth <= SKY_EPS)) * ssfx_wpn_dof_1.z);

    return filtered;
}