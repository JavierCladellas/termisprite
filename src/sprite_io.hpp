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

    static bool importProject( std::string const& filepath, Sprite & targetSprite, EditorState & editorState, std::unordered_map<std::string, std::vector<ftxui::Color>> & palettes );

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
                   EditorState const& editorState,
                   std::unordered_map<std::string, std::vector<ftxui::Color>> const& palettes );

    enum class ExportFormat
    {
        PNG,
        JPG,
        ASCII
    };
    static bool
    exportImage( std::string const& filepath, Sprite const& targetSprite, std::string const& format = "png" );

private:
};


}
