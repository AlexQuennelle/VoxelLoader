#version 330

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

out vec4 finalColor;

void main()
{
    //finalColor = vec4(1.0f, 1.0f, 1.0f, 10.0f / 255); //overdraw visualization
    finalColor = fragColor;
}
