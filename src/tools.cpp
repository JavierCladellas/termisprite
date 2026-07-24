#include "tools.hpp"
#include <ftxui/dom/elements.hpp>

namespace Termisprite
{

ToolsComponent::ToolsComponent( EditorState & editorState, ShortcutManager * shortcutManager )
    : M_editorState( editorState ), M_shortcutManager( shortcutManager )
{
    auto drawGroup = ftxui::Container::Vertical({
        makeToolButton("✎", shortcutManager->getShortcut(ShortcutType::SELECT_BRUSH_TOOL), ToolType::DRAW),
        makeToolButton("✖", shortcutManager->getShortcut(ShortcutType::SELECT_ERASER_TOOL), ToolType::ERASER)
    });

    auto shapeGroup = ftxui::Container::Vertical({
        makeToolButton("▭", shortcutManager->getShortcut(ShortcutType::SELECT_RECTANGLE_TOOL), ToolType::SQUARE),
        makeToolButton("◯", shortcutManager->getShortcut(ShortcutType::SELECT_ELLIPSE_TOOL), ToolType::CIRCLE),
        makeToolButton("╱", shortcutManager->getShortcut(ShortcutType::SELECT_LINE_TOOL), ToolType::LINE)
    });

    auto utilGroup = ftxui::Container::Vertical({
        makeToolButton("◧", shortcutManager->getShortcut(ShortcutType::SELECT_EYE_DROPPER_TOOL), ToolType::EYE_DROPPER),
        makeToolButton("▼", shortcutManager->getShortcut(ShortcutType::SELECT_PAINT_FILL_TOOL), ToolType::PAINT_FILL),
        makeToolButton("⬚", shortcutManager->getShortcut(ShortcutType::SELECT_BOX_SELECT_TOOL), ToolType::BOX_SELECT)
    });

    M_container = ftxui::Container::Vertical({ drawGroup, shapeGroup, utilGroup });

    ftxui::ComponentBase::Add( M_container );
}

ftxui::Component
ToolsComponent::makeToolButton( std::string icon, Shortcut const& shortcut, ToolType type )
{
    ftxui::ButtonOption option;

    std::string displayName = shortcut.displayName;
    std::string hotkeyText = shortcut.hotkeyText;

    option.transform = [this, icon, displayName, hotkeyText, type](const ftxui::EntryState& s)
    {
        bool isActive = (M_editorState.toolType == type);

        auto prefix = isActive ? ftxui::text("▶ ") | ftxui::bold : ftxui::text("  ");

        auto content = ftxui::hbox({
            prefix,
            ftxui::text(icon + " " + displayName) | ftxui::flex,
            ftxui::text( hotkeyText) | ftxui::dim
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

std::shared_ptr<ToolsComponent> ToolsSection( EditorState & editorState, ShortcutManager * shortcutManager )
{
    return std::make_shared<ToolsComponent>( editorState, shortcutManager );
}

}
