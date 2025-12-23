#include "Scene.h"
#include "Sphere.h"
#include "Plane.h"
#include "DirectionalLight.h"
#include "Spotlight.h"
#include "IntersectionPoint.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <limits>
#include <cmath>

Scene::Scene() : distanceToScreen(0.0f), screenHeight(0.0f), screenWidth(0.0f) {}

Scene::~Scene() = default;

bool Scene::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (!parseLine(line)) {
            std::cerr << "Error parsing line: " << line << std::endl;
            return false;
        }
    }

    createObjects();
    createLights();

    return true;
}

bool Scene::parseLine(const std::string& line) {
    std::istringstream iss(line);
    std::string token;
    iss >> token;

    if (token == "e") { // eye (camera position)
        iss >> cameraPosition.x >> cameraPosition.y >> cameraPosition.z >> distanceToScreen;
    } else if (token == "u") { // up vector
        iss >> upVector.x >> upVector.y >> upVector.z >> screenHeight;
        upVector = glm::normalize(upVector);
    } else if (token == "f") { // forward vector
        iss >> forwardVector.x >> forwardVector.y >> forwardVector.z >> screenWidth;
        forwardVector = glm::normalize(forwardVector);
    } else if (token == "a") { // ambient intensity
        iss >> ambientIntensity.r >> ambientIntensity.g >> ambientIntensity.b;
        float dummy;
        iss >> dummy; // 4th coordinate (always 1.0)
    } else if (token == "o" || token == "r" || token == "t") { // object (sphere or plane)
        MaterialType materialType = NORMAL;
        if (token == "r") materialType = REFLECTIVE;
        else if (token == "t") materialType = TRANSPARENT;

        float x, y, z, w;
        iss >> x >> y >> z >> w;

        if (w > 0) { // sphere: w is radius
            objects.push_back(std::make_unique<Sphere>(glm::vec3(x, y, z), w, materialType, glm::vec3(0.0f), 0.0));
        } else { // plane: w is d coefficient
            objects.push_back(std::make_unique<Plane>(x, y, z, w, materialType, glm::vec3(0.0f), 0.0));
        }
    } else if (token == "c") { // color
        glm::vec3 color;
        float shininess;
        iss >> color.r >> color.g >> color.b >> shininess;
        objectColors.push_back(color);
        objectShininess.push_back(shininess);
    } else if (token == "d") { // directional light or spotlight direction
        glm::vec3 direction;
        iss >> direction.x >> direction.y >> direction.z;
        float typeFlag;
        iss >> typeFlag; // 0.0 for directional, 1.0 for spotlight
        lightDirections.push_back(direction);
    } else if (token == "p") { // spotlight position
        glm::vec3 position;
        iss >> position.x >> position.y >> position.z;
        float cutoffCosine;
        iss >> cutoffCosine;
        lightPositions.push_back(position);
        lightCutoffCosines.push_back(cutoffCosine);
    } else if (token == "i") { // light intensity
        glm::vec3 intensity;
        iss >> intensity.r >> intensity.g >> intensity.b;
        float dummy;
        iss >> dummy; // 4th coordinate (always 1.0)
        lightIntensities.push_back(intensity);
    }

    return true;
}

void Scene::createObjects() {
    size_t colorIndex = 0;
    for (auto& obj : objects) {
        if (colorIndex < objectColors.size()) {
            obj->setColor(objectColors[colorIndex]);
            obj->setShininess(objectShininess[colorIndex]);
            colorIndex++;
        }
    }
}

void Scene::createLights() {
    size_t dirIndex = 0;
    size_t posIndex = 0;
    size_t cutoffIndex = 0;
    size_t intIndex = 0;

    while (intIndex < lightIntensities.size()) {
        if (dirIndex < lightDirections.size()) {
            // Check if this is a spotlight (has position)
            if (posIndex < lightPositions.size() && cutoffIndex < lightCutoffCosines.size()) {
                lights.push_back(std::make_unique<Spotlight>(
                    lightPositions[posIndex],
                    lightDirections[dirIndex],
                    lightCutoffCosines[cutoffIndex],
                    lightIntensities[intIndex]
                ));
                posIndex++;
                cutoffIndex++;
            } else {
                lights.push_back(std::make_unique<DirectionalLight>(
                    lightDirections[dirIndex],
                    lightIntensities[intIndex]
                ));
            }
            dirIndex++;
        }
        intIndex++;
    }
}

void Scene::printSceneInfo() const {
    std::cout << "Camera Position: (" << cameraPosition.x << ", " << cameraPosition.y << ", " << cameraPosition.z << ")" << std::endl;
    std::cout << "Distance to Screen: " << distanceToScreen << std::endl;
    std::cout << "Up Vector: (" << upVector.x << ", " << upVector.y << ", " << upVector.z << ")" << std::endl;
    std::cout << "Screen Height: " << screenHeight << std::endl;
    std::cout << "Screen Width: " << screenWidth << std::endl;
    std::cout << "Forward Vector: (" << forwardVector.x << ", " << forwardVector.y << ", " << forwardVector.z << ")" << std::endl;
    std::cout << "Ambient Intensity: (" << ambientIntensity.r << ", " << ambientIntensity.g << ", " << ambientIntensity.b << ")" << std::endl;

    std::cout << "Objects:" << std::endl;
    for (size_t i = 0; i < objects.size(); ++i) {
        const auto& obj = objects[i];
        std::cout << "  Object " << i + 1 << ": ";
        if (obj->getType() == SPHERE) {
            const Sphere* sphere = dynamic_cast<const Sphere*>(obj.get());
            std::cout << "Sphere at (" << sphere->getCenter().x << ", " << sphere->getCenter().y << ", " << sphere->getCenter().z
                      << ") with radius " << sphere->getRadius();
        } else {
            const Plane* plane = dynamic_cast<const Plane*>(obj.get());
            std::cout << "Plane with coefficients (" << plane->getA() << ", " << plane->getB() << ", " << plane->getC() << ", " << plane->getD() << ")";
        }
        std::cout << "Object Shinines: " << obj->getShininess() << std::endl;
    }

    std::cout << "Lights:" << std::endl;
    for (size_t i = 0; i < lights.size(); ++i) {
        const auto& light = lights[i];
        std::cout << "  Light " << i + 1 << ": ";
        if (light->getType() == DIRECTIONAL) {
            const DirectionalLight* dirLight = dynamic_cast<const DirectionalLight*>(light.get());
            std::cout << "Directional with direction (" << dirLight->getDirection().x << ", " << dirLight->getDirection().y << ", " << dirLight->getDirection().z << ")";
        } else {
            const Spotlight* spotLight = dynamic_cast<const Spotlight*>(light.get());
            std::cout << "Spotlight at (" << spotLight->getPosition().x << ", " << spotLight->getPosition().y << ", " << spotLight->getPosition().z
                      << ") with direction (" << spotLight->getDirection().x << ", " << spotLight->getDirection().y << ", " << spotLight->getDirection().z << ")";
        }
        std::cout << ", Intensity: (" << light->getIntensity().r << ", " << light->getIntensity().g << ", " << light->getIntensity().b << ")" << std::endl;
    }
}

void Scene::generateImage(int width, int height, unsigned char* buffer) {
    glm::vec3 rightVector = glm::normalize(glm::cross(forwardVector, upVector));
    glm::vec3 screenCenter = cameraPosition + distanceToScreen * forwardVector;
    // Half dimensions for centering
    float halfWidth = screenWidth / 2.0f;
    float halfHeight = screenHeight / 2.0f;
    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            // Normalized coordinates from 0 to width/heigth - 1 to -half to +half
            float u = (2.0f * (i + 0.5f) / width - 1.0f) * halfWidth;
            float v = (2.0f * (j + 0.5f) / height - 1.0f) * halfHeight;
            glm::vec3 pixelPosition = screenCenter + u * rightVector + v * upVector;
            glm::vec3 direction = glm::normalize(pixelPosition - cameraPosition);
            
            // Calculate pixel color using recursive ray tracing
            glm::vec3 pixelColor = traceRay(cameraPosition, direction, 0);
            int index = ((width - 1 - j) * width + i) * 4; // To go from down to up cord to up to down cord the image uses
            buffer[index] = static_cast<unsigned char>(pixelColor.r * 255.0f);
            buffer[index + 1] = static_cast<unsigned char>(pixelColor.g * 255.0f);
            buffer[index + 2] = static_cast<unsigned char>(pixelColor.b * 255.0f);
            buffer[index + 3] = 255;
        }
    }
}

glm::vec3 Scene::calculatePhongLighting(const IntersectionPoint& intersection, const glm::vec3& viewDirection) const {
    const Object* obj = intersection.getObject();
    glm::vec3 hitPoint = intersection.getPoint();
    glm::vec3 normal = intersection.getNormal();
    glm::vec3 K_A;
    glm::vec3 K_D;
    
    // For planes, use checkerboard pattern for both ambient and diffuse
    if (obj->getType() == PLANE) {
        const Plane* plane = dynamic_cast<const Plane*>(obj);
        K_A = plane->checkerboardColor(hitPoint); 
        K_D = plane->checkerboardColor(hitPoint); 
    } else {
        K_A = obj->getColor(hitPoint); 
        K_D = K_A; 
    }
    
    glm::vec3 K_S = glm::vec3(0.7f, 0.7f, 0.7f);
    float n = obj->getShininess(); 
    
    // Ambient component: K_A * I_A
    glm::vec3 ambient = K_A * ambientIntensity;
    glm::vec3 result = ambient;
    
    // For each light source, calculate diffuse and specular
    for (const auto& light : lights) {
        glm::vec3 L_i; 
        glm::vec3 I_i = light->getIntensity();
        bool lightContributes = true;
        
        if (light->getType() == DIRECTIONAL) {
            DirectionalLight* dirLight = dynamic_cast<DirectionalLight*>(light.get());
            L_i = -dirLight->getDirection(); // light direction points away from surface
        } else {
            // Spotlight
            Spotlight* spotLight = dynamic_cast<Spotlight*>(light.get());
            glm::vec3 lightPos = spotLight->getPosition();
            glm::vec3 toLight = lightPos - hitPoint;
            float distanceToLight = glm::length(toLight);
            
            // Check if object is behind the spotlight
            if (distanceToLight < 1e-6) {
                lightContributes = false; 
            } else {
                L_i = toLight / distanceToLight; 
                
                // Check cutoff angle
                float cosAngle = glm::dot(-L_i, spotLight->getDirection());
                if (cosAngle < spotLight->getCutoffCosine()) {
                    lightContributes = false; 
                }
            }
        }
        
        if (!lightContributes) {
            continue;
        }
        
        float S_i;
        if (isInShadow(hitPoint, normal, light.get(), obj)) {
            S_i = 0.0f;
        } else {
            S_i = 1.0f;
        }

        // flip normal if it points away from light
        glm::vec3 N = normal;
        if (obj->getType() == PLANE) {
            float NdotL_check = glm::dot(normal, L_i);
            if (NdotL_check < 0.0f) {
                N = -normal; 
            }
        }
        
        // Calculate N·L_i 
        float NdotL = glm::dot(N, L_i);
        
        // Diffuse component: K_D * (N·L_i) * I_i * S_i (only if N·L_i > 0)
        if (NdotL > 0.0f) {
            glm::vec3 diffuse = K_D * NdotL * I_i * S_i;
            result += diffuse;
            
            // Specular component: K_S * (V·R_i)^n * I_i * S_i
            // Calculate reflection vector: R_i = 2 * (N·L_i) * N - L_i
            glm::vec3 R_i = 2.0f * NdotL * N - L_i;
            R_i = glm::normalize(R_i);
            
            // Calculate V·R_i 
            float VdotR = glm::dot(viewDirection, R_i);
            
            // Only add specular if V·R_i > 0
            if (VdotR > 0.0f) {
                float specularFactor = std::pow(VdotR, n);
                glm::vec3 specular = K_S * specularFactor * I_i * S_i;
                result += specular;
            }
        }
    }
    
    result = glm::clamp(result, glm::vec3(0.0f), glm::vec3(1.0f));
    
    return result;
}

bool Scene::isInShadow(const glm::vec3& hitPoint, const glm::vec3& normal, const Light* light, const Object* hitObject) const {
    const float epsilon = 1e-6f;
    glm::vec3 shadowRayOrigin = hitPoint + epsilon * normal;

    glm::vec3 shadowRayDirection;
    float maxDistance = std::numeric_limits<float>::max();
    
    if (light->getType() == DIRECTIONAL) {
        const DirectionalLight* dirLight = dynamic_cast<const DirectionalLight*>(light);
        shadowRayDirection = -dirLight->getDirection(); 
    } else {
        // Spotlight
        const Spotlight* spotLight = dynamic_cast<const Spotlight*>(light);
        glm::vec3 lightPos = spotLight->getPosition();
        glm::vec3 toLight = lightPos - shadowRayOrigin;
        maxDistance = glm::length(toLight);
        
        if (maxDistance < 1e-6) {
            return false; 
        }
        
        shadowRayDirection = toLight / maxDistance; 
        
        
        glm::vec3 fromLightToHit = hitPoint - lightPos;
        if (glm::dot(fromLightToHit, spotLight->getDirection()) < 0) {
            // Hit point is behind the spotlight - no shadow
            return false;
        }
    }
    
    // Cast shadow ray and check for intersections
    for (const auto& obj : objects) {
        if (obj.get() == hitObject) {
            continue;
        }
        
        float t;
        if (obj->intersect(shadowRayOrigin, shadowRayDirection, t)) {
            // Use a threshold to avoid self-intersection due to nomerical errors
            if (t > 1e-6f) {
                if (light->getType() == DIRECTIONAL) {
                    return true;
                } else {
                    if (t < maxDistance - 1e-4f) {
                        const Spotlight* spotLight = dynamic_cast<const Spotlight*>(light);
                        glm::vec3 intersectionPoint = shadowRayOrigin + t * shadowRayDirection;
                        glm::vec3 fromLightToIntersection = intersectionPoint - spotLight->getPosition();
                        
                        //intersection is behind the light
                        if (glm::dot(fromLightToIntersection, spotLight->getDirection()) < 0) {
                            continue;
                        }
                        
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

glm::vec3 Scene::traceRay(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, int depth, const Object* excludeObject) const {
    const int MAX_DEPTH = 5;
    
    if (depth > MAX_DEPTH) {
        return glm::vec3(0.0f);
    }
    
    IntersectionPoint* closestIntersection = nullptr;
    float minT = std::numeric_limits<float>::max();
    
    for (const auto& obj : objects) {
        float t;
        
        // Special handling for spheres when we're inside (for refraction)
        if (obj.get() == excludeObject && obj->getType() == SPHERE) {
            // We're inside a sphere, need to find the exit point (farther intersection)
            const Sphere* sphere = dynamic_cast<const Sphere*>(obj.get());
            glm::vec3 oc = rayOrigin - sphere->getCenter();
            float a = glm::dot(rayDirection, rayDirection);
            float b = 2.0f * glm::dot(oc, rayDirection);
            float c = glm::dot(oc, oc) - sphere->getRadius() * sphere->getRadius();
            float discriminant = b * b - 4 * a * c;
            
            if (discriminant >= 0) {
                float sqrtDisc = std::sqrt(discriminant);
                float t = (-b + sqrtDisc) / (2 * a); 

                if (t > 1e-6f && t < minT) {
                    minT = t;
                    glm::vec3 hitPoint = rayOrigin + t * rayDirection;
                    glm::vec3 normal = obj->getNormal(hitPoint);
                    
                    if (closestIntersection == nullptr) {
                        closestIntersection = new IntersectionPoint(hitPoint, normal, t, obj.get());
                    } else {
                        closestIntersection->setPoint(hitPoint);
                        closestIntersection->setNormal(normal);
                        closestIntersection->setT(t);
                        closestIntersection->setObject(obj.get());
                    }
                }
            }
            continue;
        }
        
        if (obj.get() == excludeObject) {
            continue;
        }
        //t > 1e-6f due to nomerical errors
        if (obj->intersect(rayOrigin, rayDirection, t) && t > 1e-6f && t < minT) {
            minT = t;
            glm::vec3 hitPoint = rayOrigin + minT * rayDirection;
            glm::vec3 normal = obj->getNormal(hitPoint);
            
            if (closestIntersection == nullptr) {
                closestIntersection = new IntersectionPoint(hitPoint, normal, minT, obj.get());
            } else {
                closestIntersection->setPoint(hitPoint);
                closestIntersection->setNormal(normal);
                closestIntersection->setT(minT);
                closestIntersection->setObject(obj.get());
            }
        }
    }
    
    if (closestIntersection == nullptr) {
        return glm::vec3(0.0f);
    }
    
    const Object* obj = closestIntersection->getObject();
    glm::vec3 hitPoint = closestIntersection->getPoint();
    glm::vec3 normal = closestIntersection->getNormal();
    MaterialType materialType = obj->getMaterialType();
    
    glm::vec3 result;
    
    if (materialType == REFLECTIVE) {
        // Reflection: R = I - 2 * (I·N) * N
        glm::vec3 N = normal;
        
        if (obj->getType() == PLANE) {
            float IdotN_check = glm::dot(rayDirection, normal);
            if (IdotN_check > 0.0f) {
                N = -normal;
            }
        }
        
        float IdotN = glm::dot(rayDirection, N);
        glm::vec3 reflectedDirection = rayDirection - 2.0f * IdotN * N;
        reflectedDirection = glm::normalize(reflectedDirection);
        
        // Small offset along normal to avoid self-intersection
        const float epsilon = 1e-4f;
        glm::vec3 newOrigin = hitPoint + epsilon * N;
        
        result = traceRay(newOrigin, reflectedDirection, depth + 1);
        
    } else if (materialType == TRANSPARENT) {
        if (obj->getType() != SPHERE) {
            result = glm::vec3(0.0f);
        } else {
            //  air = 1.0, sphere = 1.5
            bool insideSphere = (excludeObject == obj);
            // Current medium
            float n1;
            if (insideSphere) {
                n1 = 1.5f;
            } else {
                n1 = 1.0f;
            }
            // Next medium
            float n2;
            if (insideSphere) {
                n2 = 1.0f;
            } else {
                n2 = 1.5f;
            }
            // Normal points outward from sphere center
            // When inside, normal should point toward center, so flip it
            glm::vec3 N;
            if (insideSphere) {
                N = -normal;
            } else {
                N = normal;
            }
            // Snell's law
            float IdotN = glm::dot(rayDirection, N);
            float n = n1 / n2;
            float cosI = -IdotN;
            float sinT2 = n * n * (1.0f - cosI * cosI);
            
            if (sinT2 > 1.0f) {
                glm::vec3 reflectedDirection = rayDirection - 2.0f * IdotN * N;
                reflectedDirection = glm::normalize(reflectedDirection);
                const float epsilon = 1e-4f;
                glm::vec3 newOrigin = hitPoint + epsilon * N;
                result = traceRay(newOrigin, reflectedDirection, depth + 1, obj);
            } else {
                // Refraction
                float cosT = std::sqrt(1.0f - sinT2);
                glm::vec3 refractedDirection = n * rayDirection + (n * cosI - cosT) * N;
                refractedDirection = glm::normalize(refractedDirection);
                
                const float epsilon = 1e-4f;
                glm::vec3 newOrigin = hitPoint - epsilon * N; 
                
                // Determine if we're now inside or outside the sphere
                const Object* newExclude;
                if (insideSphere) {
                    newExclude = nullptr;
                } else {
                    newExclude = obj;
                }

                result = traceRay(newOrigin, refractedDirection, depth + 1, newExclude);
            }
        }
        
    } else {
        // NORMAL - Phong lighting
        glm::vec3 viewDirection = glm::normalize(rayOrigin - hitPoint);
        result = calculatePhongLighting(*closestIntersection, viewDirection);
    }
    
    delete closestIntersection;
    return result;
}

// Getters
glm::vec3 Scene::getCameraPosition() const { return cameraPosition; }
float Scene::getDistanceToScreen() const { return distanceToScreen; }
glm::vec3 Scene::getUpVector() const { return upVector; }
float Scene::getScreenHeight() const { return screenHeight; }
float Scene::getScreenWidth() const { return screenWidth; }
glm::vec3 Scene::getForwardVector() const { return forwardVector; }
glm::vec3 Scene::getAmbientIntensity() const { return ambientIntensity; }
const std::vector<std::unique_ptr<Object>>& Scene::getObjects() const { return objects; }
const std::vector<std::unique_ptr<Light>>& Scene::getLights() const { return lights; }
