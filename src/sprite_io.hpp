#pragma once

#include <string>

#include <nlohmann/json.hpp>

#include "editor.hpp"
#include "sprite.hpp"

namespace Termisprite
{

enum class ImageFormat { PNG, JPG, ASCII, BMP };
enum class PaletteFormat { PNG, GPL };

class SpriteImporter
{
public:
    SpriteImporter() = default;

    static bool importImage( std::string const& filepath, Sprite & targetSprite, int targetWidth, int targetHeight, std::string const& format = "png" );
  
    static bool
    importProject( std::string const& filepath, Sprite & targetSprite, EditorState & editorState, std::unordered_map<std::string, std::vector<ftxui::Color>> & palettes );


    static bool
    importPalette( std::string const& filepath, std::string const& paletteName, std::unordered_map<std::string, std::vector<ftxui::Color>> & palettes, std::string const& format = "gpl" );

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

    static bool
    exportImage( std::string const& filepath, Sprite const& targetSprite, std::string const& format = "png" );

    static bool
    exportPalette( std::string const& filepath, std::vector<ftxui::Color> const& palette, std::string const& format = "gpl", std::string const& paletteName = "Unnamed Palette" );

private:
};


}
