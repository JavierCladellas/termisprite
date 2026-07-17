#pragma once

#include "sprite.hpp"
#include <string>

namespace Termisprite
{

class SpriteImporter
{
public:
    SpriteImporter() = default;

    static bool importImage( std::string const& filepath, Sprite & targetSprite, int targetWidth, int targetHeight);

private:

};

class SpriteExporter
{
public:
    SpriteExporter() = default;

    static bool exportProject( std::string const& projectName, Sprite & targetSprite );

};


}
