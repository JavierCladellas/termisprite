
#pragma once

#include "editor.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>


namespace Termisprite
{


class ColorGridComponent
    : public ftxui::ComponentBase
{
public:
    ColorGridComponent( ftxui::Color & targetColor, int & saturation )
        : M_targetColor( targetColor ), M_saturation( saturation )
    { }

    ftxui::Element OnRender() override;
    bool OnEvent( ftxui::Event event ) override;
    bool Focusable() const override { return true; }

private:
    void updateColorFromCursor();

private:
    ftxui::Color & M_targetColor;
    int& M_saturation;
    int M_cursorX = 0;
    int M_cursorY = 0;

    ftxui::Box M_box;
};


class ColorPickerComponent : public ftxui::ComponentBase
{
public:
    ColorPickerComponent( ftxui::Color & targetColor )
        : M_targetColor( targetColor )
    {
        M_gridComponent = std::make_shared<ColorGridComponent>( M_targetColor, M_saturation );
        M_saturationSlider = ftxui::Slider(" Saturation:", &M_saturation, 0, 255, 5);

        ftxui::ComponentBase::Add(
            ftxui::Container::Vertical({ M_gridComponent, M_saturationSlider })
        );
    }

    ftxui::Element OnRender() override;
    bool OnEvent( ftxui::Event event ) override;


private:
    ftxui::Color & M_targetColor;

    int M_saturation = 255;
    ftxui::Component M_gridComponent;
    ftxui::Component M_saturationSlider;
    ftxui::Component M_container;

};


std::shared_ptr<ColorPickerComponent> ColorPicker( ftxui::Color & targetColor );


class ColorPaletteComponent
    : public ftxui::ComponentBase
{
public:
    ColorPaletteComponent( EditorState & editorState );

    ftxui::Element OnRender() override;

private:
    void rebuildPalette();

private:
    std::vector<ftxui::Color> M_lastPalette;
    EditorState & M_editorState;

    ftxui::Component M_container = ftxui::Container::Vertical({});
};


std::shared_ptr<ColorPaletteComponent> ColorPalette( EditorState & editorState );



class ColorSectionComponent
    : public ftxui::ComponentBase
{
public:
    ColorSectionComponent( EditorState & editorState )
        : M_editorState( editorState ),
          M_colorPicker( ColorPicker( editorState.color ) ),
          M_colorPalette( ColorPalette( editorState ) )
    {
        ftxui::ComponentBase::Add(
            ftxui::Container::Vertical({ M_colorPicker, M_colorPalette })
        );
    }

    ftxui::Element OnRender() override;
    bool OnEvent( ftxui::Event event ) override;

private:
    std::shared_ptr<ColorPickerComponent> M_colorPicker;
    std::shared_ptr<ColorPaletteComponent> M_colorPalette;

    EditorState & M_editorState;

};


std::shared_ptr<ColorSectionComponent> ColorSection( EditorState & editorState );


}
