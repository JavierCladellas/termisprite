#include "colorpicker.hpp"
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/terminal.hpp>


namespace Termisprite
{

void
ColorGridComponent::updateColorFromCursor()
{
    int hue = M_cursorX * 6;
    int value = M_cursorY * 16;
    M_targetColor = ftxui::Color::HSV(hue, M_saturation, value);
}

ftxui::Element
ColorGridComponent::OnRender()
{
    const int maxValue = 255;
    const int valueIncrement = 16;
    const int hueIncrement = 6;

    ftxui::Elements gridRows;
    int gridY = 0;

    for ( int value = 0; value < maxValue; value += valueIncrement )
    {
        ftxui::Elements line;
        int gridX = 0;

        for ( int hue = 0; hue < maxValue; hue += hueIncrement )
        {
            bool isCursor = Focused() && ( gridX == M_cursorX ) && ( gridY == M_cursorY );

            std::string textContent = isCursor ? "X" : "█";

            ftxui::Element cell = ftxui::text( textContent ) | ftxui::color( ftxui::Color::HSV( hue, M_saturation, value ) );

            if (isCursor)
                cell = cell | ftxui::inverted;

            line.push_back( cell );
            gridX++;
        }
        gridRows.push_back( ftxui::hbox( std::move( line ) ) );
        gridY++;
    }

    return ftxui::vbox( std::move( gridRows ) ) | ftxui::reflect( M_box );
}


bool
ColorGridComponent::OnEvent( ftxui::Event event )
{
    if ( event.is_mouse() )
    {
        auto mouse = event.mouse();
        if ( mouse.button == ftxui::Mouse::Button::Left && mouse.motion == ftxui::Mouse::Pressed ) 
        {
            if ( M_box.Contain( mouse.x, mouse.y ) )
            {
                int selectedX = mouse.x - M_box.x_min;
                int selectedY = mouse.y - M_box.y_min;

                M_cursorX = std::max(0, std::min(42, selectedX));
                M_cursorY = std::max(0, std::min(15, selectedY));

                TakeFocus();
                updateColorFromCursor();
                return true;
            }
        }
    }

    if ( Focused() )
    {
        if (event == ftxui::Event::ArrowLeft && M_cursorX > 0)
        {
            M_cursorX--;
            updateColorFromCursor();
            return true;
        }
        if (event == ftxui::Event::ArrowRight && M_cursorX < 42)
        {
            M_cursorX++;
            updateColorFromCursor();
            return true;
        }
        if (event == ftxui::Event::ArrowUp && M_cursorY > 0)
        {
            M_cursorY--;
            updateColorFromCursor();
            return true;
        }
        if (event == ftxui::Event::ArrowDown && M_cursorY < 15)
        {
            M_cursorY++;
            updateColorFromCursor();
            return true;
        }
        if (event == ftxui::Event::Character(' ') || event == ftxui::Event::Return)
        {
            updateColorFromCursor();
            return true;
        }
    }

    return false;
}


ftxui::Element
ColorPickerComponent::OnRender()
{

    auto focusIndicator = M_saturationSlider->Focused() ? ftxui::text("> ") | ftxui::color(ftxui::Color::Cyan) | ftxui::bold : ftxui::text("  ");

    return ftxui::vbox({
        M_gridComponent->Render(),
        ftxui::separatorEmpty(),
        ftxui::hbox({
            focusIndicator,
            M_saturationSlider->Render()
        })
    });
}

bool
ColorPickerComponent::OnEvent(ftxui::Event event)
{
    if (M_gridComponent->Focused())
    {
        if ( event == ftxui::Event::Tab )
        {
            M_saturationSlider->TakeFocus();
            return true;
        }

        if ( event == ftxui::Event::TabReverse )
        {
            return false;
        }

        if ( event == ftxui::Event::ArrowUp )
        {
            if ( M_gridComponent->OnEvent(event ))
                return true;

            return false;
        }
    }

    if ( M_saturationSlider->Focused() )
    {
        if ( event == ftxui::Event::Tab )
            return false;

        if ( event == ftxui::Event::TabReverse )
        {
            M_gridComponent->TakeFocus();
            return true;
        }

        if ( event == ftxui::Event::ArrowDown )
            return false;
    }

    return ComponentBase::OnEvent(event);
}


std::shared_ptr<ColorPickerComponent>
ColorPicker( ftxui::Color & targetColor )
{
    return std::make_shared<ColorPickerComponent>( targetColor );
}

int
TerminalPaletteComponent::gridWidth() const
{
    return M_maxColors <= 16 ? 8 : 36;
}

int
TerminalPaletteComponent::gridHeight() const
{
    return M_maxColors <= 16 ? 2 : 8;
}

int
TerminalPaletteComponent::getIndex(int x, int y) const
{
    if (M_maxColors <= 16)
    {
        if (x < 0 || x >= 8 || y < 0 || y >= 2)
            return -1;

        return y * 8 + x;
    }

    // ANSI 256 color layout:
    //
    // 0..15                : standard colors
    // 16..231              : 6x6x6 RGB cube
    // 232..255             : grayscale

    if (x < 0 || x >= 36 || y < 0 || y >= 8)
        return -1;

    if (y == 0)
    {
        return (x < 16) ? x : -1;
    }

    if (y >= 1 && y <= 6)
    {
        const int r = x / 6;
        const int g = y - 1;
        const int b = x % 6;

        return 16 + (r * 36) + (g * 6) + b;
    }

    // y == 7
    return (x < 24) ? 232 + x : -1;
}

void
TerminalPaletteComponent::updateColorFromCursor()
{
    int index = getIndex(M_cursorX, M_cursorY);
    if ( index != -1 && index < M_maxColors )
        M_targetColor = ftxui::Color::Palette256(index);
}

ftxui::Element TerminalPaletteComponent::OnRender()
{
    ftxui::Elements gridRows;
    for (int y = 0; y < gridHeight(); ++y)
    {
        ftxui::Elements line;
        for (int x = 0; x < gridWidth(); ++x)
        {
            int index = getIndex(x, y);
            if (index == -1)
            {
                line.push_back(ftxui::text(" "));
                continue;
            }

            bool isCursor = Focused() && (x == M_cursorX) && (y == M_cursorY);
            std::string textContent = isCursor ? "X" : "█";

            ftxui::Element cell = ftxui::text(textContent) | ftxui::color(ftxui::Color::Palette256(index));

            if (isCursor)
                cell = cell | ftxui::inverted;

            line.push_back(cell);
        }
        gridRows.push_back(ftxui::hbox(std::move(line)));
    }
    return ftxui::vbox(std::move(gridRows)) | ftxui::reflect(M_box);
}

bool
TerminalPaletteComponent::OnEvent(ftxui::Event event)
{
    if (event.is_mouse())
    {
        auto mouse = event.mouse();

        if (mouse.button == ftxui::Mouse::Button::Left &&
            mouse.motion == ftxui::Mouse::Pressed &&
            M_box.Contain(mouse.x, mouse.y))
        {
            const int x = mouse.x - M_box.x_min;
            const int y = mouse.y - M_box.y_min;

            if (getIndex(x, y) != -1)
            {
                M_cursorX = x;
                M_cursorY = y;

                TakeFocus();
                updateColorFromCursor();
                return true;
            }
        }
    }

    if (!Focused())
        return false;

    if (event == ftxui::Event::ArrowLeft)
    {
        if (getIndex(M_cursorX - 1, M_cursorY) != -1)
        {
            --M_cursorX;
            updateColorFromCursor();
            return true;
        }

        return false;
    }

    if (event == ftxui::Event::ArrowRight)
    {
        if (getIndex(M_cursorX + 1, M_cursorY) != -1)
        {
            ++M_cursorX;
            updateColorFromCursor();
            return true;
        }

        return false;
    }

    if (event == ftxui::Event::ArrowUp)
    {
        if (getIndex(M_cursorX, M_cursorY - 1) != -1)
        {
            --M_cursorY;
            updateColorFromCursor();
            return true;
        }

        return false;
    }

    if (event == ftxui::Event::ArrowDown)
    {
        if (getIndex(M_cursorX, M_cursorY + 1) != -1)
        {
            ++M_cursorY;
            updateColorFromCursor();
            return true;
        }

        return false;
    }

    if (event == ftxui::Event::Character(' ') ||
        event == ftxui::Event::Return)
    {
        updateColorFromCursor();
        return true;
    }

    if (event == ftxui::Event::Tab ||
        event == ftxui::Event::TabReverse)
    {
        return false;
    }

    return false;
}


bool
ColorSectionComponent::OnEvent(ftxui::Event event)
{
    if (event == ftxui::Event::Tab)
    {
        if (M_tabToggle->Focused())
        {
            M_tabContainer->TakeFocus();
            return true;
        }

        if (M_tabContainer->Focused())
        {
            if (M_trueColorPicker && M_trueColorPicker->Focused())
            {
                if (M_trueColorPicker->OnEvent(event))
                    return true;
            }

            M_colorPalette->TakeFocus();
            return true;
        }

        if (M_colorPalette->Focused())
            return false;
    }

    if (event == ftxui::Event::TabReverse)
    {
        if (M_colorPalette->Focused())
        {
            M_tabContainer->TakeFocus();
            return true;
        }

        if (M_tabContainer->Focused())
        {

            if (M_trueColorPicker && M_trueColorPicker->Focused())
            {
                if ( M_trueColorPicker->OnEvent(event) )
                    return true;
            }

            M_tabToggle->TakeFocus();
            return true;
        }

        if (M_tabToggle->Focused())
            return false;
    }

    return ComponentBase::OnEvent(event);
}


ColorSectionComponent::ColorSectionComponent( ftxui::Color & activeColor, std::vector<ftxui::Color> const& colorsInCanvas )
{
    ftxui::Components tabComponents;
    auto colorSupport = ftxui::Terminal::ColorSupport();

    bool hasTrueColor = colorSupport == ftxui::Terminal::Color::TrueColor;
    bool has256 = hasTrueColor || ( colorSupport == ftxui::Terminal::Color::Palette256 );
    bool has16 = has256 || ( colorSupport == ftxui::Terminal::Color::Palette16  );

    if (hasTrueColor)
    {
        M_trueColorPicker = ColorPicker(activeColor);
        M_tabNames.push_back("True Color");
        tabComponents.push_back(M_trueColorPicker);
    }

    if ( has256 )
    {
        M_palette256 = std::make_shared<TerminalPaletteComponent>(activeColor, 256, 36);
        M_tabNames.push_back("256");
        tabComponents.push_back(M_palette256);
    }
    if ( has16 )
    {
        M_palette16 = std::make_shared<TerminalPaletteComponent>(activeColor, 16, 8);
        M_tabNames.push_back("16");
        tabComponents.push_back(M_palette16);
    }

    M_colorPalette = ColorPalette(activeColor, colorsInCanvas);

    M_tabToggle = ftxui::Toggle(&M_tabNames, &M_tabIndex);

    M_tabContainer = ftxui::Container::Tab(tabComponents, &M_tabIndex);

    auto baseContainer = ftxui::Container::Vertical({ M_tabToggle, M_tabContainer, M_colorPalette });

    M_mainContainer = ftxui::Renderer(baseContainer,[this,&activeColor]{
        return ftxui::window( ftxui::text(" Color ") | ftxui::bold | ftxui::center,
            ftxui::vbox({
                M_tabToggle->Render(),
                ftxui::separator(),
                M_tabContainer->Render(),
                ftxui::separator(),
                ftxui::hbox({
                    ftxui::text( " Active Color: " ) | ftxui::color( ftxui::Color::White ),
                    ftxui::text( " " ) | ftxui::center
                                       | ftxui::bgcolor( activeColor )
                                       | ftxui::size( ftxui::HEIGHT, ftxui::EQUAL, 1 )
                                       | ftxui::xflex
                }),
                ftxui::separator(),
                M_colorPalette->Render()
            })
        ) | ftxui::color( Focused() ? ftxui::Color::Cyan : ftxui::Color::White );
    });

    M_mainContainer = M_colorPalette->applyModals( M_mainContainer );
    Add(M_mainContainer);
}

ftxui::Element
ColorSectionComponent::OnRender()
{
    if ( M_tabNames.empty() )
    {
        ftxui::Color borderColor = Focused() ? ftxui::Color::Cyan : ftxui::Color::White;
        return ftxui::window( ftxui::text(" Color ") | ftxui::bold | ftxui::center,
           ftxui::text("No color support")
        ) | ftxui::color( borderColor );
    }

    return M_mainContainer->Render();

}


std::shared_ptr<ColorSectionComponent> ColorSection( ftxui::Color & activeColor, std::vector<ftxui::Color> const& colorsInCanvas )
{
    return std::make_shared<ColorSectionComponent>( activeColor, colorsInCanvas );
}



}
