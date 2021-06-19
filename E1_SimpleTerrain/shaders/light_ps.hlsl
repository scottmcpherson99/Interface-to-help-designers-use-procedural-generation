// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D waterTexture : register(t0);
Texture2D sandTexture: register(t1);
Texture2D grassTexture : register(t2);
Texture2D snowTexture : register(t3);
Texture2D rockTexture : register(t4);
SamplerState sampler0 : register(s0);

cbuffer LightBuffer : register(b0)
{
	float4 ambientColour;
	float4 diffuseColour;
	float3 lightDirection;
	float padding[3];
};

cbuffer SliderTextureBuffer : register (b1)
{
	float grass;
	float water;
	float snow;
	float rock;
}
struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 worldPosition : TEXCOORD1;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
	float intensity = saturate(dot(normal, lightDirection));
	float4 colour = saturate(diffuse * intensity);
	return colour;
}

float4 main(InputType input) : SV_TARGET
{
	float4 waterTextureColour;
	float4 sandTextureColour;
	float4 grassTextureColour;
	float4 snowTextureColour;
	float4 rockTextureColour;
	float4 lightColour;

	// Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.

	waterTextureColour = waterTexture.Sample(sampler0, input.tex);
	sandTextureColour = sandTexture.Sample(sampler0, input.tex);
	grassTextureColour = grassTexture.Sample(sampler0, input.tex);
	snowTextureColour = snowTexture.Sample(sampler0, input.tex);
	rockTextureColour = rockTexture.Sample(sampler0, input.tex);

	//textureColour2 = texture0.Sample(sampler0, input.tex);

	lightColour = ambientColour + calculateLighting(-lightDirection, input.normal, diffuseColour);



	if (input.worldPosition.y < water)
	{
		return lightColour * waterTextureColour;
	}
	else if (input.worldPosition.y > water)
	{
		if (input.worldPosition.y <= grass)
		{
			return lightColour * sandTextureColour;
		}

		else if (input.worldPosition.y > grass)
		{
			if (input.worldPosition.y <= rock)
			{
				return lightColour * grassTextureColour;
			}
			else if (input.worldPosition.y > rock)
			{
				if (input.worldPosition.y <= snow)
				{
					return lightColour * rockTextureColour;
				}
				else {
					return lightColour * snowTextureColour;
				}
			}
		}
	}

		return lightColour * waterTextureColour;


		//return lerp( lightColour * textureColour, lightColour*textureColour2, saturate(input.worldPosition.y/10.0));
}



