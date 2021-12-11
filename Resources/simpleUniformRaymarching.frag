uniform float uPan;
uniform float uVolume;
uniform float uTime;
uniform vec2 uResolution;

//---------------------------------------------
// you can edit value
//---------------------------------------------

// sphere
const float sphereSize = 1.0;
const vec3 spherePosition = vec3(0, 0, 0);
const vec3 sphereColor = vec3(0.9);

// area
const float areaSize = 10.0;
const float areaHalf = areaSize * 0.5;
const vec3 areaCenterPosition = vec3(0, 0, 0);
const vec3 leftWallColor = vec3(0.9, 0.1, 0.1);
const vec3 rightWallColor = vec3(0.1, 0.9, 0.1);
const vec3 wallColor = vec3(0.9);

// camera
const vec3 cameraPosition = vec3(0.0, 0.0, 8.0);
const vec3 lookAtPosition = vec3(0.0, 0.0, 0.0);

// light
const float pointLightZ = 4.0;
vec3 lightColor = vec3(1.0);
const float ambient = 0.2;
const bool enableDiffuse = true;
const bool enableSpecular = true;
const float specularCurve = 50.0;

// shadow
float shadowAmount = clamp(0.5, 0.0, 1.0);

// light fx
const bool enableLightFx = true;
const float sinFreq = 1.0;
const float sinCurve = 1.0;
const float lightFx = 0.5;
const float colorGap = 0.75;

//---------------------------------------------
// constants & variables
//---------------------------------------------
const int countMax = 30;
vec3 pointLightPosition;

const float EPS = 0.01;
const vec3 deltaX = vec3(EPS, 0, 0);
const vec3 deltaY = vec3(0, EPS, 0);
const vec3 deltaZ = vec3(0, 0, EPS);

//---------------------------------------------
// signed distance functions & operator
//---------------------------------------------
float opSubtraction( float d1, float d2 ) { return max(-d1,d2); }

vec3 opRep(vec3 p, vec3 c)
{
  return mod(p+0.5*c,c)-0.5*c;
}

float sdSphere( vec3 p, float s )
{
  return length(p)-s;
}

float sdBox( vec3 p, vec3 b )
{
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float sdPlane( vec3 p, vec3 n ) {
   return dot( p, n.xyz ) + 1.0;
}

//---------------------------------------------
// custom distance functions
//---------------------------------------------
float sdSphereStripe( vec3 p, float s )
{
  float sphereDist = length(p)-s;
  float boxDist = sdBox(opRep(p + vec3(0, uTime, 0), vec3(1, 0.5, 10.0)), vec3(s * 2.0, 0.1, s * 2.0));
  return opSubtraction(boxDist, sphereDist);
}

float distFunc(vec3 p) {
  float sphereDist = sdSphere(p - spherePosition, sphereSize);
  float leftWallDist = sdBox(p - (areaCenterPosition + vec3(-areaHalf, 0, 0)), vec3(0.1, areaSize, areaSize));
  float rightWallDist = sdBox(p - (areaCenterPosition + vec3(areaHalf, 0, 0)), vec3(0.1, areaSize, areaSize));
  float topWallDist = sdBox(p - (areaCenterPosition + vec3(0, -areaHalf, 0)), vec3(areaSize, 0.1, areaSize));
  float bottomWallDist = sdBox(p - (areaCenterPosition + vec3(0, areaHalf, 0)), vec3(areaSize, 0.1, areaSize));
  float frontWallDist = sdBox(p - (areaCenterPosition + vec3(0, 0, -areaHalf)), vec3(areaSize, areaSize, 0.1));
 
  float dist;
  dist = min(sphereDist, leftWallDist);
  dist = min(dist, rightWallDist);
  dist = min(dist, topWallDist);
  dist = min(dist, bottomWallDist);
  dist = min(dist, frontWallDist);
  return dist;
}

float sdLightSphere(vec3 p, float r) {
  return sdSphere(p - pointLightPosition, r);
}

//---------------------------------------------
vec3 distFuncNormal(vec3 p) {
  return normalize(vec3(distFunc(p + deltaX) - distFunc(p - deltaX),
                        distFunc(p + deltaY) - distFunc(p - deltaY),
                        distFunc(p + deltaZ) - distFunc(p - deltaZ))
                  );
}

vec3 distFuncColor(vec3 p, float dist) {
  float sphereDist = sdSphere(p - spherePosition, sphereSize);
 float leftWallDist = sdBox(p - (areaCenterPosition + vec3(-areaHalf, 0, 0)), vec3(0.1, areaSize, areaSize));
  float rightWallDist = sdBox(p - (areaCenterPosition + vec3(areaHalf, 0, 0)), vec3(0.1, areaSize, areaSize));

  if (abs(dist - sphereDist) < EPS) {
    return sphereColor;
  }
  else if (abs(dist - leftWallDist) < EPS) {
    return leftWallColor;
  }
  else if (abs(dist - rightWallDist) < EPS) {
    return rightWallColor;
  }
  else {
    return wallColor;
  }
}

//---------------------------------------------
float genShadow(vec3 ro, vec3 dst){
    vec3 rd = normalize(dst - ro);
    float h = 0.0;
    float c = 0.001;
    float r = 1.0;
    float shadowCoef = 0.5;
    for(float t = 0.0; t < 50.0; t++){
        h = distFunc(ro + rd * c);
        if(h < 0.001){
            return shadowCoef;
        }
        r = min(r, h * 16.0 / c);
        c += h;
        if (c > distance(ro, dst)) break;
    }
    return mix(shadowCoef, 1.0, r);
}

//---------------------------------------------
// custom distance functions
//---------------------------------------------
void main() {
  float w = 0.0;
  vec2 uv = (gl_FragCoord.xy * 2.0 - uResolution.xy) / max(uResolution.x, uResolution.y);
  vec2 mouse = vec2(uPan, (uVolume - 0.5) * 2.0);
  if (uResolution.y < uResolution.x) mouse.y *= (uResolution.y / uResolution.x);
  else mouse.x *= (uResolution.y / uResolution.x);

  vec4 color = vec4(vec3(0), 1);

  // calculate camera vector
  vec3 cameraDirection = normalize(lookAtPosition - cameraPosition);
  vec3 cameraUp = vec3(0, 1, 0);
  vec3 cameraRight = normalize(cross(cameraDirection, cameraUp));
  cameraUp = normalize(cross(cameraRight, cameraDirection));

  // initialize ray position & direction
  vec3 rayPosition = cameraPosition;
  vec3 rayDirection = normalize(cameraDirection + uv.x * cameraRight + uv.y * cameraUp);

  pointLightPosition = vec3(0, 0, pointLightZ) + 4.0 * mouse.x * cameraRight + 4.0 * mouse.y * cameraUp;

  for (int i = 0; i < countMax; i++) {
    float dist = distFunc(rayPosition);

    if (dist < EPS) {
      vec3 normal = distFuncNormal(rayPosition);
      vec3 pointLightVec = normalize(pointLightPosition - rayPosition);
      vec3 halfLE = normalize(pointLightVec + -rayDirection);

      if (enableLightFx) {
        float pointLightDistance = distance(cameraPosition, rayPosition);
        lightColor.r = (sin(pow(pointLightDistance * sinFreq, sinCurve) + uTime + colorGap * 0.0) + 1.0) * 0.5;
        lightColor.g = (sin(pow(pointLightDistance * sinFreq, sinCurve) + uTime + colorGap * 1.0) + 1.0) * 0.5;
        lightColor.b = (sin(pow(pointLightDistance * sinFreq, sinCurve) + uTime + colorGap * 2.0) + 1.0) * 0.5;
        lightColor = mix(vec3(1.0), lightColor, lightFx);
      }

      vec3 objColor = distFuncColor(rayPosition, dist);

      if (enableDiffuse) {
        float diffuse = clamp(dot(normal, pointLightVec), 0.0, 1.0) + ambient;
        objColor *= diffuse * lightColor;
      }

      if (enableSpecular) {
        float specular = pow(clamp(dot(normal, halfLE), 0.0, 1.0), specularCurve);
        objColor +=  vec3(specular) * lightColor;
      }

      float shadow = genShadow(rayPosition + normal * EPS, pointLightPosition);
      objColor *= max(1.0 - shadowAmount, shadow);
      color.rgb += objColor;
      break;
    }

    rayPosition += dist * rayDirection;
  }
  color.rgb += smoothstep(0.2, 0.1, distance(mouse, uv));
  gl_FragColor = color;

}
