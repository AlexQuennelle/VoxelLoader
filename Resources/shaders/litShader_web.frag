#version 100

precision mediump float;

varying vec3 fragPosition;
varying vec2 fragTexCoord;
varying vec4 fragColor;
varying vec3 fragNormal;

float ambient = 0.2;
vec3 lightPos = vec3(40, 50, -60);

uniform sampler2D texture0;
uniform vec4 colDiffuse;

void main()
{
    vec3 dir = normalize(lightPos - fragPosition);
    float diffuse = max(dot(fragNormal, dir), ambient);
    gl_FragColor = vec4(fragColor.xyz * diffuse, 1.0);
}
