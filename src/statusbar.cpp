#include "statusbar.hpp"



namespace Termisprite
{

StatusBarComponent::StatusBarComponent( EditorState & editorState )
    : M_editorState( editorState )
{
    initializeShortcuts();
}

void
StatusBarComponent::initializeShortcuts()
{
    M_shortcuts = {
        { "g", "Toggle Grid", [](const EditorState&) { return true; } },
        { "Ctrl+Z/u", "Undo", [](const EditorState&) { return true; } },
        { "Ctrl+Y/Ctrl+r", "Redo", [](const EditorState&) { return true; } },
        { "Arrows/hjkl", "Move", [](const EditorState& s) { return !s.selection.isActive; } },
        { "Ctrl+C/y", "Copy", [](const EditorState& s) { return s.selection.isActive; } },
        { "Ctrl+X/x", "Cut", [](const EditorState& s) { return s.selection.isActive; } },
        { "Esc/Enter", "Drop", [](const EditorState& s) { return s.selection.isActive; } },
        { "Ctrl+V/p", "Paste", [](const EditorState& s) { return s.clipboard.hasData && !s.selection.isActive; } }
    };
}


std::string
StatusBarComponent::toolTypeToString( ToolType type ) const
{
    switch ( type )
    {
        case ToolType::DRAW:        return "Draw";
        case ToolType::ERASER:      return "Eraser";
        case ToolType::SQUARE:      return "Rectangle";
        case ToolType::CIRCLE:      return "Ellipse";
        case ToolType::LINE:        return "Line";
        case ToolType::EYE_DROPPER: return "Eye Dropper";
        case ToolType::PAINT_FILL:  return "Paint Fill";
        case ToolType::BOX_SELECT:  return "Box Select";
        default:                    return "Unknown";
    }
}



ftxui::Element
StatusBarComponent::OnRender()
{
    using namespace ftxui;

    auto toolElement = hbox({
        text( " Tool: " ) | bold,
        text( toolTypeToString( M_editorState.toolType ) ) | color( Color::Cyan ) | bold,
    });

    Elements stateIndicators;
    if ( M_editorState.selection.isActive )
        stateIndicators.push_back( text( " [Selecting] " ) | color( Color::Yellow ) | bold );

    if ( M_editorState.clipboard.hasData )
        stateIndicators.push_back( text( " [Copied] " ) | color( Color::Green ) | bold );

    auto statesElement = hbox( std::move( stateIndicators ) );

    Elements shortcutElements;
    bool isFirst = true;

    for ( const auto& shortcut : M_shortcuts )
    {
        if ( shortcut.condition( M_editorState ) )
        {
            if ( !isFirst )
            {
                shortcutElements.push_back( text( " | " ) | dim );
            }

            // Bold the keys, dim the action description for visual hierarchy
            shortcutElements.push_back( text( shortcut.keys ) | bold );
            shortcutElements.push_back( text( ": " + shortcut.action ) | dim );

            isFirst = false;
        }
    }

    auto finalShortcuts = hbox({ hbox(std::move(shortcutElements)), text(" ") });

    return hbox({
        toolElement,
        separatorEmpty(),
        statesElement,
        filler(),
        finalShortcuts
    });
}


std::shared_ptr<StatusBarComponent> StatusBar( EditorState & editorState )
{
    return std::make_shared<StatusBarComponent>( editorState );
}

}
