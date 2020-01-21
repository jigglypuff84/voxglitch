//
// Voxglitch "Equation Composer" module for VCV Rack
//
// This is a port of my Equation Composer hardware synth the myself, John
// Staskavich, and Hannes Pasqualini built under the company name Microbe Modular


#include "plugin.hpp"
#include "osdialog.h"
#include "dr_wav.h"
#include <vector>
#include "cmath"
#include <memory>

using namespace std;

class ByteBeatSafeMath
{
	public:

	uint32_t mod(uint32_t a, uint32_t b)
	{
		if(b == 0) return(0);
		return(a % b);
	}
};

struct EquationComposer : Module
{
	// counter
	uint32_t t;

	// Temporary variables for use in equations
	uint32_t p;
    uint32_t q;

    // parameters used in equations to modify the sound
	uint32_t p1;
    uint32_t p2;
    uint32_t p3;

 	// The final output of the equation
	uint8_t w;

	// Helper class for doing dangerous math
	ByteBeatSafeMath m;

	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		P1_INPUT,
		P2_INPUT,
		P3_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		EQUATION_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	//
	// Constructor
	//
	EquationComposer()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}

	json_t *dataToJson() override
	{
		json_t *rootJ = json_object();
		// json_object_set_new(rootJ, "path", json_string(this->path.c_str()));
		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override
	{
	}

	float calculate_inputs(int input_index, float scale)
	{
		float input_value = inputs[input_index].getVoltage() / 10.0;

		return(input_value * scale);
	}

	void process(const ProcessArgs &args) override
	{
		t = t + 1;

		p1 = (uint32_t) calculate_inputs(P1_INPUT, 255);
		p2 = (uint32_t) calculate_inputs(P2_INPUT, 255);
		p3 = (uint32_t) calculate_inputs(P3_INPUT, 255);

		// original w = ((t>>((t>>12)%(p3>>4)))+((p1|t)%p2))<<2;
		w = ((t>>(m.mod(t>>12,p3>>4)))+(m.mod(p1|t,p2)))<<2;

		float output = (((float) w) / 255.0f) * 10.0f;

		outputs[EQUATION_OUTPUT].setVoltage(output);
	}
};

struct EquationComposerWidget : ModuleWidget
{
	EquationComposerWidget(EquationComposer* module)
	{
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/equation_composer_front_panel.svg")));

		// Cosmetic rack screws
		addChild(createWidget<ScrewSilver>(Vec(15, 0)));
		addChild(createWidget<ScrewSilver>(Vec(15, 365)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10, 60)), module, EquationComposer::P1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10, 80)), module, EquationComposer::P2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10, 100)), module, EquationComposer::P3_INPUT));

		// WAV output
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(34.236, 114.893)), module, EquationComposer::EQUATION_OUTPUT));
	}
};



Model* modelEquationComposer = createModel<EquationComposer, EquationComposerWidget>("equation_composer");
