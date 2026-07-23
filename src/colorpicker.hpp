
#pragma once

#include "editor.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>


namespace Termisprite
{



class ColorPickerComponent : public ftxui::ComponentBase
{
public:
    ColorPickerComponent( ftxui::Color & targetColor )
        : M_targetColor( targetColor )
    {
        M_saturationSlider = ftxui::Slider(" Saturation:", &M_saturation, 0, 255, 1);
        ftxui::ComponentBase::Add(M_saturationSlider);
    }

    ftxui::Element OnRender() override;
    bool OnEvent( ftxui::Event event ) override;


private:
    ftxui::Color & M_targetColor;

    int M_saturation = 255;
    ftxui::Component M_saturationSlider;

    ftxui::Box M_box;
};


std::shared_ptr<ColorPickerComponent> ColorPicker( ftxui::Color & targetColor );


class ColorPaletteComponent
    : public ftxui::ComponentBase
{
public:
    ColorPaletteComponent( EditorState & editorState )
        : M_editorState( editorState )
    {
        ftxui::ComponentBase::Add( M_container );
    }

    ftxui::Element OnRender() override;

private:
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
        ftxui::ComponentBase::Add( M_colorPicker );
        ftxui::ComponentBase::Add( M_colorPalette );
    }

    ftxui::Element OnRender() override;
    bool Focusable() const override { return true; }

private:
    std::shared_ptr<ColorPickerComponent> M_colorPicker;
    std::shared_ptr<ColorPaletteComponent> M_colorPalette;

    EditorState & M_editorState;

};


std::shared_ptr<ColorSectionComponent> ColorSection( EditorState & editorState );


}
