#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <thread>
#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "renderer.hpp"
#include "map.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void update_light(glm::vec3 &lightPos, GLFWwindow *window, float dt);
void run_aStar();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
Camera camera(glm::vec3(float(WIDTH) / 2, 25.0f, float(LENGTH) / 2));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

std::vector<glm::vec3> lightPos{glm::vec3(WIDTH / 2, 3.0f, LENGTH), glm::vec3(0)};

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr)
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
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    Shader colorShader("res/shaders/shader.vert", "res/shaders/shader_color.frag");
    Shader textureShader("res/shaders/shader.vert", "res/shaders/shader_texture.frag");
    Shader lightCubeShader("res/shaders/light_cube.vert", "res/shaders/light_cube.frag");

    model_t wall_cube = generate_cube({0.5f, 0.5f, 0.5f},
                                      true,
                                      {"res/textures/container2.png",
                                  "res/textures/container2_specular.png"});
    model_t unvisited_cube = generate_cube({0.49f, 0.49f, 0.49f},
                                           false,
                                           {},
                                           glm::vec4(0.8, 0.75, 0.5, 1));
    model_t visited_cube = generate_cube({0.49f, 0.49f, 0.49f},
                                      false,
                                      {},
                                      glm::vec4(1, 0.31, 0, 1));
    model_t road_cube = generate_cube({0.49f, 0.49f, 0.49f},
                                         false,
                                         {},
                                         glm::vec4(0.3, 0.3, 0.3, 1));
    model_t start_cube = generate_cube({0.49f, 0.49f, 0.49f},
                                      false,
                                      {},
                                      glm::vec4(0, 0, 0, 1));
    model_t end_cube = generate_cube({0.49f, 0.49f, 0.49f},
                                      false,
                                      {},
                                      glm::vec4(0.8, 0, 0, 1));
    model_t light_sphere = generate_sphere({32, 32},
                                           glm::vec4(1, 1, 1, 1));
    model_t light_cube = generate_cube({0.49f, 0.49f, 0.49f},
                                         false,
                                         {},
                                         glm::vec4(0,0.67,0.41, 1));
    model_t bicycle = load_obj("res/obj/Bicycle/10489_bicycle_L2.obj");

    renderer_t renderer = make_renderer(glm::perspective(glm::radians(camera.Zoom), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 100.0f));

    // 开启A*寻路算法线程
    std::thread th(run_aStar);
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            camera.Position = glm::vec3{0, 20, 0};
            camera.Yaw = -89.0f;
            camera.Pitch = -89.0f;
            camera.Mode = FLY_MODE;
        }
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
            auto start = instance.getStart();
            camera.Position = glm::vec3{start.first - 1, 1, start.second};
            camera.Yaw = 2.29f;
            camera.Pitch = -3.39f;
            camera.Mode = FPS_MODE;
        }

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // 观察矩阵
        glm::mat4 view = camera.GetViewMatrix();
        // 投影矩阵
        renderer.projection = glm::perspective(glm::radians(camera.Zoom), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 100.0f);
        // 模型矩阵
        glm::mat4 model = glm::mat4(1.0f);


        for (size_t i = 0; i < LENGTH; ++i) {
            for (size_t j = 0; j < WIDTH; ++j) {
                // calculate the model matrix for each object and pass it to shader before drawing
                model = glm::mat4(1.0f);
                glm::vec3 cubePositions{i, 0, j};
                model = glm::translate(model, cubePositions);
                switch (instance.data()[i][j]->type) {
                    case Map::ROAD_UNVISITED: {
                        draw_cube_color(renderer, colorShader, model, view, lightPos, camera.Position, unvisited_cube);
                        break;
                    }
                    case Map::ROAD_VISITED: {
                        draw_cube_color(renderer, colorShader, model, view, lightPos, camera.Position, visited_cube);
                        break;
                    }
                    case Map::ACTUAL_ROAD: {
                        draw_cube_color(renderer, colorShader, model, view, lightPos, camera.Position, road_cube);
                        break;
                    }
                    case Map::START: {
                        glm::mat4 tmp = model;
                        tmp = glm::translate(tmp, glm::vec3(0, 1.0f, 0));
                        lightPos[1] = cubePositions + glm::vec3(0, 1.0f, 0);
                        tmp = glm::scale(tmp, glm::vec3(0.4f));
                        draw_cube_color(renderer, colorShader, model, view, lightPos, camera.Position, start_cube);
                        draw_light(renderer, lightCubeShader, tmp, view, light_cube);
                        break;
                    }
                    case Map::END: {
                        draw_cube_color(renderer, colorShader, model, view, lightPos, camera.Position, end_cube);
                        glm::mat4 tmp = model;
                        tmp = glm::translate(tmp, glm::vec3(0, 0.49f, 0));
                        tmp = glm::rotate(tmp, glm::radians(-90.0f), glm::vec3(1, 0, 0));
                        tmp = glm::rotate(tmp, glm::radians(45.0f), glm::vec3(0, 0, 1));
                        tmp = glm::scale(tmp, glm::vec3(0.005f));
                        draw_obj(renderer, textureShader, tmp, view, lightPos, camera.Position, bicycle);
                        break;
                    }
                    case Map::WALL: {
                        model = glm::translate(model, glm::vec3(0, 1.0f, 0));
                        draw_cube_texture(renderer, textureShader, model, view, lightPos, camera.Position, wall_cube);
                    }
                }
            }
        }

        lightCubeShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos[0]);
        draw_light(renderer, lightCubeShader, model, view, light_sphere);
        update_light(lightPos[0], window, deltaTime);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    destroy_obj(start_cube);
    destroy_obj(end_cube);
    destroy_obj(light_cube);
    destroy_obj(wall_cube);
    destroy_obj(visited_cube);
    destroy_obj(unvisited_cube);
    destroy_obj(light_sphere);
    destroy_obj(bicycle);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    // 等待寻路算法线程结束
    th.join();
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

void update_light(glm::vec3 &lightPos, GLFWwindow *window, float dt) {
    using namespace glm;

    glm::vec3 &pos = lightPos;

    // in degrees
    static float INCREMENT = 25;
    static float curr_angle = 90;

    float change = 0;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        change = INCREMENT * dt;
    } else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        change = -INCREMENT * dt;
    }

    curr_angle += change;

    pos = rotate(identity<mat4>(), radians(change), { 0, 1, 0 }) * vec4(pos, 1);
}

void run_aStar() {
    Map& instance = Map::getInstance();
    instance.solve_AStar();
}