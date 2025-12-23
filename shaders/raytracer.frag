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
    hit.t = -1.0;
    // Check plane intersection
    float denom = dot(rayDir, uPlane.normal);
    if (abs(denom) > 0.0001) {
        float t = dot((uPlane.point - rayOrigin), uPlane.normal) / denom;
        if (t > 0.0) {
            hit.t = t;
            hit.hitPoint = rayOrigin + t * rayDir;
            hit.normal = uPlane.normal;
            hit.baseColor = checkerboardColor(uPlane.color, hit.hitPoint);
            hit.type = TYPE_DIFFUSE;
            hit.rayOrigin = rayOrigin;
            hit.rayDir = rayDir;
            hit.inside = 0;
        }
    }
    // Check sphere intersections
    for (int i = 0; i < uNumSpheres; i++) {
        vec3 oc = rayOrigin - uSpheres[i].center;
        float a = dot(rayDir, rayDir);
        float b = 2.0 * dot(oc, rayDir);
        float c = dot(oc, oc) - uSpheres[i].radius * uSpheres[i].radius;
        float discriminant = b * b - 4.0 * a * c;
        if (discriminant > 0.0) {
            float t = (-b - sqrt(discriminant)) / (2.0 * a);
            if (t > 0.0 && (hit.t < 0.0 || t < hit.t)) {
                hit.t = t;
                hit.hitPoint = rayOrigin + t * rayDir;
                hit.normal = normalize(hit.hitPoint - uSpheres[i].center);
                hit.baseColor = uSpheres[i].color;
                hit.type = uSpheres[i].type;
                hit.rayOrigin = rayOrigin;
                hit.rayDir = rayDir;
                hit.inside = 0;
            }
        }
    }

    return hit;
}

/* calculates color based on hit data */
vec3 calcColor(HitInfo hitInfo) {
    if (hitInfo.t < 0.0) {
        return vec3(0.0, 0.0, 0.0); // background color
    } else {
        return hitInfo.baseColor;
    }
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

