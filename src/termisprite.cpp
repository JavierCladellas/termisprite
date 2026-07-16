#include <ftxui/dom/elements.hpp>

#include "termisprite.hpp"


namespace Termisprite
{

Termisprite::Termisprite()
{
    M_editorCanvas = EditorCanvas( 64, 32 );

    M_menu = Menu({
        { "File", {"New", "Open", "Save", "Import", "Export" ,"Quit"}, [](int idx) {
            switch (idx) {
                case 0: /* New */ break;
                case 1: /* Open */ break;
                case 2: /* Save */ break;
                case 3: exit(0); break;
            }
        }},
        { "Edit", {"Undo", "Redo", "Clear"}, [this](int idx) {
            switch (idx)
            {
                case 0: M_editorCanvas->undo(); break;
                case 1: M_editorCanvas->redo(); break;
                case 2: M_editorCanvas->clear(); break;
            }
        }},
        { "Canvas", {"Resize", "Flip Vertical", "Flip Horizontal", "Rotate 90°"}, [this](int idx) {
            switch (idx)
            {
                case 0: /* Resize */ break;
                case 1: break;
                case 2: break;
                case 3: break;
            }
        }},
        { "View", { "Zoom In", "Zoom Out", "Toggle Grid", "Toggle Pan"}, [this](int idx) {
            switch (idx)
            {
                case 0: /* Zoom In */ break;
                case 1: /* Zoom Out */ break;
                case 2: M_editorCanvas->toggleGrid(); break;
                case 3: /* Toggle Pan */ break;
            }
        }},
        { "Tool", {"Brush", "Eraser", "Rectangle", "Ellipse", "Line", "Eye Dropper", "Paint Fill", "Box Select"}, [this](int idx) {
            switch (idx)
            {
                case 0: M_tools->selectTool( ToolType::DRAW ); break;
                case 1: M_tools->selectTool( ToolType::ERASER ); break;
                case 2: M_tools->selectTool( ToolType::SQUARE ); break;
                case 3: M_tools->selectTool( ToolType::CIRCLE ); break;
                case 4: M_tools->selectTool( ToolType::LINE ); break;
                case 5: M_tools->selectTool( ToolType::EYE_DROPPER); break;
                case 6: M_tools->selectTool( ToolType::PAINT_FILL ); break;
                case 7: M_tools->selectTool( ToolType::BOX_SELECT ); break;
            }
        }},
        { "Help", {"About","Shortcuts"}, [](int idx) {
             switch(idx)
             {
                case 0: /* About */ break;
                case 1: /* Shortcuts */ break;
             }
        }}
    });
    M_tools = ToolsSection( M_editorCanvas->currentState() );
    M_colorSection = ColorSection( M_editorCanvas->currentState() );
    M_statusBar = StatusBar( M_editorCanvas->currentState() );

    ftxui::Component toolsContainer = ftxui::Container::Vertical({ M_tools, M_colorSection });

    M_masterComponent = ftxui::Container::Vertical({
        M_menu,
        ftxui::Container::Horizontal({ M_editorCanvas, toolsContainer }),
        M_statusBar
    });

    ftxui::ComponentBase::Add(M_masterComponent);
}


ftxui::Element
Termisprite::OnRender()
{
    return ftxui::dbox({
        ftxui::vbox({
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
        }),
        M_menu->RenderOverlay()
    });
}



}
