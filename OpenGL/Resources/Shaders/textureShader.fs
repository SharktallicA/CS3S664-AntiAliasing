#version 330

uniform sampler2D textSampler;

in float resX;
in float resY;
in float eblSSAA;
in vec2 texCoord;

layout (location=0) out vec4 fragColour;

void main(void)
{
	//Sample current texel
	vec4 C = texture2D(textSampler, texCoord);

	if (eblSSAA == 0)
	{
		fragColour = C;
	}
	else
	{
		//Work out texel size (relative to given resolution)
		float tX = 1.0 / resX;
		float tY = 1.0 / resY;

		//top-left
		vec2 tLOffset = vec2(0, 0);
		vec4 tLCol = texture2D(textSampler, texCoord + tLOffset);
		//top-right
		vec2 tROffset = vec2(0.5 * tX, 0);
		vec4 tRCol  = texture2D(textSampler, vec2(texCoord.s, texCoord.t - tY));
		//bottom-left
		vec2 bLOffset = vec2(0, 0.5 * tY);
		vec4 bLCol  = texture2D(textSampler, vec2(texCoord.s + tX, texCoord.t));
		//bottom-right
		vec2 bROffset = vec2(0.5 * tX, 0.5 * tY);
		vec4 bRCol  = texture2D(textSampler, vec2(texCoord.s - tX, texCoord.t));

		vec4 avgCol = (tLCol + tRCol + bLCol + bRCol) / 4;
		fragColour = avgCol;
	}
}