#pragma once

#include "cursor.hpp"
#include "sprite.hpp"
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <string>

#include <ftxui/screen/color.hpp>


namespace Termisprite
{

class BrushTool
{
public:
    BrushTool() {}

    void apply( Sprite & sprite, int targetX, int targetY );
    void apply( Pixel & pixel )
    {
        pixel.brush = M_currentChar;
        pixel.color = M_color;
    }
    bool processKeyboardEvent( Sprite & sprite, CanvasCursor & cursor, ftxui::Event event );

    void setCurrentBrush( std::string const& currentChar ){ M_currentChar = currentChar; }
    void setColor( ftxui::Color const& color ){ M_color = color; }

    bool operator==( Pixel const& pixel ) const { return pixel.brush == M_currentChar && pixel.color == M_color; }

    ftxui::Color & activeColor(){ return M_color; }
    ftxui::Color const& activeColor() const { return M_color; }

    int & size() { return M_size; }
    int size() const { return M_size; }

    std::string & selectedChar(){ return M_selectedChar; }
    std::string & currentChar(){ return M_currentChar; }
    std::string const& selectedChar() const { return M_selectedChar; }
    std::string const& currentChar() const { return M_currentChar; }

    void updateCurrentBrush(){ M_currentChar = M_selectedChar; }

private:
    std::string M_currentChar = "█";
    std::string M_selectedChar = "█";
    int M_size = 1;
    ftxui::Color M_color = ftxui::Color::RGB(255, 255, 255);

};


}
