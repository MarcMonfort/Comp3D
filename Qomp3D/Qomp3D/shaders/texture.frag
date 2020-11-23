#version 330

uniform sampler2D tex;
uniform bool bLighting;

in vec3 normalFrag;
in vec2 texCoordFrag;

out vec4 outColor;


const vec3 lightVector = normalize(vec3(1, 2, 3));


void main()
{
	// Discard fragment if texture sample has alpha < 0.5
	// otherwise compose the texture sample with the fragment's interpolated color
	vec4 texColor = texture(tex, texCoordFrag);
	if(texColor.a < 0.1f)
		discard;
	
	float lightContribution;
	if(bLighting)
	{
		// Diffuse directional light
		lightContribution = dot(lightVector, normalize(normalFrag));
		if(lightContribution < 0.f)
			lightContribution *= -0.3f;
			
		// Add ambient
		lightContribution = 0.7f * lightContribution + 0.3f;
	}
	else
		lightContribution = 1.f;
	
	outColor = vec4(lightContribution * texColor.rgb, texColor.a);
}

