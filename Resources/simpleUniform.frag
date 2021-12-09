uniform float uTime;
uniform vec2 uMouseNorm;
uniform vec2 uResolution;

void main() {
  float w = 0.0;
  vec2 uv = gl_FragCoord.xy / uResolution.xy;
  vec2 mouse = vec2(uMouseNorm.x, 1.0 - uMouseNorm.y);

  // line
  float lineWidth = 0.01;
  w += step(mouse.x - lineWidth * 0.5, uv.x) * step(uv.x, mouse.x + lineWidth * 0.5);
  w += step(mouse.y - lineWidth * 0.5, uv.y) * step(uv.y, mouse.y + lineWidth * 0.5);

  // circle
  float radius = 0.1;
  w += smoothstep(radius + 0.2, radius, distance(mouse.xy, uv.xy));

  // wave
  float dist = distance(vec2(0.5), uv);
  float r = (sin(pow(dist * 2.0, 1.5) * 10.0 - uTime + 0.0) + 1.0) * 0.5;
  float g = (sin(pow(dist * 2.0, 1.5) * 10.0 - uTime + 0.75) + 1.0) * 0.5;
  float b = (sin(pow(dist * 2.0, 1.5) * 10.0 - uTime + 1.4) + 1.0) * 0.5;

  gl_FragColor = vec4(vec3(w) + vec3(r, g, b), 1.0);
}
