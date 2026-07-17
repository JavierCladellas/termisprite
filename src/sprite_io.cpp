#include "sprite_io.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"


namespace Termisprite
{


bool SpriteImporter::importImage(const std::string& filepath, Sprite& targetSprite, int targetWidth, int targetHeight)
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
    //TODO:
    return true;
}


bool
SpriteExporter::exportImage( std::string const& filepath, Sprite const& targetSprite, ExportFormat format )
{
    //TODO
    return true;
}



}
