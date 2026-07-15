#include "tools.hpp"
#include <ftxui/dom/elements.hpp>


namespace Termisprite
{

ToolsComponent::ToolsComponent( EditorState & editorState )
    : M_editorState( editorState )
{
    ftxui::MenuOption drawingOption;
    drawingOption.on_change = [this] { M_activeCategory = Category::DRAWING; updateState(); };

    ftxui::MenuOption shapeOption;
    shapeOption.on_change = [this] { M_activeCategory = Category::SHAPES; updateState(); };

    ftxui::MenuOption toolOption;
    toolOption.on_change = [this] { M_activeCategory = Category::TOOLS; updateState(); };

    M_drawingMenu = ftxui::Menu( &M_drawingModes, &M_drawingIndex, drawingOption );
    M_shapeMenu   = ftxui::Menu( &M_shapeModes,   &M_shapeIndex,   shapeOption );
    M_toolMenu    = ftxui::Menu( &M_toolModes,    &M_toolIndex,    toolOption );

    M_container = ftxui::Container::Vertical({ M_drawingMenu, M_shapeMenu, M_toolMenu });

    ftxui::ComponentBase::Add( M_container );

    updateState();
}

void
ToolsComponent::updateState()
{
    std::string selectedMode;

    if ( M_activeCategory == Category::DRAWING )
        selectedMode = M_drawingModes[M_drawingIndex];
    else if ( M_activeCategory == Category::SHAPES )
        selectedMode = M_shapeModes[M_shapeIndex];
    else if ( M_activeCategory == Category::TOOLS )
        selectedMode = M_toolModes[M_toolIndex];

    if ( selectedMode != "Box Select" )
        M_editorState.selection.isActive = false;

    if ( selectedMode != "Eraser" )
        M_editorState.brush = "█";

    if ( selectedMode == "Draw" )
        M_editorState.toolType = ToolType::DRAW;
    else if ( selectedMode == "Eraser" )
    {
        M_editorState.toolType = ToolType::ERASER;
        M_editorState.brush = " ";
    }
    else if ( selectedMode == "Rectangle" )
        M_editorState.toolType = ToolType::SQUARE;
    else if ( selectedMode == "Ellipse" )
        M_editorState.toolType = ToolType::CIRCLE;
    else if ( selectedMode == "Line" )
        M_editorState.toolType = ToolType::LINE;
    else if ( selectedMode == "Eye Dropper" )
        M_editorState.toolType = ToolType::EYE_DROPPER;
    else if ( selectedMode == "Paint Fill" )
        M_editorState.toolType = ToolType::PAINT_FILL;
    else if ( selectedMode == "Box Select" )
        M_editorState.toolType = ToolType::BOX_SELECT;
}

ftxui::Element
ToolsComponent::OnRender()
{
    using namespace ftxui;
    auto renderMenu = [this](ftxui::Component& menu, Category cat) {
        if (M_activeCategory == cat)
            return menu->Render();
        return menu->Render() | dim;
    };

    return window( text(" Tools ") | bold | center,
        vbox({
            text(" Drawing ") | bold | color(Color::Cyan),
            separatorLight(),
            renderMenu(M_drawingMenu, Category::DRAWING),

            separatorEmpty(),

            text(" Shapes ") | bold | color(Color::Cyan),
            separatorLight(),
            renderMenu(M_shapeMenu, Category::SHAPES),

            separatorEmpty(),

            text(" Tools ") | bold | color(Color::Cyan),
            separatorLight(),
            renderMenu(M_toolMenu, Category::TOOLS),
        })
    );
}

std::shared_ptr<ToolsComponent> ToolsSection( EditorState & editorState )
{
    return std::make_shared<ToolsComponent>( editorState );
}

}
