#pragma once

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

#include <ftxui/screen/color.hpp>

namespace Termisprite
{

struct Pixel
{
    std::string brush = " ";
    ftxui::Color color = ftxui::Color::RGB(255, 255, 255);
};

class Sprite
{
public:
    using GridData = std::vector<std::vector<Pixel>>;
public:
    Sprite( int width = 32, int height = 32 )
        : M_width( width ), M_height( height )
    {
        M_grid.resize( M_height, std::vector<Pixel>( M_width ) );
    }


    std::vector<Pixel> & operator[]( int y ) { return M_grid[y]; }
    Pixel & at( int x, int y )
    {
        if ( x < 0 || x >= M_width || y < 0 || y >= M_height )
            throw std::out_of_range("Sprite::at: Coordinates out of bounds");
        return M_grid[y][x];
    }

    Pixel const& at( int x, int y ) const
    {
        if ( x < 0 || x >= M_width || y < 0 || y >= M_height )
            throw std::out_of_range("Sprite::at: Coordinates out of bounds");
        return M_grid[y][x];
    }

    GridData::iterator begin() { return M_grid.begin(); }
    GridData::iterator end() { return M_grid.end(); }
    GridData::const_iterator begin() const { return M_grid.begin(); }
    GridData::const_iterator end() const { return M_grid.end(); }

    void clear() { M_grid.assign(M_height, std::vector<Pixel>(M_width)); };
    void resize( int width, int height );

    void flipVertical()
    {
        std::reverse( M_grid.begin(), M_grid.end() );
    }

    void flipHorizontal()
    {
        for ( auto & row : M_grid )
            std::reverse( row.begin(), row.end() );
    }

    std::pair<int, int> size() const { return { M_width, M_height }; }


private:
    int M_width, M_height;
    GridData M_grid;

};


}
