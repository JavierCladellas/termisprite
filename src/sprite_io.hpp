#pragma once

#include <string>

#include <nlohmann/json.hpp>

#include "editor.hpp"
#include "sprite.hpp"

namespace Termisprite
{

class SpriteImporter
{
public:
    SpriteImporter() = default;

    static bool importImage( std::string const& filepath, Sprite & targetSprite, int targetWidth, int targetHeight);

    static bool importProject( std::string const& filepath, Sprite & targetSprite, EditorState & editorState );

private:

};

class SpriteExporter
{
public:
    SpriteExporter() = default;

    static bool
    exportProject( std::string const& filepath,
                   std::string const& projectName,
                   Sprite const& targetSprite,
                   EditorState const& editorState );

    enum class ExportFormat
    {
        PNG,
        JPG,
        ASCII
    };
    static bool
    exportImage( std::string const& filepath, Sprite const& targetSprite, ExportFormat format = ExportFormat::PNG );

private:
};


}
