SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

Texture2D gTexture;

DepthStencilState EnableDepthWriting
{
	DepthEnable = true;
	DepthWriteMask = ALL;
};

RasterizerState BackCulling
{
	CullMode = BACK;
};

struct VS_INPUT
{
    float3 Position : POSITION;
	float2 TexCoord : TEXCOORD0;

};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD1;
};

PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	output.Position = float4(input.Position, 1.0f);
	output.TexCoord = input.TexCoord;
	
	return output;
}

float4 PS(PS_INPUT input): SV_Target
{
	float4 color = gTexture.Sample(samPoint, input.TexCoord);

	// Vignette effect
	float2 uv = input.TexCoord;
	uv -= 0.5; // shift origin to center
	float dist = length(uv);
	float vignette = smoothstep(0.85, 0.35, dist);

	// Apply vignette
	color.rgb *= vignette;

	return color;
}

technique11 Vignette
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}