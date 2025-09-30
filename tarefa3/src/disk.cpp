#include "disk.h"
#include <vector>
#include <iostream>

#define PI 3.141592653589793

DiskPtr Disk::Make(int nslice) {
    return DiskPtr(new Disk(nslice));
}

Disk::Disk(int nslice) : m_nslice(nslice + 2)
{
    if (nslice < 3) {
        nslice = 3;
    }

    std::vector<float> coord;

    coord.push_back(0.0f);
    coord.push_back(0.0f);

    float raio = 1.0f;
    for (int i = 0; i <= nslice; ++i)
    {
        float angulo = 2.0f * PI * static_cast<float>(i) / static_cast<float>(nslice);
        float x = raio * std::cos(angulo);
        float y = raio * std::sin(angulo);
        coord.push_back(x);
        coord.push_back(y);
    }

    // create VAO
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    // create coord buffer
    GLuint id[1];
    glGenBuffers(1, id);
    glBindBuffer(GL_ARRAY_BUFFER, id[0]);
    glBufferData(GL_ARRAY_BUFFER, coord.size() * sizeof(float), coord.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);  // coord
    glEnableVertexAttribArray(0);
}

Disk::~Disk() {
    glDeleteVertexArrays(1, &m_vao);
}

void Disk::Draw(StatePtr st) {
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, m_nslice);
}