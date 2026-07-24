#include "statusbar.hpp"
#include "shortcuts.hpp"



namespace Termisprite
{

StatusBarComponent::StatusBarComponent( EditorState & editorState, ShortcutManager * shortcutManager )
    : M_editorState( editorState ), M_shortcutManager( shortcutManager )
{
    M_displayedShortcuts = {
        {{ M_shortcutManager->getName(ShortcutType::UNDO), M_shortcutManager->getHotkeyText(ShortcutType::UNDO)}, {} },
        {{ M_shortcutManager->getName(ShortcutType::REDO), M_shortcutManager->getHotkeyText(ShortcutType::REDO)}, {} },
        {{ M_shortcutManager->getName(ShortcutType::CLEAR), M_shortcutManager->getHotkeyText(ShortcutType::CLEAR)}, {} },
        {{ M_shortcutManager->getName(ShortcutType::TOGGLE_GRID), M_shortcutManager->getHotkeyText(ShortcutType::TOGGLE_GRID)}, {} },

        {{ "Move Cursor", "[Arrows / hjkl]" }, [](EditorState & s) { return !s.selection.isActive; } },

        {{ "Move Selection", "[Arrows / hjkl]" }, [](EditorState & s) { return s.selection.isActive; } },
        {{ "Drop", "[Enter / Esc]" }, [](EditorState & s) { return s.selection.isActive; } },
        {{ M_shortcutManager->getName(ShortcutType::CLIPBOARD_COPY), M_shortcutManager->getHotkeyText(ShortcutType::CLIPBOARD_COPY)}, [](EditorState & s) { return s.selection.isActive; } },
        {{ M_shortcutManager->getName(ShortcutType::CLIPBOARD_CUT), M_shortcutManager->getHotkeyText(ShortcutType::CLIPBOARD_CUT)}, [](EditorState & s) { return s.selection.isActive; } },

        {{ M_shortcutManager->getName(ShortcutType::CLIPBOARD_PASTE), M_shortcutManager->getHotkeyText(ShortcutType::CLIPBOARD_PASTE)}, [](EditorState & s) { return s.clipboard.hasData && !s.selection.isActive; } }
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
        case ToolType::PAN:         return "Panning";
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

    for ( auto const& [shortcutText, condition] : M_displayedShortcuts )
    {
        if ( !condition || condition( M_editorState ) )
        {
            if ( !isFirst )
                shortcutElements.push_back( text( " | " ) | dim );

            shortcutElements.push_back( text( shortcutText.first ) | bold );
            shortcutElements.push_back( text( " " + shortcutText.second ) | dim );

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


std::shared_ptr<StatusBarComponent> StatusBar( EditorState & editorState, ShortcutManager * shortcutManager )
{
    return std::make_shared<StatusBarComponent>( editorState, shortcutManager );
}

}
