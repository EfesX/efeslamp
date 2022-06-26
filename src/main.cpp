#include "network.h"
#include "clapsensor.h"
#include "lamp.h"


Network net;
ClapSensor clap;





void setup() {
  Serial.begin(9600);

  Lamp::begin();
  net.begin();
}


uint64_t lasttime = 0;
uint8_t lock = 0;

void loop() {
  Lamp::tick();
  net.tick();
}


/*
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include <Arduino.h>

#define NUM_LEDS 22


NeoPixelBus<NeoGrbFeature, NeoEsp8266Dma800KbpsMethod> strip(17, D9);

NeoPixelAnimator animations(1);

AnimEaseFunction moveEase =
//      NeoEase::Linear;
//      NeoEase::QuadraticInOut;
//      NeoEase::CubicInOut;
        NeoEase::QuarticInOut;
//      NeoEase::QuinticInOut;
//      NeoEase::SinusoidalInOut;
//      NeoEase::ExponentialInOut;
//      NeoEase::CircularInOut;

void FadeAll(uint8_t darkenBy){
    RgbColor color;
    for (uint16_t indexPixel = 0; indexPixel < strip.PixelCount(); indexPixel++){
        color = strip.GetPixelColor(indexPixel);
        color.Darken(darkenBy);
        strip.SetPixelColor(indexPixel, color);
    }
}


void UnFadeAll(uint8_t darkenBy){
    RgbColor color;
    for (uint16_t indexPixel = 0; indexPixel < strip.PixelCount(); indexPixel++){
        color = strip.GetPixelColor(indexPixel);
        color.Lighten(darkenBy);
        strip.SetPixelColor(indexPixel, color);
    }
}


#define MIN_BRIGHT 50
#define MAX_BRIGHT 150

void FadeAnimUpdate(const AnimationParam& param){
  float progress = moveEase(param.progress);
  if (param.state == AnimationState_Completed){
    FadeAll(20);
    Serial.println(millis());
    animations.RestartAnimation(param.index);
  }
}


struct MyAnimationState
{
    RgbColor StartingColor;
    RgbColor EndingColor;
};
MyAnimationState animationState;

// simple blend function
void BlendAnimUpdate(const AnimationParam& param)
{
    // this gets called for each animation on every time step
    // progress will start at 0.0 and end at 1.0
    // we use the blend function on the RgbColor to mix
    // color based on the progress given to us in the animation
    RgbColor updatedColor = RgbColor::LinearBlend(
        animationState.StartingColor,
        animationState.EndingColor,
        param.progress);

    // apply the color to the strip
    for (uint16_t pixel = 0; pixel < 22; pixel++)
    {
        strip.SetPixelColor(pixel, updatedColor);
    }
}
boolean fadeToColor = true;
void FadeInFadeOutRinseRepeat(float luminance)
{
    if (fadeToColor)
    {
        // Fade upto a random color
        // we use HslColor object as it allows us to easily pick a hue
        // with the same saturation and luminance so the colors picked
        // will have similiar overall brightness
        RgbColor target = HslColor(random(360) / 360.0f, 1.0f, luminance);
        uint16_t time = random(800, 2000);

        animationState.StartingColor = strip.GetPixelColor(0);
        animationState.EndingColor = target;

        animations.StartAnimation(0, time, BlendAnimUpdate);
    }
    else 
    {
        // fade to black
        uint16_t time = random(600, 700);

        animationState.StartingColor = strip.GetPixelColor(0);
        animationState.EndingColor = RgbColor(0);

        animations.StartAnimation(0, time, BlendAnimUpdate);
    }

    // toggle to the next effect state
    fadeToColor = !fadeToColor;
}

void setup(){
  Serial.begin(9600);

  strip.Begin();
  strip.Show();

  animationState.StartingColor = HtmlColor(0x00ff00);
  animationState.EndingColor = HtmlColor(0xff0000);


  RgbColor color = HtmlColor(0x00ff00);
    for (uint16_t indexPixel = 0; indexPixel < strip.PixelCount(); indexPixel++){
        strip.SetPixelColor(indexPixel, color);
    }



  animations.StartAnimation(0, 5000, BlendAnimUpdate);
}


void loop(){
  if(animations.IsAnimating()){
    animations.UpdateAnimations();
    strip.Show();
  }else{
    FadeInFadeOutRinseRepeat(0.2f); 
  }
    
}

*/