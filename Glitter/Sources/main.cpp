#include "glitter.hpp"

// Local includes
#include "camera.hpp"
#include "shader.hpp"
#include "sphere.hpp"
#include "utils.h"
#include "scenes.hpp"

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);

// screen size
int windowHeight = 800;
int windowWidth = 1400;

// frametime
double deltaTime = 0.0f;
double lastFrameTime = 0.0f;

// last mouse pointer position
double lastX;
double lastY;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 6.0f);
Camera* cam = new Camera(cameraPos, 50.0f, windowWidth, windowHeight);

int main(int argc, char* argv[]) {
    // GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 2);

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, 
		"Cool B-Splines | fps : 0",nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // on init glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, windowWidth, windowHeight);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    // ImGui Setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // import shaders
    Shader lightShader{"lamp.vert", "lamp.frag"};         // point light shader
    Shader geometryShader{"colors.vert", "colors.frag"};  // sphere shader
    // coordinate of lights in scene
    glm::vec3 pointLightPositions[] = {glm::vec3(0.0f, 3.0f, 0.0f),
                                       glm::vec3(6.0f, 3.0f, 6.0f),
                                       glm::vec3(2.5f, 4.0f, 2.5f)};

    // draw point light geometry
    Sphere sph{};
    sph.genIcoSphere(1.0, 2);
    std::vector<float> sphNormals = sph.getNormals();
    std::vector<float> sphVerts = sph.getVerts();
    std::vector<int> sphIndices = sph.getIndices();

    // view matrix "camera space"
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    // translation in the reverse direction
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -15.0f));

    // clip space matrix
    glm::mat4 projection = glm::perspective(glm::radians(cam->getFov()),
        (float)windowWidth / windowHeight, 0.1f, 100.0f);

    int framecount = 0;
    double fpstime = glfwGetTime();
    // create scene to display
    static int selectedItem = 1;
    static const char* items[] = {"B-Spline Sphères","B-Surface Sphères","B-Surface Sphères"};
    
    BSplineSphere scene_bspS{geometryShader};
    BSplinePolyLine scene_bspP{geometryShader};
    BSurfaceSphere scene_bsuS{geometryShader};
    BSurfaceQuads scene_bsuQ{geometryShader};
    
    Scene* activeScene = &scene_bsuS;

    // principal display loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.2f, 0.2f, 0.2f, 0.2f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Utils::updateFpsCounter(window, framecount, fpstime, deltaTime, lastFrameTime);

        // fov update
        projection = glm::perspective(glm::radians(cam->getFov()),(float)windowWidth / windowHeight, 0.1f, 100.0f);
        // camera update
        view = cam->getView();

        geometryShader.use();
        geometryShader.setMat4("model", model);
        // shading/lights parameters
        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 lightDiffuseColor = lightColor * glm::vec3(5.0f);
        glm::vec3 lightAmbientColor = lightDiffuseColor * glm::vec3(0.3f);
        geometryShader.setVec3("material.diffuse", glm::vec3(0.65f, 0.15f, 0.0f));
        geometryShader.setFloat("material.specularStrength", 1.2f);
        geometryShader.setFloat("material.shininess", 64.0f);
        using namespace std;
        for (int i = 0; i < sizeof(pointLightPositions) / sizeof(glm::vec3);
             i++) {
            geometryShader.setVec3("pointLights[" + to_string(i) + "].position",pointLightPositions[i]);
            geometryShader.setVec3("pointLights[" + to_string(i) + "].ambient",lightAmbientColor);
            geometryShader.setVec3("pointLights[" + to_string(i) + "].diffuse",lightDiffuseColor);
            geometryShader.setVec3("pointLights[" + to_string(i) + "].specular",glm::vec3(1.0f, 1.0f, 1.0f));
            geometryShader.setFloat("pointLights[" + to_string(i) + "].constant",1.0f);
            geometryShader.setFloat("pointLights[" + to_string(i) + "].linear",0.22f);
            geometryShader.setFloat( "pointLights[" + to_string(i) + "].quadratic",0.2f);
        }
        geometryShader.setVec3("viewPos", cam->getPos());
        geometryShader.setMat4("projection", projection);
        geometryShader.setMat4("view", view);

        //draw  our bspline/bsurface
        activeScene->drawGeo();

        // render geometry for point lights
        lightShader.use();
        lightShader.setMat4("model", model);

        for (glm::vec3 lightpos : pointLightPositions) {
            glm::mat4 geometryModel = glm::mat4(1.0f);
            geometryModel = glm::translate(geometryModel, lightpos);
            geometryModel = glm::scale(geometryModel, glm::vec3(0.1f));
            lightShader.setVec3("lightColor", glm::vec3(1,1,1));
            lightShader.setMat4("model", geometryModel);
            glDrawArrays(GL_TRIANGLES, 0, sphIndices.size());
        }
        // imgui part
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // spline param tab
        activeScene->drawUI();
        //second tab
        ImGui::Begin("General Parameters");
        ImGui::Text("\nContrôles :\n\nW/S = Avancer/Reculer (ou flèches directionelles)\nA/D = Gauche/Droite"
        "\nCtrl/Space = Haut/Bas\nU/I = Active/Desactive UI\n");
        ImGui::Text("Selectionnez une scène à afficher:\n");
        int prevItem = selectedItem;
        ImGui::Combo("scn",&selectedItem, items, IM_ARRAYSIZE(items));
        if (prevItem != selectedItem){
            switch(selectedItem){
                case 0:
                    activeScene = &scene_bspS;
                    break;
                case 1:
                    activeScene = &scene_bsuS;
                    break;
                
                case 2:
                    activeScene = &scene_bsuQ;
                    break;
                //case 3:
                //    activeScene = &scene_bsuQ;
                //    break;
                default:
                    break;
            }
        }
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    
    } // end of render loop
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    delete cam;
    // glfw cleanup
    glfwTerminate();
    return 0;
}








// support function for render size update
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    const float sensitivity = 0.07f;
    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;
    float yaw = cam->offsetYaw(xoffset, sensitivity);
    float pitch = cam->offsetPitch(yoffset, sensitivity);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    cam->offsetFov(yoffset);
}

// manage inputs
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    // keys 1,2,3 to switch polygon display mode
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

    // camera movement
    const float cameraSpeed = 2.0f * deltaTime;  // framerate independent

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cam->moveZ(cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cam->moveZ(-cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cam->moveX(-cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cam->moveX(cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) cam->moveZ(cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) cam->moveZ(-cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) cam->moveX(-cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) cam->moveX(cameraSpeed);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cam->moveY(cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        cam->moveY(-cameraSpeed);

    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPosCallback(window, nullptr);
        glfwSetScrollCallback(window, nullptr);
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);
    }
}