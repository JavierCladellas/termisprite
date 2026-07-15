#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <optional>
#include "editor.hpp"


namespace Termisprite
{


class ToolsComponent
    : public ftxui::ComponentBase
{
public:
    ToolsComponent( EditorState & editorState )
        : M_editorState( editorState )
    {
        ftxui::ComponentBase::Add( M_brushModeSelector );
    }

    ftxui::Element OnRender() override;

private:

    std::vector<std::string> M_brushModes{ "Draw","Eraser","Square","Circle","Line","Eye Dropper", "Paint Fill", "Box Select" };
    int M_selectedBrushModeIndex = 0;

    ftxui::Component M_brushModeSelector = ftxui::Menu( &M_brushModes, &M_selectedBrushModeIndex );

    EditorState & M_editorState;

};


std::shared_ptr<ToolsComponent> ToolsSection( EditorState & editorState );


}
