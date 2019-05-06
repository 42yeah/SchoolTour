uniform sampler2D texture;
uniform sampler2D back;
uniform float yOffset;

float rand(vec2 uv) {
    float a = dot(uv, vec2(92.0, 80.0));
    float b = dot(uv, vec2(41.0, 62.0));
    float x = (sin(a) + cos(b)) * 51.0;
    return fract(x);
}

void main() {
    vec2 somehowFlipped = vec2(gl_TexCoord[0].x, 1.0 - gl_TexCoord[0].y);
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
//    if (pixel.r <= 0.0 && pixel.g <= 0.0 && pixel.b <= 0.0) { discard; }
    vec2 uv = vec2(gl_TexCoord[0].x, gl_TexCoord[0].y - yOffset);
    vec2 rnd = vec2(rand(uv), rand(uv));
    uv += rnd * 0.05;
    gl_FragColor = texture2D(back, uv) * 0.3 + pixel * 0.7;
}
