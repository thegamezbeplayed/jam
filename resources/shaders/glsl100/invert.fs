#version 100
precision mediump float;

varying vec2 fragTexCoord;
varying vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

void main()
{
    vec4 texelColor = texture2D(texture0, fragTexCoord)*colDiffuse*fragColor;
    gl_FragColor = vec4(1.0 - texelColor.rgb, texelColor.a);
}
