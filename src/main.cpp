#include <iostream>
#include <functional>
#include <memory>
#include <string>

#include "ftxui/screen/screen.hpp"
#include "ftxui/screen/string.hpp"
#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Input, Renderer, Vertical
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for InputOption
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for text, hbox, separator, Element, operator|, vbox, border
//#include <ftxui/dom/elements.hpp>  // for color, Fit, LIGHT, align_right, bold, DOUBLE
#include <ftxui/dom/table.hpp>      // for Table, TableSelection
#include "ftxui/util/ref.hpp"  // for Ref
#include "ftxui/dom/node.hpp"  // for Render
#include "ftxui/screen/color.hpp"  // for Color, Color::Blue, Color::Cyan, Color::White, ftxui

using namespace ftxui;

// Display a component nicely with a title on the left.
Component Wrap(std::string name, Component component) {
  return Renderer(component, [name, component] {
    return hbox({
               text(name) | size(WIDTH, EQUAL, 8),
               separator(),
               component->Render() | xflex,
           }) |
           xflex;
  });
}

//vertical wrapper with components under the name.
Component vWrap(std::string name, Component component) {
  return Renderer(component, [name, component] {
    return vbox({
               text(name) | xflex, //size(WIDTH, EQUAL, 8),
               component->Render() | xflex,
           }) |
           xflex;
  });
}

ButtonOption Style() {
  auto option = ButtonOption::Animated();
  option.transform = [](const EntryState& s) {
    auto element = text(s.label);
    if (s.focused) {
      element |= bold;
    }
    return element | center | flex;
    //return element | center | borderEmpty | flex;
  };
  return option;
}

void loadOperation(){
    std::cout<<"Load clicked"<<std::endl;
}
void collapseAllOperation(){
    std::cout<<"Collapse all"<<std::endl;
}
void expandAllOperation(){
    std::cout<<"Expand all"<<std::endl;
}

int main(void) {
    auto screen = ScreenInteractive::FitComponent();

    //--1st pannel: Devices--------------------------------------------------------------------------------------------
    int selectedDevice = 0;
    std::vector<std::string> devNames = {
        "DUMMY1",
        "DUMMY2",
        "DUMMY5"
    };
    MenuOption deviceMenuOption;
    deviceMenuOption.on_enter = screen.ExitLoopClosure();
    Component deviceMenuComp = Menu(&devNames, &selectedDevice, deviceMenuOption);

    //Device Property input components
    std::string deviceName;
    std::string deviceFile;
    std::string mapFile;
    Component deviceNameInputComp = vWrap("Device name",Input(&deviceName, "DUMMY1"));
    Component deviceFileInputComp = vWrap("Device file",Input(&deviceFile, "/dev/utcadummys0"));
    Component mapFileInputComp = vWrap("Map file",Input(&mapFile, "./utcadummy.map"));

    //Load Button
    std::string loadButtonLabel = "Load Boards";
    //std::function<void()> on_button_clicked_;
    auto loadButton = Button("Load Boards", [&]{loadOperation();});//, Style() );
    //auto loadButton = Button(&loadButtonLabel, [&]{loadOperation();});//, Style() );
    
    auto devicePropsComp = Container::Vertical({
                vWrap("Devices",deviceMenuComp | vscroll_indicator | frame | border),

                vWrap("Device status",Container::Vertical({
                deviceNameInputComp,
                deviceFileInputComp,
                mapFileInputComp,
                loadButton | size(WIDTH, EQUAL, 10),
                })) | border

                });

    //--2nd pannel: Register-------------------------------------------------------------------------------------------
    std::vector<std::string> regNames = {
        "WORD_FIRMWARE",
        "WORD_CLK_MUX_2",
        "WORD_CLK_MUX_3",
        "AREA_DMA"
    };
    int selectedRegister = 0;

    MenuOption registerMenuOption;
    registerMenuOption.on_enter = screen.ExitLoopClosure();
    Component registerMenuComp = Menu(&regNames,&selectedRegister,  registerMenuOption);

    std::string modelSearchString;
    Component findModuleInputComp = vWrap("Find module",Input(&modelSearchString, "search"));

    bool checkbox_autoselectPrevReg_selected = false;
    bool checkbox_sortModReg_selected = false;

    auto collapseAllButton = Button("Collapse All", [&]{collapseAllOperation();});//, Style() );
    auto expandAllButton = Button("Expand All", [&]{expandAllOperation();});//, Style() );
    
    auto regVbox = vWrap("Modules/Registers",Container::Vertical({
                registerMenuComp | vscroll_indicator | frame | border,
                findModuleInputComp,
                Container::Horizontal({
                        collapseAllButton,
                        expandAllButton 
                        }),
                Checkbox("Autoselect previous register", &checkbox_autoselectPrevReg_selected ),
                Checkbox("Sort Mudules/Registers", &checkbox_sortModReg_selected)
            }));

    //--3rd pannel: Register Properties--------------------------------------------------------------------------------
    int registerBar;
    int registerAddress;
    int numElements;
    std::string registerBarStr;
    std::string registerAddressStr;
    std::string numElementsStr;
    Component registerBarInputComp = vWrap("Register bar",Input(&registerBarStr, "2"));
    Component registerAddressInputComp = vWrap("Register address",Input(&registerAddressStr, "0"));
    Component numElementsInputComp = vWrap("Number of elements",Input(&numElementsStr, "1024"));

    // Table	(	std::vector< std::vector< std::string >> 	input	)	
    // Table	(	std::vector< std::vector< Element >> 	input	)
    // Element(node)
    //  Element(e)
    std::vector<std::string> row1 = {"one", "1"};
    std::vector<std::string> row2 = {"two", "2"};
    std::vector<std::string> row3 = {"three", "3"};
    std::vector<std::vector<std::string>> rows = {row1, row2, row3};
    auto table = Table(rows);

    table.SelectAll().Border(LIGHT);

    // Add border around the first column.
    table.SelectColumn(0).Border(LIGHT);

    // Make first row bold with a double border.
    table.SelectRow(0).Decorate(bold);
    table.SelectRow(0).SeparatorVertical(LIGHT);
    table.SelectRow(0).Border(DOUBLE);

    // Align right the "Release date" column.
    table.SelectColumn(2).DecorateCells(align_right);

    // Select row from the second to the last.
    auto content = table.SelectRows(1, -1);
    // Alternate in between 3 colors.
    content.DecorateCellsAlternateRow(color(Color::Blue), 3, 0);
    content.DecorateCellsAlternateRow(color(Color::Cyan), 3, 1);
    content.DecorateCellsAlternateRow(color(Color::White), 3, 2);

    std::vector<Component> regPropComps;
    regPropComps.push_back(registerBarInputComp);
    regPropComps.push_back(registerAddressInputComp);
    regPropComps.push_back(numElementsInputComp);
    //regPropComps.push_back(table);
    auto regProps = vWrap("Register Properties\n",Container::Vertical(regPropComps));
    /*auto regProps = vWrap("Register Properties\n",Container::Vertical({
            registerBarInputComp,
            registerAddressInputComp,
            numElementsInputComp,
            table
            }));*/

    //--4th pannel: Options--------------------------------------------------------------------------------------------
    bool checkbox_ContRead_enable = false;
    bool checkbox_readAftWrite_selected = false;
    bool checkbox1_showPlotWin_selected = false;

    //std::string quitButtonLabel = "Quit"; 
    auto quitButton = Button("Quit", screen.ExitLoopClosure(), ButtonOption::Simple());

    //auto text1 = text("asdf");
    //auto text2 = text("¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿");
    auto checkboxes = Container::Vertical({
            Checkbox("Read After Write", &checkbox_readAftWrite_selected),
            Checkbox("Show Plot Window", &checkbox1_showPlotWin_selected),
            quitButton | size(WIDTH, EQUAL, 10) | center  //paragraph("asdf")
            });
    checkboxes = vWrap("Options",checkboxes) | border;

    //--Main pannel--------------------------------------------------------------------------------------------

    auto topComp = Container::Horizontal({
            devicePropsComp,
            regVbox,
            regProps,
            checkboxes
            });

    auto ui = Renderer(topComp, [&]{
          return hbox({
                  //text("hello world"), //that works, but messes it all up 
                  devicePropsComp->Render(),
                  separatorHeavy(),
                  regVbox->Render(),
                  separatorHeavy(),
                  regProps->Render(),
                  separatorHeavy(),
                  checkboxes->Render()
                  })  |
           xflex | size(WIDTH, GREATER_THAN, 100) | border | color(Color::RGB(214,210,221) ) | bgcolor(Color::RGB(47,22,25) );
           //xflex | size(WIDTH, GREATER_THAN, 100) | border | color(Color::RGB(250,251,252) ) | bgcolor(Color::RGB(39,40,24) );
           
          });

  screen.Loop(ui);
  return 0;
}

/*
             *****************             
         *****               *****          
      ****            __         ****       
     **     ^~___~^^^&%Obrss__      **      
   **   -~^^~~~~~^^^/%%%%%%--         **    
  **    ###Q##     / %%%  Jl           **   
  *     %__%%%##    %%%*                *   
**     *MM %%|%%##   %%%*                *  
*      5555%%J%%%##  %%%****%%%%=-        * 
*  ___ _    _                   _____ _  __@  
* / __| |_ (_)_ __  ___ _ _ __ |_   _| |/ /@
*| (__| ' \| | '  \/ -_) '_/ _` || | | ' < @
* \___|_||_|_|_|_|_\___|_| \__,_||_| |_|\_\@
 *     %%%%%%%%%%%%%%%%%%%%%%%%%%\%%%%     * 
 **  MMM%%%%%%%%%%%%%%%%%% %%%%%%%\%%%%%  *  
   *         %%%%%%%%%   *********   %%% *   
   **      %%%%%%       **    %%%   %%%%**   
     * MMM%%%%          ll   %%%%  MM% *     
      **                ll MMM%    M **      
        **                         **        
           **                   **           
             ******************              
                                             
     */


/*
@  ___ _    _                   _____ _  __@  
@ / __| |_ (_)_ __  ___ _ _ __ |_   _| |/ /@
@| (__| ' \| | '  \/ -_) '_/ _` || | | ' < @
@ \___|_||_|_|_|_|_\___|_| \__,_||_| |_|\_\@
                                           
.-. . . .-. .  . .-. .-. .-. .-. . . 
|   |-|  |  |\/| |-  |(  |-|  |  |<  
`-' ' ` `-' '  ` `-' ' ' ` '  '  ' ` 
 __                  ___   
/  |_  o __  _  __ _  | |/ 
\__| | | |||(/_ | (_| | |\ 
                           
  _            ____
 / )/ '_  _ _ _ /  /__/
(__/)///)(-/ (/(  /  )
 */

