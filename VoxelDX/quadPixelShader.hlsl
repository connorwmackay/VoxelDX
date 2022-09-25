struct PS_INPUT {
	float4 position : SV_POSITION;
	float4 colour : COLOR;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	float4 colour = {1.0f, 0.0f, 0.0f, 1.0f};
	return colour;
}