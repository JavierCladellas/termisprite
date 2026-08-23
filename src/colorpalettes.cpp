#include "modals.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>

#include "colorpalettes.hpp"



namespace Termisprite
{

ColorPaletteComponent::ColorPaletteComponent( ftxui::Color & activeColor, std::vector<ftxui::Color> const& colorsInCanvas )
    : M_activeColor(activeColor), M_colorsInCanvas( colorsInCanvas)
{
    M_newPaletteModal = std::make_shared<NewPaletteModal>( M_palettes, [this] {
        M_showNewPaletteModal = false;
        rebuildPaletteTabs();
    } );
    M_importPaletteModal = std::make_shared<ImportPaletteModal>( M_palettes, [this] {
        M_showImportPaletteModal = false;
        rebuildPaletteTabs();
    } );
    M_exportPaletteModal = std::make_shared<ExportPaletteModal>( [this] { M_showExportPaletteModal = false; } );

    M_createPaletteButton = ftxui::Button( "[+ Create]", [this] { M_showNewPaletteModal = true; }, ftxui::ButtonOption::Ascii() );
    M_importPaletteButton = ftxui::Button( "[+ Import]", [this] { M_showImportPaletteModal = true; }, ftxui::ButtonOption::Ascii() );
    M_deletePaletteButton = ftxui::Button( "[Delete]", [this] {
        if ( !M_paletteNames.empty() )
        {
            std::string paletteToDelete = M_paletteNames[M_paletteTabIndex];
            M_palettes.erase( paletteToDelete );
            rebuildPaletteTabs();
        }
    }, ftxui::ButtonOption::Ascii() );
    M_exportPaletteButton = ftxui::Button( "[Export]", [this] {
        if ( !M_paletteNames.empty() )
        {
            std::string const& paletteName = M_paletteNames[M_paletteTabIndex];
            M_exportPaletteModal->setPalette( M_palettes[paletteName], paletteName );
            M_showExportPaletteModal = true;
        }
    }, ftxui::ButtonOption::Ascii() );

    M_colorsInCanvasContainer = ftxui::Container::Vertical({});

    M_paletteTabToggle = ftxui::Toggle( &M_paletteNames, &M_paletteTabIndex );

    M_paletteTabContainer = ftxui::Container::Tab( {}, &M_paletteTabIndex );

    M_container = ftxui::Container::Vertical({
        M_colorsInCanvasContainer,
        ftxui::Container::Horizontal({ M_createPaletteButton, M_importPaletteButton }),
        M_paletteTabToggle,
        M_paletteTabContainer,
        ftxui::Container::Horizontal({ M_exportPaletteButton, M_deletePaletteButton })
    });

    ftxui::ComponentBase::Add( M_container );
}


ftxui::Component
ColorPaletteComponent::applyModals(ftxui::Component main) {
    return main
        | ftxui::Modal(M_newPaletteModal, &M_showNewPaletteModal)
        | ftxui::Modal(M_importPaletteModal, &M_showImportPaletteModal)
        | ftxui::Modal(M_exportPaletteModal, &M_showExportPaletteModal);
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
            auto btn = buildColorButton( colors[i],[this, paletteName, i] {
                auto & palette = M_palettes[paletteName];
                if ( i < palette.size() )
                {
                    palette.erase( palette.begin() + i );
                    rebuildPaletteTabs();
                }
            });
            currentRow->Add( btn );

            if ( (i + 1) % columns == 0 )
            {
                grid->Add( currentRow );
                currentRow = ftxui::Container::Horizontal({});
            }
        }

        auto addButton = ftxui::Button( "[+]", [this, paletteName] {
            M_palettes[paletteName].push_back( M_activeColor );
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
ColorPaletteComponent::buildColorButton( ftxui::Color color, std::function<void()> onDelete )
{
    auto box = std::make_shared<ftxui::Box>();
    ftxui::ButtonOption option;
    option.transform = [color]( const ftxui::EntryState& s ) {
        auto block = ftxui::text( "██" ) | ftxui::color( color );

        if ( s.focused )
            return ftxui::hbox({ ftxui::text("["), block, ftxui::text("]") });

        return ftxui::hbox({ ftxui::text(" "), block, ftxui::text(" ") });
    };

    auto btn = ftxui::Button("", [this, color] { M_activeColor = color; }, option);

    btn |= ftxui::reflect( *box );

    btn |= ftxui::CatchEvent([btn, onDelete, box]( ftxui::Event event ) {
        bool isRightClick = event.is_mouse() &&
                           event.mouse().button == ftxui::Mouse::Button::Right &&
                           event.mouse().motion == ftxui::Mouse::Pressed &&
                           box->Contain(event.mouse().x, event.mouse().y);

        bool isDeleteKey = (event == ftxui::Event::Backspace || event == ftxui::Event::Delete);

        if ( isRightClick || isDeleteKey )
        {
            if ( onDelete )
                onDelete();
            return true;
        }
        return false;
    });
    return btn;
}

void
ColorPaletteComponent::rebuildColorsInCanvas()
{
    M_colorsInCanvasContainer->DetachAllChildren();

    auto grid = ftxui::Container::Vertical({});
    auto currentRow = ftxui::Container::Horizontal({});
    const int columns = 8;

    for ( size_t i = 0; i < M_colorsInCanvas.size(); ++i )
    {
        ftxui::Color color = M_colorsInCanvas[i];

        auto btn = buildColorButton( color );
        currentRow->Add( btn );

        if ( (i + 1) % columns == 0 )
        {
            grid->Add( currentRow );
            currentRow = ftxui::Container::Horizontal({});
        }
    }

    if ( M_colorsInCanvas.size() % columns != 0 )
        grid->Add( currentRow );

    M_colorsInCanvasContainer->Add( grid );
    M_lastColorsInCanvas = M_colorsInCanvas;
}


ftxui::Element
ColorPaletteComponent::OnRender()
{
    if ( M_colorsInCanvas != M_lastColorsInCanvas )
        this->rebuildColorsInCanvas();

    return ftxui::vbox({
        ftxui::text( " Palettes " ) | ftxui::color( Focused() ? ftxui::Color::Cyan : ftxui::Color::White ),

        M_colorsInCanvasContainer->ChildCount() > 0 ? ftxui::text( " In Canvas" ) : ftxui::emptyElement() | ftxui::color( ftxui::Color::White ) | ftxui::dim,
        M_colorsInCanvasContainer->ChildCount() > 0 ? M_colorsInCanvasContainer->Render() : ftxui::emptyElement(),

        ftxui::hbox({
            M_createPaletteButton->Render() | ftxui::color( ftxui::Color::White ) | ftxui::dim,
            M_importPaletteButton->Render() | ftxui::color( ftxui::Color::White ) | ftxui::dim
        }),

        M_paletteNames.empty() ? ftxui::emptyElement() : M_paletteTabToggle->Render() | ftxui::color( ftxui::Color::White ),
        M_paletteNames.empty() ? ftxui::emptyElement() : M_paletteTabContainer->Render() | ftxui::color( ftxui::Color::White ),

        ftxui::hbox({
            M_paletteNames.empty() ? ftxui::emptyElement() : M_exportPaletteButton->Render(),
            M_paletteNames.empty() ? ftxui::emptyElement() : M_deletePaletteButton->Render()
        }) | ftxui::color( ftxui::Color::White ) | ftxui::dim | ftxui::align_right
    });
}


std::shared_ptr<ColorPaletteComponent>
ColorPalette( ftxui::Color & activeColor, std::vector<ftxui::Color> const& colorsInCanvas )
{
    return std::make_shared<ColorPaletteComponent>( activeColor, colorsInCanvas );
}

}
