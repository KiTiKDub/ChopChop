#include "chopOverlayComp.h"

ChopOverlayComp::ChopOverlayComp(ChopChopAudioProcessor &p) : audioProcessor(p)
{
}

void ChopOverlayComp::resized()
{

}

void ChopOverlayComp::paint(juce::Graphics &g)
{
    auto skew = audioProcessor.apvts.getParameter("skew");
    auto chops = audioProcessor.apvts.getParameter("chops");
    auto skewValue = skew->getNormalisableRange().convertFrom0to1(skew->getValue());
    auto chopsAmount = chops->getNormalisableRange().convertFrom0to1(chops->getValue());

    auto bounds = getLocalBounds();

    float length = bounds.getWidth();
    float height = bounds.getHeight();

    auto normalizeSkew = skewValue / 5;

    g.setColour(juce::Colour(64u, 194u, 230u));
    for (int i = 0; i < chopsAmount + 1; i++)
    {
        float currentDistance = (float)i / chopsAmount * length;
        if (currentDistance == 0)
            currentDistance = 1;

        auto top = std::pow(currentDistance, normalizeSkew) * std::log10(currentDistance);
        auto bottom = std::pow(length, normalizeSkew) * std::log10(length);

        auto plzWork = (top / bottom) * length;
        if (skewValue == 0)
            plzWork = currentDistance;

        g.fillRect(plzWork, (float)bounds.getTopLeft().getY(), 1.f, height);
    }
}
