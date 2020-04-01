struct OverviewSequencer : Module
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
  OverviewSequencer()
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
