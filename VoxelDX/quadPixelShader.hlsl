struct PS_INPUT {
	float4 position : SV_POSITION;
	float4 colour : COLOR;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	float4 colour = input.colour;
	return colour;
}