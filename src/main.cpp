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

int main(int argc, char* argv[])
{
    Scene scene;

    std::cout << "Loading scene1.txt..." << std::endl;
    if (scene.loadFromFile("../scene1.txt")) {
        int width = 1000;
        int heigth = 1000;
        int req_comp = 4;
        std::cout << "Scene loaded successfully!" << std::endl;
        scene.printSceneInfo();
        unsigned char* buffer = new unsigned char[width * heigth * req_comp];
        scene.generateImage(1000, 1000, buffer);
        int result = stbi_write_png("res/textures/Test1.png", width, heigth, req_comp, buffer, width*req_comp);
        std::cout << "Test1 result: " << result << std::endl;
        delete[] buffer;
    }
    return 0;
}