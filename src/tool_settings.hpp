#pragma once

#include "editor.hpp"
#include "shortcuts.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>



namespace Termisprite
{

class ToolSettingsComponent
    : public ftxui::ComponentBase
{
public:
    ToolSettingsComponent( EditorState & editorState, ShortcutManager * shortcutManager )
        : M_editorState( editorState ), M_shortcutManager( shortcutManager )
    {}

    virtual ftxui::Element OnRender() override = 0;
    virtual bool OnEvent( ftxui::Event event ) override;

protected:
    ftxui::Component M_container;
    EditorState & M_editorState;
    ShortcutManager * M_shortcutManager;
};

class BrushSettingsComponent
    : public ToolSettingsComponent
{
public:
    BrushSettingsComponent( EditorState & editorState, ShortcutManager * shortcutManager );
    ftxui::Element OnRender() override;

private:
    ftxui::Component M_brushCharInput;
    int & M_brushSize;
    ftxui::Component M_sizeSpinner;
};

class EraserSettingsComponent
    : public ToolSettingsComponent
{
public:
    EraserSettingsComponent( EditorState & editorState, ShortcutManager * shortcutManager );
    ftxui::Element OnRender() override;

private:
    int & M_brushSize;
    ftxui::Component M_sizeSpinner;
};


class RectangleSettingsComponent
    : public ToolSettingsComponent
{
public:
    RectangleSettingsComponent( EditorState & editorState, ShortcutManager * shortcutManager );
    ftxui::Element OnRender() override;

private:
    int & M_brushSize;
    ftxui::Component M_sizeSpinner;
};


class EllipseSettingsComponent
    : public ToolSettingsComponent
{
public:
    EllipseSettingsComponent( EditorState & editorState, ShortcutManager * shortcutManager );
    ftxui::Element OnRender() override;

private:
    int & M_brushSize;
    ftxui::Component M_sizeSpinner;
};

class LineSettingsComponent
    : public ToolSettingsComponent
{
public:
    LineSettingsComponent( EditorState & editorState, ShortcutManager * shortcutManager );
    ftxui::Element OnRender() override;

private:
    int & M_brushSize;
    ftxui::Component M_sizeSpinner;
};


class PaintFillSettingsComponent
    : public ToolSettingsComponent
{
public:
    PaintFillSettingsComponent( EditorState & editorState, ShortcutManager * shortcutManager );
    ftxui::Element OnRender() override;

private:
};

class BoxSelectSettingsComponent
    : public ToolSettingsComponent
{
public:
    BoxSelectSettingsComponent( EditorState & editorState, ShortcutManager * shortcutManager );
    ftxui::Element OnRender() override;

private:
};

std::shared_ptr<BrushSettingsComponent> BrushSettings( EditorState & editorState, ShortcutManager * shortcutManager );
std::shared_ptr<EraserSettingsComponent> EraserSettings( EditorState & editorState, ShortcutManager * shortcutManager );

std::shared_ptr<RectangleSettingsComponent> RectangleSettings( EditorState & editorState, ShortcutManager * shortcutManager );
std::shared_ptr<EllipseSettingsComponent> EllipseSettings( EditorState & editorState, ShortcutManager * shortcutManager );
std::shared_ptr<LineSettingsComponent> LineSettings( EditorState & editorState, ShortcutManager * shortcutManager );

std::shared_ptr<PaintFillSettingsComponent> PaintFillSettings( EditorState & editorState, ShortcutManager * shortcutManager );
std::shared_ptr<BoxSelectSettingsComponent> BoxSelectSettings( EditorState & editorState, ShortcutManager * shortcutManager );


}

