struct TimelineSequencer
{
  // "points" is a map with the keys representing milliseconds and the value
  // representing the VC value at that position in time.
  // std::map<double, float> points;
  std::vector<Vec> points;

  TimelineSequencerViewport viewport;

  // constructor
  TimelineSequencer()
  {
    points.push_back(Vec(100, 100.50));
    points.push_back(Vec(220, 120.00));
    points.push_back(Vec(300, 60.00));
  }

  Vec getPoint(unsigned int index)
  {
    return(Vec(points[index].x, points[index].y));
  }

  Vec getPointPositionRelativeToViewport(unsigned int index)
  {
    return(Vec(points[index].x - viewport.offset, points[index].y));
  }

  Vec viewportFromIndex(Vec position)
  {
    return(Vec(position.x - viewport.offset, position.y));
  }

  float viewportFromIndex(float x)
  {
    return(x - viewport.offset);
  }

  Vec indexFromViewport(Vec position)
  {
    return(Vec(viewport.offset + position.x, position.y));
  }

  float indexFromViewport(float x)
  {
    return(x + viewport.offset);
  }

  void removePoint(unsigned int index)
  {
    points.erase(points.begin() + index);
  }

  void setViewpointOffset(float offset)
  {
    if(offset > 0) viewport.offset = offset;
  }

  float getViewpointOffset()
  {
    return(viewport.offset);
  }

  std::pair<unsigned int, unsigned int> getPointIndexesWithinViewport()
  {
    unsigned int begin_index = 0;
    unsigned int end_index = 0;

    std::tie(begin_index, end_index) = getPointIndexesWithinRange(viewport.offset, (viewport.offset + viewport.width));

    return { begin_index, end_index };
  }

  std::pair<unsigned int, unsigned int> getPointIndexesWithinRange(float start, float end)
  {
    unsigned int begin_index = 0;
    unsigned int end_index = 0;

    bool begin_index_located = false;

    for(std::size_t i=0; i < points.size(); i++)
    {
      if((points[i].x >= start) && (points[i].x <= end))
      {
        if(! begin_index_located)
        {
          begin_index = i;
          begin_index_located = true;
        }
        end_index = i;
      }
    }
    return { begin_index, end_index };
  }

};
