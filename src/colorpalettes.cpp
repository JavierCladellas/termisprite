#include "modals.hpp"
#include <ftxui/component/component.hpp>

#include "colorpalettes.hpp"



namespace Termisprite
{

ColorPaletteComponent::ColorPaletteComponent( EditorState & editorState )
    : M_editorState( editorState )
{
    M_newPaletteModal = std::make_shared<NewPaletteModal>( M_palettes, [this] {
        M_showNewPaletteModal = false;
        rebuildPaletteTabs();
    } );
    M_importPaletteModal = std::make_shared<ImportPaletteModal>( M_palettes, [this] {
        M_showImportPaletteModal = false;
        rebuildPaletteTabs();
    } );

    M_createPaletteButton = ftxui::Button( "[+ Create]", [this] { M_showNewPaletteModal = true; }, ftxui::ButtonOption::Ascii() );
    M_importPaletteButton = ftxui::Button( "[+ Import]", [this] { M_showImportPaletteModal = true; }, ftxui::ButtonOption::Ascii() );

    M_colorsInCanvasContainer = ftxui::Container::Vertical({});

    M_paletteTabToggle = ftxui::Toggle( &M_paletteNames, &M_paletteTabIndex );

    M_paletteTabContainer = ftxui::Container::Tab( {}, &M_paletteTabIndex );

    auto baseContainer = ftxui::Container::Vertical({
        ftxui::Container::Horizontal({ M_createPaletteButton, M_importPaletteButton }),
        M_paletteTabToggle,
        M_paletteTabContainer,
        M_colorsInCanvasContainer
    });

    M_container = ftxui::Renderer(baseContainer, [this] {
        return ftxui::vbox({
            ftxui::text( " Palettes " ) | ftxui::color( ftxui::Color::White ),
            ftxui::hbox({
                M_createPaletteButton->Render() | ftxui::color( ftxui::Color::White ) | ftxui::dim,
                M_importPaletteButton->Render() | ftxui::color( ftxui::Color::White ) | ftxui::dim
            }),

            M_paletteNames.empty() ? ftxui::text("") : M_paletteTabToggle->Render() | ftxui::color( ftxui::Color::White ),
            M_paletteNames.empty() ? ftxui::text("") : M_paletteTabContainer->Render() | ftxui::color( ftxui::Color::White ),

            M_colorsInCanvasContainer->ChildCount() > 0 ? ftxui::text( " In Canvas" ) : ftxui::text("") | ftxui::color( ftxui::Color::White ) | ftxui::dim,
            M_colorsInCanvasContainer->ChildCount() > 0 ? M_colorsInCanvasContainer->Render() : ftxui::text("")
        }) | ftxui::size( ftxui::HEIGHT, ftxui::LESS_THAN, 6  );
    });

    ftxui::ComponentBase::Add( M_container );
}


ftxui::Component
ColorPaletteComponent::applyModals(ftxui::Component main) {
    return main
        | ftxui::Modal(M_newPaletteModal, &M_showNewPaletteModal)
        | ftxui::Modal(M_importPaletteModal, &M_showImportPaletteModal);
}

void
ColorPaletteComponent::rebuildPaletteTabs()
{
    M_paletteNames.clear();
    M_paletteTabContainer->DetachAllChildren();

    const int columns = 8;

    for ( auto const& [paletteName, colors] : M_palettes )
    {
        M_paletteNames.push_back( paletteName );

        auto grid = ftxui::Container::Vertical({});
        auto currentRow = ftxui::Container::Horizontal({});

        for ( size_t i = 0; i < colors.size(); ++i )
        {
            auto btn = buildColorButton( colors[i] );
            currentRow->Add( btn );

            if ( (i + 1) % columns == 0 )
            {
                grid->Add( currentRow );
                currentRow = ftxui::Container::Horizontal({});
            }
        }

        auto addButton = ftxui::Button( "[+]", [this, paletteName] {
            M_palettes[paletteName].push_back( M_editorState.color );
            rebuildPaletteTabs();
        }, ftxui::ButtonOption::Ascii() );
        currentRow->Add( addButton );

        grid->Add( currentRow );

        M_paletteTabContainer->Add( grid );
    }

    if (M_paletteTabIndex >= M_paletteNames.size() && !M_paletteNames.empty())
        M_paletteTabIndex = M_paletteNames.size() - 1;
}

ftxui::Component
ColorPaletteComponent::buildColorButton( ftxui::Color color )
{
    ftxui::ButtonOption option;
    option.transform = [color]( const ftxui::EntryState& s ) {
        auto block = ftxui::text( "██" ) | ftxui::color( color );

        if ( s.focused )
            return ftxui::hbox({ ftxui::text("["), block, ftxui::text("]") });

        return ftxui::hbox({ ftxui::text(" "), block, ftxui::text(" ") });
    };

    return ftxui::Button("", [this, color] { M_editorState.color = color; }, option);
}

void
ColorPaletteComponent::rebuildColorsInCanvas()
{
    M_colorsInCanvasContainer->DetachAllChildren();

    auto grid = ftxui::Container::Vertical({});
    auto currentRow = ftxui::Container::Horizontal({});
    const int columns = 8;

    for ( size_t i = 0; i < M_editorState.palette.size(); ++i )
    {
        ftxui::Color color = M_editorState.palette[i];

        auto btn = buildColorButton( color );
        currentRow->Add( btn );

        if ( (i + 1) % columns == 0 )
        {
            grid->Add( currentRow );
            currentRow = ftxui::Container::Horizontal({});
        }
    }

    if ( M_editorState.palette.size() % columns != 0 )
        grid->Add( currentRow );

    M_colorsInCanvasContainer->Add( grid );
    M_lastColorsInCanvas = M_editorState.palette;
}


ftxui::Element
ColorPaletteComponent::OnRender()
{
    if ( M_editorState.palette != M_lastColorsInCanvas )
        this->rebuildColorsInCanvas();

    return M_container->Render();
}


std::shared_ptr<ColorPaletteComponent>
ColorPalette( EditorState & editorState )
{
    return std::make_shared<ColorPaletteComponent>( editorState );
}

}
