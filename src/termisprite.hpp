#pragma once

#include "colorpicker.hpp"
#include "editor.hpp"
#include "menu.hpp"
#include "statusbar.hpp"
#include "tools.hpp"

#include <ftxui/component/component_base.hpp>


namespace Termisprite
{

class Termisprite
    : public ftxui::ComponentBase
{
public:
    Termisprite();

    ftxui::Element OnRender() override;

    bool OnEvent( ftxui::Event event ) override;

private:

    bool processHistoryEvents( ftxui::Event event );
    bool processClipboardEvents( ftxui::Event event );
    bool processToggleGrid( ftxui::Event event );
    bool processClearSelection( ftxui::Event event );
    bool processToolSelection( ftxui::Event event );


private:
    ftxui::Component M_masterComponent;

    std::shared_ptr<EditorCanvasComponent> M_editorCanvas;
    std::shared_ptr<MenuComponent> M_menu;
    std::shared_ptr<ToolsComponent> M_tools;
    std::shared_ptr<ColorSectionComponent> M_colorSection;
    std::shared_ptr<StatusBarComponent> M_statusBar;


};


}
