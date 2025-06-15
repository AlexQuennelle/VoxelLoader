#version 100

precision mediump float;

varying vec3 fragPosition;
varying vec2 fragTexCoord;
varying vec4 fragColor;
varying vec3 fragNormal;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

void main()
{
    //finalColor = fragColor;
    gl_FragColor = vec4(fragNormal, 1.0);
}
