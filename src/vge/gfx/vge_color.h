#pragma once
#include <glm/glm.hpp>

namespace VGE
{
    struct Color
    {
        Color(glm::vec4 color)
            : raw(color)
        {}

        Color(float r, float g, float b, float a = 1.0f)
        : raw(r, g, b, a)
        {}

        Color(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 255)
        {
            raw.r = (float)r / 255;
            raw.g = (float)g / 255;
            raw.b = (float)b / 255;
            raw.a = (float)a / 255;
        }

        glm::vec4 raw;

        const static Color White;
        const static Color Red;
        const static Color Blue;
        const static Color Green;
    };
}
