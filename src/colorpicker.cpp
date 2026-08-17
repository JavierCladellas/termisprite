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
ColorPickerComponent::OnEvent( ftxui::Event event )
{
    if ( M_saturationSlider->Focused() )
        if ( event == ftxui::Event::Tab || event == ftxui::Event::ArrowDown )
            return false;

    if ( M_gridComponent->Focused() )
    {
        if ( event == ftxui::Event::TabReverse )
            return false;

        if ( event == ftxui::Event::ArrowUp )
        {
            if ( M_gridComponent->OnEvent(event) )
                return true;

            return false;
        }
    }

    return ComponentBase::OnEvent(event);
}


std::shared_ptr<ColorPickerComponent>
ColorPicker( ftxui::Color & targetColor )
{
    return std::make_shared<ColorPickerComponent>( targetColor );
}

int
TerminalPaletteComponent::gridWidth() const  { return M_maxColors <= 16 ? 8 : 36; }

int
TerminalPaletteComponent::gridHeight() const { return M_maxColors <= 16 ? 2 : 8;  }

// Standard ANSI 256-color map layout
int
TerminalPaletteComponent::getIndex(int x, int y) const
{
    if (M_maxColors <= 16) {
        if (x < 0 || x >= 8 || y < 0 || y >= 2) return -1;
        return y * 8 + x;
    } else {
        if (x < 0 || x >= 36 || y < 0 || y >= 8) return -1;
        if (y == 0) {
            return (x < 16) ? x : -1;
        } else if (y >= 1 && y <= 6) {
            int r = x / 6;
            int g = y - 1;
            int b = x % 6;
            return 16 + (r * 36) + (g * 6) + b;
        } else if (y == 7) {
            return (x < 24) ? 232 + x : -1;
        }
    }
    return -1;
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
            ftxui::Element cell = ftxui::text(isCursor ? "X" : "█") | ftxui::color(ftxui::Color::Palette256(index));

            if (isCursor) cell = cell | ftxui::inverted;
            line.push_back(cell);
        }
        gridRows.push_back(ftxui::hbox(std::move(line)));
    }
    return ftxui::vbox(std::move(gridRows)) | ftxui::reflect(M_box);
}

bool TerminalPaletteComponent::OnEvent(ftxui::Event event)
{
    int rows = (M_maxColors + M_columns - 1) / M_columns;

    if (event.is_mouse())
    {
        auto mouse = event.mouse();
        if ( mouse.button == ftxui::Mouse::Button::Left && mouse.motion == ftxui::Mouse::Pressed )
        {
            if (M_box.Contain(mouse.x, mouse.y))
            {
                M_cursorX = mouse.x - M_box.x_min;
                M_cursorY = mouse.y - M_box.y_min;

                int index = (M_cursorY * M_columns) + M_cursorX;
                if ( index >= M_maxColors )
                    M_cursorX = (M_maxColors - 1) % M_columns;

                TakeFocus();
                updateColorFromCursor();
                return true;
            }
        }
    }

    if ( Focused() ) {
        if (event == ftxui::Event::ArrowLeft && M_cursorX > 0)
            M_cursorX--; updateColorFromCursor(); return true;

        if (event == ftxui::Event::ArrowRight && M_cursorX < M_columns - 1)
            if (((M_cursorY * M_columns) + M_cursorX + 1) < M_maxColors)
                M_cursorX++; updateColorFromCursor(); return true;

        if (event == ftxui::Event::ArrowUp && M_cursorY > 0)
            M_cursorY--; updateColorFromCursor(); return true;

        if (event == ftxui::Event::ArrowDown && M_cursorY < rows - 1)
            if ((((M_cursorY + 1) * M_columns) + M_cursorX) < M_maxColors)
                M_cursorY++; updateColorFromCursor(); return true;

        if (event == ftxui::Event::Character(' ') || event == ftxui::Event::Return)
            updateColorFromCursor(); return true;
    }
    return false;
}

ColorPaletteComponent::ColorPaletteComponent( EditorState & editorState )
    : M_editorState( editorState )
{
    ftxui::ComponentBase::Add( M_container );
}


void ColorPaletteComponent::rebuildPalette()
{
    M_container->DetachAllChildren();

    auto grid = ftxui::Container::Vertical({});
    auto currentRow = ftxui::Container::Horizontal({});
    const int columns = 8;

    for ( size_t i = 0; i < M_editorState.palette.size(); ++i )
    {
        ftxui::Color color = M_editorState.palette[i];

        ftxui::ButtonOption option;
        option.transform = [color]( const ftxui::EntryState& s ) {
            auto block = ftxui::text( "██" ) | ftxui::color( color );

            if ( s.focused )
                return ftxui::hbox({ ftxui::text("["), block, ftxui::text("]") });

            return ftxui::hbox({ ftxui::text(" "), block, ftxui::text(" ") });
        };

        auto btn = ftxui::Button("", [this, color] { M_editorState.color = color; }, option);

        currentRow->Add( btn );

        if ( (i + 1) % columns == 0 )
        {
            grid->Add( currentRow );
            currentRow = ftxui::Container::Horizontal({});
        }
    }

    if ( M_editorState.palette.size() % columns != 0 )
        grid->Add( currentRow );

    M_container->Add( grid );
    M_lastPalette = M_editorState.palette;
}


ftxui::Element
ColorPaletteComponent::OnRender()
{
    if ( M_editorState.palette != M_lastPalette )
        this->rebuildPalette();

    if ( M_editorState.palette.empty() )
        return ftxui::vbox({
            ftxui::text( " Palette " ) | ftxui::color( ftxui::Color::White ),
            ftxui::text( "" )
        });

    return ftxui::vbox({
        ftxui::text( " Palette " ),
        M_container->Render()
    }) | ftxui::size( ftxui::HEIGHT, ftxui::LESS_THAN, 6  );
}


bool
ColorSectionComponent::OnEvent( ftxui::Event event )
{
    if ( M_colorPalette->Focused() )
    {
        if ( event == ftxui::Event::Tab )
            return false;
        if ( event == ftxui::Event::TabReverse )
        {
            M_tabContainer->TakeFocus();
            return true;
        }
    }

    if ( M_tabContainer->Focused() )
    {
        if ( event == ftxui::Event::TabReverse )
        {
            if ( M_tabContainer->OnEvent(event) )
                return true;

            return false;
        }
    }

    return ComponentBase::OnEvent(event);
}

std::shared_ptr<ColorPaletteComponent>
ColorPalette( EditorState & editorState )
{
    return std::make_shared<ColorPaletteComponent>( editorState );
}


ColorSectionComponent::ColorSectionComponent( EditorState & editorState )
    : M_editorState( editorState )
{
    ftxui::Components tabComponents;
    auto colorSupport = ftxui::Terminal::ColorSupport();

    bool hasTrueColor = colorSupport == ftxui::Terminal::Color::TrueColor;
    bool has256 = hasTrueColor || ( colorSupport == ftxui::Terminal::Color::Palette256 );
    bool has16 = has256 || ( colorSupport == ftxui::Terminal::Color::Palette16  );

    if (hasTrueColor)
    {
        M_trueColorPicker = ColorPicker(editorState.color);
        M_tabNames.push_back("True Color");
        tabComponents.push_back(M_trueColorPicker);
    }

    if ( has256 )
    {
        M_palette256 = std::make_shared<TerminalPaletteComponent>(editorState.color, 256, 16); // 16x16 grid
        M_tabNames.push_back("256");
        tabComponents.push_back(M_palette256);
    }
    if ( has16 )
    {
        M_palette16 = std::make_shared<TerminalPaletteComponent>(editorState.color, 16, 8);    // 8x2 grid
        M_tabNames.push_back("16");
        tabComponents.push_back(M_palette16);
    }

    M_colorPalette = ColorPalette(editorState);

    M_tabToggle = ftxui::Toggle(&M_tabNames, &M_tabIndex);

    M_tabContainer = ftxui::Container::Tab(tabComponents, &M_tabIndex);

    M_mainContainer = ftxui::Container::Vertical({ M_tabToggle, M_tabContainer, M_colorPalette });

    Add(M_mainContainer);
}

ftxui::Element
ColorSectionComponent::OnRender()
{
    ftxui::Color borderColor = Focused() ? ftxui::Color::Cyan : ftxui::Color::White;

    if ( M_tabNames.empty() )
    {
        return ftxui::window( ftxui::text(" Color ") | ftxui::bold | ftxui::center,
           ftxui::text("No color support")
        ) | ftxui::color( borderColor );
    }

    return ftxui::window( ftxui::text(" Color ") | ftxui::bold | ftxui::center,
        ftxui::vbox({
            M_tabToggle->Render(),
            ftxui::separator(),
            M_tabContainer->Render(),
            ftxui::separator(),
            ftxui::hbox({
                ftxui::text( " Active Color: " ) | ftxui::color( ftxui::Color::White ),
                ftxui::text( " " ) | ftxui::center
                                   | ftxui::bgcolor( M_editorState.color )
                                   | ftxui::size( ftxui::HEIGHT, ftxui::EQUAL, 1 )
                                   | ftxui::xflex
            }),
            ftxui::separator(),
            M_colorPalette->Render()
        })
    ) | ftxui::color( borderColor );
}


std::shared_ptr<ColorSectionComponent> ColorSection( EditorState & editorState )
{
    return std::make_shared<ColorSectionComponent>( editorState );
}



}
