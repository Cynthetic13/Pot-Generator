#include "src/BackendSDL.h"

#include <iostream>
#include <cmath>
#include <vector>

constexpr Uint32 WIDTH = 1280;
constexpr Uint32 HEIGHT = 720;
constexpr float aspect = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);

constexpr float PI = 3.14159265358979323846f;

static std::vector<IBackend::Vertex> vertices {
    {0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},     // top vertex
    {-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f},   // bottom left vertex
    {0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f}     // bottom right vertex
};

static std::vector<IBackend::Vertex> circle;

void GenerateCircle(int segments, float radius, std::vector<IBackend::Vertex>& out) {
    out.clear();

    IBackend::Vertex center = { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f };

    std::vector<IBackend::Vertex> ring;

    for (int i = 0; i <= segments; i++) {
        float angle = static_cast<float>(i) / static_cast<float>(segments) * 2.0f * PI;

        float x = (radius * std::cos(angle)) / aspect;
        float y = radius * std::sin(angle);

        ring.push_back({x, y, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f});
    }

    for (int i = 0; i < segments; i++) {
        out.push_back(center);
        out.push_back(ring[i]);
        out.push_back(ring[i + 1]);
    }
}

int main(void) {
    BackendSDL backend;

    GenerateCircle(24, 1.0f, circle);

    backend.SubmitMesh(circle);

    if (backend.AppInit(WIDTH, HEIGHT)) {
        std::cout << "Failed to initialize backend" << std::endl;
    }

    backend.AppUpdate();

    backend.AppQuit();

    return 0;
}