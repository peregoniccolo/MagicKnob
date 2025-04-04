#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_analytics/juce_analytics.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_events/juce_events.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_plugin_client/juce_audio_plugin_client.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_box2d/juce_box2d.h>
#include <juce_core/juce_core.h>
#include <juce_cryptography/juce_cryptography.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_opengl/juce_opengl.h>
#include <juce_osc/juce_osc.h>
#include <juce_product_unlocking/juce_product_unlocking.h>
#include <juce_video/juce_video.h>

#include <iostream>
#include <RTNeural/RTNeural.h>

using ModelType = RTNeural::ModelT<float, 2, 1, RTNeural::LSTMLayerT<float, 2, 16>, RTNeural::DenseT<float, 16, 1>>;

/**
	PluginProcessor
	Manages the audio manipulation and houses the values for the distortion and the LPF effects
*/
class MagicKnobProcessor : public juce::AudioProcessor
#if JucePlugin_Enable_ARA
	,
						   public juce::AudioProcessorARAExtension
#endif
{
public:
	MagicKnobProcessor();
	~MagicKnobProcessor() override;

	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
#endif

	void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

	juce::AudioProcessorEditor *createEditor() override;
	bool hasEditor() const override;

	const juce::String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const juce::String getProgramName(int index) override;
	void changeProgramName(int index, const juce::String &newName) override;

	void getStateInformation(juce::MemoryBlock &destData) override;
	void setStateInformation(const void *data, int sizeInBytes) override;

	void addMidi(juce::MidiMessage msg, int sampleOffset);

	void searchJsonModelsInDir(std::string modelFolder);
	void loadModel(std::ifstream &jsonStream, ModelType &model);

	void setDistKnobValue(float val);
	void setLPFKnobValue(float val);
	float getDistKnobValue();
	float getLPFKnobValue();

	bool getCurrPowerState();
	void setCurrPowerState(bool newState);
	void togglePowerState();

	void loadNextModel(std::string knobId);
	std::string getCurrentModel(std::string knobId);

private:
	bool powerState;

	float distKnobValue, lpfKnobValue;

	int currModelDist, currModelLPF;

	std::string modelFolder;																				 // model folder path
	std::string distModelJson, distInvModelJson, distVShapeModelJson, distRandomModelJson, distSinModelJson; // dist models
	std::string lpfModelJson, lpfRandomModelJson;															 // lpf models
	std::vector<std::string> distModelFiles, lpfModelFiles;

	ModelType modelsDist[2], modelsLPF[2];

	void loadModelFromJson(ModelType *models, std::string path);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MagicKnobProcessor)
};