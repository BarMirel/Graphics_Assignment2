#version 300 es
precision highp float;
precision highp int;

struct Camera {
    vec3 pos;
    vec3 forward;
    vec3 right;
    vec3 up;
};

struct Plane {
    vec3 point;
    vec3 normal;
    vec3 color;
};

struct Sphere {
    vec3 center;
    float radius;
    vec3 color;
    int type; // 0: opaque, 1: reflective, 2: refractive
};

struct Light {
    vec3 position;
    vec3 direction;
    vec3 color;
    float shininess;
    float cutoff; // if > 0.0 then spotlight else directional light
};

struct HitInfo {
    vec3 rayOrigin;
    vec3 rayDir;
    float t;
    vec3 baseColor;
    int inside; // 1 if inside the sphere, 0 otherwise
    vec3 hitPoint;
    vec3 normal;
    int type; // 0: diffuse, 1: reflective
    int isPlane; // 1 if hit is plane, 0 if sphere
};

const int TYPE_DIFFUSE = 0;
const int TYPE_REFLECTIVE = 1;
const int TYPE_REFRACTIVE = 2;

const int MAX_SPHERES = 16;
const int MAX_LIGHTS = 4;
const int MAX_DEPTH = 5;

in vec2 vUV;
out vec4 FragColor;

uniform float uTime;
uniform ivec2 uResolution; // width and height of canvas


uniform Camera cam;
uniform Sphere uSpheres[MAX_SPHERES];
uniform int uNumSpheres;

uniform Light uLights[MAX_LIGHTS];
uniform int uNumLights;

uniform Plane uPlane;

vec3 checkerboardColor(vec3 rgbColor, vec3 hitPoint) {
    // Checkerboard pattern
    float scaleParameter = 2.0;
    float checkerboard = 0.0;
    if (hitPoint.x < 0.0) {
    checkerboard += floor((0.5 - hitPoint.x) / scaleParameter);
    }
    else {
    checkerboard += floor(hitPoint.x / scaleParameter);
    }
    if (hitPoint.z < 0.0) {
    checkerboard += floor((0.5 - hitPoint.z) / scaleParameter);
    }
    else {
    checkerboard += floor(hitPoint.z / scaleParameter);
    }
    checkerboard = (checkerboard * 0.5) - float(int(checkerboard * 0.5));
    checkerboard *= 2.0;
    if (checkerboard > 0.5) {
    return 0.5 * rgbColor;
    }
    return rgbColor;
}

/* intersects scene. gets ray origin and direction, returns hit data*/
HitInfo intersectScene(vec3 rayOrigin, vec3 rayDir) {
    HitInfo hit;

    hit.rayOrigin = rayOrigin;
    hit.rayDir    = rayDir;
    hit.t         = 1e20;          // large value means no hit yet
    hit.baseColor = vec3(0.0);
    hit.inside    = 0;
    hit.hitPoint  = vec3(0.0);
    hit.normal    = vec3(0.0, 1.0, 0.0);
    hit.type      = TYPE_DIFFUSE;
    hit.isPlane   = 0;

    // Plane intersection
    float denom = dot(rayDir, uPlane.normal);
    if (abs(denom) > 1e-4) {
        float tPlane = dot(uPlane.point - rayOrigin, uPlane.normal) / denom;
        if (tPlane > 0.0 && tPlane < hit.t) {
            hit.t        = tPlane;
            hit.hitPoint = rayOrigin + tPlane * rayDir;
            hit.normal   = normalize(uPlane.normal);
            hit.baseColor = checkerboardColor(uPlane.color, hit.hitPoint);
            hit.type      = TYPE_DIFFUSE; 
            hit.isPlane   = 1;
        }
    }

    // Sphere intersections
    for (int i = 0; i < uNumSpheres; ++i) {
        vec3  oc = rayOrigin - uSpheres[i].center;
        float a  = dot(rayDir, rayDir);
        float b  = 2.0 * dot(oc, rayDir);
        float c  = dot(oc, oc) - uSpheres[i].radius * uSpheres[i].radius;
        float det = b * b - 4.0 * a * c;

        if (det > 0.0) {
            float sDet = sqrt(det);
            float t1 = (-b - sDet) / (2.0 * a);
            float t2 = (-b + sDet) / (2.0 * a);

            float tSphere = t1;
            if (tSphere < 0.0) tSphere = t2;

            if (tSphere > 0.0 && tSphere < hit.t) {
                hit.t        = tSphere;
                hit.hitPoint = rayOrigin + tSphere * rayDir;
                hit.normal   = normalize(hit.hitPoint - uSpheres[i].center);
                hit.baseColor = uSpheres[i].color;
                hit.type      = uSpheres[i].type;
                hit.isPlane   = 0;
            }
        }
    }

    return hit;
}

/* calculates color based on hit data */
vec3 calcColor(HitInfo hitInfo) {
    if (hitInfo.t > 1e19) {
        return vec3(0.0); 
    }

    vec3 V = normalize(cam.pos - hitInfo.hitPoint); // view direction
    vec3 N = normalize(hitInfo.normal);

    vec3 K_A = hitInfo.baseColor;
    vec3 I_A = vec3(0.1, 0.2, 0.3); 
    vec3 color = K_A * I_A;

    vec3 K_S = vec3(0.7);

    // Diffuse + specular
    for (int i = 0; i < uNumLights; ++i) {
        Light light = uLights[i];

        vec3 L;          // direction from hit point to light
        float cutoff = light.cutoff;

        if (cutoff <= 0.0) {
            // Directional light: direction field points FROM light, so invert
            L = normalize(-light.direction);
        } else {
            // Spotlight
            vec3 toLight = light.position - hitInfo.hitPoint;
            float dist = length(toLight);
            if (dist < 1e-6) {
                continue;
            }
            L = toLight / dist;

            float cosAngle = dot(-L, normalize(light.direction));
            if (cosAngle < cutoff) {
                continue;
            }
        }

        // For planes, flip the normal toward the light if needed
        if (hitInfo.isPlane == 1) {
            if (dot(N, L) < 0.0) {
                N = -N;
            }
        }

        float NdotL = max(dot(N, L), 0.0);
        if (NdotL == 0.0) {
            continue;
        }

        // Diffuse: K_D * (N·L) * I
        vec3 K_D = hitInfo.baseColor;
        vec3 diffuse = K_D * NdotL * light.color;
        color += diffuse;
        
        // Specular: K_S * (V·R)^n * I
        // Calculate reflection vector: R = 2 * (N·L) * N - L
        vec3 R = 2.0 * NdotL * N - L;
        R = normalize(R);
        float VdotR = dot(V, R);
        if (VdotR > 0.0) {
            float specPower = light.shininess;
            vec3 specular = K_S * pow(VdotR, specPower) * light.color;
            color += specular;
        }
    }

    return clamp(color, 0.0, 1.0);
}

/* scales UV coordinates based on resolution
 * uv given uv are [0, 1] range
 * returns new coordinates where y range [-1, 1] and x scales according to window resolution
 */
vec2 scaleUV(vec2 uv) {
    float aspect = float(uResolution.x) / float(uResolution.y);
    vec2 scaledUV;
    scaledUV.x = (uv.x - 0.5) * 2.0 * aspect;
    scaledUV.y = (uv.y - 0.5) * 2.0;
    return scaledUV;
}

void main() {
    vec2 uv = scaleUV(vUV);
    vec3 rayDir = normalize(cam.forward + uv.x * cam.right + uv.y * cam.up);

    HitInfo hitInfo = intersectScene(cam.pos, rayDir);

    vec3 color = calcColor(hitInfo);

    FragColor = vec4(color, 1.0);
}

