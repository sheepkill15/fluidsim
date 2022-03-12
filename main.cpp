#include <iostream>
#include <fstream>
#include <streambuf>
#include <string>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "components/constants.h"
#include "components/window.h"
#include "components/texture.h"
#include "components/renderer.h"
#include "components/ubo.h"
#include "components/ssbo.h"

#include "components/compute_shader.h"
#include "components/vert_frag_shader.h"

using namespace std;

#define M_PI 3.14f

struct Cell
{
    int data = 0;
};


Window simWindow;
Texture texture;

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    // simWindow.on_resize({(unsigned int)width, (unsigned int)height});

    // texture.resize(width, height);

    // glBufferData(GL_UNIFORM_BUFFER, 8, &simWindow.get_size(), GL_STATIC_DRAW);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    simWindow.create();
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }
    simWindow.on_resize(simWindow.get_size());
    glfwSetFramebufferSizeCallback(simWindow.get_glfwwindow(), framebuffer_size_callback);

    VertFragShader vf("shaders/vertex.vert", "shaders/frag.frag");
    glUseProgram(vf.program);

    Renderer::init();
    
    Ubo uboWindowSize(8, (void*)&simWindow.get_size());
    uboWindowSize.bind(2);

    ComputeShader cellShader("shaders/compute.comp");
    cellShader.bind_ubo("WindowSize", 2);

    ComputeShader copyShader("shaders/copy.comp");

    double previousTime = glfwGetTime();
    GLuint deltaTimeLocation = glGetUniformLocation(cellShader.program, "deltaTime"); 
    
    texture.create(100, 100, 0);
    texture.bind_as_image();

    Texture copyTexture;
    copyTexture.create(100, 100, 3);
    copyTexture.bind_as_image();
    int width, height, channels;
    unsigned char* img = stbi_load("res/init.png", &width, &height, &channels, STBI_rgb_alpha);
    if(img == nullptr) {
        cout << "ERROR loading image" << endl;
        return -1;
    }
    copyTexture.load(img);
    texture.load(img);

    stbi_image_free(img);


    double sum = 0;
    while (simWindow.running())
    {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - previousTime;
        previousTime = currentTime;
        sum += deltaTime;
        if(sum > 0.5)
        {
            copyShader.dispatch(100, 100, 1);

            cellShader.set_uniform(deltaTimeLocation, deltaTime);
            cellShader.dispatch(100, 100, 1);
            sum = 0;
        }
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(vf.program);
        Renderer::render();
        glfwSwapBuffers(simWindow.get_glfwwindow());
        glfwPollEvents();
        cout << 1 / deltaTime << '\r';
    }
    cout << endl;

    glfwTerminate();

    return 0;
}
