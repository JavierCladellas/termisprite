#pragma once

#include <string>
#include <vector>

#include <ftxui/screen/color.hpp>

namespace Termisprite
{

struct Pixel
{
    std::string brush = " ";
    ftxui::Color color = ftxui::Color::White;
};

class Sprite
{
public:
    using GridData = std::vector<std::vector<Pixel>>;
public:
    Sprite( int width = 64, int height = 32 )
        : M_width( width ), M_height( height )
    {
        M_grid.resize( M_height, std::vector<Pixel>( M_width ) );
    }


    std::vector<Pixel> operator[]( int y ) { return M_grid[y]; }
    Pixel & at( int x, int y ) { return M_grid[y][x]; }
    //iterator
    GridData::iterator begin() { return M_grid.begin(); }
    GridData::iterator end() { return M_grid.end(); }
    GridData::const_iterator begin() const { return M_grid.begin(); }
    GridData::const_iterator end() const { return M_grid.end(); }


private:
    int M_width, M_height;
    GridData M_grid;

};


}
