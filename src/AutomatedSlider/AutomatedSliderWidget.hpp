struct AutomatedSliderWidget : ModuleWidget
{
	AutomatedSliderWidget(AutomatedSlider* module)
	{
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/automated_slider_front_panel.svg")));

    AutomatedSliderDisplay *automated_slider_display;
    automated_slider_display = new AutomatedSliderDisplay(module);
    automated_slider_display->box.pos = mm2px(Vec(.35, 6.10));
    addChild(automated_slider_display);
	}

};
