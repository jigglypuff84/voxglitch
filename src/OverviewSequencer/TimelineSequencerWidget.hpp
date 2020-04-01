struct TimelineSequencerWidget : TransparentWidget
{
  OverviewSequencer *module;
  Vec drag_position;
  unsigned int selected_point_index = 0;
  bool dragging_point = false;
  unsigned int hover_point_index = 0;
  bool hovering_over_point = true;

  TimelineSequencerWidget()
  {
    box.size = Vec(DRAW_AREA_WIDTH, DRAW_AREA_HEIGHT);
  }

  void draw(const DrawArgs &args) override
  {
    const auto vg = args.vg;

    // Save the drawing context to restore later
    nvgSave(vg);

    if(module)
    {
      // for each point
      //   if point is in window
      //     draw point
      //     draw line from previous point to this point
      // end foreach
      // draw line from last visible point to point offscreen

      float previous_x = -1;
      float previous_y = -1;

      unsigned int start_index;
      unsigned int end_index;

      std::tie(start_index, end_index) = module->sequencer.getPointIndexesWithinViewport();

      // if(start_index > 0) start_index--;

      //
      // Draw all the lines first
      //

      if (start_index > 0) draw_line_offscreen_left(vg, start_index);
      if (end_index < (module->sequencer.points.size() - 1)) draw_line_offscreen_right(vg, end_index);

      for(std::size_t i=start_index; i <= end_index; i++)
      {
        Vec position = module->sequencer.getPointPositionRelativeToViewport(i);

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

      for(std::size_t i=start_index; i <= end_index; i++)
      {
        // for (std::pair<std::double, float> element : module->sequencer.points) {
        // Accessing KEY from element

        Vec position = module->sequencer.getPointPositionRelativeToViewport(i);

        // (outer circle)
        nvgBeginPath(vg);
        nvgCircle(vg, position.x, position.y, 10);
        nvgFillColor(vg, nvgRGBA(156, 167, 185, 20));
        nvgFill(vg);

        bool highlight_point = false;
        if(dragging_point && selected_point_index == i) highlight_point = true;
        if(hovering_over_point && hover_point_index == i) highlight_point = true;

        // (inner circle)
        nvgBeginPath(vg);
        nvgCircle(vg, position.x, position.y, 5);
        nvgFillColor(vg, nvgRGBA(156, 167, 185, 255));
        if(highlight_point) nvgFillColor(vg, nvgRGBA(255, 255, 255, 255));
        nvgFill(vg);
      }

      // testing draw area
      /*
      nvgBeginPath(vg);
      nvgRect(vg, 0, 0, DRAW_AREA_WIDTH, DRAW_AREA_HEIGHT);
      nvgFillColor(vg, nvgRGBA(120, 20, 20, 100));
      nvgFill(vg);
      */
    }

    nvgRestore(vg);
  }

  void draw_line_offscreen_left(NVGcontext *vg, float start_index)
  {
    // draw line from left side of the screen to the first point
    Vec position = module->sequencer.getPointPositionRelativeToViewport(start_index);
    Vec previous_position =  module->sequencer.getPointPositionRelativeToViewport(start_index-1);

    float x2 = position.x;
    float y2 = position.y;
    float x1 = previous_position.x;
    float y1 = previous_position.y;

    float m = (y2 - y1) / (x2 - x1);
    float yIntercept = y1 - (m * x1);

    nvgBeginPath(vg);
    nvgMoveTo(vg, position.x, position.y);
    nvgLineTo(vg, 0, yIntercept);
    nvgStrokeColor(vg, nvgRGBA(156, 167, 185, 255));
    nvgStroke(vg);
  }


  void draw_line_offscreen_right(NVGcontext *vg, float end_index)
  {
    // draw line from left side of the screen to the first point
    //     Vec position = module->sequencer.getPointPositionRelativeToViewport(end_index);
    //    Vec next_position =  module->sequencer.getPointPositionRelativeToViewport(end_index + 1);

    Vec position = module->sequencer.getPointPositionRelativeToViewport(end_index);
    Vec next_position =  module->sequencer.getPointPositionRelativeToViewport(end_index + 1);

    float x2 = position.x;
    float y2 = position.y;
    float x1 = next_position.x;
    float y1 = next_position.y;

    float m = (y2 - y1) / (x2 - x1);
    float yIntercept = y1 - (m * x1);

    nvgBeginPath(vg);
    nvgMoveTo(vg, position.x, position.y);
    nvgLineTo(vg, DRAW_AREA_WIDTH, yIntercept);
    nvgStrokeColor(vg, nvgRGBA(156, 167, 185, 255));
    nvgStroke(vg);
  }

  void onButton(const event::Button &e) override
  {
    if(e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS)
    {
      e.consume(this);
      drag_position = e.pos;

      if(e.mods == GLFW_MOD_SHIFT)
      {

        //
        // If the user is mousing over a point while shift-clicking,
        // the point will be removed
        //
        bool point_is_moused_over = false;
        unsigned int point_index = 0;

        std::tie(point_is_moused_over, point_index) = mousedOverPoint(e.pos);

        if(point_is_moused_over)
        {
          // Remove point
          //...
          module->sequencer.removePoint(point_index);
        }
        else
        {
          // Insert new point
          // TODO: replace drag_position in the followin line of code
          // with the position computed given the draw window

          unsigned int number_of_points = module->sequencer.points.size();
          int insert_at_location = 0;



          if(number_of_points == 0)
          {
            insert_at_location = 0;
          }
          // If adding a point to the beginning
          else if(drag_position.x < module->sequencer.viewportFromIndex(module->sequencer.points[0].x))
          {
            insert_at_location = 0;
          }
          // If adding a point to the end
          else if(drag_position.x > module->sequencer.viewportFromIndex(module->sequencer.points.back().x))
          {
            insert_at_location = module->sequencer.points.size();
          }

          // If adding a point somewhere in the middle
          else
          {
            for(std::size_t i=0; i<module->sequencer.points.size() - 1; i++)
            {
              float first_point_viewport_x = module->sequencer.viewportFromIndex(module->sequencer.points[i].x);
              float next_point_viewport_x = module->sequencer.viewportFromIndex(module->sequencer.points[i+1].x);

              if((drag_position.x > first_point_viewport_x) && (drag_position.x < next_point_viewport_x))
              {
                insert_at_location = i + 1;
              }
            }
          }


          // This cannot be done while iterating over the vector
          module->sequencer.points.insert(module->sequencer.points.begin() + insert_at_location, drag_position);

          // Begin dragging
          selected_point_index = insert_at_location;
          dragging_point = true;
        }
      }
      else
      {
        // see if the user is selecting a point
        std::tie(dragging_point, selected_point_index) = mousedOverPoint(e.pos);
      }
    }

    if(e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_RELEASE)
    {
      dragging_point = false;
    }
  }

  void onDragMove(const event::DragMove &e) override
  {
    TransparentWidget::onDragMove(e);
    double zoom = std::pow(2.f, settings::zoom);
    drag_position = drag_position.plus(e.mouseDelta.div(zoom));

    // IF the user is dragging a point
    if(dragging_point && module->sequencer.points.size() > 0)
    {
      Vec point_position = module->sequencer.indexFromViewport(drag_position);

      if(selected_point_index > 0 )
      {
        if (point_position.x > module->sequencer.points[selected_point_index - 1].x)
        {
          module->sequencer.points[selected_point_index] = point_position;
        }
        else
        {
          module->sequencer.points[selected_point_index].x = module->sequencer.points[selected_point_index - 1].x;
        }
      }
      else
      {
        Vec point_position = module->sequencer.indexFromViewport(drag_position);
        module->sequencer.points[selected_point_index] = point_position;
      }
    }
    // Otherwise let the mouse drag the viewport offset
    else
    {
      float drag_x = e.mouseDelta.div(zoom).x;
      module->sequencer.setViewpointOffset(module->sequencer.getViewpointOffset() - drag_x);
    }
  }

  void onLeave(const event::Leave &e) override
  {
    TransparentWidget::onLeave(e);
  }

  void onHover(const event::Hover& e) override
  {
    TransparentWidget::onHover(e);
    e.consume(this);

    hovering_over_point = false;
    std::tie(hovering_over_point, hover_point_index) = mousedOverPoint(e.pos);
  }

  std::pair<bool, unsigned int> mousedOverPoint(Vec mouse_position)
  {
    unsigned int point_index = 0;
    bool mouse_is_over_point = false;

    // TODO: change this to only iterate over visible points
    for(std::size_t i=0; i<module->sequencer.points.size(); i++)
    {
      Vec position = module->sequencer.viewportFromIndex(module->sequencer.points[i]);

      if(position.x > (mouse_position.x - 16) &&
      position.x < (mouse_position.x + 16) &&
      position.y > (mouse_position.y - 16) &&
      position.y < (mouse_position.y + 16)
    )
    {
      point_index = i;
      mouse_is_over_point = true;
    }
  }

  return { mouse_is_over_point, point_index };
}
};
