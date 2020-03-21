//
// Voxglitch "Transition Sequencer" module for VCV Rack
// by Bret Truchan
//


#include "plugin.hpp"
#include "osdialog.h"
#include "settings.hpp"
#include <fstream>
#include <array>

#define NUMBER_OF_SEQUENCERS 6
#define MAX_SEQUENCER_STEPS 32
#define NUMBER_OF_VOLTAGE_RANGES 8
#define NUMBER_OF_SNAP_DIVISIONS 8

// Constants for patterns
#define DRAW_AREA_WIDTH 486.0
#define DRAW_AREA_HEIGHT 214.0
#define BAR_HEIGHT 214.0
#define BAR_HORIZONTAL_PADDING .8
#define DRAW_AREA_POSITION_X 9
#define DRAW_AREA_POSITION_Y 9.5


#define TOOLTIP_WIDTH 33.0
#define TOOLTIP_HEIGHT 20.0

/*
double voltage_ranges[NUMBER_OF_VOLTAGE_RANGES][2] = {
    { 0.0, 10.0 },
    { -10.0, 10.0 },
    { 0.0, 5.0 },
    { -5.0, 5.0 },
    { 0.0, 3.0 },
    { -3.0, 3.0 },
    { 0.0, 1.0},
    { -1.0, 1.0}
};
*/

struct TimelineSequencer
{
    // "points" is a map with the keys representing milliseconds and the value
    // representing the VC value at that position in time.
    std::map<double, float> points;

    // constructor
    TimelineSequencer()
    {
        points.insert({ 100, 100.50 });
        points.insert({ 220, 120.00 });
        points.insert({ 300, 60.00 });
    }
};

struct TransitionSequencer : Module
{
    int selected_sequencer_index = 0;
    double sample_rate;
    TimelineSequencer sequencer;

    dsp::SchmittTrigger sequencer_1_button_trigger;
    dsp::SchmittTrigger sequencer_2_button_trigger;
    dsp::SchmittTrigger sequencer_3_button_trigger;
    dsp::SchmittTrigger sequencer_4_button_trigger;
    dsp::SchmittTrigger sequencer_5_button_trigger;
    dsp::SchmittTrigger sequencer_6_button_trigger;

    bool sequencer_1_button_is_triggered;
    bool sequencer_2_button_is_triggered;
    bool sequencer_3_button_is_triggered;
    bool sequencer_4_button_is_triggered;
    bool sequencer_5_button_is_triggered;
    bool sequencer_6_button_is_triggered;

    int voltage_outputs[NUMBER_OF_SEQUENCERS];

    std::string voltage_range_names[NUMBER_OF_VOLTAGE_RANGES] = {
        "0.0 to 10.0",
        "-10.0 to 10.0",
        "0.0 to 5.0",
        "-5.0 to 5.0",
        "0.0 to 3.0",
        "-3.0 to 3.0",
        "0.0 to 1.0",
        "-1.0 to 1.0"
    };

	enum ParamIds {
        SEQUENCER_1_BUTTON,
        SEQUENCER_2_BUTTON,
        SEQUENCER_3_BUTTON,
        SEQUENCER_4_BUTTON,
        SEQUENCER_5_BUTTON,
        SEQUENCER_6_BUTTON,
		NUM_PARAMS
	};
	enum InputIds {
        RESET_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
        SEQ1_CV_OUTPUT,
        SEQ2_CV_OUTPUT,
        SEQ3_CV_OUTPUT,
        SEQ4_CV_OUTPUT,
        SEQ5_CV_OUTPUT,
        SEQ6_CV_OUTPUT,

		NUM_OUTPUTS
	};
	enum LightIds {
        SEQUENCER_1_LIGHT,
        SEQUENCER_2_LIGHT,
        SEQUENCER_3_LIGHT,
        SEQUENCER_4_LIGHT,
        SEQUENCER_5_LIGHT,
        SEQUENCER_6_LIGHT,
		NUM_LIGHTS
	};

	//
	// Constructor
	//
	TransitionSequencer()
	{
        voltage_outputs[0] = SEQ1_CV_OUTPUT;
        voltage_outputs[1] = SEQ2_CV_OUTPUT;
        voltage_outputs[2] = SEQ3_CV_OUTPUT;
        voltage_outputs[3] = SEQ4_CV_OUTPUT;
        voltage_outputs[4] = SEQ5_CV_OUTPUT;
        voltage_outputs[5] = SEQ6_CV_OUTPUT;

		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(SEQUENCER_1_BUTTON, 0.f, 1.f, 0.f, "Sequence1Button");
        configParam(SEQUENCER_2_BUTTON, 0.f, 1.f, 0.f, "Sequence2Button");
        configParam(SEQUENCER_3_BUTTON, 0.f, 1.f, 0.f, "Sequence3Button");
        configParam(SEQUENCER_4_BUTTON, 0.f, 1.f, 0.f, "Sequence4Button");
        configParam(SEQUENCER_5_BUTTON, 0.f, 1.f, 0.f, "Sequence5Button");
        configParam(SEQUENCER_6_BUTTON, 0.f, 1.f, 0.f, "Sequence6Button");
	}

    /*
        ==================================================================================================================================================
                                 ___                 _
                                / / |               | |
          ___  __ ___   _____  / /| | ___   __ _  __| |
        / __|/ _` \ \ / / _ \ / / | |/ _ \ / _` |/ _` |
        \__ \ (_| |\ V /  __// /  | | (_) | (_| | (_| |
        |___/\__,_| \_/ \___/_/   |_|\___/ \__,_|\__,_|

        ==================================================================================================================================================
    */

	json_t *dataToJson() override
	{
        json_t *json_root = json_object();
        return json_root;
	}

	// Autoload settings
	void dataFromJson(json_t *json_root) override
	{
	}


    /*

    ______
    | ___ \
    | |_/ / __ ___   ___ ___  ___ ___
    |  __/ '__/ _ \ / __/ _ \/ __/ __|
    | |  | | | (_) | (_|  __/\__ \__ \
    \_|  |_|  \___/ \___\___||___/___/


    */

	void process(const ProcessArgs &args) override
	{
        this->sample_rate = args.sampleRate;

        sequencer_1_button_is_triggered = sequencer_1_button_trigger.process(params[SEQUENCER_1_BUTTON].getValue());
        sequencer_2_button_is_triggered = sequencer_2_button_trigger.process(params[SEQUENCER_2_BUTTON].getValue());
        sequencer_3_button_is_triggered = sequencer_3_button_trigger.process(params[SEQUENCER_3_BUTTON].getValue());
        sequencer_4_button_is_triggered = sequencer_4_button_trigger.process(params[SEQUENCER_4_BUTTON].getValue());
        sequencer_5_button_is_triggered = sequencer_5_button_trigger.process(params[SEQUENCER_5_BUTTON].getValue());
        sequencer_6_button_is_triggered = sequencer_6_button_trigger.process(params[SEQUENCER_6_BUTTON].getValue());

		if(sequencer_1_button_is_triggered) selected_sequencer_index = 0;
        if(sequencer_2_button_is_triggered) selected_sequencer_index = 1;
        if(sequencer_3_button_is_triggered) selected_sequencer_index = 2;
        if(sequencer_4_button_is_triggered) selected_sequencer_index = 3;
        if(sequencer_5_button_is_triggered) selected_sequencer_index = 4;
        if(sequencer_6_button_is_triggered) selected_sequencer_index = 5;


        lights[SEQUENCER_1_LIGHT].setBrightness(selected_sequencer_index == 0);
        lights[SEQUENCER_2_LIGHT].setBrightness(selected_sequencer_index == 1);
        lights[SEQUENCER_3_LIGHT].setBrightness(selected_sequencer_index == 2);
        lights[SEQUENCER_4_LIGHT].setBrightness(selected_sequencer_index == 3);
        lights[SEQUENCER_5_LIGHT].setBrightness(selected_sequencer_index == 4);
        lights[SEQUENCER_6_LIGHT].setBrightness(selected_sequencer_index == 5);
	}

};

/*

 _    _ _     _            _
| |  | (_)   | |          | |
| |  | |_  __| | __ _  ___| |_ ___
| |/\| | |/ _` |/ _` |/ _ \ __/ __|
\  /\  / | (_| | (_| |  __/ |_\__ \
\/  \/|_|\__,_|\__, |\___|\__|___/
                __/ |
               |___/
*/

struct TimelineSequencerWidget : TransparentWidget
{
    TransitionSequencer *module;
    Vec drag_position;

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

            for (auto const& pair : module->sequencer.points)
            {
                // for (std::pair<std::double, float> element : module->sequencer.points) {
        		// Accessing KEY from element
        		double time = pair.first;
        		float cv_value = pair.second;

                // calculate position based on time and the position of the
                // drawing window.
                float x = (float) time;
                float y = cv_value;

                if(previous_x >= 0)
                {
                    nvgBeginPath(vg);
                    nvgMoveTo(vg, previous_x, previous_y);
                	nvgLineTo(vg, x, y);
                	nvgStrokeColor(vg, nvgRGBA(156, 167, 185, 255));
                	nvgStroke(vg);
                }

                // (outer circle)
                nvgBeginPath(vg);
                nvgCircle(vg, x, y, 10);
                nvgFillColor(vg, nvgRGBA(156, 167, 185, 20));
                nvgFill(vg);

                // (inner circle)
                nvgBeginPath(vg);
                nvgCircle(vg, x, y, 5);
                nvgFillColor(vg, nvgRGBA(156, 167, 185, 255));
                nvgFill(vg);

                previous_x = x;
                previous_y = y;
        	}


            // draw lines first
            /*
            nvgBeginPath(vg);
            nvgMoveTo(vg, 100, 100);
        	nvgLineTo(vg, 220, 120);
        	nvgStrokeColor(vg, nvgRGBA(156, 167, 185, 255));
        	nvgStroke(vg);
            */

            /*
            // draw fisrt point
            // (outer circle)
            nvgBeginPath(vg);
            nvgCircle(vg, 100, 100, 10);
            nvgFillColor(vg, nvgRGBA(156, 167, 185, 20));
            nvgFill(vg);

            // (inner circle)
            nvgBeginPath(vg);
            nvgCircle(vg, 100, 100, 5);
            nvgFillColor(vg, nvgRGBA(156, 167, 185, 255));
            nvgFill(vg);
            */

            // draw second point
            // (outer circle)
            /*
            nvgBeginPath(vg);
            nvgCircle(vg, 220, 120, 10);
            nvgFillColor(vg, nvgRGBA(156, 167, 185, 20));
            nvgFill(vg);

            // (inner circle)
            nvgBeginPath(vg);
            nvgCircle(vg, 220, 120, 5);
            nvgFillColor(vg, nvgRGBA(156, 167, 185, 255));
            nvgFill(vg);
            */

        }

        nvgRestore(vg);
    }
};

struct TransitionSequencerWidget : ModuleWidget
{
    TransitionSequencer* module;

	TransitionSequencerWidget(TransitionSequencer* module)
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
        addParam(createParamCentered<LEDButton>(mm2px(Vec(button_group_x, button_group_y)), module, TransitionSequencer::SEQUENCER_1_BUTTON));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(button_group_x, button_group_y)), module, TransitionSequencer::SEQUENCER_1_LIGHT));
        // Sequence 2 button
        addParam(createParamCentered<LEDButton>(mm2px(Vec(button_group_x + (button_spacing * 1.0), button_group_y)), module, TransitionSequencer::SEQUENCER_2_BUTTON));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(button_group_x + (button_spacing * 1.0), button_group_y)), module, TransitionSequencer::SEQUENCER_2_LIGHT));
        // Sequence 3 button
        addParam(createParamCentered<LEDButton>(mm2px(Vec(button_group_x + (button_spacing * 2.0), button_group_y)), module, TransitionSequencer::SEQUENCER_3_BUTTON));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(button_group_x + (button_spacing * 2.0), button_group_y)), module, TransitionSequencer::SEQUENCER_3_LIGHT));
        // Sequence 4 button
        addParam(createParamCentered<LEDButton>(mm2px(Vec(button_group_x + (button_spacing * 3.0), button_group_y)), module, TransitionSequencer::SEQUENCER_4_BUTTON));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(button_group_x + (button_spacing * 3.0), button_group_y)), module, TransitionSequencer::SEQUENCER_4_LIGHT));
        // Sequence 5 button
        addParam(createParamCentered<LEDButton>(mm2px(Vec(button_group_x + (button_spacing * 4.0), button_group_y)), module, TransitionSequencer::SEQUENCER_5_BUTTON));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(button_group_x + (button_spacing * 4.0), button_group_y)), module, TransitionSequencer::SEQUENCER_5_LIGHT));
        // Sequence 6 button
        addParam(createParamCentered<LEDButton>(mm2px(Vec(button_group_x + (button_spacing * 5.0), button_group_y)), module, TransitionSequencer::SEQUENCER_6_BUTTON));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(button_group_x + (button_spacing * 5.0), button_group_y)), module, TransitionSequencer::SEQUENCER_6_LIGHT));

        // 6 sequencer outputs
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(118, 108.224)), module, TransitionSequencer::SEQ1_CV_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(129, 108.224)), module, TransitionSequencer::SEQ2_CV_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(140, 108.224)), module, TransitionSequencer::SEQ3_CV_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(151, 108.224)), module, TransitionSequencer::SEQ4_CV_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(162, 108.224)), module, TransitionSequencer::SEQ5_CV_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(173, 108.224)), module, TransitionSequencer::SEQ6_CV_OUTPUT));

        TimelineSequencerWidget *timeline_sequencer_widget = new TimelineSequencerWidget();
		timeline_sequencer_widget->box.pos = mm2px(Vec(DRAW_AREA_POSITION_X, DRAW_AREA_POSITION_Y));
		timeline_sequencer_widget->module = module;
		addChild(timeline_sequencer_widget);

	}

	void appendContextMenu(Menu *menu) override
	{
        TransitionSequencer *module = dynamic_cast<TransitionSequencer*>(this->module);
		assert(module);
	}

    void step() override {
        ModuleWidget::step();
    }

};

Model* modelTransitionSequencer = createModel<TransitionSequencer, TransitionSequencerWidget>("transitionsequencer");
