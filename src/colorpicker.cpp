#include "colorpicker.hpp"
#include <ftxui/dom/elements.hpp>


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
            M_colorPicker->TakeFocus();
            return true;
        }
    }

    if ( M_colorPicker->Focused() )
    {
        if ( event == ftxui::Event::TabReverse )
        {
            if ( M_colorPicker->OnEvent(event) )
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

ftxui::Element
ColorSectionComponent::OnRender()
{

    ftxui::Color borderColor = Focused() ? ftxui::Color::Cyan : ftxui::Color::White;

    return ftxui::window( ftxui::text(" Color ") | ftxui::bold | ftxui::center,
        ftxui::vbox({
            M_colorPicker->Render(),
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
