uniform sampler2D texture;
uniform vec2 resolution;
uniform float time;

void main() {
    vec2 uv = gl_TexCoord[0].xy;
    vec2 realUV = vec2(resolution.x * uv.x, resolution.y * uv.y);
    vec2 fakeUV = floor(realUV / max(2.5 / time, 1.0)) * max(2.5 / time, 1.0);
    
    uv = fakeUV / resolution.xy;
    vec2 texCoord = uv;
    uv *= 1.0 - uv.yx;
    float vig = uv.x * uv.y * 15.0;
    vig = pow(vig, 0.25);
    
    vec4 pixel = texture2D(texture, texCoord);
    pixel.rgb -= mod(realUV.y + ceil(time * 20.0), 5.0) < 3.0 ? 0.1 : 0.0;
    gl_FragColor = vec4(pixel * vig);
}
