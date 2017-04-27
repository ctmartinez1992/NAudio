//
//  ADSR.h
//  Tonic 
//
//  Created by Morgan Packard on 2/28/13.

//
// See LICENSE.txt for license and usage information.
//


#ifndef NAUDIO_ADSR_H
#define NAUDIO_ADSR_H

#include "Generator.h"
#include "ControlGenerator.h"
#include "FilterUtils.h"

namespace NAudio {
  
  namespace NAudio_DSP {
  
    class ADSR_ : public Generator_{
      
    protected:
    
    
      ControlGenerator mTrigger;
      ControlGenerator attack;
      ControlGenerator decay;
      ControlGenerator sustain;
      ControlGenerator release;
      ControlGenerator doesSustain;
      ControlGenerator isLegato;
      ControlGenerator isExponential;
      
      float attackTime;
      float decayTime;
      float sustainLevelVal;
      float releaseTime;
      bool       bIsLegato;
      bool       bDoesSustain;
      bool       bIsExponential;
      
      // state variables
      unsigned long segCounter;
      unsigned long segLength;
      float targetValue;
      float lastValue;
      float increment;
      float pole;
      
      enum ADSRState {
        NEUTRAL,
        ATTACK,
        SUSTAIN,
        DECAY,
        RELEASE
      };
      
      ADSRState state;
      void switchState(ADSRState newState);
      
      void computeSynthesisBlock( const SynthesisContext_ &context );
      
    public:
      
      ADSR_();
      ~ADSR_();
            
      void setTrigger(ControlGenerator trig){mTrigger = trig;}
      void setAttack(ControlGenerator gen){attack = gen;}
      void setDecay(ControlGenerator gen){decay = gen;}
      void setSustain(ControlGenerator gen){sustain = gen;}
      void setRelease(ControlGenerator gen){release = gen;}
      
      //! Exponential or linear ramp
      void setIsExponential(ControlGenerator gen){isExponential = gen;}
      
      //! Controls whether the envelope picks up from current position or zero when re-triggered while still releasing
      void setIsLegato(ControlGenerator gen){isLegato = gen;}
      
      //! Controls whether or not the envelope pauses on the SUSTAIN stage
      void setDoesSustain(ControlGenerator gen){doesSustain = gen;};
      
    };
    
    inline void ADSR_::computeSynthesisBlock(const SynthesisContext_ &context){
      
      ControlGeneratorOutput triggerOutput = mTrigger.tick(context);
      
      // frames to go in this block
      
      // Tick ALL inputs every time to keep everything in sync
      attackTime = attack.tick(context).value;
      decayTime = decay.tick(context).value;
      sustainLevelVal = sustain.tick(context).value;
      releaseTime = release.tick(context).value;
      bIsExponential = (isExponential.tick(context).value) ? true : false;
      bDoesSustain = (doesSustain.tick(context).value) ? true : false;
      bIsLegato = (isLegato.tick(context).value) ? true : false;
      
      float * fdata = &outputFrames_[0];
      
      if(triggerOutput.triggered){
        
        if(triggerOutput.value != 0){
          switchState(ATTACK);
        }else if(bDoesSustain){
          switchState(RELEASE);
        }
        
      }
      
      int samplesRemaining = kSynthesisBlockSize;
      
      while (samplesRemaining > 0)
      {
        switch (state) {
          
          // Both of these cases just fill the synthesis block the rest of the way up
          case NEUTRAL:
          case SUSTAIN:
          {
            std::fill(fdata, fdata + samplesRemaining, lastValue);
            samplesRemaining = 0;
          }
            break;
            
          case ATTACK:
          case DECAY:
          case RELEASE:
          {
            
            // how many samples remain in current segment
            unsigned long remainder = (segCounter > segLength) ? 0 : segLength - segCounter;
            if (remainder < (unsigned long)samplesRemaining){
              
              // fill up part of the ramp then switch segment

              if (bIsExponential){

                // one pole filter
                for (unsigned long i=0; i<remainder; i++){
                  onePoleLPFTick(targetValue, lastValue, pole);
                  *fdata++ = lastValue;
                }
                
              }
              else{
                for (unsigned long i=0; i<remainder; i++){
                  lastValue += increment;
                  *fdata++ = lastValue;
                }
              }
              
              segCounter += remainder;
              samplesRemaining -= remainder;
              
              // switch segment
              if (state == ATTACK){
                switchState(DECAY);
              }
              else if (state == DECAY){
                switchState(bDoesSustain ? SUSTAIN : RELEASE);
              }
              else{
                switchState(NEUTRAL);
              }
              
            }
            else{
              
              if (bIsExponential){
                
                // one pole filter
                for (int i=0; i<samplesRemaining; i++){
                  onePoleLPFTick(targetValue, lastValue, pole);
                  *fdata++ = lastValue;
                }
                
              }
              else{
                // fill the rest of the ramp up
                for (int i=0; i<samplesRemaining; i++){
                  lastValue += increment;
                  *fdata++ = lastValue;
                }
              }
              
              segCounter += samplesRemaining;
              samplesRemaining = 0;
            }
          }
            break;
            
          default:
            break;
        }
        
        
      }
      
    }
    
  }
  
  
  /*!
    Classic ADSR envlelope. Non-zero trigger values correspond to key down. Trigger values of zero correspond to keyup.
    Time values are rounded up to the nearest buffer size.
    Time values are in milliseconds. 
  */
  
  class ADSR : public TemplatedGenerator<NAudio_DSP::ADSR_>{
    
    public:
    
      ADSR(float attack = 0.001f, float decay = 0.03f, float sustain = 1.0f, float release = 0.05f);
    
      NAUDIO_MAKE_CTRL_GEN_SETTERS(ADSR, trigger, setTrigger);
      NAUDIO_MAKE_CTRL_GEN_SETTERS(ADSR, attack, setAttack);
      NAUDIO_MAKE_CTRL_GEN_SETTERS(ADSR, decay, setDecay);
      NAUDIO_MAKE_CTRL_GEN_SETTERS(ADSR, sustain, setSustain);
      NAUDIO_MAKE_CTRL_GEN_SETTERS(ADSR, release, setRelease);
      NAUDIO_MAKE_CTRL_GEN_SETTERS(ADSR, exponential, setIsExponential);
      NAUDIO_MAKE_CTRL_GEN_SETTERS(ADSR, doesSustain, setDoesSustain);
      NAUDIO_MAKE_CTRL_GEN_SETTERS(ADSR, legato, setIsLegato);

  };
  
}

#endif


