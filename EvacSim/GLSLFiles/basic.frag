#version 330 core

in  vec3 ex_Color;  //colour arriving from the vertex
out vec4 out_Color; //colour for the pixel

void main()
{
	//out_Color = vec4(ex_Color,1.0);
	out_Color = vec4(1.0,0.0,0.0,1.0);
}