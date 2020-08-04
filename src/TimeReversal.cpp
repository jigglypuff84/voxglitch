//
// Voxglitch "Time Reversal" module for VCV Rack
//

#include <stack>
#include <vector>
#include "plugin.hpp"
#include "osdialog.h"
#include "Common/common.hpp"
#include "Common/audio_buffer.hpp"
#include "Common/submodules.hpp"

#include "TimeReversal/defines.h"
#include "TimeReversal/TimeReversal.hpp"
#include "TimeReversal/TimeReversalWidget.hpp"

Model* modelTimeReversal = createModel<TimeReversal, TimeReversalWidget>("TimeReversal");
