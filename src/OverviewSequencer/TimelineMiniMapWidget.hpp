struct TimelineMiniMapWidget : TransparentWidget
{
  OverviewSequencer *module;
  Vec drag_position;
  float window_box_position;
  float window_box_width = 32;
  float content_offset = 0;

  // TODO: don't hard code these. :-)
  float range_start = 0;
  float range_end = 600;

  TimelineMiniMapWidget()
  {
    box.size = Vec(MINI_MAP_DRAW_AREA_WIDTH, MINI_MAP_DRAW_AREA_HEIGHT);
    window_box_position = 0;
  }

  void draw(const DrawArgs &args) override
  {
    const auto vg = args.vg;

    // Save the drawing context to restore later
    nvgSave(vg);

    if(module)
    {
      // Visualize draw area for development testing
      // The draw area should be fine already, but keeping this code around
      // just in case the front-panel design changes at some point.
      /*
      nvgBeginPath(vg);
      nvgRect(vg, 0, 0, MINI_MAP_DRAW_AREA_WIDTH, MINI_MAP_DRAW_AREA_HEIGHT);
      nvgFillColor(vg, nvgRGBA(120, 120, 20, 40));
      nvgFill(vg);
      */

      // Store previous points while iterating through points so that we can
      // draw a line from the previous point to the current point
      float previous_x = -1;
      float previous_y = -1;

      // TODO: I'm not sure if this is correct
      // Even if it is, it will need to be variable later
      float range_end = MINI_MAP_WINDOW_BOX_WIDTH * DRAW_AREA_WIDTH;

      unsigned int start_index;
      unsigned int end_index;
      std::tie(start_index, end_index) = module->sequencer.getPointIndexesWithinRange(range_start, range_end);

      // Draw lines on the minimap

      for(std::size_t i=start_index; i <= end_index; i++)
      {
        // TODO: If not all of the points fit within the mini-map, draw the
        // the minimap offset similar to the scroll feature in the Atom editor
        //
        // In order to offset the minimap, I'll probably need to calculate
        // the total length of the minimap based on the last point x location
        // of the sequence being shown.  The x-location of the last point can
        // be found using: module->sequencer.points.back().x.  So the length
        // of the minimap might be:
        //
        // Is this right?  I don't know.
        // float minimap_length = module->sequencer.points.back().x / MINI_MAP_WINDOW_BOX_WIDTH;
        //
        // To log this, use:
        // DEBUG(std::to_string(minimap_length).c_str());
        //

        Vec position = module->sequencer.getPoint(i);
        position.x = ((position.x / DRAW_AREA_WIDTH) * MINI_MAP_WINDOW_BOX_WIDTH);
        position.y = ((position.y / DRAW_AREA_HEIGHT) * MINI_MAP_DRAW_AREA_HEIGHT);

        if(previous_x >= 0)
        {
          nvgBeginPath(vg);
          nvgMoveTo(vg, previous_x, previous_y);
          nvgLineTo(vg, position.x, position.y);
          nvgStrokeColor(vg, nvgRGBA(156, 167, 185, 255));
          nvgStroke(vg);
        }

        previous_x = position.x;
        previous_y = position.y;
      }

      // draw window box
      nvgBeginPath(vg);
      nvgRoundedRect(vg, window_box_position, 0, MINI_MAP_WINDOW_BOX_WIDTH, MINI_MAP_DRAW_AREA_HEIGHT, 3);
      nvgFillColor(vg, nvgRGBA(100, 100, 100, 150));
      nvgFill(vg);
    }

    nvgRestore(vg);
  }

  void onButton(const event::Button &e) override
  {
    if(e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS)
    {
      e.consume(this);
      drag_position = e.pos;
      this->reposition(e.pos.x);
    }
  }

  void onDragMove(const event::DragMove &e) override
  {
    TransparentWidget::onDragMove(e);
    double zoom = std::pow(2.f, settings::zoom);
    drag_position = drag_position.plus(e.mouseDelta.div(zoom));
    this->reposition(drag_position.x);
  }

  void reposition(float x)
  {
    // DEBUG(std::to_string(x).c_str());
    float centered_position = x - (MINI_MAP_WINDOW_BOX_WIDTH / 2.0);
    if(centered_position < 0) centered_position = 0;
    if(centered_position > 472.76) centered_position = 472.76;
    window_box_position = centered_position;

    // The viewport offset refers to the larger screen at the top of the module that
    // displays the sequence.  This code computes which "slice" of the sequence
    // to show in the viewport.
    float viewport_offset = ((x - (MINI_MAP_WINDOW_BOX_WIDTH / 2.0)) / MINI_MAP_WINDOW_BOX_WIDTH) * DRAW_AREA_WIDTH;
    module->sequencer.setViewpointOffset(viewport_offset);

  }
};
