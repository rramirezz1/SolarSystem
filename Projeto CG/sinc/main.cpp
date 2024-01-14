#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../common/shader.hpp"
#include "../common/texture.hpp"
#include "../common/controls.hpp"
#include "Sphere.h"


GLFWwindow* window;
unsigned int loadTexture(char const* path);



bool initializeGLFW() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    return true;
}

bool createWindow() {
    window = glfwCreateWindow(1024, 740, "Projeto", nullptr, nullptr);
    
    if (!window) {
        std::cerr << "Failed to open GLFW window.\n";
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    
    return true;
}

bool initializeGLEW() {
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        return false;
    }
    return true;
}

bool initializeOpenGL() {
    if (!initializeGLFW() || !createWindow() || !initializeGLEW()) {
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    float globalAmb[] = {0.0f, 0.0f, 0.0f, 1.f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmb);

    float lightPos[] = {0.0f, 0.0f, 0.0f, 1.0f};
    float lightColor[] = {0.8f, 0.8f, 0.8f};

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);

    return true;
}
void cleanup() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, height, width, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void renderSphere(float r, int sectors, int stacks) {
    Sphere sphere(r, sectors, stacks);
    sphere.Draw();
}


void setTexture(GLuint textureID, GLuint programID) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), 0);
}

void setShaderUniforms(GLuint programID, const glm::vec3& lightColor, const glm::vec3& lightPos, const glm::vec3& viewPos,
    float ambientStrength, float specularStrength, float shininess,
    const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model) {
    glUniform3f(glGetUniformLocation(programID, "lightColor"), lightColor.r, lightColor.g, lightColor.b);
    glUniform3f(glGetUniformLocation(programID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
    glUniform3f(glGetUniformLocation(programID, "viewPos"), viewPos.x, viewPos.y, viewPos.z);

    glUniform1f(glGetUniformLocation(programID, "ambientStrength"), ambientStrength);
    glUniform1f(glGetUniformLocation(programID, "specularStrength"), specularStrength);
    glUniform1f(glGetUniformLocation(programID, "shininess"), shininess);

    glUniformMatrix4fv(glGetUniformLocation(programID, "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(programID, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, &model[0][0]);
}


int main(int argc, char* argv[]) {
    
    if (!initializeOpenGL()) { return -1; }


    
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwPollEvents();
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);

    glClearColor(0.0f, 0.0f, 0.3f, 0.0f);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    GLuint programID = LoadShaders("test.vert", "test.frag");


    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    glm::mat4 MVP, Projection, View;

    GLuint ModelMatrixID = glGetUniformLocation(programID, "ModelMatrix");
    GLuint lightPosID = glGetUniformLocation(programID, "lightPos");
    GLuint lightColorID = glGetUniformLocation(programID, "lightColor");

  

    glUseProgram(programID);
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

    // Load textures for Earth, Mars, Sun, Moon, and Venus
    GLuint earthTextureID = loadTexture("earth.jpg");
    GLuint marsTextureID = loadTexture("mars.jpg");
    GLuint sunTextureID = loadTexture("sun.jpg");
    GLuint moonTextureID = loadTexture("moon.jpg");
    GLuint venusTextureID = loadTexture("venus.jpg");
    GLuint jupiterTextureID = loadTexture("jupiter.jpg");
    GLuint uranusTextureID = loadTexture("uranus.jpg");
    GLuint mercuryTextureID = loadTexture("mercury.jpg");
    GLuint neptuneTextureID = loadTexture("neptune.jpg");
    GLuint saturnTextureID = loadTexture("saturn.jpg");
    GLuint saturnRingTextureID = loadTexture("saturn_ring.png");
    GLuint ceresTextureID = loadTexture("ceres.jpg");
    GLuint jupiterMoonTextureID = loadTexture("ceres.jpg");
    GLuint fundoTextureID = loadTexture("estrelas.jpg");



    double ang[11] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 , 0.0 ,0.0, 0.0};
    float pos_earth = 50.0;
    double b = 10;

    auto orbitRadius = [pos_earth](double theta, double e, double rad) -> double { return pos_earth * rad * ((1.0 - e * e) / (1.0 + e * std::cos(theta))); };
    auto delta_ang = [](double year_in_days, double seconds_on_day) -> double {return (2.0 * 3.14159 / (year_in_days * seconds_on_day)); };
    auto angular_speed = [b](double orbit_in_days) -> double {return ((2 * 3.14159) / orbit_in_days) * b; };
    bool rodar = true;
    float velocidade[12] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 , 0.0 , 0.0,0.0 ,1.570796};
    float x[11] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 , 0.0,0.0, 0.0 };
    float y[11] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 , 0.0,0.0, 0.0 };
    
    float escala = 0.00005;

    glm::vec3 lightpos(0.0f, 0.0f, 0.0f);
    glm::vec3 lightcolor(0.5f, 0.5f, 0.5f);
    
    //Posição inicial da câmara
    glm::vec3 position = glm::vec3(-5.5, 35.5, 85.5);



    do {
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        computeMatricesFromInputs();
        
        double radius = orbitRadius(3.14159 * 2 * ang[2] / 360, 0.017, 1);
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS or rodar == true) {

            //Transla��o Terra
            ang[2] += angular_speed(365.25);
            x[2] = radius * sin(3.14159 * 2 * ang[2] / 360);
            y[2] = radius * cos(3.14159 * 2 * ang[2] / 360);
            //Rota��o
            velocidade[2] += 0.0787f;


            //Transla��o Marte
            ang[3] += angular_speed(687);
            radius = orbitRadius(3.14159 * 2 * ang[3] / 360, 0.093, 1.524);
            x[3] = radius * sin(3.14159 * 2 * ang[3] / 360);
            y[3] = radius * cos(3.14159 * 2 * ang[3] / 360);
            //Rota��o
            velocidade[3] += 866 * escala;

            //Transla��o Venus
            ang[1] += angular_speed(224.70);
            radius = orbitRadius(3.14159 * 2 * ang[1] / 360, 0.007, 0.723);
            x[1] = radius * sin(3.14159 * 2 * ang[1] / 360);
            y[1] = radius * cos(3.14159 * 2 * ang[1] / 360);
            //Rota��o
            velocidade[1] += 1.52 * escala;


            //Transla��o Jupiter
            ang[4] += angular_speed(4328.9);
            radius = orbitRadius(3.14159 * 2 * ang[4] / 360, 0.007, 5.204);
            x[4] = radius * sin(3.14159 * 2 * ang[4] / 360);
            y[4] = radius * cos(3.14159 * 2 * ang[4] / 360) - 160;
            //Rota��o
            velocidade[4] += 558 * escala;


            //Transla��o Urano
            ang[6] += angular_speed(84.01 * 365);

            radius = orbitRadius(3.14159 * 2 * ang[6] / 360, 0.046, 19.22);

            x[6] = radius * sin((3.14159 * 2 * ang[6] / 360));
            y[6] = radius * cos((3.14159 * 2 * ang[6] / 360)) - 700;

            //Rota��o
            velocidade[6] += 14794 * escala;


            //Transla��o Mercurio
            ang[0] += angular_speed(87.97);

            radius = orbitRadius(3.14159 * 2 * ang[0] / 360, 0.206, 0.387);

            x[0] = radius * sin(3.14159 * 2 * ang[0] / 360);
            y[0] = radius * cos(3.14159 * 2 * ang[0] / 360);
            //Rota��o
            velocidade[0] += 10.83 * escala;


            //Transla��o Neptuno
            ang[7] += angular_speed(164.8 * 365);

            radius = 2 * orbitRadius(3.14159 * 2 * ang[7] / 360, 0.01, 30.05);

            x[7] = radius * sin((3.14159 * 2 * ang[7] / 360));
            y[7] = radius * cos((3.14159 * 2 * ang[7] / 360) )- 2700;

            
            //Rota��o
            velocidade[7] += 9719 * escala;


            //Transla��o Saturno
            ang[5] += angular_speed(29.46 * 365);


            radius = orbitRadius(3.14159 * 2 * ang[5] / 360, 0.056, 9.582);

            x[5] = radius * sin((3.14159 * 2 * ang[5] / 360));
            y[5] = radius * cos((3.14159 * 2 * ang[5] / 360)) - 300;
            //Rota��o
            velocidade[5] += 36840 * escala;


            //Rota��o do Sol
            velocidade[8] += 1574 / 30 * escala;

            //Transla��o Lua
            ang[8] += angular_speed(27.32);
            radius = orbitRadius(3.14159 * 2 * ang[8] / 360, 0.055, 0.1);
            x[8] = radius * sin(3.14159 * 2 * ang[8] / 360);
            y[8] = radius * cos(3.14159 * 2 * ang[8] / 360);

            rodar = true;

            //Transla��o Lua de Jupiter
            ang[9] += angular_speed(27.32);
            radius = orbitRadius(3.14159 * 2 * ang[9] / 360, 0.055, 0.15);
            x[9] = radius * sin(3.14159 * 2 * ang[9] / 360);
            y[9] = radius * cos(3.14159 * 2 * ang[9] / 360);


            //Transla��o segunda Lua de Jupiter
            ang[10] += angular_speed(56.32);
            radius = orbitRadius(3.14159 * 2 * ang[10] / 360, 0.055, 0.2);
            x[10] = radius * sin(3.14159 * 2 * ang[10] / 360);
            y[10] = radius * cos(3.14159 * 2 * ang[10] / 360);

            rodar = true;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    rodar = !rodar; // Toggle the value of 'rodar'
}


        glm::vec3 lightPosition(0.0f, 0.0f, 0.0f);


        glUniform3fv(lightPosID, 1, &lightPosition[0]);
        glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

        glUniform3fv(lightColorID, 1, &lightColor[0]);
  


        // Render Earth
        glm::mat4 earthModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x[2], 0.0f, y[2]));
        earthModelMatrix = glm::rotate(earthModelMatrix, velocidade[2], glm::vec3(0.0f, 1.0f, 0.0f));
        earthModelMatrix = glm::rotate(earthModelMatrix, velocidade[11], glm::vec3(1.0f, 0.0f, 0.0f));

        Projection = getProjectionMatrix();
        View = getViewMatrix();
        MVP = Projection * View * earthModelMatrix;
        glm::vec3 viewPos = getCameraPosition();

        setShaderUniforms(programID, lightcolor, lightpos, viewPos, 0.5f, 0.5f, 3.0f, Projection, View, earthModelMatrix);

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        setTexture(earthTextureID, programID);
        renderSphere(1.0f, 36, 18);


        // Render Moon
        glm::mat4 moonModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x[2] + x[8], 0.0f, y[2] + y[8]));
        moonModelMatrix = glm::rotate(moonModelMatrix, velocidade[2], glm::vec3(0.0f, 1.0f, 0.0f));
        moonModelMatrix = glm::rotate(moonModelMatrix, velocidade[11], glm::vec3(0.5f, 0.0f, 0.0f));

        Projection = getProjectionMatrix();
        View = getViewMatrix();
        MVP = Projection * View * moonModelMatrix;
        viewPos = getCameraPosition();

        setShaderUniforms(programID, lightcolor, lightpos, viewPos, 0.5f, 0.5f, 3.0f, Projection, View, moonModelMatrix);

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        setTexture(moonTextureID, programID);
        renderSphere(0.55f, 36, 18);


        // Render Mars
        glm::mat4 marsModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x[3], 0.0f, y[3]));
        marsModelMatrix = glm::rotate(marsModelMatrix, velocidade[3], glm::vec3(0.0f, 1.0f, 0.0f));
        marsModelMatrix = glm::rotate(marsModelMatrix, velocidade[11], glm::vec3(1.0f, 0.0f, 0.0f));
        
        Projection = getProjectionMatrix();
        View = getViewMatrix();
        MVP = Projection * View * marsModelMatrix;
        viewPos = getCameraPosition();

        setShaderUniforms(programID, lightcolor, lightpos, viewPos, 0.5f, 0.5f, 3.0f, Projection, View, marsModelMatrix);

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        setTexture(marsTextureID, programID);
        renderSphere(1.2f, 36, 18);


        // Render Sun
       glm::mat4 sunModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        sunModelMatrix = glm::rotate(sunModelMatrix, velocidade[8], glm::vec3(0.0f, 1.0f, 0.0f));
        sunModelMatrix = glm::rotate(sunModelMatrix, velocidade[11], glm::vec3(1.0f, 0.0f, 0.0f));
       
        Projection = getProjectionMatrix();
        View = getViewMatrix();
        MVP = Projection * View * sunModelMatrix;
        viewPos = getCameraPosition();

        setShaderUniforms(programID, lightcolor, lightpos, viewPos, 1.f, 0.1f, 51.2f, Projection, View, sunModelMatrix);


        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        setTexture(sunTextureID, programID);
        renderSphere(10.0f, 36, 18);


        // Render Venus
      glm::mat4 venusModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x[1], 0.0f, y[1]));
        venusModelMatrix = glm::rotate(venusModelMatrix, velocidade[1], glm::vec3(0.0f, 1.0f, 0.0f));
        venusModelMatrix = glm::rotate(venusModelMatrix, velocidade[11], glm::vec3(1.0f, 0.0f, 0.0f));
       
        Projection = getProjectionMatrix();
        View = getViewMatrix();
        MVP = Projection * View * venusModelMatrix;
        viewPos = getCameraPosition();

        setShaderUniforms(programID, lightcolor, lightpos, viewPos,0.5f, 0.5f, 3.0f, Projection, View, venusModelMatrix);

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        setTexture(venusTextureID, programID);
        renderSphere(0.95f, 36, 18);


        // Render Jupiter
   glm::mat4 jupiterModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x[4], 0.0f, y[4]));
        jupiterModelMatrix = glm::rotate(jupiterModelMatrix, velocidade[4], glm::vec3(0.0f, 1.0f, 0.0f));
        jupiterModelMatrix = glm::rotate(jupiterModelMatrix, velocidade[11], glm::vec3(1.0f, 0.0f, 0.0f));
        
        Projection = getProjectionMatrix();
        View = getViewMatrix();
        MVP = Projection * View * jupiterModelMatrix;
        viewPos = getCameraPosition();

        setShaderUniforms(programID, lightcolor, lightpos, viewPos, 0.5f, 0.5f, 3.0f, Projection, View, jupiterModelMatrix);

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        setTexture(jupiterTextureID, programID);
        renderSphere(4.2f, 36, 18);


        // Render Uranus
       glm::mat4 uranusModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x[6], 0.0f, y[6]));
        uranusModelMatrix = glm::rotate(uranusModelMatrix, velocidade[6], glm::vec3(0.0f, 1.0f, 0.0f));
        uranusModelMatrix = glm::rotate(uranusModelMatrix, velocidade[11], glm::vec3(1.0f, 0.0f, 0.0f));
        
        Projection = getProjectionMatrix();
        View = getViewMatrix();
        MVP = Projection * View * uranusModelMatrix;
        viewPos = getCameraPosition();

        setShaderUniforms(programID, lightcolor, lightpos, viewPos, 0.5f, 0.5f, 3.0f, Projection, View, uranusModelMatrix);


        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        setTexture(uranusTextureID, programID);
        renderSphere(2.9f, 36, 18);


        // Render Mercury
       glm::mat4 mercuryModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x[0], 0.0f, y[0]));
        mercuryModelMatrix = glm::rotate(mercuryModelMatrix, velocidade[0], glm::vec3(0.0f, 1.0f, 0.0f));
        mercuryModelMatrix = glm::rotate(mercuryModelMatrix, velocidade[11], glm::vec3(1.0f, 0.0f, 0.0f));
        
        Projection = getProjectionMatrix();
        View = getViewMatrix();
        MVP = Projection * View * mercuryModelMatrix;
        viewPos = getCameraPosition();

        setShaderUniforms(programID, lightcolor, lightpos, viewPos, 0.5f, 0.5f, 3.0f, Projection, View, mercuryModelMatrix);

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        setTexture(mercuryTextureID, programID);
        renderSphere(0.383f, 36, 18);

        // Render Neptune
      glm::mat4 neptuneModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x[7], 0.0f, y[7]));
        neptuneModelMatrix = glm::rotate(neptuneModelMatrix, velocidade[7], glm::vec3(0.0f, 1.0f, 0.0f));
        neptuneModelMatrix = glm::rotate(neptuneModelMatrix, velocidade[11], glm::vec3(1.0f, 0.0f, 0.0f));
        
        Projection = getProjectionMatrix();
        View = getViewMatrix();
        MVP = Projection * View * neptuneModelMatrix;
        viewPos = getCameraPosition();

        setShaderUniforms(programID, lightcolor, lightpos, viewPos, 0.5f, 0.5f, 3.0f, Projection, View, neptuneModelMatrix);


        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        setTexture(neptuneTextureID, programID);
        renderSphere(2.7f, 36, 18);

        // Render Saturn
        glm::mat4 saturnModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x[5], 0.0f, y[5]));
        saturnModelMatrix = glm::rotate(saturnModelMatrix, velocidade[5], glm::vec3(0.0f, 1.0f, 0.0f));
        saturnModelMatrix = glm::rotate(saturnModelMatrix, velocidade[11], glm::vec3(1.0f, 0.0f, 0.0f));
        
        Projection = getProjectionMatrix();
        View = getViewMatrix();
        MVP = Projection * View * saturnModelMatrix;
        viewPos = getCameraPosition();

        setShaderUniforms(programID, lightcolor, lightpos, viewPos, 0.5f, 0.5f, 3.0f, Projection, View, saturnModelMatrix);

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        setTexture(saturnTextureID, programID);
        renderSphere(3.7f, 36, 18);

        // Render Jupiter's Moon
        glm::mat4 jupiterMoonModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x[4] + x[9], 0.0f, y[4] + y[9]));
        jupiterMoonModelMatrix = glm::rotate(jupiterMoonModelMatrix, velocidade[9], glm::vec3(0.0f, 1.0f, 0.0f));
        jupiterMoonModelMatrix = glm::rotate(jupiterMoonModelMatrix, velocidade[11], glm::vec3(1.0f, 0.0f, 0.0f));

        Projection = getProjectionMatrix();
        View = getViewMatrix();
        MVP = Projection * View * jupiterMoonModelMatrix;
        viewPos = getCameraPosition();

        setShaderUniforms(programID, lightcolor, lightpos, viewPos, 0.5f, 0.5f, 3.0f, Projection, View, jupiterMoonModelMatrix);


        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        setTexture(ceresTextureID, programID);
        renderSphere(0.3f, 36, 18);


        // Render 2nd Jupiter's Moon

        glm::mat4 jupiterMoon2ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x[4] + x[10], 0.0f, y[4] + y[10]));
        jupiterMoon2ModelMatrix = glm::rotate(jupiterMoon2ModelMatrix, velocidade[9], glm::vec3(0.0f, 1.0f, 0.0f));
        jupiterMoon2ModelMatrix = glm::rotate(jupiterMoon2ModelMatrix, velocidade[11], glm::vec3(1.0f, 0.0f, 0.0f));

        Projection = getProjectionMatrix();
        View = getViewMatrix();
        MVP = Projection * View * jupiterMoon2ModelMatrix;
        viewPos = getCameraPosition();

        setShaderUniforms(programID, lightcolor, lightpos, viewPos, 0.5f, 0.5f, 3.0f, Projection, View, jupiterMoon2ModelMatrix);


        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        setTexture(jupiterMoonTextureID, programID);
        renderSphere(0.7f, 36, 18);

        //render sky
        glm::mat4 skyModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        Projection = getProjectionMatrix();
        View = getViewMatrix();
        MVP = Projection * View * skyModelMatrix;
        viewPos = getCameraPosition();

        setShaderUniforms(programID, lightcolor, glm::vec3(), viewPos, 1.0f, 0.f, 0.0f, Projection, View, skyModelMatrix);
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        setTexture(fundoTextureID, programID);
        renderSphere(2000.0f, 36, 18);



    
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glColor3f(0.0f, 0.0f, 0.0f);
            //renderText("Olá, FreeType!", -0.5f, 0.0f, 1.0f);

            glfwSwapBuffers(window);
            glfwPollEvents();
            glfwWaitEventsTimeout(0.1);
        }
       
        
        glfwSwapBuffers(window);
        glfwPollEvents();

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window));

    cleanup();
    return 0;
}