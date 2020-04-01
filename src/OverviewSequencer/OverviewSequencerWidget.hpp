struct OverviewSequencerWidget : ModuleWidget
{
  OverviewSequencer* module;

  OverviewSequencerWidget(OverviewSequencer* module)
  {
    this->module = module;
    setModule(module);
    setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/transition_sequencer_front_panel.svg")));

    // Cosmetic rack screws
    addChild(createWidget<ScrewSilver>(Vec(15, 0)));
    addChild(createWidget<ScrewSilver>(Vec(15, 365)));
    addChild(createWidget<ScrewSilver>(mm2px(Vec(171.5, 0))));

    double button_spacing = 9.6; // 9.1
    double button_group_x = 48.0;
    double button_group_y = 103.0;
    // Sequence 1 button
    addParam(createParamCentered<LEDButton>(mm2px(Vec(button_group_x, button_group_y)), module, OverviewSequencer::SEQUENCER_1_BUTTON));
    addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(button_group_x, button_group_y)), module, OverviewSequencer::SEQUENCER_1_LIGHT));
    // Sequence 2 button
    addParam(createParamCentered<LEDButton>(mm2px(Vec(button_group_x + (button_spacing * 1.0), button_group_y)), module, OverviewSequencer::SEQUENCER_2_BUTTON));
    addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(button_group_x + (button_spacing * 1.0), button_group_y)), module, OverviewSequencer::SEQUENCER_2_LIGHT));
    // Sequence 3 button
    addParam(createParamCentered<LEDButton>(mm2px(Vec(button_group_x + (button_spacing * 2.0), button_group_y)), module, OverviewSequencer::SEQUENCER_3_BUTTON));
    addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(button_group_x + (button_spacing * 2.0), button_group_y)), module, OverviewSequencer::SEQUENCER_3_LIGHT));
    // Sequence 4 button
    addParam(createParamCentered<LEDButton>(mm2px(Vec(button_group_x + (button_spacing * 3.0), button_group_y)), module, OverviewSequencer::SEQUENCER_4_BUTTON));
    addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(button_group_x + (button_spacing * 3.0), button_group_y)), module, OverviewSequencer::SEQUENCER_4_LIGHT));
    // Sequence 5 button
    addParam(createParamCentered<LEDButton>(mm2px(Vec(button_group_x + (button_spacing * 4.0), button_group_y)), module, OverviewSequencer::SEQUENCER_5_BUTTON));
    addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(button_group_x + (button_spacing * 4.0), button_group_y)), module, OverviewSequencer::SEQUENCER_5_LIGHT));
    // Sequence 6 button
    addParam(createParamCentered<LEDButton>(mm2px(Vec(button_group_x + (button_spacing * 5.0), button_group_y)), module, OverviewSequencer::SEQUENCER_6_BUTTON));
    addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(button_group_x + (button_spacing * 5.0), button_group_y)), module, OverviewSequencer::SEQUENCER_6_LIGHT));

    // 6 sequencer outputs
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(118, 108.224)), module, OverviewSequencer::SEQ1_CV_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(129, 108.224)), module, OverviewSequencer::SEQ2_CV_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(140, 108.224)), module, OverviewSequencer::SEQ3_CV_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(151, 108.224)), module, OverviewSequencer::SEQ4_CV_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(162, 108.224)), module, OverviewSequencer::SEQ5_CV_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(173, 108.224)), module, OverviewSequencer::SEQ6_CV_OUTPUT));

    TimelineSequencerWidget *timeline_sequencer_widget = new TimelineSequencerWidget();
    timeline_sequencer_widget->box.pos = mm2px(Vec(DRAW_AREA_POSITION_X, DRAW_AREA_POSITION_Y));
    timeline_sequencer_widget->module = module;
    addChild(timeline_sequencer_widget);

    TimelineMiniMapWidget *timeline_mini_map_widget = new TimelineMiniMapWidget();
    timeline_mini_map_widget->box.pos = mm2px(Vec(MINI_MAP_POSITION_X, MINI_MAP_POSITION_Y));
    timeline_mini_map_widget->module = module;
    addChild(timeline_mini_map_widget);
  }

  void appendContextMenu(Menu *menu) override
  {
    OverviewSequencer *module = dynamic_cast<OverviewSequencer*>(this->module);
    assert(module);
  }

  void step() override {
    ModuleWidget::step();
  }

};
