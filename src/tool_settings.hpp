#pragma once

#include "brush_tool.hpp"
#include "shortcuts.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>



namespace Termisprite
{

class ToolSettingsComponent
    : public ftxui::ComponentBase
{
public:
    ToolSettingsComponent( BrushTool & brushTool, ShortcutManager * shortcutManager )
        : M_brushTool( brushTool ), M_shortcutManager( shortcutManager )
    {}

    virtual ftxui::Element OnRender() override = 0;
    virtual bool OnEvent( ftxui::Event event ) override;

protected:
    ftxui::Component M_container;
    BrushTool & M_brushTool;
    ShortcutManager * M_shortcutManager;
};

class BrushSettingsComponent
    : public ToolSettingsComponent
{
public:
    BrushSettingsComponent( BrushTool & brushTool, ShortcutManager * shortcutManager );
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
    EraserSettingsComponent( BrushTool & brushTool, ShortcutManager * shortcutManager );
    ftxui::Element OnRender() override;

private:
    int & M_brushSize;
    ftxui::Component M_sizeSpinner;
};


class RectangleSettingsComponent
    : public ToolSettingsComponent
{
public:
    RectangleSettingsComponent( BrushTool & brushTool, ShortcutManager * shortcutManager );
    ftxui::Element OnRender() override;

private:
    int & M_brushSize;
    ftxui::Component M_sizeSpinner;
};


class EllipseSettingsComponent
    : public ToolSettingsComponent
{
public:
    EllipseSettingsComponent( BrushTool & brushTool, ShortcutManager * shortcutManager );
    ftxui::Element OnRender() override;

private:
    int & M_brushSize;
    ftxui::Component M_sizeSpinner;
};

class LineSettingsComponent
    : public ToolSettingsComponent
{
public:
    LineSettingsComponent( BrushTool & brushTool, ShortcutManager * shortcutManager );
    ftxui::Element OnRender() override;

private:
    int & M_brushSize;
    ftxui::Component M_sizeSpinner;
};


class PaintFillSettingsComponent
    : public ToolSettingsComponent
{
public:
    PaintFillSettingsComponent( BrushTool & brushTool, ShortcutManager * shortcutManager );
    ftxui::Element OnRender() override;

private:
};

class BoxSelectSettingsComponent
    : public ToolSettingsComponent
{
public:
    BoxSelectSettingsComponent( BrushTool & brushTool, ShortcutManager * shortcutManager );
    ftxui::Element OnRender() override;

private:
};

std::shared_ptr<BrushSettingsComponent> BrushSettings( BrushTool & brushTool, ShortcutManager * shortcutManager );
std::shared_ptr<EraserSettingsComponent> EraserSettings( BrushTool & brushTool, ShortcutManager * shortcutManager );

std::shared_ptr<RectangleSettingsComponent> RectangleSettings( BrushTool & brushTool, ShortcutManager * shortcutManager );
std::shared_ptr<EllipseSettingsComponent> EllipseSettings( BrushTool & brushTool, ShortcutManager * shortcutManager );
std::shared_ptr<LineSettingsComponent> LineSettings( BrushTool & brushTool, ShortcutManager * shortcutManager );

std::shared_ptr<PaintFillSettingsComponent> PaintFillSettings( BrushTool & brushTool, ShortcutManager * shortcutManager );
std::shared_ptr<BoxSelectSettingsComponent> BoxSelectSettings( BrushTool & brushTool, ShortcutManager * shortcutManager );


}

