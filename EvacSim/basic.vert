#version 330 core
//#version 150

uniform mat4 ModelViewMatrix;
//uniform mat4 ViewMatrix;
//uniform mat4 ModelMatrix;
uniform mat4 ProjectionMatrix;

layout (location = 0) in vec3 in_Position; // Position coming in
layout (location = 1) in vec3 in_Color; // colour coming in

out vec3 ex_Color;     // colour leaving the vertex, this will be sent to the fragment shader

void main()
{
	gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(in_Position, 1.0);
    //gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(in_Position, 0.1);
    //gl_Position = vec4(in_Position, 1.0);
    //gl_Position = ModelViewMatrix * vec4(in_Position, 1.0);

    //gl_Position = 1.0f * ModelViewMatrix * vec4(in_Position, 1.0f);
	
	ex_Color = in_Color;
}