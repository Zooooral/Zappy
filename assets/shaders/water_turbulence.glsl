#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform float time;
uniform vec2 resolution;

out vec4 finalColor;

#define TAU 6.28318530718
#define MAX_ITER 5

void main() {
    float timeScaled = time * 0.5 + 23.0;
    
    vec2 uv = fragTexCoord * 11.0;
    
    vec2 p = mod(uv * TAU, TAU) - 250.0;
    vec2 i = vec2(p);
    float c = 1.0;
    float inten = 0.005;
    
    for (int n = 0; n < MAX_ITER; n++) {
        float t = timeScaled * (1.0 - (3.5 / float(n + 1)));
        i = p + vec2(cos(t - i.x) + sin(t + i.y), sin(t - i.y) + cos(t + i.x));
        c += 1.0 / length(vec2(p.x / (sin(i.x + t) / inten), p.y / (cos(i.y + t) / inten)));
    }
    
    c /= float(MAX_ITER);
    c = 1.17 - pow(c, 1.4);
    
    vec3 colour = vec3(pow(abs(c), 4.0)) * 0.4;
    colour = clamp(colour + vec3(0.0, 0.10, 0.15), 0.0, 1.0);
    vec3 cartoonBlue = vec3(0.25, 0.65, 1.0);
    colour = mix(colour, cartoonBlue, 0.55);
    colour = mix(colour, vec3(0.15, 0.50, 0.95), 0.25);
    finalColor = vec4(colour, 1.0);
}