#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include "shader.h"
#include "stb_image.h"
#include "chanCamera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(std::vector<std::string> faces);
void glfwSetup();
void glfwConfigureWindow(GLFWwindow* window);
bool gladSetup();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
//Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));//FlyCamera
chanCamera camera(glm::vec3(0.0f, 0.0f, 3.0f));//chanCamera
//Camera camera(1.0f, SCR_WIDTH, SCR_HEIGHT, 0.1f, 100.0f, glm::vec3(0.0f, 0.0f, 3.0f));//FreeCamera
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//DeltaTime
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

//light source
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

glm::vec3 texturedPos(-1.2f, 1.0f, 2.0f);

int main(){
    glfwSetup();

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if(window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwConfigureWindow(window);
    if(!gladSetup()) return -1;

    // build and compile our shader program
    Shader ourShader("nReflectiveShader.vs", "nReflectiveShader.fs");
    Shader reflectShader("reflectiveShader.vs", "reflectiveShader.fs");
    Shader flatShader("reflective2.vs", "reflective2.fs");
    Shader lightCubeShader("lightCube.vs", "lightCube.fs");
    Shader skyboxShader("skybox.vs", "skybox.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    float planeVertices[] = {//plane
        //positions           //colors             //texture coords    //normals
        -20.5f, -3.5f, -20.5f,   1.0f, 0.0f, 0.0f,    0.0f, 1.0f,     0.0f, -1.0f,  0.0f,// bottom left
         20.5f, -3.5f, -20.5f,   0.0f, 1.0f, 0.0f,    1.0f, 1.0f,     0.0f, -1.0f,  0.0f,// top left 
         20.5f, -3.5f,  20.5f,   0.0f, 0.0f, 1.0f,    1.0f, 0.0f,     0.0f, -1.0f,  0.0f,// top right
         20.5f, -3.5f,  20.5f,   1.0f, 1.0f, 0.0f,    1.0f, 0.0f,     0.0f, -1.0f,  0.0f,
        -20.5f, -3.5f,  20.5f,   0.0f, 1.0f, 1.0f,    0.0f, 0.0f,     0.0f, -1.0f,  0.0f,// bottom right
        -20.5f, -3.5f, -20.5f,   1.0f, 0.0f, 1.0f,    0.0f, 1.0f,     0.0f, -1.0f,  0.0f
    };
    float cubeVertices[] = {//cube
        //positions           //colors             //texture coords //normals
        -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,    0.0f, 0.0f,     0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,    1.0f, 0.0f,     0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f,    1.0f, 1.0f,     0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f,    1.0f, 1.0f,     0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 1.0f,    0.0f, 1.0f,     0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 1.0f,    0.0f, 0.0f,     0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,    0.0f, 0.0f,     0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 0.0f,    1.0f, 0.0f,     0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,    1.0f, 1.0f,     0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 0.0f,    1.0f, 1.0f,     0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 1.0f,    0.0f, 1.0f,     0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 1.0f,    0.0f, 0.0f,     0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,    1.0f, 0.0f,    -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,    1.0f, 1.0f,    -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,    0.0f, 1.0f,    -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f,    0.0f, 1.0f,    -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 1.0f,    0.0f, 0.0f,    -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 1.0f,    1.0f, 0.0f,    -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,    1.0f, 0.0f,     1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,    1.0f, 1.0f,     1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,    0.0f, 1.0f,     1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f,    0.0f, 1.0f,     1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 1.0f,    0.0f, 0.0f,     1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 1.0f,    1.0f, 0.0f,     1.0f,  0.0f,  0.0f,
         //bottom
        -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,    0.0f, 1.0f,     0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,    1.0f, 1.0f,     0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,    1.0f, 0.0f,     0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 0.0f,    1.0f, 0.0f,     0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 1.0f,    0.0f, 0.0f,     0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 1.0f,    0.0f, 1.0f,     0.0f, -1.0f,  0.0f,
        //top
        -0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,    0.0f, 1.0f,     0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,    1.0f, 1.0f,     0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,    1.0f, 0.0f,     0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 0.0f,    1.0f, 0.0f,     0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 1.0f,    0.0f, 0.0f,     0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 1.0f,    0.0f, 1.0f,     0.0f,  1.0f,  0.0f
    };
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    glm::vec3 cubePositions[] = {// world space positions of our cubes
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    unsigned int cubeVBO, VAO;
    glGenVertexArrays(1, &VAO);//vertex array object
    glGenBuffers(1, &cubeVBO);//vertex buffer object
    //glGenBuffers(1, &EBO);//element buffer object - allows vertex overlap (without it you have to define overlapping vertices seperately)
    
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);//copy user defined data to bound buffer

    glBindVertexArray(VAO);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);// position attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);// color attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);// texture attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);// normal attribute

    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    std::vector<std::string> faces{
        "textures/skybox/right.jpg",
        "textures/skybox/left.jpg",
        "textures/skybox/top.jpg",
        "textures/skybox/bottom.jpg",
        "textures/skybox/front.jpg",
        "textures/skybox/back.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);


    // configure light's VAO (VBO stays same, vertices are the same for the light object)
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    //belongs to the white cube (lamp)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int texturedVAO;
    glGenVertexArrays(1, &texturedVAO);
    glBindVertexArray(texturedVAO);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // normal attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //load textures
    unsigned int texture1 = loadTexture("textures/container.jpg");
    unsigned int texture2 = loadTexture("textures/awesomeface.png");
    unsigned int texture3 = loadTexture("textures/matrix.jpg");

    ourShader.use(); // don't forget to activate the shader before setting uniforms!  
    //glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0); // set it manually
    ourShader.setInt("texture1", 0);// or with shader class
    ourShader.setInt("texture2", 1);

    reflectShader.use();
    reflectShader.setInt("material.diffuse", 0);
    reflectShader.setInt("material.specular", 1);
    reflectShader.setInt("material.emission", 2);

    //render the triangle - can be outside as long as it doesn't exceed texture limit
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);// bind diffuse map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);// bind specular map
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture3);// bind emission map
    
    // world,        camera/view, screen projection transformations
    glm::mat4 model, view, projection, transform;
    glm::vec3 lightColor, diffuseColor, ambientColor;
    float currentFrame;

    glfwSetCursorPos(window, lastX, lastY);//avoids cursor jump at program start
    // render loop
    while(!glfwWindowShouldClose(window)){
        processInput(window);// input

        //per-frame time calculation
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);// background color

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//clear color and depth buffer
        
        lightColor.x = sin(glfwGetTime() * 2.0f);
        lightColor.y = sin(glfwGetTime() * 0.7f);
        lightColor.z = sin(glfwGetTime() * 1.3f);

        diffuseColor = lightColor * glm::vec3(0.5f);
        ambientColor = diffuseColor * glm::vec3(0.2f);

        reflectShader.use();
        reflectShader.setVec3("viewPos", camera.Position);
        reflectShader.setFloat("material.shininess", 32.0f);
        // directional light
        reflectShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        reflectShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        reflectShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        reflectShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        reflectShader.setVec3("pointLights[0].position", lightPos);
        reflectShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        reflectShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        reflectShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        reflectShader.setFloat("pointLights[0].constant", 1.0f);
        reflectShader.setFloat("pointLights[0].linear", 0.09f);
        reflectShader.setFloat("pointLights[0].quadratic", 0.032f);
        // spotLight
        reflectShader.setVec3("spotLight.position", camera.Position);
        reflectShader.setVec3("spotLight.direction", camera.Front);
        reflectShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        reflectShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        reflectShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        reflectShader.setFloat("spotLight.constant", 1.0f);
        reflectShader.setFloat("spotLight.linear", 0.09f);
        reflectShader.setFloat("spotLight.quadratic", 0.032f);
        reflectShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        reflectShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        // projection transformation
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);
        reflectShader.setMat4("projection", projection);

        // camera/view transformation
        view = camera.GetViewMatrix();
        reflectShader.setMat4("view", view);

        // create transformations - make sure to initialize matrix to identity matrix first
        transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, -0.5f, 0.0f));
        transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

        glBindVertexArray(VAO);//render cubes
        for (unsigned int i = 0; i < 10; i++){// calculate model matrix for each object, pass it to shader
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            if (i % 3 == 0)  //every 3rd cube, we set the angle using time function
                angle = glfwGetTime() * 25.0f;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            reflectShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);//when doing a cube, only rendered a triangle

        // also draw the lamp object
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lightCubeShader.setMat4("model", model);

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //textured cube
        ourShader.use();//activate shader
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, texturedPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        ourShader.setMat4("model", model);

        glBindVertexArray(texturedVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //de-allocate resources:
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &skyboxVBO);
    //glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window){
    float cameraSpeed = static_cast<float>(3.5 * deltaTime);//changes dynamically according to fps
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){//whenever the window size changed (by OS or user resize)
    // make sure viewport matches new window dimensions; note that width-height
    //  will be significantly larger than specified on retina displays
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn){//whenever the mouse moves
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if(firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;// reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){//whenever the mouse scroll wheel scrolls
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const* path){
    unsigned int textureID;
    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    glGenTextures(1, &textureID);

    if(data){
        GLenum format;
        if (nrComponents == 1)      format = GL_RED;
        else if (nrComponents == 3) format = GL_RGB;
        else format = GL_RGBA;//else if (nrComponents == 4) format = GL_RGBA;
        

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else{
        std::cout << "Texture failed to load. Texture path: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}
unsigned int loadCubemap(std::vector<std::string> faces){
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for(unsigned int i = 0; i < faces.size(); i++){
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);

        if(data){
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else{
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void glfwSetup(){//initialize glfw and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}
void glfwConfigureWindow(GLFWwindow* window){// glfw window creation
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);// tell GLFW to capture our mouse
}
bool gladSetup(){// glad: load OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    glEnable(GL_DEPTH_TEST);//enable depth buffer
    //glEnable(GL_CULL_FACE);
    return true;
}