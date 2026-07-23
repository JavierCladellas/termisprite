#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>

#include "termisprite.hpp"
#include "editor.hpp"


namespace Termisprite
{


Termisprite::Termisprite()
{
    M_editorCanvas = EditorCanvas( 32, 32 );

    M_menu = Menu({
        { "File", {"New [Ctrl+N]", "Open [Ctrl+O]", "Save [Ctrl+S]", "Import [Ctrl+P]", "Export [Ctrl+E]" ,"Quit [Ctrl+Q]"}, [this](int idx) {
            switch (idx) {
                case 0: M_showNewProjectModal = true; break;
                case 1: M_showOpenProjectModal = true; break;
                case 2: M_showSaveModal = true; break;
                case 3: M_showImportModal = true; break;
                case 4: M_showExportModal = true; break;
                case 5: exit(0); break;
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
        { "Canvas", {"Resize [H]", "Flip Vertical", "Flip Horizontal", "Background [Ctrl+B]"}, [this](int idx) {
            switch (idx)
            {
                case 0: M_showResizeModal = true; break;
                case 1: M_editorCanvas->flipVertical(); break;
                case 2: M_editorCanvas->flipHorizontal(); break;
                case 3: M_showBackgroundColorModal = true; break;
            }
        }},
        { "View", { "Zoom In", "Zoom Out", "Toggle Grid [g]", "Toggle Checkerboard [G]","Toggle Pan [M]"}, [this](int idx) {
            switch (idx)
            {
                case 0: /* Zoom In */ break;
                case 1: /* Zoom Out */ break;
                case 2: M_editorCanvas->toggleGrid(); break;
                case 3: M_editorCanvas->toggleCheckerboardGrid(); break;
                case 4:
                    if ( M_editorCanvas->currentState().toolType == ToolType::PAN )
                        M_tools->selectTool(ToolType::DRAW);
                    else
                        M_tools->selectTool(ToolType::PAN);
                    break;
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
        { "Help", {"About","Shortcuts"}, [this](int idx) {
             switch(idx)
             {
                case 0: M_showAboutModal = true; break;
                case 1: M_showShortcutsModal = true; break;
             }
        }}
    });
    M_tools = ToolsSection( M_editorCanvas->currentState() );
    M_colorSection = ColorSection( M_editorCanvas->currentState() );
    M_statusBar = StatusBar( M_editorCanvas->currentState() );

    M_newProjectModal = std::make_shared<NewProjectModal>( *M_editorCanvas, [this]{ M_showNewProjectModal = false; });
    M_openProjectModal = std::make_shared<OpenProjectModal>( *M_editorCanvas, [this]{ M_showOpenProjectModal = false; });
    M_saveModal = std::make_shared<SaveModal>( *M_editorCanvas, [this]{ M_showSaveModal = false; });
    M_exportModal = std::make_shared<ExportModal>( *M_editorCanvas, [this]{ M_showExportModal = false; });
    M_resizeModal = std::make_shared<ResizeModal>( *M_editorCanvas, [this]{ M_showResizeModal = false; });
    M_aboutModal = std::make_shared<AboutModal>( [this]{ M_showAboutModal = false; });
    M_shortcutsModal = std::make_shared<ShortcutsModal>( [this]{ M_showShortcutsModal = false; });

    M_editorCanvas->onBackgroundChangeRequested = [this] { M_showBackgroundColorModal = true; };
    M_backgroundColorModal = std::make_shared<BackgroundColorModal>( M_editorCanvas->currentState(), [this]{ M_showBackgroundColorModal = false; });

    M_importModal = std::make_shared<ImportModal>( *M_editorCanvas, [this]{ M_showImportModal = false; });

    ftxui::Component toolsContainer = ftxui::Container::Vertical({ M_tools, M_colorSection });
    ftxui::Component baseContainer = ftxui::Container::Vertical({
        M_menu,
        ftxui::Container::Horizontal({ M_editorCanvas, toolsContainer }),
        M_statusBar
    });

    ftxui::Component mainLayoutRenderer = ftxui::Renderer(baseContainer, [this] {
        return ftxui::dbox({
            ftxui::vbox({
                M_menu->Render(),
                ftxui::separatorEmpty(),
                ftxui::hbox({
                    M_editorCanvas->Render() | ftxui::flex,
                    ftxui::vbox({
                        M_tools->Render(),
                        M_colorSection->Render()
                    })
                }) | ftxui::flex,
                ftxui::filler(),
                M_statusBar->Render()
            }),
            M_menu->RenderOverlay()
        });
    });

    M_masterComponent = ftxui::Modal(mainLayoutRenderer, M_newProjectModal, &M_showNewProjectModal);
    M_masterComponent |= ftxui::Modal(M_saveModal, &M_showSaveModal);
    M_masterComponent |= ftxui::Modal(M_exportModal, &M_showExportModal);
    M_masterComponent |= ftxui::Modal(M_openProjectModal, &M_showOpenProjectModal);
    M_masterComponent |= ftxui::Modal(M_resizeModal, &M_showResizeModal);
    M_masterComponent |= ftxui::Modal(M_aboutModal, &M_showAboutModal);
    M_masterComponent |= ftxui::Modal(M_shortcutsModal, &M_showShortcutsModal);
    M_masterComponent |= ftxui::Modal(M_backgroundColorModal, &M_showBackgroundColorModal);
    M_masterComponent |= ftxui::Modal(M_importModal, &M_showImportModal);

    ftxui::ComponentBase::Add(M_masterComponent);
}


ftxui::Element
Termisprite::OnRender()
{
    return M_masterComponent->Render();
}


bool
Termisprite::OnEvent( ftxui::Event event )
{
    if ( ftxui::ComponentBase::OnEvent( event ) )
        return true;

    if ( event == ftxui::Event::CtrlQ )
        exit(0);

    if ( event == ftxui::Event::CtrlN )
    {
        M_showNewProjectModal = true;
        return true;
    }

    if ( event == ftxui::Event::CtrlO )
    {
        M_showOpenProjectModal = true;
        return true;
    }

    if ( event == ftxui::Event::CtrlS )
    {
        M_showSaveModal = true;
        return true;
    }

    if ( event == ftxui::Event::CtrlB )
    {
        M_showBackgroundColorModal = true;
        return true;
    }


    if ( event == ftxui::Event::CtrlE )
    {
        M_showExportModal = true;
        return true;
    }

    if ( event == ftxui::Event::Character('H') )
    {
        M_showResizeModal = true;
        return true;
    }

    if ( event == ftxui::Event::CtrlP )
    {
        M_showImportModal = true;
        return true;
    }


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

    return false;

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
    if ( event == ftxui::Event::Character('g')  )
    {
        M_editorCanvas->toggleGrid();
        return true;
    }

    if ( event == ftxui::Event::Character('G') )
    {
        M_editorCanvas->toggleCheckerboardGrid();
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
    if ( event == ftxui::Event::Character('m') || event == ftxui::Event::Character('M') )
    {
        if ( M_editorCanvas->currentState().toolType == ToolType::PAN )
            M_tools->selectTool(ToolType::DRAW);
        else
            M_tools->selectTool(ToolType::PAN);
        return true;
    }

    return false;
}


}
