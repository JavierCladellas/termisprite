#include "sprite_io.hpp"
#include <fstream>
#include <unordered_map>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace Termisprite
{


bool SpriteImporter::importProject( std::string const& filepath, EditorCanvasComponent & editorCanvas, std::unordered_map<std::string, std::vector<ftxui::Color>> & palettes )
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

        editorCanvas.resize( w, h );
    }

    if ( importJson.contains( "palettes" ) && importJson["palettes"].is_object() )
    {
        palettes.clear();
        for ( auto const& [paletteName, colorArray] : importJson["palettes"].items() )
        {
            if ( colorArray.is_array() )
            {
                std::vector<ftxui::Color> colors;
                for ( auto const& colorCode : colorArray )
                {
                    unsigned int r = 255, g = 255, b = 255;
                    if ( sscanf( colorCode.get<std::string>().c_str(), "38;2;%u;%u;%u", &r, &g, &b ) == 3 )
                        colors.push_back(ftxui::Color::RGB(r, g, b));
                }
                palettes[paletteName] = colors;
            }
        }
    }


    if ( importJson.contains("background_color") )
    {
        std::string bgColorCode = importJson["background_color"];

        unsigned int r = 255, g = 255, b = 255;
        if ( sscanf( bgColorCode.c_str(), "48;2;%u;%u;%u", &r, &g, &b ) == 3 )
            editorCanvas.currentState().backgroundColor = ftxui::Color::RGB(r, g, b);

    }

    if ( importJson.contains( "layers" ) && importJson["layers"].is_array() )
    {
        while ( editorCanvas.layers().size() > 1 )
            editorCanvas.removeLayer( editorCanvas.layers().size() - 1 );

        bool isFirstLayer = true;
        auto const& layersJson = importJson["layers"];

        for ( auto it = layersJson.rbegin(); it != layersJson.rend(); ++it )
        {
            auto const& layerJson = *it;
            std::string layerName = layerJson.value("name", "Layer");

            auto [layerW, layerH] = editorCanvas.size(); //Fallback
            if ( layerJson.contains("size") && layerJson["size"].size() == 2 )
            {
                layerW = layerJson["size"][0];
                layerH = layerJson["size"][1];
            }

            Layer newLayer( editorCanvas.camera(), layerW, layerH, layerName );

            if ( layerJson.contains("position") && layerJson["position"].size() == 2 )
                newLayer.setPosition( layerJson["position"][0], layerJson["position"][1] );

            if ( layerJson.contains("visible") && !layerJson["visible"].get<bool>() )
                newLayer.toggleVisibility(); //Assumes default is visible

            if ( layerJson.contains("sprite") && layerJson["sprite"].is_array() )
            {
                for ( auto const& pixelJson : layerJson["sprite"] )
                {
                    int x = pixelJson.value("x", -1);
                    int y = pixelJson.value("y", -1);
                    std::string brush = pixelJson.value("brush", " ");
                    std::string colorCode = pixelJson.value("color", "");
                    unsigned int r = 255, g = 255, b = 255;

                    if ( x >= 0 && x < layerW && y >= 0 && y < layerH )
                    {
                        Pixel & cell = newLayer.at( x, y );
                        cell.brush = brush;
                        if ( sscanf( colorCode.c_str(), "38;2;%u;%u;%u", &r, &g, &b ) == 3 )
                            cell.color = ftxui::Color::RGB(r, g, b);
                    }
                }
            }

            if ( isFirstLayer )
            {
                editorCanvas.setActiveLayer(0);
                editorCanvas.activeLayer() = newLayer;
                isFirstLayer = false;
            }
            else
            {
                editorCanvas.addLayer(layerName);
                editorCanvas.activeLayer() = newLayer;
            }
        }
    }

    editorCanvas.saveState();

    if ( editorCanvas.onLayersChanged )
        editorCanvas.onLayersChanged();

    return true;
}


bool SpriteImporter::importImage( std::string const& filepath, Layer& targetLayer, int targetWidth, int targetHeight, std::string const& format )
{

    std::map<std::string, ImageFormat> formatMap = {
        {"png", ImageFormat::PNG},
        {"jpg", ImageFormat::JPG},
        {"jpeg", ImageFormat::JPG},
        {"bmp", ImageFormat::BMP},
        {"ascii", ImageFormat::ASCII},
        {"txt", ImageFormat::ASCII}
    };

    if (!formatMap.contains(format))
        return false;

    ImageFormat imageFormat = formatMap[format];

    targetLayer.resize(targetWidth, targetHeight);

    if ( imageFormat == ImageFormat::ASCII )
    {
        std::ifstream inFile(filepath);
        if (!inFile.is_open())
            return false;

        std::vector<std::string> lines;
        std::string line;
        while (std::getline(inFile, line))
        {
            //Windows
            if (!line.empty() && line.back() == '\r')
                line.pop_back();
            lines.push_back(line);
        }
        inFile.close();

        for (int y = 0; y < targetHeight; ++y)
        {
            std::string const& currentLine = (y < lines.size()) ? lines[y] : "";
            size_t charIndex = 0;

            for (int x = 0; x < targetWidth; ++x)
            {
                Pixel & cell = targetLayer.at(x, y);
                cell.color = ftxui::Color::White;
                if (charIndex < currentLine.length())
                {
                    //Multi-byte character handling (UTF-8)
                    unsigned char c = currentLine[charIndex];
                    int cplen = 1;
                    if ((c & 0xF8) == 0xF0)
                        cplen = 4;
                    else if ((c & 0xF0) == 0xE0)
                        cplen = 3;
                    else if ((c & 0xE0) == 0xC0)
                        cplen = 2;

                    if (charIndex + cplen > currentLine.length())
                        cplen = currentLine.length() - charIndex;

                    cell.brush = currentLine.substr(charIndex, cplen);
                    charIndex += cplen;
                }
                else
                    cell.brush = " ";
            }
        }

        return true;

    }




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

            Pixel & cell = targetLayer.at(x, y);

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
SpriteImporter::importPalette( std::string const& filepath, std::string const& paletteName, std::unordered_map<std::string, std::vector<ftxui::Color>> & palettes, std::string const& format  )
{
    std::map<std::string, PaletteFormat> formatMap = {
        {"png", PaletteFormat::PNG},
        {"gpl", PaletteFormat::GPL}
    };

    if ( !formatMap.contains(format) )
        return false;

    PaletteFormat paletteFormat = formatMap[format];

    std::string parsedFilepath = filepath;

    std::vector<ftxui::Color> extractedColors;
    switch( paletteFormat )
    {
        case PaletteFormat::PNG:
        {
            if ( !filepath.ends_with(".png") )
                parsedFilepath += ".png";

            int imgWidth, imgHeight, channels;
            unsigned char* imgData = stbi_load(parsedFilepath.c_str(), &imgWidth, &imgHeight, &channels, 4);

            if (!imgData)
                return false;

            for (int y = 0; y < imgHeight; ++y)
            {
                for (int x = 0; x < imgWidth; ++x)
                {
                    int index = (y * imgWidth + x) * 4;
                    unsigned char r = imgData[index];
                    unsigned char g = imgData[index + 1];
                    unsigned char b = imgData[index + 2];
                    unsigned char a = imgData[index + 3];

                    if (a >= 128)
                        extractedColors.push_back(ftxui::Color::RGB(r, g, b));
                }
            }

            stbi_image_free(imgData);
            break;
        }
        case PaletteFormat::GPL:
        {
            if ( !filepath.ends_with(".gpl") )
                parsedFilepath += ".gpl";

            std::ifstream inFile(parsedFilepath);
            if (!inFile.is_open())
                return false;

            std::string line;
            while (std::getline(inFile, line))
            {
                if (line.empty() || line[0] == '#')
                    continue;

                int r, g, b;
                if (sscanf(line.c_str(), "%d %d %d", &r, &g, &b) == 3)
                    extractedColors.push_back(ftxui::Color::RGB(r, g, b));
            }
            inFile.close();
            break;
        }
        default:
            return false;
    }

    palettes[paletteName] = extractedColors;
    return true;

}





bool
SpriteExporter::exportProject( std::string const& filepath,
                               std::string const& projectName,
                               EditorCanvasComponent const& editorCanvas,
                               std::unordered_map<std::string, std::vector<ftxui::Color>> const& palettes )
{
    nlohmann::json exportJson;

    exportJson["project_name"] = projectName;

    auto [w,h] = editorCanvas.size();
    exportJson["width"] = w;
    exportJson["height"] = h;

    exportJson["background_color"] = editorCanvas.currentState().backgroundColor.Print(true);

    exportJson["palettes"] = nlohmann::json::object();
    for ( auto const& [paletteName, colors] : palettes )
    {
        exportJson["palettes"][paletteName] = nlohmann::json::array();
        for ( auto const& color : colors )
            exportJson["palettes"][paletteName].push_back(color.Print(false));
    }


    exportJson["layers"] = nlohmann::json::array();
    for ( auto const& layer : editorCanvas.layers() )
    {
        if ( !layer ) continue;

        nlohmann::json layerJson = {
            {"name", layer->name()},
            {"visible", layer->isVisible()},
            {"position", {layer->position().first, layer->position().second}},
            {"size", {layer->size().first, layer->size().second}},
            {"sprite", nlohmann::json::array()}
        };
        auto [layerW, layerH] = layer->size();

        for ( int y = 0; y < layerH; ++y )
        {
            for ( int x = 0; x < layerW; ++x )
            {
                Pixel const& cell = layer->at(x,y);

                if (cell.brush == " " )
                    continue;


                layerJson["sprite"].push_back({
                    {"x", x},
                    {"y", y},
                    {"brush", cell.brush},
                    {"color", cell.color.Print(false)}
                });
            }
        }

        exportJson["layers"].push_back(layerJson);

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
SpriteExporter::exportImage( std::string const& filepath, std::vector<std::unique_ptr<Layer>> const& layers, std::string const& format )
{
    std::map<std::string, ImageFormat> formatMap = {
        {"png", ImageFormat::PNG},
        {"jpg", ImageFormat::JPG},
        {"jpeg", ImageFormat::JPG},
        {"bmp", ImageFormat::BMP},
        {"ascii", ImageFormat::ASCII}
    };
    if ( layers.empty() )
        return false;

    int w = 0, h = 0;
    for ( auto const& layer : layers )
    {
        auto [lw, lh] = layer->size();
        w = std::max(w, lw);
        h = std::max(h, lh);
    }

    if ( !formatMap.contains(format) )
        return false;

    ImageFormat exportFormat = formatMap[format];
    int channels;

    switch ( exportFormat )
    {
        case ImageFormat::PNG:
        case ImageFormat::BMP: channels = 4; break;
        case ImageFormat::JPG: channels = 3; break;
        case ImageFormat::ASCII: channels = 1; break;
    }

    std::vector<unsigned char> exportData(w * h * channels, 0);

    if ( exportFormat != ImageFormat::ASCII )
    {
        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                Pixel cell;
                for ( auto const& layer : layers )
                {
                    if ( !layer || !layer->isVisible() )
                        continue;
                    auto [lw, lh] = layer->size();
                    if ( x >= lw || y >= lh )
                        continue;
                    cell = layer->at(x,y);
                    if ( cell.brush != " " )
                        break;
                }
                int index = (y * w + x) * channels;

                if (cell.brush == " ")
                    for ( int i = 0; i < 3; i++ ) //RGB
                        exportData[index + i] = channels == 3 ? 255 : 0 ;   // R
                else
                {
                    unsigned int r = 255, g = 255, b = 255;
                    std::string colorCode = cell.color.Print(false);
                    sscanf(colorCode.c_str(), "38;2;%u;%u;%u", &r, &g, &b);

                    exportData[index + 0] = static_cast<unsigned char>(r);
                    exportData[index + 1] = static_cast<unsigned char>(g);
                    exportData[index + 2] = static_cast<unsigned char>(b);
                }

                if ( channels == 4 )
                    exportData[index + 3] = cell.brush == " " ? 0 : 255; //Alpha
            }
        }
    }

    int result = 0;
    std::string parsedFilepath = filepath;
    switch ( exportFormat )
    {
        case ImageFormat::PNG:
        {
            if (!parsedFilepath.ends_with(".png"))
                parsedFilepath += ".png";

            result = stbi_write_png(parsedFilepath.c_str(), w, h, channels, exportData.data(), w * channels);
            break;
        }
        case ImageFormat::JPG:
        {
            if (!parsedFilepath.ends_with(".jpg") && !parsedFilepath.ends_with(".jpeg")  )
                parsedFilepath += ".jpg";

            int quality = 100;
            result = stbi_write_jpg(parsedFilepath.c_str(), w, h, channels, exportData.data(), quality);
            break;
        }
        case ImageFormat::BMP:
        {
            if (!parsedFilepath.ends_with(".bmp"))
                parsedFilepath += ".bmp";

            result = stbi_write_bmp(parsedFilepath.c_str(), w, h, channels, exportData.data());
            break;
        }
        case ImageFormat::ASCII:
        {
            if (!parsedFilepath.ends_with(".txt") && !parsedFilepath.ends_with(".ascii"))
                parsedFilepath += ".txt";
            std::ofstream outFile( parsedFilepath );
            if ( !outFile.is_open() )
                return false;
            for ( int y = 0; y < h; ++y )
            {
                for ( int x = 0; x < w; ++x )
                {
                    Pixel cell;
                    for ( auto const& layer : layers )
                    {
                        if ( !layer || !layer->isVisible() ) continue;
                        auto [lw, lh] = layer->size();
                        if ( x >= lw || y >= lh )
                            continue;
                        cell = layer->at(x,y);
                        if ( cell.brush != " " )
                            break;
                    }

                    if ( !cell.brush.empty() )
                        outFile << cell.brush;
                }
                outFile << "\n";
            }
            outFile.close();
            result = 1;
            break;
        }

    }

    return result != 0;
}


bool
SpriteExporter::exportPalette( std::string const& filepath, std::vector<ftxui::Color> const& palettes, std::string const& format, std::string const& paletteName )
{
    std::map<std::string, PaletteFormat> formatMap = {
        {"png", PaletteFormat::PNG},
        {"gpl", PaletteFormat::GPL}
    };

    if ( !formatMap.contains(format) )
        return false;

    PaletteFormat paletteFormat = formatMap[format];

    std::string parsedFilepath = filepath;
    int result = 0;

    switch( paletteFormat )
    {
        case PaletteFormat::PNG:
        {
            if ( !filepath.ends_with(".png") )
                parsedFilepath += ".png";

            int width = palettes.size();
            int height = 1;

            std::vector<unsigned char> exportData(width * height * 4, 0);

            for (int x = 0; x < width; ++x)
            {
                Pixel cell;
                cell.brush = "█";
                cell.color = palettes[x];

                unsigned int r = 255, g = 255, b = 255;
                std::string colorCode = cell.color.Print(false);
                sscanf(colorCode.c_str(), "38;2;%u;%u;%u", &r, &g, &b);

                int index = (0 * width + x) * 4;
                exportData[index + 0] = static_cast<unsigned char>(r);
                exportData[index + 1] = static_cast<unsigned char>(g);
                exportData[index + 2] = static_cast<unsigned char>(b);
                exportData[index + 3] = 255;
            }

            result = stbi_write_png(parsedFilepath.c_str(), width, height, 4, exportData.data(), width * 4);
        }
        case PaletteFormat::GPL:
        {
            if ( !filepath.ends_with(".gpl") )
                parsedFilepath += ".gpl";

            std::ofstream outFile(parsedFilepath);
            if (!outFile.is_open())
                return false;

            outFile << "GIMP Palette\n";
            outFile << "#\n";
            outFile << "Name: " << paletteName << "\n";
            outFile << "Columns: " << palettes.size() << "\n";
            outFile << "#\n";

            for (const auto& color : palettes)
            {
                unsigned int r = 255, g = 255, b = 255;
                std::string colorCode = color.Print(false);
                sscanf(colorCode.c_str(), "38;2;%u;%u;%u", &r, &g, &b);

                outFile << r << " " << g << " " << b << "\n";
            }

            outFile.close();
            result = 1;
        }
        default:
            return false;
    }

    return result != 0;

}


}
