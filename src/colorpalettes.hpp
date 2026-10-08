#pragma once

#include <ftxui/component/component.hpp>
#include <unordered_map>


namespace Termisprite
{


class NewPaletteModal;
class ImportPaletteModal;
class ExportPaletteModal;

class ColorPaletteComponent
    : public ftxui::ComponentBase
{
public:
    ColorPaletteComponent( ftxui::Color & activeColor, std::vector<ftxui::Color> const& colorsInCanvas );

    ftxui::Element OnRender() override;

    ftxui::Component applyModals(ftxui::Component main);

    std::unordered_map<std::string, std::vector<ftxui::Color>> & palettes() { return M_palettes; }

    void rebuildPaletteTabs();

private:
    void rebuildColorsInCanvas();
    ftxui::Component buildColorButton( ftxui::Color color, std::function<void()> onDelete = {} );

private:
    ftxui::Color & M_activeColor;
    std::vector<ftxui::Color> M_lastColorsInCanvas;
    std::vector<ftxui::Color> const& M_colorsInCanvas;

    ftxui::Component M_container;
    ftxui::Component M_colorsInCanvasContainer;
    ftxui::Component M_createPaletteButton;
    ftxui::Component M_importPaletteButton;
    ftxui::Component M_deletePaletteButton;
    ftxui::Component M_exportPaletteButton;

    std::vector<std::string> M_paletteNames; //TODO: redundant, use single data struct
    std::unordered_map<std::string, std::vector<ftxui::Color>> M_palettes;

    bool M_showNewPaletteModal = false;
    std::shared_ptr<NewPaletteModal> M_newPaletteModal;

    bool M_showImportPaletteModal = false;
    std::shared_ptr<ImportPaletteModal> M_importPaletteModal;

    bool M_showExportPaletteModal = false;
    std::shared_ptr<ExportPaletteModal> M_exportPaletteModal;

    ftxui::Component M_paletteTabToggle;
    ftxui::Component M_paletteTabContainer;
    int M_paletteTabIndex = 0;


};


std::shared_ptr<ColorPaletteComponent> ColorPalette( ftxui::Color & activeColor, std::vector<ftxui::Color> const& colorsInCanvas );

}
