//This file knows nothing about compiled GL data
//Should only know vertex data/VAO/VBO data

#pragma once

#include <glad/glad.h>
#include <Windows.h>
#include <shader.h>

struct Mesh {
    unsigned int VBO;
    unsigned int VAO;

    //Fixed-size array with initializer
    float vertices[30] = {
        //What I assume to be a quad 
     //Bottom left texcoords
     -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
     //Bottom right texcoords
     0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
     //Upper left texcoords
     -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
     //Upper right texcoords
     0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
     //Bottom right texcoords
     0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
     //Upper left texcoords
     -0.5f, 0.5f, 0.0f, 0.0f, 1.0f
 };

    //Constructor - GPU initialization
    Mesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        //---OpenGL stuff time!!!---

        //Vertex Buffering stuff
        //1. bind Vertex Array Object
        glBindVertexArray(VAO);
        //2. Copy vertice array into a buffer for OpenGl to use
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        //3. set the vertices attributes pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
    }

    //Destructor - CLeanup
    ~Mesh() {
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
    }



};