struct TimelineMiniMapWidget : TransparentWidget
{
  OverviewSequencer *module;
  Vec drag_position;
  float window_box_position;
  float window_box_width = 32;

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
      // testing draw area
      /*
      nvgBeginPath(vg);
      nvgRect(vg, 0, 0, MINI_MAP_DRAW_AREA_WIDTH, MINI_MAP_DRAW_AREA_HEIGHT);
      nvgFillColor(vg, nvgRGBA(120, 120, 20, 40));
      nvgFill(vg);
      */

      float previous_x = -1;
      float previous_y = -1;

      // window_box_width = (MINI_MAP_DRAW_AREA_WIDTH / (module->sequencer.points.back().x + MINI_MAP_DRAW_AREA_WIDTH)) * MINI_MAP_DRAW_AREA_WIDTH;
      // DEBUG(std::to_string(window_box_width).c_str());

      unsigned int start_index;
      unsigned int end_index;
      std::tie(start_index, end_index) = module->sequencer.getPointIndexesWithinRange(range_start, range_end);

      // Draw lines on the minimap

      for(std::size_t i=start_index; i <= end_index; i++)
      {
        //  Vec position = module->sequencer.getPointPositionRelativeToViewport(i);

        Vec position = module->sequencer.getPoint(i);
        position.x = (position.x / DRAW_AREA_WIDTH) * MINI_MAP_WINDOW_BOX_WIDTH;
        position.y = (position.y / DRAW_AREA_HEIGHT) * MINI_MAP_DRAW_AREA_HEIGHT;

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
    float centered_position = x - 16;
    if(centered_position < 0) centered_position = 0;
    if(centered_position > 472.76) centered_position = 472.76;
    // DEBUG(std::to_string(centered_position).c_str());

    module->sequencer.setViewpointOffset(centered_position * MINI_MAP_MULTIPLIER);
    window_box_position = centered_position;
  }
};
