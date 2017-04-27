//
//  ADSR.cpp
//  Tonic
//
//  Created by Morgan Packard on 2/28/13.

//

#include "ADSR.h"

namespace NAudio { namespace NAudio_DSP{
  
  ADSR_::ADSR_() :
    state(NEUTRAL),
    lastValue(0),
    targetValue(0),
    increment(0),
    segCounter(0),
    segLength(0),
    pole(0)
  {
    mTrigger = ControlValue(0); // empty trigger by default
    isLegato = ControlValue(false);
    doesSustain = ControlValue(true);
    isExponential = ControlValue(false);
  }
  
  ADSR_::~ADSR_(){
    
  }
  
  void ADSR_::switchState(ADSRState newState){

    state = newState;
    segCounter = 0;

    switch(state){
        
      case NEUTRAL:{  
      
        lastValue = 0.f;
        increment = 0.f;
        
      }
      break;
        
      case ATTACK:{
        
        if (!bIsLegato){
          lastValue = 0.f;
        }
        
        segLength = attackTime * SampleRate();
        pole = t60ToOnePoleCoef(attackTime);
        
        if (segLength == 0){
          lastValue = 1.0f;
          switchState(DECAY);
        }
        else{
          targetValue = 1.0f;
          increment = (float)(1.0f - lastValue)/segLength;
        }

      }
      break;
        
      case DECAY:{
        
        segLength = decayTime * SampleRate();
        pole = t60ToOnePoleCoef(decayTime);
        
        targetValue = sustainLevelVal;
        
        if (segLength == 0){
          lastValue = sustainLevelVal;
          switchState(bDoesSustain ? SUSTAIN : RELEASE);
        }
        else{
          increment = (float)(sustainLevelVal - lastValue)/segLength;
        }
        
      }
      break;
        
      case SUSTAIN:
      {
        targetValue = sustainLevelVal;
        lastValue = sustainLevelVal;
        increment = 0.f;
      }
      break;
        
      case RELEASE:{
        
        segLength = releaseTime * SampleRate();
        pole = t60ToOnePoleCoef(releaseTime);
        
        targetValue = 0.f;
        
        if (segLength == 0){
          lastValue = 0.f;
          switchState(NEUTRAL);
        }
        else{
          increment = (float)(-lastValue)/segLength;
        }
      }
      break;
      
      default:
      break;
    }
  }
  
  
} // Namespace NAudio_DSP

  
  ADSR::ADSR(float attackArg, float decayArg, float sustainArg, float releaseArg){
    attack(attackArg);
    decay(decayArg);
    sustain(sustainArg);
    release(releaseArg);
  }

  
} // namespace NAudio
