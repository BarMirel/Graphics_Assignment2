#include "Scene.h"
#include "Sphere.h"
#include "Plane.h"
#include "DirectionalLight.h"
#include "Spotlight.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

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
    } else if (token == "f") { // forward vector
        iss >> forwardVector.x >> forwardVector.y >> forwardVector.z >> screenWidth;
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
            objects.push_back(std::make_unique<Sphere>(glm::vec3(x, y, z), w, materialType, glm::vec3(0.0f)));
        } else { // plane: w is d coefficient
            objects.push_back(std::make_unique<Plane>(x, y, z, w, materialType, glm::vec3(0.0f)));
        }
    } else if (token == "c") { // color
        glm::vec3 color;
        iss >> color.r >> color.g >> color.b;
        float dummy;
        iss >> dummy; // 4th coordinate (ignored)
        objectColors.push_back(color);
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
            colorIndex++;
        }
    }
}

void Scene::createLights() {
    size_t dirIndex = 0;
    size_t posIndex = 0;
    size_t intIndex = 0;

    while (intIndex < lightIntensities.size()) {
        if (dirIndex < lightDirections.size()) {
            // Check if this is a spotlight (has position)
            if (posIndex < lightPositions.size()) {
                lights.push_back(std::make_unique<Spotlight>(
                    lightPositions[posIndex],
                    lightDirections[dirIndex],
                    lightPositions[posIndex].z, // Using z as cutoff cosine for now
                    lightIntensities[intIndex]
                ));
                posIndex++;
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
        std::cout << ", Color: (" << obj->getColor().r << ", " << obj->getColor().g << ", " << obj->getColor().b << ")" << std::endl;
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
