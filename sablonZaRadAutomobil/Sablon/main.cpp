// Autor: Bogdan Davinic
// Autor nadogradnje projekta (Betmobil): Bosko Kulusic RA122-2020

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>   
#include <GLFW/glfw3.h>
//Biblioteke za pokretanje generatora podataka
#include "TestBed.h"

#define CRES 30 // Circle Resolution = Rezolucija kruga'

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned int compileShader(GLenum type, const char* source); 
unsigned int createShader(const char* vsSource, const char* fsSource);
static unsigned loadImageToTexture(const char* filePath);

int main(void)
{

    //Inicijalizacija objekta tipa Car
    Car car = getCar();
    
    if (!glfwInit()) 
    {
        std::cout<<"GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window; 
    unsigned int wWidth = 1250;
    unsigned int wHeight = 800;
    const char wTitle[] = "[Betmobil]";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL); 
    if (window == NULL) 
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate(); 
        return 2; 
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) 
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }


    unsigned int unifiedShader = createShader("basic.vert", "basic.frag"); 
    unsigned int textureShader = createShader("texture.vert", "texture.frag");

    float verteciesBackroundSignatureTacho[] =
    {   //X    Y      S    T 
        -1.0, -1.0,   0.0, 0.0,
        -1.0, 1.0,  0.0, 1.0, 
        1.0, -1.0,    1.0, 0.0, 
        1.0, 1.0,    1.0, 1.0 
    };
    unsigned int stride1 = (2 + 2) * sizeof(float);
    unsigned int VAO1;
    glGenVertexArrays(1, &VAO1); 
    glBindVertexArray(VAO1); 

    unsigned int VBO1;
    glGenBuffers(1, &VBO1); 
    glBindBuffer(GL_ARRAY_BUFFER, VBO1); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(verteciesBackroundSignatureTacho), verteciesBackroundSignatureTacho, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride1, (void*)0); 
    glEnableVertexAttribArray(0); 
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride1, (void*)(2 * sizeof(float))); 
    glEnableVertexAttribArray(1);


    //Tekstura
    unsigned tachometerTexture = loadImageToTexture("res/obrtaji1.png");
    glBindTexture(GL_TEXTURE_2D, tachometerTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    unsigned uTachoTexLoc = glGetUniformLocation(textureShader, "uTex");
    glUniform1i(uTachoTexLoc, 0); // Indeks teksturne jedinice (sa koje teksture ce se citati boje)

    unsigned backgroundTexture = loadImageToTexture("res/background.png");
    glBindTexture(GL_TEXTURE_2D, backgroundTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    unsigned uSkylineTexLoc = glGetUniformLocation(textureShader, "uTex");
    glUniform1i(uSkylineTexLoc, 0); // Indeks teksturne jedinice (sa koje teksture ce se citati boje)

    unsigned signatureTexture = loadImageToTexture("res/potpis.png");
    glBindTexture(GL_TEXTURE_2D, signatureTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    unsigned uSignatureTexLoc = glGetUniformLocation(textureShader, "uTex");
    glUniform1i(uSkylineTexLoc, 0); // Indeks teksturne jedinice (sa koje teksture ce se citati boje)

    //novi VAO 
    unsigned int VAO[8];
    glGenVertexArrays(8, VAO);
    unsigned int VBO[8];
    glGenBuffers(8, VBO);

    unsigned int stride = (2 + 4) * sizeof(float);

    float verticesCheckEngineAndBattery[] =
    {
        -0.55, -0.35,    0.0, 0.2, 0.1, 1.0,
        0.55, -0.35,    0.0, 0.2, 0.1, 1.0,
        0.0, 0.5,    0.0, 0.2, 0.1, 1.0,
    };

    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCheckEngineAndBattery), verticesCheckEngineAndBattery, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    float verticesDisplay[] =
    {
        -1.0, -1.0,    0.8, 0.3, 0.6, 1.0,
        -1.0, 1.0,    0.8, 0.3, 0.6, 1.0,
         1.0, -1.0,    0.8, 0.3, 0.6, 1.0,
        1.0, 1.0,    0.8, 0.3, 0.6, 1.0,
    };

    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesDisplay), verticesDisplay, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    float verticesBatLogo[] =
    {
        -0.25, -0.05,    0.0, 1.0, 0.0, 0.5,
        -0.2, 0.1,       0.0, 1.0, 0.0, 0.5,
        -0.13, 0.0,      0.0, 1.0, 0.0, 0.5,
        -0.1, 0.15,      0.0, 1.0, 0.0, 0.5,
        -0.04, -0.1,     0.0, 1.0, 0.0, 0.5,
        -0.03, 0.1,      0.0, 1.0, 0.0, 0.5,
        -0.01, -0.05,    0.0, 1.0, 0.0, 0.5,
         0.0, 0.16,      0.0, 1.0, 0.0, 0.5,
        0.01, -0.05,     0.0, 1.0, 0.0, 0.5,
        0.03, 0.1,       0.0, 1.0, 0.0, 0.5,
        0.04, -0.1,      0.0, 1.0, 0.0, 0.5,
        0.1, 0.15,       0.0, 1.0, 0.0, 0.5,
        0.13, 0.0,       0.0, 1.0, 0.0, 0.5,
        0.2, 0.1,        0.0, 1.0, 0.0, 0.5,
        0.25, -0.05,     0.0, 1.0, 0.0, 0.5,
    };

    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesBatLogo), verticesBatLogo, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    float needleVertices[] = {
        0.0, 0.0,       0.0, 0.0, 0.0, 0.5,
        -0.01, 0.01,       0.0, 0.0, 0.0, 0.5,
    };
    glBindVertexArray(VAO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(needleVertices), needleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //kod sa vezbi za crtanje kruga
    float circle[CRES * 2 + 4]; // +4 je za x i y koordinate centra kruga, i za x i y od nultog ugla
    float r = 0.5; //poluprecnik
    circle[0] = 0; //Centar X0
    circle[1] = 0; //Centar Y0
    int i;
    for (i = 0; i <= CRES; i++)
    {
        circle[2 + 2 * i] = r * cos((3.141592 / 180) * (i * 360 / CRES)); //Xi
        circle[2 + 2 * i + 1] = r * sin((3.141592 / 180) * (i * 360 / CRES)); //Yi
    }

    glBindVertexArray(VAO[4]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circle), circle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    float verticesProgressBar[] =
    {
        -1.0, -1.0,    0.0, 0.3, 0.1, 1.0,
       -1.0, 1.0,     0.0, 0.3, 0.1, 1.0,
        1.0, -1.0,    0.0, 0.3, 0.1, 1.0,
        1.0, 1.0,     0.0, 0.3, 0.1, 1.0,
    };

    //progres bar
    glBindVertexArray(VAO[5]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesProgressBar), verticesProgressBar, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    float verticesProgressBarNew[] =
    {
        -1.0, -1.0,    0.4, 0.4, 0.1, 1.0,
       -1.0, 1.0,      0.4, 0.4, 0.1, 1.0,
        -0.999, -1.0,    0.4, 0.4, 0.1, 1.0,
        -0.990, 1.0,     0.4, 0.4, 0.1, 1.0,
    };

    //napredak u progres baru
    glBindVertexArray(VAO[6]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesProgressBarNew), verticesProgressBarNew, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    float verticesProgressBarLines[] =
    {
       -1.0, -1.0,    0.4, 0.4, 0.1, 1.0,
       -1.0, 1.0,     0.4, 0.4, 0.1, 1.0,
       -0.8, 1.0,     0.4, 0.4, 0.1, 1.0,
       -0.8, -1.0,    0.4, 0.4, 0.1, 1.0,
       -0.6, -1.0,    0.4, 0.4, 0.1, 1.0,
       -0.6, 1.0,     0.4, 0.4, 0.1, 1.0,
       -0.4, 1.0,     0.4, 0.4, 0.1, 1.0,
       -0.4, -1.0,    0.4, 0.4, 0.1, 1.0,
       -0.2, -1.0,    0.4, 0.4, 0.1, 1.0,
       -0.2, 1.0,     0.4, 0.4, 0.1, 1.0,
        0.0, 1.0,     0.4, 0.4, 0.1, 1.0,
        0.0, -1.0,    0.4, 0.4, 0.1, 1.0,
        0.2, -1.0,    0.4, 0.4, 0.1, 1.0,
        0.2, 1.0,     0.4, 0.4, 0.1, 1.0,
        0.4, 1.0,     0.4, 0.4, 0.1, 1.0,
        0.4, -1.0,    0.4, 0.4, 0.1, 1.0,
        0.6, -1.0,    0.4, 0.4, 0.1, 1.0,
        0.6, 1.0,     0.4, 0.4, 0.1, 1.0,
        0.8, 1.0,     0.4, 0.4, 0.1, 1.0,
        0.8, -1.0,    0.4, 0.4, 0.1, 1.0,
        1.0, -1.0,    0.4, 0.4, 0.1, 1.0,
        1.0, 1.0,     0.4, 0.4, 0.1, 1.0,
    };

    //linije na progres baru
    glBindVertexArray(VAO[7]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[7]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesProgressBarLines), verticesProgressBarLines, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    float x = 0;
    float y = 0;

    unsigned int batShader = createShader("batLogo.vert", "basic.frag");
    unsigned int uPosLoc = glGetUniformLocation(batShader, "uPos");

    unsigned int needleShader = createShader("needle.vert", "changingColors.frag");
    unsigned int uN = glGetUniformLocation(needleShader, "uN");
    unsigned int uNA = glGetUniformLocation(needleShader, "uA");
    
    unsigned int blinkingShader = createShader("basic.vert", "changingColors.frag");
    unsigned int uA = glGetUniformLocation(blinkingShader, "uA");

    unsigned int progressShader = createShader("progress.vert", "basic.frag");
    unsigned int uProgress = glGetUniformLocation(progressShader, "uP");

    //Pokretanje simulacije, kako bi se menjalo stanje automobila kroz vreme
    startSimulation(&car);
    
    while (!glfwWindowShouldClose(window)) 
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
         
        glViewport(0, 0, wWidth, wHeight);
        //pozadina tekstura
        glUseProgram(textureShader);
        glBindVertexArray(VAO1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, backgroundTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        //potpis tekstura
        glViewport(wWidth * 0.615, wHeight * 0.025, wWidth * 0.36, wHeight * 0.10);
        glUseProgram(textureShader);
        glBindVertexArray(VAO1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, signatureTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        //tekstura za tahometar 
        glViewport(wWidth * 0.07, wHeight * 0.545, 115, 115);
        glUseProgram(textureShader);
        glBindVertexArray(VAO1);
        glActiveTexture(GL_TEXTURE0); //tekstura koja se bind-uje nakon ovoga ce se koristiti sa SAMPLER2D uniformom u sejderu koja odgovara njenom indeksu
        glBindTexture(GL_TEXTURE_2D, tachometerTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0); 

        //needle
        glLineWidth(4);
        glUseProgram(needleShader);
        glBindVertexArray(VAO[3]);
        glUniform2f(uN, 0.7 * cos(-(3.141592 / 180) * 37 * car.getTachometer() / 1000 + 3.141592 * 1.31), 0.7 * sin(-(3.141592 / 180) * 37 * car.getTachometer() / 1000 + 3.141592 * 1.31));
        glUniform3f(uNA, abs(sin(glfwGetTime() / 2)), abs(sin(3 * glfwGetTime())), abs(sin(glfwGetTime())));
        glDrawArrays(GL_LINE_STRIP, 0, 2);
        glLineWidth(1);

        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        {
            car.setCheckEngine(true);
        }
        //indikator check engine
        glViewport(wWidth * 0.56, wHeight * 0.595, wWidth * 0.06, wHeight * 0.09);
        if (car.getCheckEngineLight()) {
            glUseProgram(blinkingShader);
            glUniform3f(uA, abs(sin(15 * glfwGetTime())), 0, 0);
            glBindVertexArray(VAO[0]);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 3);
        }
        else {
            glUseProgram(unifiedShader);
            glBindVertexArray(VAO[0]);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 3);
        }

        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        {
            car.setBatteryLight(true);
        }
        //indikator problema sa akumulatorom
        glViewport(wWidth * 0.56, wHeight * 0.532, wWidth * 0.06, wHeight * 0.09);
        if (car.getBatteryProblemLight()) {
            glUseProgram(blinkingShader);
            glUniform3f(uA, abs(sin(15 * glfwGetTime())), 0, 0);
            glBindVertexArray(VAO[0]);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 3);
        }
        else {
            glUseProgram(unifiedShader);
            glBindVertexArray(VAO[0]);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 3);
        }

        //indikator brzine
        glViewport(wWidth * 0.228, wHeight * 0.55, wWidth * 0.1, wHeight * 0.15);
        glUseProgram(blinkingShader);
        glUniform3f(uA, 0, abs(sin(1.5 * car.getGear() * glfwGetTime())), 0);
        glBindVertexArray(VAO[4]);
        glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(circle) / (2 * sizeof(float)));

        //progres bar
        glViewport(wWidth * 0.74, wHeight * 0.58, wWidth * 0.15, wHeight * 0.03);
        glUseProgram(unifiedShader);
        glBindVertexArray(VAO[5]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        //linije na progres baru
        glUseProgram(unifiedShader);
        glBindVertexArray(VAO[7]);
        glDrawArrays(GL_LINE_STRIP, 0, 22);

        //napredak u progres baru
        if ((int)car.getOdometer() % 10 != 0) {
            glUseProgram(progressShader);
            glBindVertexArray(VAO[6]);
            glUniform2f(uProgress, 0.2 * ((int)car.getOdometer() % 10), 0.0);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }

        //displej
        glViewport(wWidth * 0.41, wHeight * 0.19, wWidth * 0.32, wHeight * 0.28);
        glUseProgram(unifiedShader);
        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        //kretanje slepog misa i pomeranje i providnost
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            if (y <= 0.82)
                y += 0.004;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            if (y > -0.9)
                y -= 0.004;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            if (x <= 0.75)
                x += 0.004;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            if (x >= -0.75)
                x -= 0.004;
        }

        if (car.getGear() == 1)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            glEnable(GL_PROGRAM_POINT_SIZE);
            glPointSize(4);
            //logo slepog misa
            glUseProgram(batShader);
            glBindVertexArray(VAO[2]);
            glUniform2f(uPosLoc, x, y);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 15);
        }
        else if (car.getGear() == 2)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            //logo slepog misa
            glUseProgram(batShader);
            glBindVertexArray(VAO[2]);
            glUniform2f(uPosLoc, x, y);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 15);
        }
        //logo slepog misa
        glUseProgram(batShader);
        glBindVertexArray(VAO[2]);
        glUniform2f(uPosLoc, x, y);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 15);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_PROGRAM_POINT_SIZE);
        glPointSize(1);

        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        {
            glDisable(GL_BLEND);
        }

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    // Zaustavljanje simulacije
    endSimulation(&car);
    glDeleteTextures(1, &tachometerTexture);
    glDeleteTextures(1, &backgroundTexture);
    glDeleteTextures(1, &signatureTexture);
    glDeleteBuffers(1, &VBO1);
    glDeleteVertexArrays(1, &VAO1);
    glDeleteBuffers(8, VBO);
    glDeleteVertexArrays(8, VAO);
    glDeleteProgram(unifiedShader);
    glDeleteProgram(needleShader);
    glDeleteProgram(blinkingShader);
    glDeleteProgram(batShader);
    glDeleteProgram(textureShader);
    glfwTerminate();
    return 0;
}

unsigned int compileShader(GLenum type, const char* source)
{
    //Uzima kod u fajlu na putanji "source", kompajlira ga i vraca sejder tipa "type"
    //Citanje izvornog koda iz fajla
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
     std::string temp = ss.str();
     const char* sourceCode = temp.c_str(); //Izvorni kod sejdera koji citamo iz fajla na putanji "source"

    int shader = glCreateShader(type); //Napravimo prazan sejder odredjenog tipa (vertex ili fragment)
    
    int success; //Da li je kompajliranje bilo uspjesno (1 - da)
    char infoLog[512]; //Poruka o gresci (Objasnjava sta je puklo unutar sejdera)
    glShaderSource(shader, 1, &sourceCode, NULL); //Postavi izvorni kod sejdera
    glCompileShader(shader); //Kompajliraj sejder

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success); //Provjeri da li je sejder uspjesno kompajliran
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog); //Pribavi poruku o gresci
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}
unsigned int createShader(const char* vsSource, const char* fsSource)
{
    //Pravi objedinjeni sejder program koji se sastoji od Vertex sejdera ciji je kod na putanji vsSource

    unsigned int program; //Objedinjeni sejder
    unsigned int vertexShader; //Verteks sejder (za prostorne podatke)
    unsigned int fragmentShader; //Fragment sejder (za boje, teksture itd)

    program = glCreateProgram(); //Napravi prazan objedinjeni sejder program

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource); //Napravi i kompajliraj vertex sejder
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource); //Napravi i kompajliraj fragment sejder

    //Zakaci verteks i fragment sejdere za objedinjeni program
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program); //Povezi ih u jedan objedinjeni sejder program
    glValidateProgram(program); //Izvrsi provjeru novopecenog programa

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success); //Slicno kao za sejdere
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    //Posto su kodovi sejdera u objedinjenom sejderu, oni pojedinacni programi nam ne trebaju, pa ih brisemo zarad ustede na memoriji
    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}

static unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        //Slike se osnovno ucitavaju naopako pa se moraju ispraviti da budu uspravne
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        // Provjerava koji je format boja ucitane slike
        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        // oslobadjanje memorije zauzete sa stbi_load posto vise nije potrebna
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Textura nije ucitana! Putanja texture: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}
