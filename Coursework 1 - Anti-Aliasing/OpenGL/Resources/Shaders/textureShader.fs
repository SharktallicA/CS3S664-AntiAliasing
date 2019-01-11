#version 330

uniform sampler2D textSampler;
uniform int sampleSize;

in vec2 texCoord;

layout(pixel_center_integer) in vec4 gl_FragCoord;
layout (location=0) out vec4 fragColour;

//Maximum amount of samples needed (to complete x16 AA)
//Needed as const to initialise samples array,
//otherwise non-const expression error
const int maxSamples = 16 * 16;

void main(void)
{
	if (sampleSize == 1) //If given sample size is 1, sample normally
	{
		vec4 texColor = texture2D(textSampler, texCoord);
		fragColour = texColor;
	}
	else //Else supersample
	{
		//Total amount of samples needed to supersample at given size
		//Simply sampleSize squared
		int totalSamples = sampleSize * sampleSize;

		//Array of sampled colours to later be averaged out
		//Size is always maximum supported sample count (x16)y
		vec4 samples[maxSamples];

		for (int i = 0; i < sampleSize; i++)
		{
			for (int j = 0; j < sampleSize; j++)
			{
				int fragX = int(gl_FragCoord.x * sampleSize - i);
				int fragY = int(gl_FragCoord.y * sampleSize - j);
				samples[(i * sampleSize) + j] = texelFetch(textSampler, ivec2(fragX, fragY), 0);
			}
		}

		//Averaged colour of all samples
		vec4 average = vec4(0.0, 0.0, 0.0, 0.0);
		
		//average = Σ(samples) / totalSamples
		for (int i = 0; i < totalSamples; i++)
			average += samples[i];
		average = average / totalSamples;

		//Apply result as fragment's colour
		fragColour = average;
	}
}