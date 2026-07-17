#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>

#include "termisprite.hpp"
#include "editor.hpp"


namespace Termisprite
{


NewProjectModal::NewProjectModal( EditorCanvasComponent & editorCanvas, std::function<void()> onClose)
    : M_editorCanvas( editorCanvas ), M_closeCallback( onClose )
{

    Add( ftxui::Container::Vertical({
        ftxui::Container::Vertical({M_projectNameInputComponent, M_widthInputComponent, M_heightInputComponent }),
        ftxui::Container::Horizontal({ M_cancelButton,  M_okButton})
    }) );
}


ftxui::Element
NewProjectModal::OnRender()
{
    using namespace ftxui;

    auto form = vbox({
        hbox({
            text(" Project Name: ") | dim | vcenter,
            M_projectNameInputComponent->Render() | border | size(WIDTH, EQUAL, 20)
        }),
        hbox({
            text(" Width:  ") | dim | vcenter,
            M_widthInputComponent->Render() | border | size(WIDTH, EQUAL, 10),
            text(" px") | dim | vcenter
        }),
        hbox({
            text(" Height: ") | dim | vcenter,
            M_heightInputComponent->Render() | border | size(WIDTH, EQUAL, 10),
            text(" px") | dim | vcenter
        }),
    });

    auto buttons = hbox({
        filler(),
        M_cancelButton->Render(),
        text(" "),
        M_okButton->Render()
    });

    return vbox({
        text(" New Project ") | bold | center,
        separator(),
        separatorEmpty(),
        form | center,
        separatorEmpty(),
        buttons
    }) | reflect(M_box)
       | size(WIDTH, GREATER_THAN, 40)
       | borderDouble
       | bgcolor(Color::Black)
       | clear_under;

}

bool
NewProjectModal::OnEvent( ftxui::Event event )
{
    if ( event == ftxui::Event::Return )
    {
        if ( M_widthInput.empty() )
            M_widthInput = "32";
        if ( M_heightInput.empty() )
            M_heightInput = "32";
        M_editorCanvas.clear();
        M_editorCanvas.resize(std::stoi(M_widthInput), std::stoi(M_heightInput));
        M_projectNameInput = "";
        M_widthInput = "";
        M_heightInput = "";
        M_closeCallback();
        return true;
    }
    if ( event == ftxui::Event::Escape )
    {
        M_closeCallback();
        return true;
    }

    if ( event.is_mouse() )
    {
        auto mouse = event.mouse();
        if ( mouse.button == ftxui::Mouse::Button::Left && mouse.motion == ftxui::Mouse::Pressed )
        {
            if ( !M_box.Contain( mouse.x, mouse.y ) )
            {
                M_closeCallback();
                return true;
            }
        }
    }

    return ftxui::ComponentBase::OnEvent( event );
}


ResizeModal::ResizeModal( EditorCanvasComponent & editorCanvas, std::function<void()> onClose)
    : M_editorCanvas( editorCanvas ), M_closeCallback( onClose )
{
    auto [width, height] = M_editorCanvas.size();
    M_placeholderWidth = std::to_string(width);
    M_placeholderHeight = std::to_string(height);
    M_widthInputComponent = ftxui::Input(&M_widthInput, &M_placeholderWidth);
    M_heightInputComponent = ftxui::Input(&M_heightInput, &M_placeholderHeight);

    M_okButton = ftxui::Button("OK", [this] {
        auto [width, height] = M_editorCanvas.size();
        M_placeholderWidth = std::to_string(width);
        M_placeholderHeight = std::to_string(height);
        if ( !M_widthInput.empty() )
            M_placeholderWidth = M_widthInput;
        if ( !M_heightInput.empty() )
            M_placeholderHeight = M_heightInput;
        M_editorCanvas.resize(std::stoi(M_placeholderWidth), std::stoi(M_placeholderHeight));
        M_widthInput = "";
        M_heightInput = "";
        M_closeCallback();
    });
    Add( ftxui::Container::Vertical({
        ftxui::Container::Vertical({ M_widthInputComponent, M_heightInputComponent }),
        ftxui::Container::Horizontal({ M_cancelButton,  M_okButton})
    }) );
}

ftxui::Element ResizeModal::OnRender()
{
    using namespace ftxui;

    auto form = vbox({
        hbox({
            text(" Width:  ") | dim | vcenter,
            M_widthInputComponent->Render() | border | size(WIDTH, EQUAL, 10),
            text(" px") | dim | vcenter
        }),
        hbox({
            text(" Height: ") | dim | vcenter,
            M_heightInputComponent->Render() | border | size(WIDTH, EQUAL, 10),
            text(" px") | dim | vcenter
        }),
    });

    auto buttons = hbox({
        filler(),
        M_cancelButton->Render(),
        text(" "),
        M_okButton->Render()
    });

    return vbox({
        text(" Resize Canvas ") | bold | center,
        separator(),
        separatorEmpty(),
        form | center,
        separatorEmpty(),
        buttons
    }) | reflect(M_box)
       | size(WIDTH, GREATER_THAN, 35)
       | borderDouble
       | bgcolor(Color::Black)
       | clear_under;
}

bool
ResizeModal::OnEvent( ftxui::Event event )
{

    auto [width, height] = M_editorCanvas.size();
    M_placeholderWidth = std::to_string(width);
    M_placeholderHeight = std::to_string(height);
    if ( event == ftxui::Event::Return )
    {
        if ( !M_widthInput.empty() )
            M_placeholderWidth = M_widthInput;
        if ( !M_heightInput.empty() )
            M_placeholderHeight = M_heightInput;
        M_editorCanvas.resize(std::stoi(M_placeholderWidth), std::stoi(M_placeholderHeight));
        M_widthInput = "";
        M_heightInput = "";
        M_closeCallback();
        return true;
    }
    if ( event == ftxui::Event::Escape )
    {
        M_closeCallback();
        return true;
    }

    if ( event.is_mouse() )
    {
        auto mouse = event.mouse();
        if ( mouse.button == ftxui::Mouse::Button::Left && mouse.motion == ftxui::Mouse::Pressed )
        {
            if ( !M_box.Contain( mouse.x, mouse.y ) )
            {
                M_closeCallback();
                return true;
            }
        }
    }

    return ftxui::ComponentBase::OnEvent( event );
}

AboutModal::AboutModal( std::function<void()> onClose )
    : M_closeCallback( onClose )
{
    M_closeButton = ftxui::Button("Close", [this] { M_closeCallback(); });
    ftxui::ComponentBase::Add(M_closeButton);
}


ftxui::Element
AboutModal::OnRender()
{
    using namespace ftxui;

    auto content = vbox({
        text("Termisprite") | bold | center,
        separator(),
        text("A terminal-based sprite editor built with FTXUI.") | center,
        separatorEmpty(),
        text("Version 1.0.0") | center,
        separatorEmpty(),
        text("Author: Javier Cladellas") | center,
        separatorEmpty(),
        M_closeButton->Render() | center
    }) | size(WIDTH, GREATER_THAN, 40)
       | borderDouble
       | bgcolor(Color::Black)
       | clear_under;

    return content;

}



ShortcutsModal::ShortcutsModal( std::function<void()> onClose )
    : M_closeCallback( onClose )
{
    M_closeButton = ftxui::Button("Close", [this] { M_closeCallback(); });
    ftxui::ComponentBase::Add(M_closeButton);
}

ftxui::Element
ShortcutsModal::OnRender()
{
    using namespace ftxui;

    auto content = vbox({
        text("Shortcuts") | bold | center,
        M_closeButton->Render() | center
    }) | size(WIDTH, GREATER_THAN, 40)
       | borderDouble
       | bgcolor(Color::Black)
       | clear_under;

    return content;
}






Termisprite::Termisprite()
{
    M_editorCanvas = EditorCanvas( 32, 32 );

    M_menu = Menu({
        { "File", {"New [Ctrl+N]", "Open [Ctrl+O]", "Save [Ctrl+S]", "Import [Ctrl+I]", "Export [Ctrl+E]" ,"Quit [Ctrl+Q]"}, [this](int idx) {
            switch (idx) {
                case 0: M_showNewProjectModal = true; break;
                case 1: /* Open */ break;
                case 2: /* Save */ break;
                case 3: /* Import */ break;
                case 4: /* Export */ break;
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
        { "Canvas", {"Resize [H]", "Flip Vertical", "Flip Horizontal"}, [this](int idx) {
            switch (idx)
            {
                case 0: M_showResizeModal = true; break;
                case 1: M_editorCanvas->flipVertical(); break;
                case 2: M_editorCanvas->flipHorizontal(); break;
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
    M_resizeModal = std::make_shared<ResizeModal>( *M_editorCanvas, [this]{ M_showResizeModal = false; });
    M_aboutModal = std::make_shared<AboutModal>( [this]{ M_showAboutModal = false; });
    M_shortcutsModal = std::make_shared<ShortcutsModal>( [this]{ M_showShortcutsModal = false; });

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
    M_masterComponent |= ftxui::Modal(M_resizeModal, &M_showResizeModal);
    M_masterComponent |= ftxui::Modal(M_aboutModal, &M_showAboutModal);
    M_masterComponent |= ftxui::Modal(M_shortcutsModal, &M_showShortcutsModal);

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

    if ( event == ftxui::Event::CtrlQ )
        exit(0);

    if ( event == ftxui::Event::CtrlN )
    {
        M_showNewProjectModal = true;
        return true;
    }

    if ( event == ftxui::Event::Character('H') )
    {
        M_showResizeModal = true;
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
