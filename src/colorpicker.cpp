#include "colorpicker.hpp"
#include <ftxui/dom/elements.hpp>


namespace Termisprite
{

ftxui::Element
ColorPickerComponent::OnRender()
{
    const int maxValue = 255;
    const int valueIncrement = 8;
    const int hueIncrement = 6;

    int currentY = 0;

    ftxui::Elements gridRows;

    for ( int value = 0; value < maxValue; value += 2 * valueIncrement )
    {
        ftxui::Elements line;

        for ( int hue = 0; hue < maxValue; hue += hueIncrement )
        {
            line.push_back(
                ftxui::text( "▀" )
                | ftxui::color( ftxui::Color::HSV( hue, M_saturation, value ) )
                | ftxui::bgcolor( ftxui::Color::HSV( hue, M_saturation, value + valueIncrement ) )
            );
        }
        gridRows.push_back( ftxui::hbox( std::move( line ) ) );
    }

    ftxui::Element colorGrid = ftxui::vbox( std::move( gridRows ) ) | ftxui::reflect( M_box );


    return ftxui::vbox({
        colorGrid,
        ftxui::separatorEmpty(),
        M_saturationSlider->Render()
    });
}


bool
ColorPickerComponent::OnEvent( ftxui::Event event )
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

                int hue = selectedX * 6;
                int value = selectedY * 16;

                M_targetColor = ftxui::Color::HSV(hue, M_saturation, value);


                return true;
            }
        }
    }
    return ComponentBase::OnEvent(event);
}



std::shared_ptr<ColorPickerComponent>
ColorPicker( ftxui::Color & targetColor )
{
    return std::make_shared<ColorPickerComponent>( targetColor );
}


ftxui::Element
ColorPaletteComponent::OnRender()
{
    if ( M_editorState.palette.empty() )
        return ftxui::vbox({
            ftxui::text( " Palette " ) | ftxui::color( ftxui::Color::White ),
            ftxui::text( "" )
        });

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

    return ftxui::vbox({
        ftxui::text( " Palette " ),
        M_container->Render()
    }) | ftxui::size( ftxui::HEIGHT, ftxui::LESS_THAN, 6  );
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
