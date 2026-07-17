#pragma once

#include "sprite.hpp"
#include <string>

namespace Termisprite
{

class SpriteImporter
{
public:
    SpriteImporter() = default;

    static bool importImage(const std::string& filepath, Sprite & targetSprite, int targetWidth, int targetHeight);

private:

};

}
