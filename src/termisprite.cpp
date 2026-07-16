#include <ftxui/dom/elements.hpp>

#include "termisprite.hpp"


namespace Termisprite
{

Termisprite::Termisprite()
{
    M_editorCanvas = EditorCanvas( 64, 32 );

    M_menu = Menu({
        { "File", {"New [Ctrl+N]", "Open [Ctrl+O]", "Save [Ctrl+S]", "Import [Ctrl+I]", "Export [Ctrl+E]" ,"Quit [Ctrl+Q]"}, [](int idx) {
            switch (idx) {
                case 0: /* New */ break;
                case 1: /* Open */ break;
                case 2: /* Save */ break;
                case 3: exit(0); break;
            }
        }},
        { "Edit", {"Undo [Ctrl+Z]", "Redo [Ctrl+Y]", "Clear [Ctrl+D]"}, [this](int idx) {
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
        { "View", { "Zoom In", "Zoom Out", "Toggle Grid [G]", "Toggle Pan"}, [this](int idx) {
            switch (idx)
            {
                case 0: /* Zoom In */ break;
                case 1: /* Zoom Out */ break;
                case 2: M_editorCanvas->toggleGrid(); break;
                case 3: /* Toggle Pan */ break;
            }
        }},
        { "Tool", {"Brush [B]", "Eraser [E]", "Rectangle [R]", "Ellipse [C]", "Line [L]", "Eye Dropper [I]", "Paint Fill [F]", "Box Select [S]"}, [this](int idx) {
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


bool
Termisprite::OnEvent( ftxui::Event event )
{

    if ( processClipboardEvents( event ) )
        return true;

    if ( processHistoryEvents( event ) )
        return true;

    if ( processToggleGrid( event ) )
        return true;

    if ( processClearSelection( event ) )
        return true;

    if ( processToolSelection( event ) )
        return true;

    return ftxui::ComponentBase::OnEvent( event );

}


bool
Termisprite::processClipboardEvents( ftxui::Event event )
{
    if ( event == ftxui::Event::CtrlC || event == ftxui::Event::Character('y') )
    {
        if ( !M_editorCanvas->currentState().selection.isActive )
            return false;
        M_editorCanvas->copyToClipboard();
        return true;
    }

    if ( event == ftxui::Event::CtrlX || event == ftxui::Event::Character('x') )
    {
        if ( !M_editorCanvas->currentState().selection.isActive )
            return false;
        M_editorCanvas->cutToClipboard();
        return true;
    }

    if ( event == ftxui::Event::CtrlV || event == ftxui::Event::Character('p') )
    {
        if ( !M_editorCanvas->currentState().clipboard.hasData )
            return false;
        M_editorCanvas->pasteClipboard();
        return true;
    }

    return false;
}

bool
Termisprite::processHistoryEvents( ftxui::Event event )
{
    if ( event == ftxui::Event::CtrlZ || event == ftxui::Event::Character('u') )
    {
        M_editorCanvas->undo();
        return true;
    }

    if ( event == ftxui::Event::CtrlY || event == ftxui::Event::CtrlR )
    {
        M_editorCanvas->redo();
        return true;
    }

    return false;
}


bool
Termisprite::processToggleGrid( ftxui::Event event )
{
    if ( event == ftxui::Event::Character('g') || event == ftxui::Event::Character('G') )
    {
        M_editorCanvas->toggleGrid();
        return true;
    }
    return false;
}

bool
Termisprite::processClearSelection( ftxui::Event event )
{
    if ( event == ftxui::Event::CtrlD )
    {
        if ( M_editorCanvas->currentState().selection.isActive )
        {
            M_editorCanvas->deleteSelection();
            return true;
        }
        else
        {
            M_editorCanvas->clear();
            return true;
        }
    }
    return false;
}


bool
Termisprite::processToolSelection( ftxui::Event event )
{

    if ( event == ftxui::Event::Character('b') || event == ftxui::Event::Character('B') )
    {
        M_tools->selectTool(ToolType::DRAW);
        return true;
    }
    if ( event == ftxui::Event::Character('e') || event == ftxui::Event::Character('E') )
    {
        M_tools->selectTool(ToolType::ERASER);
        return true;
    }

    if ( event == ftxui::Event::Character('r') || event == ftxui::Event::Character('R') )
    {
        M_tools->selectTool(ToolType::SQUARE);
        return true;
    }
    if ( event == ftxui::Event::Character('c') || event == ftxui::Event::Character('C') )
    {
        M_tools->selectTool(ToolType::CIRCLE);
        return true;
    }
    if ( event == ftxui::Event::Character('L') )
    {
        M_tools->selectTool(ToolType::LINE);
        return true;
    }

    if ( event == ftxui::Event::Character('i') || event == ftxui::Event::Character('I') )
    {
        M_tools->selectTool(ToolType::EYE_DROPPER);
        return true;
    }
    if ( event == ftxui::Event::Character('f') || event == ftxui::Event::Character('F') )
    {
        M_tools->selectTool(ToolType::PAINT_FILL);
        return true;
    }
    if ( event == ftxui::Event::Character('s') || event == ftxui::Event::Character('S') )
    {
        M_tools->selectTool(ToolType::BOX_SELECT);
        return true;
    }

    return false;
}


}
