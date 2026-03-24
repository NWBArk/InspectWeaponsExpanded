float2x2 nvg_rot(float a)
{
	float c, s;
	sincos(a, s, c);
	return float2x2(c, s, -s, c);
}

float3 SSFX_DOF_NVG(float2 tc, float3 depth, float3 img)
{
	// When NVGs are active, s_blur_2 contains encoded NVG pipeline data
	// (luma/lightmap/depth), not blurred color. Compute blur_far from
	// s_image instead, which holds the correctly NVG-processed frame.
	// Golden-ratio spiral with 24 samples at wide radius to approximate
	// the heavy diffusion of s_blur_2 (multi-pass downsampled blur).
	float CA = 4.0f / screen_res.x;
	float ratio = screen_res.y / screen_res.x;
	float3 blur_far = 0;
	blur_far.r = s_image.SampleLevel(smp_rtlinear, tc + float2(CA, 0), 0).r;
	blur_far.g = s_image.SampleLevel(smp_rtlinear, tc, 0).g;
	blur_far.b = s_image.SampleLevel(smp_rtlinear, tc - float2(CA, 0), 0).b;

	float r = 1.0f;
	float2x2 G = nvg_rot(2.399996);
	float scale = 0.6f;
	float2 offset = float2(scale * ratio, scale);

	for (int i = 0; i < 24; i++)
	{
		r += 1.0f / r;
		offset = mul(offset, G);
		float2 off = offset * (r - 1.0f) / screen_res.xy;
		blur_far.r += s_image.SampleLevel(smp_rtlinear, tc + off + float2(CA, 0), 0).r;
		blur_far.g += s_image.SampleLevel(smp_rtlinear, tc + off, 0).g;
		blur_far.b += s_image.SampleLevel(smp_rtlinear, tc + off - float2(CA, 0), 0).b;
	}
	blur_far /= 25.0;

	// --- Rest of SSFX_DOF logic, unchanged ---

	// Use Depth to adjust blur intensity
	float blur_w = lerp(ssfx_wpn_dof_1.w, 0, smoothstep(ssfx_wpn_dof_1.x, ssfx_wpn_dof_1.y, depth.z));
	blur_w *= depth.z > SKY_EPS;

	float edgeBlur = 0;

	// Peripheral vision blur
	if (ssfx_wpn_dof_2 > 0)
	{
		float2 mid_uv = tc - float2(0.5f, 0.5f);
		edgeBlur = pow(smoothstep(0.0f, saturate(1.0f - ssfx_wpn_dof_2), length(mid_uv)), 1.5f) * 1.33f;
		blur_w = saturate(blur_w + edgeBlur) * ssfx_wpn_dof_1.w;
	}

	// Close blur (Weapon blur)
	if (blur_w > 0)
	{
		float offset_f = 0.001f;
		float ratio = screen_res.y / screen_res.x;
		float2 blur_res = float2(blur_w, blur_w) * float2(offset_f * ratio, offset_f);

		float2 blur_Offsets[8] =
		{
			float2(-blur_res.x, -blur_res.y),
			float2( blur_res.x,  blur_res.y),
			float2( blur_res.x, -blur_res.y),
			float2(-blur_res.x,  blur_res.y),
			float2( 0,          -blur_res.y),
			float2( blur_res.x,  0),
			float2(-blur_res.x,  0),
			float2( 0,           blur_res.y)
		};

		float3 Wpn_Blur = 0;
		for (int b = 0; b < 8; b++)
		{
			Wpn_Blur += s_image.SampleLevel(smp_rtlinear, tc + blur_Offsets[b], 0).rgb;
			Wpn_Blur += s_image.SampleLevel(smp_rtlinear, tc + blur_Offsets[b] * 2, 0).rgb;
		}
		img = Wpn_Blur / 16;

		// Peripheral vision blend using our NVG-safe blur_far
		img = lerp(img, blur_far, saturate(0.4f - (1.0f - edgeBlur)));
	}

	// Far blur (Reload, Inventory and PDA)
	if (ssfx_wpn_dof_1.z > 0)
	{
		img = lerp(img, blur_far, saturate(smoothstep(1.0f, 2.4f, length(depth)) + int(depth.z <= SKY_EPS)) * ssfx_wpn_dof_1.z);
	}

	return img;
}