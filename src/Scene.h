#ifndef SCENE_H
#define SCENE_H

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <string>

class Ray;
class Object;
class Light;

class Scene {
private:
    // Camera parameters
    glm::vec3 cameraPosition;
    float distanceToScreen;
    glm::vec3 upVector;
    float screenHeight;
    float screenWidth;
    glm::vec3 forwardVector;

    // Ambient lighting
    glm::vec3 ambientIntensity;

    // Objects and lights storage
    std::vector<std::unique_ptr<Object>> objects;
    std::vector<std::unique_ptr<Light>> lights;

    // Temporary storage for parsing
    std::vector<glm::vec3> objectColors;
    std::vector<float> objectShininess;
    std::vector<glm::vec3> lightDirections;
    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightIntensities;

    // Private helper methods
    bool parseLine(const std::string& line);
    void createObjects();
    void createLights();

public:
    Scene();
    ~Scene();

    // Disable copy operations to prevent accidental copying of unique_ptrs
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    // Enable move operations
    Scene(Scene&&) = default;
    Scene& operator=(Scene&&) = default;

    // File loading
    bool loadFromFile(const std::string& filename);

    // Scene information
    void printSceneInfo() const;

    // Image generation
    void generateImage(int width, int height, unsigned char* buffer);

    // Getters
    glm::vec3 getCameraPosition() const;
    float getDistanceToScreen() const;
    glm::vec3 getUpVector() const;
    float getScreenHeight() const;
    float getScreenWidth() const;
    glm::vec3 getForwardVector() const;
    glm::vec3 getAmbientIntensity() const;
    const std::vector<std::unique_ptr<Object>>& getObjects() const;
    const std::vector<std::unique_ptr<Light>>& getLights() const;
};

#endif // SCENE_H
