#pragma once

#include <NeoPixelBrightnessBus.h>
#include <NeoPixelAnimator.h>

#include <CircularBuffer.h>

#define NUM_LEDS 22

typedef struct{
    RgbColor StartingColor;
    RgbColor EndingColor;
}BlendAnimationState_t;

NeoPixelBrightnessBus<NeoGrbFeature, NeoEsp8266Dma800KbpsMethod> strip(NUM_LEDS, D9);
NeoPixelAnimator animations(3);
AnimEaseFunction moveEase = NeoEase::Linear;
BlendAnimationState_t blendclr[NUM_LEDS];
CircularBuffer<RgbColor, 16> cmds;

uint32_t blend_tau = 1000;
/**
 * @brief Плавное изменение цвета лампы
 */
void BlendAnimUpdate(const AnimationParam& param)
{
    RgbColor updatedColor;

    for (uint16_t pixel = 0; pixel < NUM_LEDS; pixel++)
    {
        updatedColor = RgbColor::LinearBlend(blendclr[pixel].StartingColor, blendclr[pixel].EndingColor, param.progress);
        strip.SetPixelColor(pixel, updatedColor);
    }
    if (param.state == AnimationState_Completed)
    {
        if(cmds.isEmpty() == false)
        {
            for (uint16_t pixel = 0; pixel < NUM_LEDS; pixel++)
            {
                blendclr[pixel].StartingColor = blendclr[pixel].EndingColor;
                blendclr[pixel].EndingColor = cmds.first();
            }
            cmds.shift();
        }
        else
        {
            for (uint16_t pixel = 0; pixel < NUM_LEDS; pixel++)
            {
                blendclr[pixel].StartingColor = blendclr[pixel].EndingColor;
            }
        }
        animations.StartAnimation(0, blend_tau, BlendAnimUpdate);
    }
}

static uint8_t bright_min = 50;
static uint8_t bright_max = 100;
static uint16_t bright_tau = 500;
static boolean dir = false;
/**
 * @brief Создает эффект дыхания путем уменьшения/увеличения яркости лампы
 */
void FadeAnimUpdate(const AnimationParam& param){
    float progress = moveEase(param.progress);
    uint8_t bstep = progress * (bright_max - bright_min);

    if(dir == false){
        strip.SetBrightness(bright_max - bstep);
    }else{
        strip.SetBrightness(bright_min + bstep);
    }

  if (param.state == AnimationState_Completed){
    if(dir == false) dir = true;
    else dir = false;
    animations.StartAnimation(1, bright_tau, FadeAnimUpdate);
  }
}
/**
 * @brief Моргает лампой
 */
static uint16_t NBlink;
void BlinkAnimUpdate(const AnimationParam& param)
{
    if (param.state == AnimationState_Completed)
    {
        static uint8_t count = 0;
        if(count != NBlink)
        {
            if((count % 2) == 0)
            {
                animations.StopAnimation(1);
                strip.SetBrightness(0);
                animations.RestartAnimation(param.index);
            }
            else
            {
                animations.StartAnimation(1, bright_tau, FadeAnimUpdate);
                animations.RestartAnimation(param.index);
            }
            count++;
        }
        else
        {
            animations.StartAnimation(1, bright_tau, FadeAnimUpdate);
            count = 0;
        }
    }
}


class Lamp {
    public:
        static void begin(){
            strip.Begin();
            strip.Show();
            strip.SetBrightness(bright_max);
            animations.StartAnimation(0, blend_tau,  BlendAnimUpdate);
            animations.StartAnimation(1, bright_tau, FadeAnimUpdate);
        }
        static void setColorBlend(Neo3ByteElements::ColorObject color, uint16_t duration = 250){
            if(cmds.available() > 0){
                cmds.push(color);
                blend_tau = duration;
            }
        }
        static void turnOn(void)
        {
            animations.StartAnimation(1, bright_tau, FadeAnimUpdate);
        }
        static void turnOff(void)
        {
            animations.StopAnimation(1);
            strip.SetBrightness(0);
        }
        static void setBrighgt(uint8_t min, uint8_t max, uint16_t tau)
        {
            animations.StopAnimation(1);
            bright_min = min;
            bright_max = max;
            bright_tau = tau;
            animations.StartAnimation(1, bright_tau, FadeAnimUpdate);
        }
        static void blink(uint8_t N, uint16_t tau)
        {
            animations.StopAnimation(1);
            animations.StopAnimation(2);
            NBlink = ((uint16_t) N) << 1;
            animations.StartAnimation(2, tau, BlinkAnimUpdate);

        }
        static uint8_t getBrightMin(void){
            return bright_min;
        }
        static uint8_t getBrightMax(void){
            return bright_min;
        }
        static uint8_t getBrightTau(void){
            return bright_tau;
        }
        static uint8_t getBlendTau(void){
            return blend_tau;
        }
        static RgbColor getCurrentColor(void){
            return blendclr[0].EndingColor;
        }

        static void tick(){
            if(animations.IsAnimating()){
                animations.UpdateAnimations();
                strip.Show();
            }
        }

};