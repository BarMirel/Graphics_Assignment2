#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb/stb_image_write.h>

#include <Debugger.h>
#include <VertexBuffer.h>
#include <VertexBufferLayout.h>
#include <IndexBuffer.h>
#include <VertexArray.h>
#include <Shader.h>
#include <Texture.h>
#include <Camera.h>
#include <Scene.h>

#include <iostream>
#include <vector>
#include <string>

int main(int argc, char* argv[])
{
    std::vector<std::string> sceneFiles = {
        "scene1.txt",
        "scene2.txt",
        "scene21.txt",
        "scene3.txt",
        "scene4.txt",
        "scene41.txt",
        "scene5.txt",
        "scene51.txt",
        "scene6.txt"
    };
    
    bool debug = false;
    int width = 1000;
    int height = 1000;
    int req_comp = 4;
    
    for (const auto& sceneFile : sceneFiles) {
        Scene scene;
        if (scene.loadFromFile(sceneFile)) {
            if(debug) scene.printSceneInfo();
            
            unsigned char* buffer = new unsigned char[width * height * req_comp];
            scene.generateImage(width, height, buffer);
            
            std::string outputName = sceneFile;
            size_t dotPos = outputName.find_last_of('.');
            if (dotPos != std::string::npos) {
                outputName = outputName.substr(0, dotPos);
            }
            
            std::string outputPath = "bin/res/textures/" + outputName + ".png";
            stbi_write_png(outputPath.c_str(), width, height, req_comp, buffer, width * req_comp);
            delete[] buffer;
        } else {
            std::cout << "Failed to load " << sceneFile << std::endl;
        }
    }
     std::cout << "All scenes processed" << std::endl;
    
    return 0;
}