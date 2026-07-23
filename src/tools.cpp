#include "tools.hpp"
#include <ftxui/dom/elements.hpp>

namespace Termisprite
{

ToolsComponent::ToolsComponent( EditorState & editorState )
    : M_editorState( editorState )
{
    auto drawGroup = ftxui::Container::Vertical({
        makeToolButton("✎", "Draw", 'B', ToolType::DRAW),
        makeToolButton("✖", "Eraser", 'E', ToolType::ERASER)
    });

    auto shapeGroup = ftxui::Container::Vertical({
        makeToolButton("▭", "Rectangle", 'R', ToolType::SQUARE),
        makeToolButton("◯", "Ellipse", 'C', ToolType::CIRCLE),
        makeToolButton("╱", "Line", 'L', ToolType::LINE)
    });

    auto utilGroup = ftxui::Container::Vertical({
        makeToolButton("◧", "Picker", 'I', ToolType::EYE_DROPPER),
        makeToolButton("▼", "Fill", 'F', ToolType::PAINT_FILL),
        makeToolButton("⬚", "Select", 'S', ToolType::BOX_SELECT)
    });

    M_container = ftxui::Container::Vertical({ drawGroup, shapeGroup, utilGroup });

    ftxui::ComponentBase::Add( M_container );
}

ftxui::Component
ToolsComponent::makeToolButton( std::string icon, std::string name, char shortcut, ToolType type )
{
    ftxui::ButtonOption option;

    option.transform = [this, icon, name, shortcut, type](const ftxui::EntryState& s)
    {
        bool isActive = (M_editorState.toolType == type);

        auto prefix = isActive ? ftxui::text("▶ ") | ftxui::bold : ftxui::text("  ");

        auto content = ftxui::hbox({
            prefix,
            ftxui::text(icon + " " + name) | ftxui::flex,
            ftxui::text(std::string(1, shortcut)) | ftxui::dim
        });

        if (isActive)
            content |= ftxui::color(ftxui::Color::Cyan) | ftxui::bold;
        else
            content |= ftxui::color( ftxui::Color::White );


        if (s.focused)
            return content | ftxui::inverted;

        return content;
    };

    return ftxui::Button("", [this, type] { selectTool(type); }, option);
}


bool
ToolsComponent::OnEvent( ftxui::Event event )
{
    if (event == ftxui::Event::Tab || event == ftxui::Event::TabReverse)
        return false;

    return ftxui::ComponentBase::OnEvent(event);
}

void
ToolsComponent::selectTool( ToolType type )
{
    M_editorState.toolType = type;

    if ( M_editorState.toolType != ToolType::BOX_SELECT )
        M_editorState.selection.isActive = false;

    if ( M_editorState.toolType == ToolType::ERASER )
        M_editorState.brush = " ";
    else
        M_editorState.brush = "█";
}


ftxui::Element
ToolsComponent::OnRender()
{
    using namespace ftxui;


    ftxui::Color borderColor = Focused() ? ftxui::Color::Cyan : ftxui::Color::White;

    return window( text(" Tools ") | bold | center,
        vbox({
            text(" Drawing ") | dim | ftxui::color( ftxui::Color::White ),
            M_container->ChildAt(0)->Render(),

            separatorEmpty(),

            text(" Shapes ") | dim | ftxui::color( ftxui::Color::White ),
            M_container->ChildAt(1)->Render(),

            separatorEmpty(),

            text(" Utility ") | dim | ftxui::color( ftxui::Color::White ),
            M_container->ChildAt(2)->Render(),
        })
    ) | ftxui::color( borderColor );
}

std::shared_ptr<ToolsComponent> ToolsSection( EditorState & editorState )
{
    return std::make_shared<ToolsComponent>( editorState );
}

}
