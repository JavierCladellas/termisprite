#include "shortcuts.hpp"

#include "termisprite.hpp"



namespace Termisprite
{


static const std::unordered_map<ShortcutType, Shortcut>
getDefaultKeymap()
{
    return {
        { ShortcutType::NEW_PROJECT,       {"New",              "[Ctrl+N]",      {ftxui::Event::CtrlN}} },
        { ShortcutType::OPEN_PROJECT,      {"Open",             "[Ctrl+O]",      {ftxui::Event::CtrlO}} },
        { ShortcutType::SAVE_PROJECT,      {"Save",             "[Ctrl+S]",      {ftxui::Event::CtrlS}} },
        { ShortcutType::IMPORT_PROJECT,    {"Import",           "[Ctrl+P]",      {ftxui::Event::CtrlP}} },
        { ShortcutType::EXPORT_PROJECT,    {"Export",           "[Ctrl+E]",      {ftxui::Event::CtrlE}} },
        { ShortcutType::QUIT,              {"Quit",             "[Ctrl+Q]",      {ftxui::Event::CtrlQ}} },

        { ShortcutType::UNDO,              {"Undo",             "[Ctrl+Z]",      {ftxui::Event::CtrlZ}} },
        { ShortcutType::REDO,              {"Redo",             "[Ctrl+Y]",      {ftxui::Event::CtrlY}} },
        { ShortcutType::CLEAR,             {"Clear",            "[Ctrl+D]",      {ftxui::Event::CtrlD}} },

        { ShortcutType::RESIZE_CANVAS,     {"Resize",           "[Ctrl+R]",      {ftxui::Event::CtrlR}} },
        { ShortcutType::FLIP_VERTICAL,     {"Flip Vertical",    "[Shift+H]",     {ftxui::Event::Character('H')}} },
        { ShortcutType::FLIP_HORIZONTAL,   {"Flip Horizontal",  "[Shift+V]",     {ftxui::Event::Character('V')}} },
        { ShortcutType::BACKGROUND_COLOR,  {"Background",       "[Ctrl+B]",      {ftxui::Event::CtrlB}} },

        { ShortcutType::ZOOM_IN,           {"Zoom In",          "",              {}} },
        { ShortcutType::ZOOM_OUT,          {"Zoom Out",         "",              {}} },
        { ShortcutType::TOGGLE_GRID,       {"Toggle Grid",      "[G]",                  {ftxui::Event::Character('g')}} },
        { ShortcutType::TOGGLE_CHECKERBOARD_GRID, {"Toggle Checkerboard", "[Shift+G]",  {ftxui::Event::Character('G')}} },
        { ShortcutType::TOGGLE_PAN,        {"Toggle Pan",       "[M]",           {ftxui::Event::Character('m'), ftxui::Event::Character('M')}} },

        { ShortcutType::SELECT_BRUSH_TOOL, {"Brush",       "[B]",           {ftxui::Event::Character('b'), ftxui::Event::Character('B')}} },
        { ShortcutType::SELECT_ERASER_TOOL,{"Eraser",      "[E]",           {ftxui::Event::Character('e'), ftxui::Event::Character('E')}} },
        { ShortcutType::SELECT_RECTANGLE_TOOL, {"Rectangle",    "[R]",           {ftxui::Event::Character('r'), ftxui::Event::Character('R')}} },
        { ShortcutType::SELECT_ELLIPSE_TOOL,   {"Ellipse",      "[C]",           {ftxui::Event::Character('c'), ftxui::Event::Character('C')}} },
        { ShortcutType::SELECT_LINE_TOOL,      {"Line",         "[Shift+L]",     {ftxui::Event::Character('L')}} },
        { ShortcutType::SELECT_EYE_DROPPER_TOOL, {"Eye Dropper", "[I]",          {ftxui::Event::Character('i'), ftxui::Event::Character('I')}} },
        { ShortcutType::SELECT_PAINT_FILL_TOOL,  {"Paint Fill",  "[F]",          {ftxui::Event::Character('f'), ftxui::Event::Character('F')}} },
        { ShortcutType::SELECT_BOX_SELECT_TOOL,  {"Box Select",  "[S]",          {ftxui::Event::Character('s'), ftxui::Event::Character('S')}} },

        { ShortcutType::HELP_ABOUT, {"About", "", {}} },
        { ShortcutType::HELP_SHORTCUTS, {"Shortcuts", "", {}} },

        { ShortcutType::CLIPBOARD_COPY, {"Copy", "[y/Ctrl+C]", {ftxui::Event::CtrlC, ftxui::Event::Character('y')}} },
        { ShortcutType::CLIPBOARD_CUT, {"Cut", "[Ctrl+X]", {ftxui::Event::CtrlX, ftxui::Event::Character('x')}} },
        { ShortcutType::CLIPBOARD_PASTE, {"Paste", "[Ctrl+V]", {ftxui::Event::CtrlV, ftxui::Event::Character('p')}} },
    };
}

void
ShortcutManager::bindAllActions( Termisprite * app )
{
    this->bindAction(ShortcutType::NEW_PROJECT, [app]() { app->showNewProjectModal(); });
    this->bindAction(ShortcutType::OPEN_PROJECT, [app]() { app->showOpenProjectModal(); });
    this->bindAction(ShortcutType::SAVE_PROJECT, [app]() { app->showSaveModal(); });
    this->bindAction(ShortcutType::IMPORT_PROJECT, [app]() { app->showImportModal(); });
    this->bindAction(ShortcutType::EXPORT_PROJECT, [app]() { app->showExportModal(); });
    this->bindAction(ShortcutType::QUIT, [app]() { exit(0); });

    this->bindAction(ShortcutType::UNDO, [app]() { app->editor()->undo(); });
    this->bindAction(ShortcutType::REDO, [app]() { app->editor()->redo(); });
    this->bindAction(ShortcutType::CLEAR, [app]() { app->editor()->clear(); });

    this->bindAction(ShortcutType::RESIZE_CANVAS, [app]() { app->showResizeModal(); });
    this->bindAction(ShortcutType::FLIP_VERTICAL, [app]() { app->editor()->flipVertical(); });
    this->bindAction(ShortcutType::FLIP_HORIZONTAL, [app]() { app->editor()->flipHorizontal(); });
    this->bindAction(ShortcutType::BACKGROUND_COLOR, [app]() { app->showBackgroundColorModal(); });

    this->bindAction(ShortcutType::TOGGLE_GRID, [app]() { app->editor()->toggleGrid(); });
        this->bindAction(ShortcutType::TOGGLE_CHECKERBOARD_GRID, [app]() { app->editor()->toggleCheckerboardGrid(); });
    this->bindAction(ShortcutType::TOGGLE_PAN, [app]() {
        if ( app->editor()->currentState().toolType == ToolType::PAN )
            app->selectTool(ToolType::DRAW);
        else
            app->selectTool(ToolType::PAN);
    });

    this->bindAction(ShortcutType::SELECT_BRUSH_TOOL, [app]() { app->selectTool(ToolType::DRAW); });
    this->bindAction(ShortcutType::SELECT_ERASER_TOOL, [app]() { app->selectTool(ToolType::ERASER); });
    this->bindAction(ShortcutType::SELECT_RECTANGLE_TOOL, [app]() { app->selectTool(ToolType::SQUARE); });
    this->bindAction(ShortcutType::SELECT_ELLIPSE_TOOL, [app]() { app->selectTool(ToolType::CIRCLE); });
    this->bindAction(ShortcutType::SELECT_LINE_TOOL, [app]() { app->selectTool(ToolType::LINE); });
    this->bindAction(ShortcutType::SELECT_EYE_DROPPER_TOOL, [app]() { app->selectTool(ToolType::EYE_DROPPER); });
    this->bindAction(ShortcutType::SELECT_PAINT_FILL_TOOL, [app]() { app->selectTool(ToolType::PAINT_FILL); });
    this->bindAction(ShortcutType::SELECT_BOX_SELECT_TOOL, [app]() { app->selectTool(ToolType::BOX_SELECT); });

    this->bindAction(ShortcutType::HELP_ABOUT, [app]() { app->showAboutModal(); });
    this->bindAction(ShortcutType::HELP_SHORTCUTS, [app]() { app->showShortcutsModal(); });

    this->bindAction(ShortcutType::CLIPBOARD_COPY, [app]() {
        if ( app->editor()->currentState().selection.isActive )
            app->editor()->copyToClipboard();
    });
    this->bindAction(ShortcutType::CLIPBOARD_CUT, [app]() {
        if ( app->editor()->currentState().selection.isActive )
            app->editor()->cutToClipboard();
    });
    this->bindAction(ShortcutType::CLIPBOARD_PASTE, [app]() {
        if ( app->editor()->currentState().clipboard.hasData )
            app->editor()->pasteClipboard();
    });
}


ShortcutManager::ShortcutManager( Termisprite * app )
{
    M_definitions = getDefaultKeymap();
    this->bindAllActions(app);
}


void
ShortcutManager::bindAction(ShortcutType type, std::function<void()> action)
{
    M_definitions.at(type).action = std::move(action);
}


bool
ShortcutManager::handleEvent( ftxui::Event event )
{
    for ( auto const& [type, def] : M_definitions )
        for ( auto const& trigger : def.triggerKeys )
            if ( event == trigger )
                if ( def.action )
                {
                    def.action();
                    return true;
                }

    return false;
}

std::string
ShortcutManager::getMenuLabel(ShortcutType type) const
{
    auto it = M_definitions.find(type);
    if (it != M_definitions.end())
        return it->second.displayName + " " + it->second.hotkeyText;
    return "Unkown";
}

void
ShortcutManager::execute(ShortcutType type) const
{
    auto it = M_definitions.find(type);
    if (it != M_definitions.end() && it->second.action)
        it->second.action();
}


}
