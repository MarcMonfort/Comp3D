#version 330

uniform mat4 projection, model, view;
uniform mat3 normalmatrix;

in vec3 position;
in vec3 normal;
in vec2 texCoord;

out vec3 normalFrag;
out vec2 texCoordFrag;


void main()
{
	// Pass texture coordinates
	texCoordFrag = texCoord;
	
	// Pass normal
	normalFrag = normalmatrix * normal;
	
	// Transform position from pixel coordinates to clipping coordinates
	gl_Position = projection * view * model * vec4(position, 1.0);
}

