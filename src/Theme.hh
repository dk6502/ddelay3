#include "BinaryData.h"
#include "juce_graphics/juce_graphics.h"
#include "juce_gui_basics/juce_gui_basics.h"
class OtherLookAndFeel : public juce::LookAndFeel_V4 {
public:
  OtherLookAndFeel() {
    setColour(juce::Slider::thumbColourId, juce::Colours::red);
    defaultFont = juce::Typeface::createSystemTypefaceFor(BinaryData::NimbusSanLBol_otf, BinaryData::NimbusSanLBol_otfSize);
    defaultFont.setBold(true);
  }

  void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&) override {
      auto radius = (float) juce::jmin (width / 2, height / 2) - 4.0f;
      auto centreX = (float) x + (float) width * 0.5f;
      auto centreY = (float) y + (float) height * 0.5f;
      auto rx = centreX - radius;
      auto ry = centreY - radius;
      auto rw = radius * 2.0f;
      auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
      g.setColour (juce::Colour::fromString("FFf7f7f7"));
      g.fillEllipse (rx, ry, rw, rw);
      // outline
      g.setColour (juce::Colour::fromString("FF000000"));
      g.drawEllipse (rx, ry, rw, rw, 1.0f);
      juce::Path p;
      auto pointerLength = radius * 0.33f;
      auto pointerThickness = 2.0f;
      p.addRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
      p.applyTransform (juce::AffineTransform::rotation (angle).translated (centreX, centreY));
      g.setColour (juce::Colour::fromString("FF000000"));
      g.fillPath (p);
  }

  void drawLabel (juce::Graphics& g, juce::Label& l) override {
      g.setColour(juce::Colour::fromString("FFf7f7f7"));
      g.fillRect(0, 0, l.getWidth(), l.getHeight());
      g.setColour(juce::Colour::fromString("FF000000"));
      g.drawText(l.getText(), 0, 0, l.getWidth(), l.getHeight(), juce::Justification::centred, false);
      g.setFont(defaultFont);
      g.drawRect(0, 0, l.getWidth(), l.getHeight());
  }

private:
juce::Font defaultFont;

};
