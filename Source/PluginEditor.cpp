/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
void GlobalControls::paint(juce::Graphics& g) {
    using namespace juce;
    auto b = getLocalBounds();
    g.setColour(Colours::blueviolet);
    g.fillAll();

    auto lB = b;
	
    b.reduce(3, 3);
    g.setColour(Colours::black);
    g.fillRoundedRectangle(b.toFloat(), 3);

    g.drawRect(lB);
}

Placeholder::Placeholder() {
    juce::Random r;
    customColor = juce::Colour(r.nextInt(255), r.nextInt(255), r.nextInt(255));
}

SimpleMBCompAudioProcessorEditor::SimpleMBCompAudioProcessorEditor (SimpleMBCompAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    addAndMakeVisible(controlBar);
    addAndMakeVisible(analyzerBar);
    addAndMakeVisible(globControlsBar);
    addAndMakeVisible(bandControlBar);

	setSize (700, 500);
}

SimpleMBCompAudioProcessorEditor::~SimpleMBCompAudioProcessorEditor()
{
}

//==============================================================================
void SimpleMBCompAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void SimpleMBCompAudioProcessorEditor::resized()
{
    auto b = getLocalBounds();
    controlBar.setBounds(b.removeFromTop(32));

    bandControlBar.setBounds(b.removeFromBottom(135));

    analyzerBar.setBounds(b.removeFromTop(255));
    globControlsBar.setBounds(b);
}
