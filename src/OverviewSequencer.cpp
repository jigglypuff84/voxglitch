//
// Voxglitch "Overview Sequencer" module for VCV Rack
// by Bret Truchan
//
// TODO: When dragging points, have 'crosshairs' show up that extend to the
// x/y legends.
// TODO: When hovering under or above a point, create a vertical line at that
//  point.  If clicking in this state, reposition point up or down.
// TODO: shift-click on existing point should remove it

#include "plugin.hpp"
#include "osdialog.h"
#include "settings.hpp"
#include <fstream>
#include <array>

#include "OverviewSequencer/defines.h"
#include "OverviewSequencer/TimelineSequencerViewport.hpp"
#include "OverviewSequencer/TimelineSequencer.hpp"
#include "OverviewSequencer/OverviewSequencer.hpp"
#include "OverviewSequencer/TimelineSequencerWidget.hpp"
#include "OverviewSequencer/TimelineMiniMapWidget.hpp"
#include "OverviewSequencer/OverviewSequencerWidget.hpp"

Model* modelOverviewSequencer = createModel<OverviewSequencer, OverviewSequencerWidget>("overviewsequencer");
