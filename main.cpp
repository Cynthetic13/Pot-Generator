#include "src/BackendSDL.h"

constexpr Uint32 WIDTH = 1280;
constexpr Uint32 HEIGHT = 720;

static IBackend::Vertex vertices[] {
    {0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},     // top vertex
    {-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f},   // bottom left vertex
    {0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f}     // bottom right vertex
};

int main(void) {
    BackendSDL backend;

    backend.SubmitMesh(vertices, sizeof(vertices) / sizeof(IBackend::Vertex));

    if (backend.AppInit(WIDTH, HEIGHT)) {
        std::cout << "Failed to initialize backend" << std::endl;
    }

    backend.AppUpdate();

    backend.AppQuit();

    return 0;
}