#include "sprite_io.hpp"
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"


namespace Termisprite
{


bool SpriteImporter::importProject( std::string const& filepath, Sprite & targetSprite, EditorState & editorState )
{
    std::ifstream inFile( filepath );
    if ( !inFile.is_open() )
        return false;

    nlohmann::json importJson;

    try
    {
        inFile >> importJson;
    }
    catch (...)
    {
        return false;
    }

    if ( importJson.contains( "width" ) && importJson.contains( "height" ) )
    {
        int w = importJson["width"];
        int h = importJson["height"];

        targetSprite.resize( w, h );
        targetSprite.clear();
    }

    if ( importJson.contains("background_color") )
    {
        nlohmann::json bgColor = importJson["background_color"];
        editorState.backgroundColor = ftxui::Color::RGB( bgColor.value("r", 0), bgColor.value("g", 0), bgColor.value("b", 0) );
    }

    if ( importJson.contains( "sprite" ) && importJson["sprite"].is_array() )
    {
        auto [w, h] = targetSprite.size();

        for ( auto const& item : importJson["sprite"] )
        {
            int x = item.value("x", -1);
            int y = item.value("y", -1);
            std::string brush = item.value("brush", " ");
            nlohmann::json color = item.value("color", nlohmann::json::object());

            if ( x >= 0 && x < w && y >= 0 && y < h )
            {
                Pixel & cell = targetSprite.at( x, y );
                cell.brush = brush;
                cell.color = ftxui::Color::RGB( color.value("r", 0), color.value("g", 0), color.value("b", 0) );
            }
        }
    }

    return true;
}


bool SpriteImporter::importImage( std::string const& filepath, Sprite& targetSprite, int targetWidth, int targetHeight)
{
    int imgWidth, imgHeight, channels;
    unsigned char* imgData = stbi_load(filepath.c_str(), &imgWidth, &imgHeight, &channels, 4);

    if (!imgData)
        return false;

    std::vector<unsigned char> resizedData(targetWidth * targetHeight * 4);

    stbir_resize_uint8_linear( imgData, imgWidth, imgHeight, 0,
                               resizedData.data(), targetWidth, targetHeight, 0,
                               STBIR_RGBA );

    for (int y = 0; y < targetHeight; ++y)
    {
        for (int x = 0; x < targetWidth; ++x)
        {
            int index = (y * targetWidth + x) * 4;
            unsigned char r = resizedData[index];
            unsigned char g = resizedData[index + 1];
            unsigned char b = resizedData[index + 2];
            unsigned char a = resizedData[index + 3];

            Pixel & cell = targetSprite.at(x, y);

            if (a < 128)
            {
                cell.brush = " ";
                cell.color = ftxui::Color::White;
            }
            else
            {
                cell.brush = "█";
                cell.color = ftxui::Color::RGB(r, g, b);
            }
        }
    }

    stbi_image_free(imgData);
    return true;
}

bool
SpriteExporter::exportProject( std::string const& filepath,
                               std::string const& projectName,
                               Sprite const& targetSprite,
                               EditorState const& editorState )
{
    nlohmann::json exportJson;

    exportJson["project_name"] = projectName;

    auto [w,h] = targetSprite.size();

    exportJson["width"] = w;
    exportJson["height"] = h;

    std::string backgroundColor;
    exportJson["background_color"] = {
        {"r", editorState.backgroundColor.Red},
        {"g", editorState.backgroundColor.Green},
        {"b", editorState.backgroundColor.Blue}
    };
    exportJson["sprite"] = nlohmann::json::array();

    for ( int y = 0; y < h; ++y )
    {
        for ( int x = 0; x < w; ++x )
        {
            Pixel const& cell = targetSprite.at(x,y);


            if (cell.brush == " " )
                continue;

            exportJson["sprite"].push_back({
                {"x", x},
                {"y", y},
                {"brush", cell.brush},
                {"color", {
                    {"r", cell.color.Red},
                    {"g", cell.color.Green},
                    {"b", cell.color.Blue}
                }}
            });
        }
    }

    std::string parsedFilepath = filepath;
    if (!parsedFilepath.ends_with(".json"))
        parsedFilepath += ".json";

    std::ofstream outFile(parsedFilepath);
    if (!outFile.is_open())
        return false;

    outFile << exportJson.dump(4, ' ', false, nlohmann::json::error_handler_t::replace);
    outFile.close();

    return true;
}


bool
SpriteExporter::exportImage( std::string const& filepath, Sprite const& targetSprite, ExportFormat format )
{
    //TODO
    return true;
}



}
