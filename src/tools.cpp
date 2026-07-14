#include "tools.hpp"


namespace Termisprite
{

ftxui::Element
ToolsComponent::OnRender()
{
    std::string selectedMode = M_brushModes[M_selectedBrushModeIndex];

    if ( selectedMode != "Box Select" )
        M_editorState.selection.isActive = false;


    if ( selectedMode == "line" )
    {
        M_editorState.toolType = ToolType::BRUSH;
        M_editorState.brush = "█";
    }
    else if ( selectedMode == "Eraser" )
    {
        M_editorState.toolType = ToolType::ERASER;
        M_editorState.brush = " ";
    }
    else if ( selectedMode == "Eye Dropper" )
        M_editorState.toolType = ToolType::EYE_DROPPER;
    else if ( selectedMode == "Paint Fill" )
        M_editorState.toolType = ToolType::PAINT_FILL;
    else if ( selectedMode == "Box Select" )
        M_editorState.toolType = ToolType::BOX_SELECT;

    return ftxui::window( ftxui::text(" Tools ") | ftxui::bold | ftxui::center,
        M_brushModeSelector->Render()
    );
}

std::shared_ptr<ToolsComponent>
ToolsSection( EditorState & editorState )
{
    return std::make_shared<ToolsComponent>( editorState );
}

}
