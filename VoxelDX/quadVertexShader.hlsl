struct VS_INPUT {
	float3 vPos : POSITION;
	float3 vColour : COLOR0;
};

cbuffer ModelViewProjectionConstantBuffer : register(b0) {
	matrix world;
	matrix viewProjection;
};

struct VS_OUTPUT {
	float4 position : SV_POSITION;
	float4 colour : COLOR;
};

VS_OUTPUT main(float3 pos: POSITION, float3 colour: COLOR0) {
	VS_OUTPUT output;

	float4 outPos = float4(pos, 1.0f);
	outPos = mul(outPos, world);
	outPos = mul(outPos, viewProjection);
	
	output.position = outPos;
	output.colour = float4(colour, 1.0f);
	return output;
}