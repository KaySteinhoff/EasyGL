#version 110

varying vec2 texcoord;
uniform sampler2D tex;
uniform vec3 color;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture2D(tex, texcoord).r);
    gl_FragColor = vec4(color, 1.0) * sampled;
}  