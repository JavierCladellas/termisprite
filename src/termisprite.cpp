#include <ftxui/dom/elements.hpp>

#include "termisprite.hpp"


namespace Termisprite
{

Termisprite::Termisprite()
{
    M_editorCanvas = EditorCanvas( 64, 32 );
    M_menu = Menu();
    M_tools = ToolsSection( M_editorCanvas->currentState() );
    M_colorSection = ColorSection( M_editorCanvas->currentState() );
    M_statusBar = StatusBar( M_editorCanvas->currentState() );

    ftxui::Component toolsContainer = ftxui::Container::Vertical({ M_tools, M_colorSection });

    M_masterComponent = ftxui::Container::Vertical({
        M_menu,
        ftxui::Container::Horizontal({ M_editorCanvas, M_tools }),
        M_statusBar
    });

    ftxui::ComponentBase::Add(M_masterComponent);
}


ftxui::Element
Termisprite::OnRender()
{
    return ftxui::vbox({
        M_menu->Render(),
        ftxui::separatorEmpty(),
        ftxui::hbox({
            ftxui::vbox({ M_editorCanvas->Render(), ftxui::filler() }) | ftxui::flex,
            ftxui::vbox({
                M_tools->Render(),
                M_colorSection->Render()
            })
        }),
        ftxui::filler(),
        M_statusBar->Render()
    });
}



}
