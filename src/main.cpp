#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "renderer.hpp"
#include "map.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


int main()
{
    Map& instance = Map::getInstance();
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    Shader ourShader("res/shaders/shader.vert", "res/shaders/shader.frag");
    Shader lightCubeShader("res/shaders/light_cube.vert", "res/shaders/light_cube.frag");
    // 顶点数据
    float vertices[] = {
            -0.5f, -0.5f, -0.5f,  0.75f,0.75f, 0.75f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  0.75f,0.75f, 0.75f,1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  0.75f,0.75f, 0.75f,1.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  0.75f,0.75f, 0.75f,1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.75f,0.75f, 0.75f,0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.75f,0.75f, 0.75f,0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.75f,0.75f, 0.75f,0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  0.75f,0.75f, 0.75f,1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  0.75f,0.75f, 0.75f,1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  0.75f,0.75f, 0.75f,1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.75f,0.75f, 0.75f,0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.75f,0.75f, 0.75f,0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f,  0.75f,0.75f, 0.75f,1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.75f,0.75f, 0.75f,1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.75f,0.75f, 0.75f,0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.75f,0.75f, 0.75f,0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.75f,0.75f, 0.75f,0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.75f,0.75f, 0.75f,1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  0.75f,0.75f, 0.75f,1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  0.75f,0.75f, 0.75f,1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.75f,0.75f, 0.75f,0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.75f,0.75f, 0.75f,0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.75f,0.75f, 0.75f,0.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  0.75f,0.75f, 0.75f,1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.75f,0.75f, 0.75f,0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.75f,0.75f, 0.75f,1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.75f,0.75f, 0.75f,1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  0.75f,0.75f, 0.75f,1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.75f,0.75f, 0.75f,0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.75f,0.75f, 0.75f,0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.75f,0.75f, 0.75f,0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  0.75f,0.75f, 0.75f,1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  0.75f,0.75f, 0.75f,1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  0.75f,0.75f, 0.75f,1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.75f,0.75f, 0.75f,0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.75f,0.0f, 0.75f,0.0f, 1.0f
    };
//    // 使用索引缓冲避免顶点浪费
//    unsigned int indices[] = { // 注意索引从0开始!
//            0, 1, 3, // first triangle
//            1, 2, 3  // second triangle
//    };

    glm::vec3 cubePositions[] = {
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3( 2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3( 2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3( 1.3f, -2.0f, -2.5f),
            glm::vec3( 1.5f,  2.0f, -2.5f),
            glm::vec3( 1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
    };

//    GLuint VBO, VAO;
//    GLuint EBO;
//    // 生成VAO, VBO
//    glGenVertexArrays(1, &VAO);
//    glGenBuffers(1, &VBO);
//    glGenBuffers(1, &EBO);
//    // 绑定VAO
//    glBindVertexArray(VAO);
//    // 把顶点数组复制到缓冲中供OpenGL使用
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//    // 复制我们的索引数组到一个索引缓冲中，供OpenGL使用
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//    // 设置顶点属性指针
//    // 第一个0 是我们希望把数据传递到这一个顶点属性中，所以这里我们传入0(shader.vert中的aPos)
//    // position attribute
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(0);
//    // color attribute
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
//    glEnableVertexAttribArray(1);
//    // texture coord attribute
//    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
//    glEnableVertexAttribArray(2);
//
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindVertexArray(0);

    model_t cube = generate_cube(1, 0.5, 3, false, "res/textures/awesomeface.png");
    model_t light_cube = generate_cube(1, 1, 1, false, "");

//    GLuint texture1;
//    // 对texture 1
//    glGenTextures(1, &texture1);
//    glBindTexture(GL_TEXTURE_2D, texture1);
//    // set the textures wrapping parameters
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set textures wrapping to GL_REPEAT (default wrapping method)
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    // load image, create textures and generate mipmaps
//    int width, height, nrChannels;
//    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
//    unsigned char *data = stbi_load("res/textures/wall.jpg", &width, &height, &nrChannels, 0);
//    if (data)
//    {
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//        glGenerateMipmap(GL_TEXTURE_2D);
//    }
//    else
//    {
//        std::cerr << "Failed to load textures" << std::endl;
//    }
//    stbi_image_free(data);
//    ourShader.use(); // don't forget to activate/use the shader before setting uniforms!
//    // either set it manually like so:
//    ourShader.setInt("texture1", 0);
    renderer_t renderer = make_renderer(glm::perspective(glm::radians(camera.Zoom), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 100.0f));
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
//        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();
        // 观察矩阵
        glm::mat4 view = camera.GetViewMatrix();
        // 投影矩阵
        renderer.projection = glm::perspective(glm::radians(camera.Zoom), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 100.0f);
        // 模型矩阵
        glm::mat4 model = glm::mat4(1.0f);
        draw_cube_normal(renderer, ourShader, model, view, lightPos, cube);
        lightCubeShader.use(); // 用了别的shader别忘了改use
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        draw_cube_light(renderer, lightCubeShader, model, view, light_cube);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    destroy_obj(cube);
//    glDeleteVertexArrays(1, &VAO);
//    glDeleteBuffers(1, &VBO);
//    glDeleteBuffers(1, &EBO);
//    glGenTextures(1, &texture1);
//    glGenTextures(1, &texture2);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}