#version 150

in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D texFramebuffer;
uniform float time = 0;

void main() {
    outColor = vec4(texture(texFramebuffer, Texcoord));
    float a = 0, b = 0;


    a = 0.001;
    b = (-(sin(time)+1)*0.75f - 0.5)*0.5;
    outColor = mix(outColor, vec4(texture(texFramebuffer, Texcoord + vec2(a, -a))), b);
    outColor = mix(outColor, vec4(texture(texFramebuffer, Texcoord + vec2(a, a))), b);
    outColor = mix(outColor, vec4(texture(texFramebuffer, Texcoord + vec2(-a, a))), b);
    outColor = mix(outColor, vec4(texture(texFramebuffer, Texcoord + vec2(-a, -a))), b);
    a = -0.25;
    b = -(abs(Texcoord.x-0.5) + abs(Texcoord.y-0.5))*0.05;
    outColor = mix(outColor, vec4(texture(texFramebuffer, Texcoord + vec2(a, -a))), b);
    outColor = mix(outColor, vec4(texture(texFramebuffer, Texcoord + vec2(a, a))), b);
    outColor = mix(outColor, vec4(texture(texFramebuffer, Texcoord + vec2(-a, a))), b);
    outColor = mix(outColor, vec4(texture(texFramebuffer, Texcoord + vec2(-a, -a))), b);

    outColor *= (fract(Texcoord.y*time) > 0.01) ? 1 : 1.1;
    outColor *= ( (mod(Texcoord.x, fract(outColor.r+cos(time*0.1)))) < 0.01 ) ? 1.05 : 1;

    
}
