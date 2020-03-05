//
// Voxglitch "DigitalSequencer" module for VCV Rack
// by Bret Truchan
//

#include "plugin.hpp"
#include "osdialog.h"
#include <fstream>
#include <array>

#define MAX_SEQUENCER_STEPS 16

// Constants for patterns
#define DRAW_AREA_WIDTH 486.0
#define DRAW_AREA_HEIGHT 214.0
#define BAR_HEIGHT 214.0
#define BAR_HORIZONTAL_PADDING .8
#define DRAW_AREA_POSITION_X 9
#define DRAW_AREA_POSITION_Y 9.5

// Constants for gate sequencer
#define GATES_DRAW_AREA_WIDTH 486.0
#define GATES_DRAW_AREA_HEIGHT 16.0
#define GATES_DRAW_AREA_POSITION_X 9
#define GATES_DRAW_AREA_POSITION_Y 86
#define GATE_BAR_HEIGHT 16.0

#define TOOLTIP_WIDTH 33.0
#define TOOLTIP_HEIGHT 20.0

struct Sequencer
{
    unsigned int sequence_length = 16;
    unsigned int sequence_playback_position = 0;

    void step()
    {
        sequence_playback_position = (sequence_playback_position + 1) % sequence_length;
    }

    void reset()
    {
        sequence_playback_position = 0;
    }

    unsigned int getPlaybackPosition()
    {
        return(sequence_playback_position);
    }

    unsigned int getLength()
    {
        return(sequence_length);
    }

    void setLength(unsigned int sequence_length)
    {
        this->sequence_length = sequence_length;
    }
};

struct VoltageSequencer : Sequencer
{
    std::array<float, MAX_SEQUENCER_STEPS> sequence;

    // constructor
    VoltageSequencer()
    {
        sequence.fill(0.0);
    }

    float getValue(int index)
    {
        return(sequence[index]);
    }

    float getValue()
    {
        return(sequence[getPlaybackPosition()]);
    }

    void setValue(int index, float value)
    {
        sequence[index] = value;
    }

    void shiftLeft()
    {
        float temp = sequence[0];
        for(unsigned int i=0; i < this->sequence_length-1; i++)
        {
            sequence[i] = sequence[i+1];
        }
        sequence[this->sequence_length-1] = temp;
    }

    void shiftRight()
    {
        float temp = sequence[this->sequence_length - 1];

        for(unsigned int i=this->sequence_length-1; i>0; i--)
        {
            sequence[i] = sequence[i-1];
        }

        sequence[0] = temp;
    }
};

struct GateSequencer : Sequencer
{
    std::array<bool, MAX_SEQUENCER_STEPS> sequence;

    // constructor
    GateSequencer()
    {
        sequence.fill(0.0);
    }

    bool getValue(int index)
    {
        return(sequence[index]);
    }

    bool getValue()
    {
        return(sequence[sequence_playback_position]);
    }

    void setValue(int index, bool value)
    {
        sequence[index] = value;
    }

    void shiftLeft()
    {
        float temp = sequence[0];
        for(unsigned int i=0; i < this->sequence_length-1; i++)
        {
            sequence[i] = sequence[i+1];
        }
        sequence[this->sequence_length-1] = temp;
    }

    void shiftRight()
    {
        float temp = sequence[this->sequence_length - 1];

        for(unsigned int i=this->sequence_length-1; i>0; i--)
        {
            sequence[i] = sequence[i-1];
        }

        sequence[0] = temp;
    }
};

struct DigitalSequencer : Module
{
	dsp::SchmittTrigger stepTrigger;
    dsp::SchmittTrigger sequencer_step_triggers[NUMBER_OF_SEQUENCERS];
    dsp::SchmittTrigger resetTrigger;

    long clock_ignore_on_reset = 0;
    unsigned int tooltip_timer = 0;

    VoltageSequencer voltage_sequencers[NUMBER_OF_SEQUENCERS];
    VoltageSequencer *selected_voltage_sequencer;

    GateSequencer gate_sequencers[NUMBER_OF_SEQUENCERS];
    GateSequencer *selected_gate_sequencer;

    int selected_sequencer_index = 0;
    int previously_selected_sequencer_index = -1;
    int voltage_outputs[NUMBER_OF_SEQUENCERS];
    int gate_outputs[NUMBER_OF_SEQUENCERS];
    int sequencer_step_inputs[NUMBER_OF_SEQUENCERS];

    dsp::PulseGenerator gateOutputPulseGenerators[NUMBER_OF_SEQUENCERS];
    float sample_rate;

	enum ParamIds {
        SEQUENCER_LENGTH_KNOB,
		NUM_PARAMS
	};
	enum InputIds {
        STEP_INPUT,
        RESET_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
        CV_OUTPUT,
        GATE_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	//
	// Constructor
	//
	DigitalSequencer()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(SEQUENCER_LENGTH_KNOB, 1, MAX_SEQUENCER_STEPS, MAX_SEQUENCER_STEPS, "SequenceLengthKnob");
	}

    /*
                                 ___                 _
                                / / |               | |
          ___  __ ___   _____  / /| | ___   __ _  __| |
        / __|/ _` \ \ / / _ \ / / | |/ _ \ / _` |/ _` |
        \__ \ (_| |\ V /  __// /  | | (_) | (_| | (_| |
        |___/\__,_| \_/ \___/_/   |_|\___/ \__,_|\__,_|

    */

	json_t *dataToJson() override
	{
        json_t *json_root = json_object();

        /*
		//
		// Save patterns
		//

		json_t *sequences_json_array = json_array();

		for(int sequencer_number=0; sequencer_number<NUMBER_OF_SEQUENCERS; sequencer_number++)
		{
			json_t *pattern_json_array = json_array();

			for(int i=0; i<MAX_SEQUENCER_STEPS; i++)
			{
				json_array_append_new(pattern_json_array, json_integer(this->voltage_sequencers[sequencer_number].getValue(i)));
			}

            json_array_append_new(sequences_json_array, pattern_json_array);
        }

        json_object_set(json_root, "patterns", sequences_json_array);
        json_decref(sequences_json_array);

        //
		// Save gates
		//

		json_t *gates_json_array = json_array();

		for(int sequencer_number=0; sequencer_number<NUMBER_OF_SEQUENCERS; sequencer_number++)
		{
			json_t *pattern_json_array = json_array();

			for(int i=0; i<MAX_SEQUENCER_STEPS; i++)
			{
				json_array_append_new(pattern_json_array, json_integer(this->gate_sequencers[sequencer_number].getValue(i)));
			}

            json_array_append_new(gates_json_array, pattern_json_array);
        }

        json_object_set(json_root, "gates", gates_json_array);
        json_decref(gates_json_array);

        //
        // Save sequencer lengths
        //
        json_t *sequencer_lengths_json_array = json_array();
        for(int sequencer_number=0; sequencer_number<NUMBER_OF_SEQUENCERS; sequencer_number++)
        {
            json_array_append_new(sequencer_lengths_json_array, json_integer(this->voltage_sequencers[sequencer_number].getLength()));
        }
        json_object_set(json_root, "lengths", sequencer_lengths_json_array);
        json_decref(sequencer_lengths_json_array);
        */

		return json_root;
	}

	// Autoload settings
	void dataFromJson(json_t *json_root) override
	{
		//
		// Load patterns
		//

		json_t *pattern_arrays_data = json_object_get(json_root, "patterns");

        /*
		if(pattern_arrays_data)
		{
			size_t pattern_number;
			json_t *json_pattern_array;

			json_array_foreach(pattern_arrays_data, pattern_number, json_pattern_array)
			{
				for(int i=0; i<MAX_SEQUENCER_STEPS; i++)
				{
					// this->sequences[pattern_number][i] = json_integer_value(json_array_get(json_pattern_array, i));
                    this->voltage_sequencers[pattern_number].setValue(i, json_integer_value(json_array_get(json_pattern_array, i)));
				}
			}
		}

        //
		// Load gates
		//

        json_t *gates_arrays_data = json_object_get(json_root, "gates");

        if(gates_arrays_data)
        {
            size_t pattern_number;
            json_t *json_pattern_array;

            json_array_foreach(gates_arrays_data, pattern_number, json_pattern_array)
            {
                for(int i=0; i<MAX_SEQUENCER_STEPS; i++)
                {
                    // this->gates[pattern_number][i] = json_integer_value(json_array_get(json_pattern_array, i));
                    this->gate_sequencers[pattern_number].setValue(i, json_integer_value(json_array_get(json_pattern_array, i)));
                }
            }
        }

        //
        // Load lengths
        //
        json_t *lengths_json_array = json_object_get(json_root, "lengths");

        if(lengths_json_array)
        {
            size_t sequencer_number;
            json_t *length_json;

            json_array_foreach(lengths_json_array, sequencer_number, length_json)
            {
                this->voltage_sequencers[sequencer_number].setLength(json_integer_value(length_json));
                this->gate_sequencers[sequencer_number].setLength(json_integer_value(length_json));
            }
        }
        */
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
        bool trigger_output_pulse = false;
        this->sample_rate = args.sampleRate;

        // On incoming RESET, reset the sequencers
        if(resetTrigger.process(rescale(inputs[RESET_INPUT].getVoltage(), 0.0f, 10.0f, 0.f, 1.f)))
        {
            // Set up a (reverse) counter so that the clock input will ignore
            // incoming clock pulses for 1 millisecond after a reset input. This
            // is to comply with VCV Rack's standards.  See section "Timing" at
            // https://vcvrack.com/manual/VoltageStandards
            /*
            clock_ignore_on_reset = (long) (args.sampleRate / 100);

            stepTrigger.reset();

            for(unsigned int i=0; i < NUMBER_OF_SEQUENCERS; i++)
            {
                sequencer_step_triggers[i].reset();
                voltage_sequencers[i].reset();
                gate_sequencers[i].reset();
            }
            */
        }
        else if(clock_ignore_on_reset == 0)
        {
            /*
            // Step ALL of the sequencers
            bool global_step_trigger = stepTrigger.process(rescale(inputs[STEP_INPUT].getVoltage(), 0.0f, 10.0f, 0.f, 1.f));
            bool step;

            for(unsigned int i=0; i < NUMBER_OF_SEQUENCERS; i++)
            {
                step = false;

                if(inputs[sequencer_step_inputs[i]].isConnected() == false)
                {
                    if(global_step_trigger) step = true;
                }
                else if (sequencer_step_triggers[i].process(rescale(inputs[sequencer_step_inputs[i]].getVoltage(), 0.0f, 10.0f, 0.f, 1.f)))
                {
                    step = true;
                }

                if(step)
                {
                    voltage_sequencers[i].step();
                    gate_sequencers[i].step();
                    if(gate_sequencers[i].getValue()) gateOutputPulseGenerators[i].trigger(0.01f);
                }
            }
            */
        }

        // output values
        /*
        for(unsigned int i=0; i < NUMBER_OF_SEQUENCERS; i++)
        {
            outputs[voltage_outputs[i]].setVoltage((voltage_sequencers[i].getValue() / 214.0) * 10.0);
        }
        */

        // process trigger outputs
        /*
        for(unsigned int i=0; i < NUMBER_OF_SEQUENCERS; i++)
        {
            trigger_output_pulse = gateOutputPulseGenerators[i].process(1.0 / args.sampleRate);
		    outputs[gate_outputs[i]].setVoltage((trigger_output_pulse ? 10.0f : 0.0f));
        }
        */

        if (clock_ignore_on_reset > 0) clock_ignore_on_reset--;
        if (tooltip_timer > 0) tooltip_timer--;
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

struct DigitalSequencerDisplay : TransparentWidget
{
    DigitalSequencer *module;
	Vec drag_position;
    float bar_width = (DRAW_AREA_WIDTH / MAX_SEQUENCER_STEPS) - BAR_HORIZONTAL_PADDING;

	void onDragStart(const event::DragStart &e) override
    {
		TransparentWidget::onDragStart(e);
	}

	void onDragEnd(const event::DragEnd &e) override
    {
		TransparentWidget::onDragEnd(e);
	}

    void step() override {
		TransparentWidget::step();
	}

    void onEnter(const event::Enter &e) override
    {
		TransparentWidget::onEnter(e);
	}

	void onLeave(const event::Leave &e) override
    {
		TransparentWidget::onLeave(e);
	}

    bool keypress(const event::HoverKey &e, int keycode)
    {
        if (e.key == keycode)
        {
            e.consume(this);
            if(e.action == GLFW_PRESS) return(true);
        }
        return(false);
    }

    bool keypressRight(const event::HoverKey &e)
    {
        return(keypress(e, GLFW_KEY_RIGHT));
    }

    bool keypressLeft(const event::HoverKey &e)
    {
        return(keypress(e, GLFW_KEY_LEFT));
    }

    bool keypressUp(const event::HoverKey &e)
    {
        return(keypress(e, GLFW_KEY_UP));
    }

    bool keypressDown(const event::HoverKey &e)
    {
        return(keypress(e, GLFW_KEY_DOWN));
    }

    void drawVerticalGuildes(NVGcontext *vg, float height)
    {
        for(unsigned int i=1; i < 8; i++)
        {
            nvgBeginPath(vg);
            int x = (i * 4 * bar_width) + (i * 4 * BAR_HORIZONTAL_PADDING);
            nvgRect(vg, x, 0, 1, height);
            nvgFillColor(vg, nvgRGBA(240, 240, 255, 40));
            nvgFill(vg);
        }
    }

    void drawBlueOverlay(NVGcontext *vg, float width, float height)
    {
        nvgBeginPath(vg);
        nvgRect(vg, 0, 0, width, height);
        nvgFillColor(vg, nvgRGBA(0, 100, 255, 28));
        nvgFill(vg);
    }

    void drawBar(NVGcontext *vg, float position, float height, float container_height, NVGcolor color)
    {
        nvgBeginPath(vg);
        nvgRect(vg, (position * bar_width) + (position * BAR_HORIZONTAL_PADDING), container_height - height, bar_width, height);
        nvgFillColor(vg, color);
        nvgFill(vg);
    }
};

struct DigitalSequencerPatternDisplay : DigitalSequencerDisplay
{
    bool draw_tooltip = false;
    float draw_tooltip_index = -1.0;
    float draw_tooltip_y = -1.0;
    float tooltip_value = 0.0;

	DigitalSequencerPatternDisplay()
	{
		// The bounding box needs to be a little deeper than the visual
		// controls to allow mouse drags to indicate '0' (off) column heights,
		// which is why 16 is being added to the draw height to define the
		// bounding box.
		box.size = Vec(DRAW_AREA_WIDTH, DRAW_AREA_HEIGHT + 16);
	}

	void draw(const DrawArgs &args) override
	{
		const auto vg = args.vg;
        int value;
        NVGcolor bar_color;

        // Save the drawing context to restore later
		nvgSave(vg);

		if(module)
		{
			//
			// Display the pattern
			//
			for(unsigned int i=0; i < MAX_SEQUENCER_STEPS; i++)
			{
				value = module->selected_voltage_sequencer->getValue(i);

                // Draw grey background bar
                if(i < module->selected_voltage_sequencer->getLength()) {
                    bar_color = nvgRGBA(60, 60, 64, 255);
                }
                else {
                    bar_color = nvgRGBA(45, 45, 45, 255);
                }

                drawBar(vg, i, BAR_HEIGHT, DRAW_AREA_HEIGHT, bar_color);

				if(i == module->selected_voltage_sequencer->getPlaybackPosition())
				{
					bar_color = nvgRGBA(255, 255, 255, 250);
				}
				else if(i < module->selected_voltage_sequencer->getLength())
				{
					bar_color = nvgRGBA(255, 255, 255, 150);
				}
                else
                {
                    bar_color = nvgRGBA(255, 255, 255, 10);
                }

                // Draw bars for the sequence values
				if(value > 0) drawBar(vg, i, value, DRAW_AREA_HEIGHT, bar_color);

                // Highlight the sequence playback column
				if(i == module->selected_voltage_sequencer->getPlaybackPosition())
				{
                    drawBar(vg, i, DRAW_AREA_HEIGHT, DRAW_AREA_HEIGHT, nvgRGBA(255, 255, 255, 20));
				}
			}

		}
        else // Draw a demo sequence so that the sequencer looks nice in the library selector
        {
            float demo_sequence[32] = {100.0,100.0,93.0,80.0,67.0,55.0,47.0,44.0,43.0,44.0,50.0,69.0,117.0,137.0,166,170,170,164,148,120,105,77,65,41,28,23,22,22,28,48,69,94};

            for(unsigned int i=0; i < MAX_SEQUENCER_STEPS; i++)
			{
                // Draw blue background bars
                drawBar(vg, i, BAR_HEIGHT, DRAW_AREA_HEIGHT, nvgRGBA(60, 60, 64, 255));

                // Draw bar for value at i
                drawBar(vg, i, demo_sequence[i], DRAW_AREA_HEIGHT, nvgRGBA(255, 255, 255, 150));

                // Highlight active step
				if(i == 5) drawBar(vg, i, DRAW_AREA_HEIGHT, DRAW_AREA_HEIGHT, nvgRGBA(255, 255, 255, 20));
            }
        }

        drawVerticalGuildes(vg, DRAW_AREA_HEIGHT);
        drawBlueOverlay(vg, DRAW_AREA_WIDTH, DRAW_AREA_HEIGHT);

        if(module)
        {
            if(module->tooltip_timer > 0) draw_tooltip = true;

            if(draw_tooltip)
            {
                drawTooltip(vg);
                draw_tooltip = false;
            }
        }

		nvgRestore(vg);
	}

    void drawTooltip(NVGcontext *vg)
    {
        nvgSave(vg);

        float x_offset = 3.0;
        float y = std::max(60.0f, draw_tooltip_y);
        float x = ((draw_tooltip_index * bar_width) + (draw_tooltip_index * BAR_HORIZONTAL_PADDING)) + bar_width + x_offset;

        if(draw_tooltip_index > 26) x = x - bar_width - TOOLTIP_WIDTH - (x_offset * 2) - BAR_HORIZONTAL_PADDING;
        y = DRAW_AREA_HEIGHT - y + 30;

        // Draw box for containing text
        nvgBeginPath(vg);
        nvgRoundedRect(vg, x, y, TOOLTIP_WIDTH, TOOLTIP_HEIGHT, 2);
        nvgFillColor(vg, nvgRGBA(20, 20, 20, 250));
        nvgFill(vg);

        // Set up font style
        nvgFontSize(vg, 13);
		nvgFillColor(vg, nvgRGBA(255, 255, 255, 0xff));
		nvgTextAlign(vg, NVG_ALIGN_CENTER);
		nvgTextLetterSpacing(vg, -1);

        // Display text
        std::string display_string = std::to_string(tooltip_value);
        display_string = display_string.substr(0,4);
        nvgText(vg, x + 16.5, y + 14, display_string.c_str(), NULL);

        nvgRestore(vg);
    }

    //
    // void editBar(Vec mouse_position)
    //
    // Called when the user clicks to edit one of the sequencer values.  Sets
    // the sequencer value that the user has selected, then sets some variables
    // for drawing the tooltip in this struct's draw(..) method.
    //
    void editBar(Vec mouse_position)
	{
        float bar_width = (DRAW_AREA_WIDTH / MAX_SEQUENCER_STEPS) - BAR_HORIZONTAL_PADDING;
		int clicked_bar_x_index = mouse_position.x / (bar_width + BAR_HORIZONTAL_PADDING);
		int clicked_y = DRAW_AREA_HEIGHT - mouse_position.y;

		clicked_bar_x_index = clamp(clicked_bar_x_index, 0, MAX_SEQUENCER_STEPS - 1);
        clicked_y = clamp(clicked_y, 0, DRAW_AREA_HEIGHT);

        module->selected_voltage_sequencer->setValue(clicked_bar_x_index, clicked_y);

        // Tooltip drawing is done in the draw method
        draw_tooltip = true;
        draw_tooltip_index = clicked_bar_x_index;
        draw_tooltip_y = clicked_y;
        tooltip_value = roundf((clicked_y / DRAW_AREA_HEIGHT) * 1000) / 100;
	}

    void onButton(const event::Button &e) override
    {
        e.consume(this);

		if(e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS)
		{
			drag_position = e.pos;
			this->editBar(e.pos);
		}
	}

	void onDragMove(const event::DragMove &e) override
    {
		TransparentWidget::onDragMove(e);
		drag_position = drag_position.plus(e.mouseDelta);
		editBar(drag_position);
	}

    void onHoverKey(const event::HoverKey &e) override
    {
        if(keypressRight(e))
        {
            module->selected_voltage_sequencer->shiftRight();
            if((e.mods & RACK_MOD_MASK) == GLFW_MOD_SHIFT) module->selected_gate_sequencer->shiftRight();
        }

        if(keypressLeft(e))
        {
            module->selected_voltage_sequencer->shiftLeft();
            if((e.mods & RACK_MOD_MASK) == GLFW_MOD_SHIFT) module->selected_gate_sequencer->shiftLeft();
        }

        if(keypressUp(e))
        {
            int bar_x_index = e.pos.x / (bar_width + BAR_HORIZONTAL_PADDING);
            float value = module->selected_voltage_sequencer->getValue(bar_x_index);

            // (.01 * (214 / 10)), where 214 is the bar height and 10 is the max voltage
            value = value + (.01 * (214.0 / 10.0));
            value = clamp(value, 0.0, DRAW_AREA_HEIGHT);

            module->selected_voltage_sequencer->setValue(bar_x_index, value);

            module->tooltip_timer = module->sample_rate * 2; // show tooltip for 2 seconds
            tooltip_value = roundf((value / DRAW_AREA_HEIGHT) * 1000) / 100;
            draw_tooltip_index = bar_x_index;
            draw_tooltip_y = value;
        }

        if(keypressDown(e))
        {
            int bar_x_index = e.pos.x / (bar_width + BAR_HORIZONTAL_PADDING);
            float value = module->selected_voltage_sequencer->getValue(bar_x_index);

            // (.01 * (214 / 10)), where 214 is the bar height and 10 is the max voltage
            value = value - (.01 * (214.0 / 10.0));
            value = clamp(value, 0.0, DRAW_AREA_HEIGHT);

            module->selected_voltage_sequencer->setValue(bar_x_index, value);

            module->tooltip_timer = module->sample_rate * 2; // show tooltip for 2 seconds
            tooltip_value = roundf((value / DRAW_AREA_HEIGHT) * 1000) / 100;
            draw_tooltip_index = bar_x_index;
            draw_tooltip_y = value;
        }
    }
};

struct DigitalSequencerGatesDisplay : DigitalSequencerDisplay
{
    bool mouse_lock = false;
    float bar_width = (DRAW_AREA_WIDTH / MAX_SEQUENCER_STEPS) - BAR_HORIZONTAL_PADDING;
    int old_drag_bar_x = -1;
    bool trigger_edit_value = false;

	DigitalSequencerGatesDisplay()
	{
		box.size = Vec(GATES_DRAW_AREA_WIDTH, GATES_DRAW_AREA_HEIGHT);
	}

	void draw(const DrawArgs &args) override
	{
		const auto vg = args.vg;
        int value;
        float value_height;
        NVGcolor bar_color;

		nvgSave(vg);

		if(module)
		{
			for(unsigned int i=0; i < MAX_SEQUENCER_STEPS; i++)
			{
				value = module->selected_gate_sequencer->getValue(i);

                // Draw grey background bar
                if(i < module->selected_gate_sequencer->getLength()) {
                    bar_color = nvgRGBA(60, 60, 64, 255);
                }
                else {
                    bar_color = nvgRGBA(45, 45, 45, 255);
                }
                drawBar(vg, i, GATE_BAR_HEIGHT, GATES_DRAW_AREA_HEIGHT, bar_color);

                if(i == module->selected_gate_sequencer->getPlaybackPosition())
				{
					bar_color = nvgRGBA(255, 255, 255, 250);
				}
				else if(i < module->selected_gate_sequencer->getLength())
				{
					bar_color = nvgRGBA(255, 255, 255, 150);
				}
                else // dim bars past playback position
                {
                    bar_color = nvgRGBA(255, 255, 255, 15);
                }
				value_height = (GATE_BAR_HEIGHT * value);
				if(value_height > 0) drawBar(vg, i, value_height, GATES_DRAW_AREA_HEIGHT, bar_color);

                // highlight active column
				if(i == module->selected_gate_sequencer->getPlaybackPosition())
				{
                    drawBar(vg, i, GATE_BAR_HEIGHT, GATES_DRAW_AREA_HEIGHT, nvgRGBA(255, 255, 255, 20));
				}
			}
		}
        else // draw demo data for the module explorer
        {
            int demo_sequence[32] = {1,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,1,0,1,1,0,0,0,0};

            for(unsigned int i=0; i < MAX_SEQUENCER_STEPS; i++)
			{
				value = demo_sequence[i];

                // Draw background grey bar
                drawBar(vg, i, GATE_BAR_HEIGHT, GATES_DRAW_AREA_HEIGHT, nvgRGBA(60, 60, 64, 255));

                // Draw value bar
                if (value > 0) drawBar(vg, i, (GATE_BAR_HEIGHT * value), GATES_DRAW_AREA_HEIGHT, nvgRGBA(255, 255, 255, 150));

                // highlight active column
				if(i == 5) drawBar(vg, i, GATE_BAR_HEIGHT, GATES_DRAW_AREA_HEIGHT, nvgRGBA(255, 255, 255, 20));
			}
        }

        drawVerticalGuildes(vg, GATES_DRAW_AREA_HEIGHT);
        drawBlueOverlay(vg, GATES_DRAW_AREA_WIDTH, GATES_DRAW_AREA_HEIGHT);

		nvgRestore(vg);
	}

	void onButton(const event::Button &e) override
    {
        e.consume(this);

		if(e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS)
		{
            if(this->mouse_lock == false)
            {
                this->mouse_lock = true;

                int index = getIndexFromX(e.pos.x);

                // Store the value that's being set for later in case the user
                // drags to set ("paints") additional triggers
                this->trigger_edit_value = ! module->selected_gate_sequencer->getValue(index);

                // Set the trigger value in the sequencer
                module->selected_gate_sequencer->setValue(index, this->trigger_edit_value);

                // Store the initial drag position
                drag_position = e.pos;
            }
		}
        else if(e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_RELEASE)
		{
            this->mouse_lock = false;
		}
	}

    void onDragMove(const event::DragMove &e) override
    {
		TransparentWidget::onDragMove(e);
		drag_position = drag_position.plus(e.mouseDelta);

        int drag_bar_x_index = getIndexFromX(drag_position.x);

        if(drag_bar_x_index != old_drag_bar_x)
        {
            // setTrigger(drag_bar_x_index, trigger_edit_value);
            module->selected_gate_sequencer->setValue(drag_bar_x_index, trigger_edit_value);
            old_drag_bar_x = drag_bar_x_index;
        }
	}

    /*
	void editBar(Vec mouse_position)
	{
		int clicked_bar_x_index = clamp(mouse_position.x / (barWidth() + BAR_HORIZONTAL_PADDING), 0, MAX_SEQUENCER_STEPS - 1);

        module->selected_gate_sequencer->setValue(clicked_bar_x_index, ! module->selected_gate_sequencer->getValue(clicked_bar_x_index));
	}
    */

    void onHoverKey(const event::HoverKey &e) override
    {
        if(keypressRight(e))
        {
            module->selected_gate_sequencer->shiftRight();
            if((e.mods & RACK_MOD_MASK) == GLFW_MOD_SHIFT) module->selected_voltage_sequencer->shiftRight();
        }

        if(keypressLeft(e))
        {
            module->selected_gate_sequencer->shiftLeft();
            if((e.mods & RACK_MOD_MASK) == GLFW_MOD_SHIFT) module->selected_voltage_sequencer->shiftLeft();
        }
    }

    int getIndexFromX(float x)
    {
        return(x / (barWidth() + BAR_HORIZONTAL_PADDING));
    }

    float barWidth()
    {
        return((DRAW_AREA_WIDTH / MAX_SEQUENCER_STEPS) - BAR_HORIZONTAL_PADDING);
    }
};

struct DigitalSequencerWidget : ModuleWidget
{
    DigitalSequencer* module;

	DigitalSequencerWidget(DigitalSequencer* module)
	{
        this->module = module;
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/digital_sequencer_front_panel.svg")));

		// Cosmetic rack screws
		addChild(createWidget<ScrewSilver>(Vec(15, 0)));
		addChild(createWidget<ScrewSilver>(Vec(15, 365)));
		addChild(createWidget<ScrewSilver>(mm2px(Vec(171.5, 0))));

        // Main voltage sequencer display
		DigitalSequencerPatternDisplay *pattern_display = new DigitalSequencerPatternDisplay();
		pattern_display->box.pos = mm2px(Vec(DRAW_AREA_POSITION_X, DRAW_AREA_POSITION_Y));
		pattern_display->module = module;
		addChild(pattern_display);

        DigitalSequencerGatesDisplay *gates_display = new DigitalSequencerGatesDisplay();
		gates_display->box.pos = mm2px(Vec(GATES_DRAW_AREA_POSITION_X, GATES_DRAW_AREA_POSITION_Y));
		gates_display->module = module;
		addChild(gates_display);

        float button_spacing = 9.6; // 9.1
        float button_group_x = 48.0;
        float button_group_y = 103.0;

        /*
        addParam(createParamCentered<Trimpot>(mm2px(Vec(button_group_x, button_group_y + 8.6)), module, DigitalSequencer::SEQUENCER_1_LENGTH_KNOB));
        addParam(createParamCentered<Trimpot>(mm2px(Vec(button_group_x + (button_spacing * 1.0), button_group_y + 8.6)), module, DigitalSequencer::SEQUENCER_2_LENGTH_KNOB));
        addParam(createParamCentered<Trimpot>(mm2px(Vec(button_group_x + (button_spacing * 2.0), button_group_y + 8.6)), module, DigitalSequencer::SEQUENCER_3_LENGTH_KNOB));
        addParam(createParamCentered<Trimpot>(mm2px(Vec(button_group_x + (button_spacing * 3.0), button_group_y + 8.6)), module, DigitalSequencer::SEQUENCER_4_LENGTH_KNOB));
        addParam(createParamCentered<Trimpot>(mm2px(Vec(button_group_x + (button_spacing * 4.0), button_group_y + 8.6)), module, DigitalSequencer::SEQUENCER_5_LENGTH_KNOB));
        addParam(createParamCentered<Trimpot>(mm2px(Vec(button_group_x + (button_spacing * 5.0), button_group_y + 8.6)), module, DigitalSequencer::SEQUENCER_6_LENGTH_KNOB));

        // 6 step inputs
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(button_group_x, button_group_y + 18.0)), module, DigitalSequencer::SEQUENCER_1_STEP_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(button_group_x + (button_spacing * 1.0), button_group_y + 18.0)), module, DigitalSequencer::SEQUENCER_2_STEP_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(button_group_x + (button_spacing * 2.0), button_group_y + 18.0)), module, DigitalSequencer::SEQUENCER_3_STEP_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(button_group_x + (button_spacing * 3.0), button_group_y + 18.0)), module, DigitalSequencer::SEQUENCER_4_STEP_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(button_group_x + (button_spacing * 4.0), button_group_y + 18.0)), module, DigitalSequencer::SEQUENCER_5_STEP_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(button_group_x + (button_spacing * 5.0), button_group_y + 18.0)), module, DigitalSequencer::SEQUENCER_6_STEP_INPUT));

        // 6 sequencer outputs
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(118, 108.224)), module, DigitalSequencer::SEQ1_CV_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(129, 108.224)), module, DigitalSequencer::SEQ2_CV_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(140, 108.224)), module, DigitalSequencer::SEQ3_CV_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(151, 108.224)), module, DigitalSequencer::SEQ4_CV_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(162, 108.224)), module, DigitalSequencer::SEQ5_CV_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(173, 108.224)), module, DigitalSequencer::SEQ6_CV_OUTPUT));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(118, 119.309)), module, DigitalSequencer::SEQ1_GATE_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(129, 119.309)), module, DigitalSequencer::SEQ2_GATE_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(140, 119.309)), module, DigitalSequencer::SEQ3_GATE_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(151, 119.309)), module, DigitalSequencer::SEQ4_GATE_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(162, 119.309)), module, DigitalSequencer::SEQ5_GATE_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(173, 119.309)), module, DigitalSequencer::SEQ6_GATE_OUTPUT));
        */

        // Step
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10, 114.893)), module, DigitalSequencer::STEP_INPUT));

        // Reset
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10 + 14.544, 114.893)), module, DigitalSequencer::RESET_INPUT));

	}

	void appendContextMenu(Menu *menu) override
	{
	}

    void step() override {
        ModuleWidget::step();
    }

};

Model* modelDigitalSequencer = createModel<DigitalSequencer, DigitalSequencerWidget>("digitalsequencer");