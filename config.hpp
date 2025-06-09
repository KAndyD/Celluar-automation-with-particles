#pragma once
#include <array>

constexpr int TYPE_COUNT = 3;

constexpr std::array<std::array<float, TYPE_COUNT>, TYPE_COUNT> getInteractionMatrix(int mode) {
    if (mode == 1) { // Охота
        return {{
            {  0.0f,  0.9f, -0.5f },
            { -0.9f,  0.0f,  0.9f },
            {  0.5f, -0.9f,  0.0f }
        }};
    } else if (mode == 2) { // Расслоение
        return {{
            { 1.0f, -0.5f, -0.5f },
            { -0.5f, 1.0f, -0.5f },
            { -0.5f, -0.5f, 1.0f }
        }};
    } else if (mode == 3) { // Хаос
        return {{
            { 0.8f, -0.6f, 0.4f },
            { 0.2f, 0.8f, -0.7f },
            { -0.5f, 0.9f, 0.5f }
        }};
    } else if (mode == 4) { // Союзы
        return {{
            { 1.0f, 1.0f, -1.0f },
            { 1.0f, 1.0f, -1.0f },
            { -1.0f, -1.0f, 1.0f }
        }};
    } else {
        return {{{0}}};
    }
}
