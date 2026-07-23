#include "sprite_io.hpp"
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

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
        std::string bgColorCode = importJson["background_color"];

        unsigned int r = 255, g = 255, b = 255;
        if ( sscanf( bgColorCode.c_str(), "48;2;%u;%u;%u", &r, &g, &b ) == 3 )
            editorState.backgroundColor = ftxui::Color::RGB(r, g, b);

    }

    if ( importJson.contains( "sprite" ) && importJson["sprite"].is_array() )
    {
        auto [w, h] = targetSprite.size();

        for ( auto const& item : importJson["sprite"] )
        {
            int x = item.value("x", -1);
            int y = item.value("y", -1);
            std::string brush = item.value("brush", " ");
            std::string colorCode = item.value("color", "");
            unsigned int r = 255, g = 255, b = 255;

            if ( x >= 0 && x < w && y >= 0 && y < h )
            {
                Pixel & cell = targetSprite.at( x, y );
                cell.brush = brush;
                if ( sscanf( colorCode.c_str(), "38;2;%u;%u;%u", &r, &g, &b ) == 3 )
                    cell.color = ftxui::Color::RGB(r, g, b);
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
    exportJson["background_color"] = editorState.backgroundColor.Print(true);
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
                {"color", cell.color.Print(false)}
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
SpriteExporter::exportImage( std::string const& filepath, Sprite const& targetSprite, std::string const& format )
{
    std::map<std::string, ExportFormat> formatMap = {
        {"png", ExportFormat::PNG},
        {"jpg", ExportFormat::JPG},
        {"jpeg", ExportFormat::JPG},
        {"ascii", ExportFormat::ASCII}
    };



    auto [w, h] = targetSprite.size();
    if (w <= 0 || h <= 0)
        return false;

    std::vector<unsigned char> exportData(w * h * 4, 0);

    switch ( formatMap[format] )
    {
        case ExportFormat::PNG:
            for (int y = 0; y < h; ++y)
            {
                for (int x = 0; x < w; ++x)
                {
                    Pixel const& cell = targetSprite.at(x, y);
                    int index = (y * w + x) * 4;

                    if (cell.brush == " ")
                    {
                        exportData[index + 0] = 0;   // R
                        exportData[index + 1] = 0;   // G
                        exportData[index + 2] = 0;   // B
                        exportData[index + 3] = 0;
                    }
                    else
                    {
                        unsigned int r = 255, g = 255, b = 255;
                        std::string colorCode = cell.color.Print(false);
                        sscanf(colorCode.c_str(), "38;2;%u;%u;%u", &r, &g, &b);

                        exportData[index + 0] = static_cast<unsigned char>(r);
                        exportData[index + 1] = static_cast<unsigned char>(g);
                        exportData[index + 2] = static_cast<unsigned char>(b);
                        exportData[index + 3] = 255;
                    }
                }
            }
            break;
        default:
            break;
    }

    std::string parsedFilepath = filepath;
    if (!parsedFilepath.ends_with(".png"))
        parsedFilepath += ".png";

    int result = stbi_write_png(parsedFilepath.c_str(), w, h, 4, exportData.data(), w * 4);

    return result != 0;
}



}
