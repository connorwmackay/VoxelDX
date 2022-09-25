struct VS_INPUT {
	float3 vPos : POSITION;
	float3 vColour : COLOR0;
};

struct VS_OUTPUT {
	float4 position : SV_POSITION;
	float4 colour : COLOR;
};

VS_OUTPUT main(float3 pos: POSITION, float3 colour: COLOR0) {
	VS_OUTPUT output;
	output.position = float4(pos, 1.0f);
	output.colour = float4(colour, 1.0f);
	return output;
}