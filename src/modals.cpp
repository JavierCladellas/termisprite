#include "modals.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>


namespace Termisprite
{


Modal::Modal( std::function<void()> onClose, std::string const& title )
    : M_closeCallback( onClose ), M_title( title )
{
    M_cancelButton = ftxui::Button("X", [this](){ M_closeCallback(); }, ftxui::ButtonOption::Ascii() );
    M_okButton = ftxui::Button("Confirm", [this](){ this->onConfirm(); M_closeCallback(); } );

}

void
Modal::initTree()
{
    ftxui::ComponentBase::Add(
        ftxui::Container::Vertical({
            M_cancelButton,
            buildContentComponent(),
            M_okButton
        }) 
    );
}


ftxui::Element
Modal::OnRender()
{
    using namespace ftxui;

    return vbox({
        hbox({
            text( M_title ) | bold | center ,
            filler(),
            M_cancelButton->Render()
        }),
        separator(),
        separatorEmpty(),
        this->renderModalContent(),
        separator(),
        hbox({
            filler(),
            M_okButton->Render()
        })
    }) | reflect( M_box )
       | size(WIDTH, GREATER_THAN, 45)
       | borderDouble
       | clear_under;

}


bool
Modal::OnEvent( ftxui::Event event )
{
    if ( event == ftxui::Event::Return )
    {
        if ( !M_cancelButton->Focused() )
            this->onConfirm();
        M_closeCallback();
        return true;
    }

    if ( event == ftxui::Event::Escape )
    {
        M_closeCallback();
        return true;
    }

    if ( event.is_mouse() )
    {
        auto mouse = event.mouse();
        if ( mouse.button == ftxui::Mouse::Button::Left && mouse.motion == ftxui::Mouse::Pressed )
        {
            if ( !M_box.Contain( mouse.x, mouse.y ) )
            {
                M_closeCallback();
                return true;
            }
        }
    }

    return ftxui::ComponentBase::OnEvent( event );


}




void
NewProjectModal::onConfirm()
{
    if ( M_widthInput.empty() )
        M_widthInput = "32";
    if ( M_heightInput.empty() )
        M_heightInput = "32";
    M_editorCanvas.clear();
    M_editorCanvas.resize(std::stoi(M_widthInput), std::stoi(M_heightInput));
    M_projectNameInput = "";
    M_widthInput = "";
    M_heightInput = "";
}


ftxui::Component
NewProjectModal::buildContentComponent() const
{
    return ftxui::Container::Vertical({
        M_projectNameInputComponent,
        M_widthInputComponent,
        M_heightInputComponent
    });
}

ftxui::Element 
NewProjectModal::renderModalContent()
{
    using namespace ftxui;

    return vbox({
        hbox({
            text(" Project Name: ") | dim | vcenter,
            M_projectNameInputComponent->Render() | border | size(WIDTH, EQUAL, 20)
        }),
        hbox({
            text(" Width:  ") | dim | vcenter,
            M_widthInputComponent->Render() | border | size(WIDTH, EQUAL, 10),
            text(" px") | dim | vcenter
        }),
        hbox({
            text(" Height: ") | dim | vcenter,
            M_heightInputComponent->Render() | border | size(WIDTH, EQUAL, 10),
            text(" px") | dim | vcenter
        })
    });
}





ResizeModal::ResizeModal( EditorCanvasComponent & editorCanvas, std::function<void()> onClose)
    : Modal( onClose, "Resize"), M_editorCanvas( editorCanvas )
{
    auto [width, height] = M_editorCanvas.size();
    M_placeholderWidth = std::to_string(width);
    M_placeholderHeight = std::to_string(height);
    M_widthInputComponent = ftxui::Input(&M_widthInput, &M_placeholderWidth);
    M_heightInputComponent = ftxui::Input(&M_heightInput, &M_placeholderHeight);

    Modal::initTree();
}


void
ResizeModal::onConfirm()
{
    auto [width, height] = M_editorCanvas.size();
    M_placeholderWidth = std::to_string(width);
    M_placeholderHeight = std::to_string(height);
    if ( !M_widthInput.empty() )
        M_placeholderWidth = M_widthInput;
    if ( !M_heightInput.empty() )
        M_placeholderHeight = M_heightInput;
    M_editorCanvas.resize(std::stoi(M_placeholderWidth), std::stoi(M_placeholderHeight));
    M_widthInput = "";
    M_heightInput = "";
}


ftxui::Component
ResizeModal::buildContentComponent() const
{
    return ftxui::Container::Vertical({
        M_widthInputComponent,
        M_heightInputComponent
    });
}

ftxui::Element 
ResizeModal::renderModalContent()
{
    using namespace ftxui;

    return vbox({
        hbox({
            text(" Width:  ") | dim | vcenter,
            M_widthInputComponent->Render() | border | size(WIDTH, EQUAL, 10),
            text(" px") | dim | vcenter
        }),
        hbox({
            text(" Height: ") | dim | vcenter,
            M_heightInputComponent->Render() | border | size(WIDTH, EQUAL, 10),
            text(" px") | dim | vcenter
        }),
    });
}


void
SaveModal::onConfirm()
{
    if (!M_filepathInput.empty())
        M_editorCanvas.exportProject(M_filepathInput);
    M_filepathInput = "";
}


ftxui::Component
SaveModal::buildContentComponent() const
{
    return M_filepathInputComponent;
}

ftxui::Element 
SaveModal::renderModalContent()
{
    using namespace ftxui;

    return hbox({
        text(" Filepath: ") | dim | vcenter,
        M_filepathInputComponent->Render() | border | size(WIDTH, EQUAL, 30)
    });
}




void
OpenProjectModal::onConfirm()
{
    if (!M_filepathInput.empty())
        M_editorCanvas.importProject(M_filepathInput);
    M_filepathInput = "";
}


ftxui::Component
OpenProjectModal::buildContentComponent() const
{
    return M_filepathInputComponent;
}

ftxui::Element 
OpenProjectModal::renderModalContent()
{
    using namespace ftxui;

    return hbox({
        text(" Filepath: ") | dim | vcenter,
        M_filepathInputComponent->Render() | border | size(WIDTH, EQUAL, 30)
    });
}




void
ExportModal::onConfirm()
{
    if (!M_filepathInput.empty())
        M_editorCanvas.exportImage(M_filepathInput, M_formatOptions[M_selectedFormatIndex] );
    M_filepathInput = "";
}


ftxui::Component
ExportModal::buildContentComponent() const
{
    return ftxui::Container::Horizontal({ M_filepathInputComponent, M_formatDropdown });
}

ftxui::Element 
ExportModal::renderModalContent()
{
    using namespace ftxui;

    return vbox({
        hbox({
            text(" Filepath: ") | dim | vcenter,
            M_filepathInputComponent->Render() | border | size(WIDTH, EQUAL, 30)
        }) | center,
        separatorEmpty(),
        hbox({
            text(" Format: ") | dim | vcenter,
            M_formatDropdown->Render() | border
        }) | center
    });
}


void
ImportModal::onConfirm()
{
    if ( M_targetWidthInput.empty() )
        M_targetWidthInput = "64";
    if ( M_targetHeightInput.empty() )
        M_targetHeightInput = "64";
    if (!M_filepathInput.empty())
        M_editorCanvas.importImage(M_filepathInput,std::stoi(M_targetWidthInput), std::stoi(M_targetHeightInput));
    M_filepathInput = "";
    M_targetWidthInput = "";
    M_targetHeightInput = "";
}


ftxui::Component
ImportModal::buildContentComponent() const
{
    return ftxui::Container::Vertical({
        M_filepathInputComponent,
        ftxui::Container::Horizontal({ M_targetWidthInputComponent, M_targetHeightInputComponent }),
    });
}

ftxui::Element 
ImportModal::renderModalContent()
{
    using namespace ftxui;

    return vbox({
        hbox({
            text(" Filepath: ") | dim | vcenter,
            M_filepathInputComponent->Render() | border | size(WIDTH, EQUAL, 30)
        }) | center,
        separatorEmpty(),
        hbox({
            M_targetWidthInputComponent->Render() | border | size(WIDTH, EQUAL, 10),
            text(" px") | dim | vcenter,
            text(" x ") | dim | vcenter,
            M_targetHeightInputComponent->Render() | border | size(WIDTH, EQUAL, 10),
            text(" px") | dim | vcenter
        }) | center,
    });
}



ftxui::Component
BackgroundColorModal::buildContentComponent() const
{
    return M_colorPicker;
}

ftxui::Element 
BackgroundColorModal::renderModalContent()
{
    return M_colorPicker->Render();
}



ftxui::Element 
AboutModal::renderModalContent()
{
    using namespace ftxui;

    return vbox({
        text("Termisprite") | bold | center,
        separator(),
        text("A terminal-based sprite editor built with FTXUI.") | center,
        separatorEmpty(),
        text("Version 1.0.0") | center,
        separatorEmpty(),
        text("Author: Javier Cladellas") | center,
    }); 
}



ftxui::Element 
ShortcutsModal::renderModalContent()
{
    using namespace ftxui;

    return vbox(); 
}



}
