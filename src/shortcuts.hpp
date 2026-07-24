#pragma once

#include <ftxui/component/event.hpp>
#include <functional>
#include <string>


namespace Termisprite
{

struct Shortcut
{
    std::string displayName;
    std::string hotkeyText;
    std::vector<ftxui::Event> triggerKeys;
    std::function<void()> action;
};

enum class ShortcutType
{
    NEW_PROJECT,
    OPEN_PROJECT,
    SAVE_PROJECT,
    IMPORT_PROJECT,
    EXPORT_PROJECT,
    QUIT,

    UNDO,
    REDO,
    CLEAR,

    RESIZE_CANVAS,
    FLIP_VERTICAL,
    FLIP_HORIZONTAL,
    BACKGROUND_COLOR,

    ZOOM_IN,
    ZOOM_OUT,
    TOGGLE_GRID,
    TOGGLE_CHECKERBOARD_GRID,
    TOGGLE_PAN,

    SELECT_BRUSH_TOOL,
    SELECT_ERASER_TOOL,
    SELECT_RECTANGLE_TOOL,
    SELECT_ELLIPSE_TOOL,
    SELECT_LINE_TOOL,
    SELECT_EYE_DROPPER_TOOL,
    SELECT_PAINT_FILL_TOOL,
    SELECT_BOX_SELECT_TOOL,

    HELP_ABOUT,
    HELP_SHORTCUTS,

    CLIPBOARD_COPY,
    CLIPBOARD_CUT,
    CLIPBOARD_PASTE,
};

class Termisprite;

class ShortcutManager
{
public:
    explicit ShortcutManager( Termisprite * app );

    bool handleEvent( ftxui::Event event );
    std::string getMenuLabel( ShortcutType type ) const;

    std::string getName(ShortcutType type) const
    {
        auto it = M_definitions.find(type);
        return (it != M_definitions.end()) ? it->second.displayName : "Unknown";
    }

    std::string getHotkeyText(ShortcutType type) const
    {
        auto it = M_definitions.find(type);
        return (it != M_definitions.end()) ? it->second.hotkeyText : "";
    }

    Shortcut getShortcut( ShortcutType type ) const { return M_definitions.at(type); }
    void execute( ShortcutType type ) const;

private:
    void bindAllActions( Termisprite * app );
    void bindAction(ShortcutType type, std::function<void()> action);

    std::unordered_map<ShortcutType, Shortcut> M_definitions;
};





}
