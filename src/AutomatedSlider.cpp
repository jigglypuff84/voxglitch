//
// Voxglitch "AutomatedSlider" module for VCV Rack
//

#include "plugin.hpp"
#include "osdialog.h"
#include "settings.hpp"
#include <fstream>

#include "AutomatedSlider/defines.h"
#include "AutomatedSlider/AutomatedSlider.hpp"
#include "AutomatedSlider/AutomatedSliderDisplay.hpp"
#include "AutomatedSlider/AutomatedSliderWidget.hpp"

Model* modelAutomatedSlider = createModel<AutomatedSlider, AutomatedSliderWidget>("automatedslider");
