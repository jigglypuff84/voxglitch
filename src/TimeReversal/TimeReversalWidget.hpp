struct TimeReversalWidget : ModuleWidget
{
  TimeReversalWidget(TimeReversal* module)
  {
    setModule(module);
    setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/time_reversal_front_panel.svg")));

    // Cosmetic rack screws
		//addChild(createWidget<ScrewSilver>(Vec(15, 0)));
		//addChild(createWidget<ScrewSilver>(Vec(15, 365)));

    // Playback Input
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10, 100)), module, TimeReversal::PLAYBACK_INPUT));


    // Audio input
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10, 114.702)), module, TimeReversal::AUDIO_INPUT_LEFT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21, 114.702)), module, TimeReversal::AUDIO_INPUT_RIGHT));

    // Audio output
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(32, 114.702)), module, TimeReversal::AUDIO_OUTPUT_LEFT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(43, 114.702)), module, TimeReversal::AUDIO_OUTPUT_RIGHT));

    // addInput(createInputCentered<PJ301MPort>(mm2px(Vec(triggers_column_x, group_y + (row_padding * 0))), module, SamplerX8::TRIGGER_INPUT_1));
    // addInput(createInputCentered<PJ301MPort>(mm2px(Vec(triggers_column_x, group_y + (row_padding * 1))), module, SamplerX8::TRIGGER_INPUT_2));

    // addParam(createParamCentered<Trimpot>(mm2px(Vec(volume_knobs_column_x, group_y + (row_padding * 0))), module, SamplerX8::VOLUME_KNOB_1));
    // addParam(createParamCentered<Trimpot>(mm2px(Vec(volume_knobs_column_x, group_y + (row_padding * 1))), module, SamplerX8::VOLUME_KNOB_2));

    // addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(output_l_column, 114.702)), module, SamplerX8::AUDIO_MIX_OUTPUT_LEFT));
		// addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(output_r_column, 114.702)), module, SamplerX8::AUDIO_MIX_OUTPUT_RIGHT));
  }


  void appendContextMenu(Menu *menu) override
  {
    TimeReversal *module = dynamic_cast<TimeReversal*>(this->module);
    assert(module);
  }
};
