#include "hw3.h"
#include "3rdparty/glad.h" // needs to be included before GLFW!
#include "3rdparty/glfw/include/GLFW/glfw3.h"
#include "hw3_scenes.h"
#include "math.h"
#include "3rdparty/glm/glm/glm/ext/matrix_transform.hpp"
#include "3rdparty/glm/glm/glm/glm.hpp"
#include "3rdparty/glm/glm/glm/vec3.hpp"
#include "3rdparty/glm/glm/glm/vec4.hpp"
#include "3rdparty/glm/glm/glm/mat4x4.hpp"
#include "3rdparty/glm/glm/glm/ext/matrix_clip_space.hpp"
#include "3rdparty/glm/glm/glm/ext/scalar_constants.hpp"
#include "3rdparty/glm/glm/glm/gtc/type_ptr.hpp"
using namespace hw3;

glm::vec3 cameraPos;
glm::vec3 cameraFront;
glm::vec3 cameraUp;
// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    float cameraSpeed = static_cast<float>(25.0 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

const char *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 rotation;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = rotation * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
        
const char *fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(0.463f, 0.71f, 0.773f, 1.0f);\n"
        "}\n\0";
const char *vertexShaderSource2 = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "out vec3 ourColor;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
        "   ourColor = aColor;\n"
        "}\0";
const char *fragmentShaderSource2 = "#version 330 core\n"
        "in vec3 ourColor;\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(ourColor, 1.0);\n"
        "}\n\0";
const char *vertexShaderSource3 = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "layout (location = 2) in vec3 aNormal;\n"
    "out vec3 ourColor;\n"
    "out vec3 Normal;\n"
    "out vec3 FragPos;\n"
    "uniform mat4 projection;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "void main()\n"
    "{\n"
    "   FragPos = vec3(model * vec4(aPos, 1.0));\n"
    "   ourColor = aColor;\n"
    "   Normal = mat3(transpose(inverse(model))) * aNormal;\n"
    "   gl_Position = projection * view * vec4(FragPos, 1.0);\n"
    "}\0";
const char *fragmentShaderSource3 = "#version 330 core\n"
        "in vec3 ourColor;\n"
        "in vec3 Normal;\n"
        "in vec3 FragPos;\n"
        "out vec4 FragColor;\n"
        "uniform vec3 viewPos;\n"
        "void main()\n"
        "{\n"
        "   vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));\n"
        "   vec3 norm = normalize(Normal);\n"
        "   vec3 lightColor = vec3(1.0, 1.0, 1.0);\n"
        "   float diff = max(dot(norm, lightDir), 0.0);\n"
        "   vec3 diffuse = diff * lightColor;\n"
        "   float ambientStrength = 0.1;\n"
        "   vec3 ambient = ambientStrength * lightColor;\n"
        "   float specularStrength = 0.5;\n"
        "   vec3 viewDir = normalize(viewPos - FragPos);\n"
        "   vec3 reflectDir = reflect(-lightDir, norm);\n"
        "   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n"
        "   vec3 specular = specularStrength * spec * lightColor;\n"
        "   vec3 result = (ambient + diffuse + specular) * ourColor;\n"
        "   FragColor = vec4(result, 1.0);\n"
        "}\n\0";

void hw_3_1(const std::vector<std::string> &params) {
    // HW 3.1: Open a window using GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "HW3_1", NULL, NULL);
    if(window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to intialize GLAD" << std::endl;
        return;
    }

    //render loop
    while(!glfwWindowShouldClose(window)) {
        //input 
        processInput(window);

        //rendering commands
        glClearColor(0.34f, 0.28f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return;
}

void hw_3_2(const std::vector<std::string> &params) {
    // HW 3.2: Render a single 2D triangle
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "HW3_2", NULL, NULL);
    if(window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to intialize GLAD" << std::endl;
        return;
    }

    //build and compile shader program
    //--vertex shader--
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    //fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    unsigned int shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success); 
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
    }    
    //free space bc we linked them already
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    //Bind Vertex Array Object (VAO) first, then bind and set vertex buffers, then configure vertex attributes
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0); //unbind so we don't modify it, but option (rarely happens)
    glBindVertexArray(VAO);
    //render loop

    while(!glfwWindowShouldClose(window)) {
        //input 
        processInput(window);

        //render
        glClearColor(0.34f, 0.28f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //activate shader 
        glUseProgram(shaderProgram);

        //update shader uniform
        double timeValue = glfwGetTime();
        float rotationAngle = static_cast<float>(timeValue);
        
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));

        int rotationLocation = glGetUniformLocation(shaderProgram, "rotation");

        glUniformMatrix4fv(rotationLocation, 1, GL_FALSE, &rotationMatrix[0][0]);

        //render triangle
        glDrawArrays(GL_TRIANGLES, 0, 3);

        //polling I/O events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //optionally deallocate arrays
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
}

void hw_3_3(const std::vector<std::string> &params) {
    // HW 3.3: Render a scene
    if (params.size() == 0) {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;
    
    Real width = scene.camera.resolution.x;
    Real height = scene.camera.resolution.y;
    Real z_near = scene.camera.z_near;
    Real z_far = scene.camera.z_far;
    Real a = width / height;
    Real s = scene.camera.s;
    Vector3f bc = scene.background;
    Matrix4x4f P;
    Matrix4x4f camToWorld = scene.camera.cam_to_world;
    P(0, 0) = Real(1) / (a * s);
    P(1, 1) = Real(1) / s;
    P(2, 2) = -z_far / (z_far - z_near);
    P(2, 3) = -(z_far*z_near) / (z_far - z_near);
    P(3, 2) = Real(-1);
    Matrix4x4f V = inverse(scene.camera.cam_to_world); 
    std::cout<<"V: " << V << std::endl;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(width, height, "HW3_3", NULL, NULL);
    if(window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to intialize GLAD" << std::endl;
        return;
    }

    //build and compile shader program
    //--vertex shader--
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource2, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    //fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource2, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success); 
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
    }    
    //free space bc we linked them already
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    cameraPos = glm::vec3(camToWorld(0, 3), camToWorld(1, 3), camToWorld(2, 3));
    cameraFront = -glm::vec3(camToWorld(0, 2), camToWorld(1, 2), camToWorld(2, 2));
    cameraUp = glm::vec3(camToWorld(0, 1), camToWorld(1, 1), camToWorld(2, 1));
        
    std::vector<unsigned int> VAOs(scene.meshes.size());
    for(int i = 0; i < scene.meshes.size(); i++) {
        TriangleMesh mesh = scene.meshes[i];
        glGenVertexArrays(1, &VAOs[i]);
        glBindVertexArray(VAOs[i]);

        unsigned int VBO_vertex;
        glGenBuffers(1, &VBO_vertex);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vector3f), mesh.vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        unsigned int VBO_color;
        glGenBuffers(1, &VBO_color);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertex_colors.size() * sizeof(Vector3f), mesh.vertex_colors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);

        unsigned int EBO;
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.faces.size() * sizeof(Vector3i), mesh.faces.data(), GL_STATIC_DRAW);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    while(!glfwWindowShouldClose(window)) {
        //input 
        processInput(window);
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //render
        glClearColor(bc.x, bc.y, bc.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //activate shader 
        glUseProgram(shaderProgram);     
        
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
           
        //render triangle
        for(int i = 0; i < scene.meshes.size(); i++) {
            TriangleMesh mesh = scene.meshes[i];
            glBindVertexArray(VAOs[i]);
            Matrix4x4f M = mesh.model_matrix;            
            int modelLocation = glGetUniformLocation(shaderProgram, "model");
            int projectionLocation = glGetUniformLocation(shaderProgram, "projection");
            int viewLocation = glGetUniformLocation(shaderProgram, "view");
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, M.ptr());
            glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, P.ptr());
            glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
            glDrawElements(GL_TRIANGLES, mesh.faces.size() * 3, GL_UNSIGNED_INT, 0);
        }
        //polling I/O events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //optionally deallocate arrays

    glfwTerminate();
}

void hw_3_4(const std::vector<std::string> &params) {
    // HW 3.4: Render a scene with lighting
    if (params.size() == 0) {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;
    
    Real width = scene.camera.resolution.x;
    Real height = scene.camera.resolution.y;
    Real z_near = scene.camera.z_near;
    Real z_far = scene.camera.z_far;
    Real a = width / height;
    Real s = scene.camera.s;
    Vector3f bc = scene.background;
    Matrix4x4f camToWorld = scene.camera.cam_to_world;
    Matrix4x4f P;
    P(0, 0) = Real(1) / (a * s);
    P(1, 1) = Real(1) / s;
    P(2, 2) = -z_far / (z_far - z_near);
    P(2, 3) = -(z_far*z_near) / (z_far - z_near);
    P(3, 2) = Real(-1);
    Matrix4x4f V = inverse(scene.camera.cam_to_world); 
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(width, height, "HW3_4", NULL, NULL);
    if(window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to intialize GLAD" << std::endl;
        return;
    }

    //build and compile shader program
    //--vertex shader--
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource3, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    //fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource3, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success); 
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
    }    
    //free space bc we linked them already
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    cameraPos = glm::vec3(camToWorld(0, 3), camToWorld(1, 3), camToWorld(2, 3));
    cameraFront = -glm::vec3(camToWorld(0, 2), camToWorld(1, 2), camToWorld(2, 2));
    cameraUp = glm::vec3(camToWorld(0, 1), camToWorld(1, 1), camToWorld(2, 1));
        
    std::vector<unsigned int> VAOs(scene.meshes.size());
    for(int i = 0; i < scene.meshes.size(); i++) {
        TriangleMesh mesh = scene.meshes[i];
        glGenVertexArrays(1, &VAOs[i]);
        glBindVertexArray(VAOs[i]);

        unsigned int VBO_vertex;
        glGenBuffers(1, &VBO_vertex);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vector3f), mesh.vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        unsigned int VBO_color;
        glGenBuffers(1, &VBO_color);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertex_colors.size() * sizeof(Vector3f), mesh.vertex_colors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        
        unsigned int VBO_normal;
        glGenBuffers(1, &VBO_normal);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_normal);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertex_normals.size() * sizeof(Vector3f), mesh.vertex_normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(2);

        unsigned int EBO;
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.faces.size() * sizeof(Vector3i), mesh.faces.data(), GL_STATIC_DRAW);
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glBindVertexArray(0);
    while(!glfwWindowShouldClose(window)) {
        //input 
        processInput(window);
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //render
        glClearColor(bc.x, bc.y, bc.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //activate shader 
        glUseProgram(shaderProgram);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);   
        glm::vec3 viewPos(camToWorld(0, 3), camToWorld(1, 3), camToWorld(2, 3));
        //render triangle
        for(int i = 0; i < scene.meshes.size(); i++) {
            TriangleMesh mesh = scene.meshes[i];
            glBindVertexArray(VAOs[i]);
            Matrix4x4f M = mesh.model_matrix;            
            int modelLocation = glGetUniformLocation(shaderProgram, "model");
            int projectionLocation = glGetUniformLocation(shaderProgram, "projection");
            int viewLocation = glGetUniformLocation(shaderProgram, "view");
            int viewPositionLocation = glGetUniformLocation(shaderProgram, "viewPos");
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, M.ptr());
            glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, P.ptr());
            glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
            glUniform3fv(viewPositionLocation, 1, glm::value_ptr(viewPos));
            glDrawElements(GL_TRIANGLES, mesh.faces.size() * 3, GL_UNSIGNED_INT, 0);
        }
        //polling I/O events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //optionally deallocate arrays

    glfwTerminate();
}
