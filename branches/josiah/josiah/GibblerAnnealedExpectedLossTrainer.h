#pragma once

#include <map>
#include <utility>
#include <ext/hash_map>

#include "Gibbler.h"
#include "ScoreComponentCollection.h"
#include "Derivation.h"
#include "GibblerExpectedLossTraining.h"
#include "GibblerMaxDerivDecoder.h"
#include "Phrase.h"

using namespace Moses;

namespace Josiah {
  
  class GainFunction;
  
  class GibblerAnnealedExpectedLossCollector : public ExpectedLossCollector {
  public:
    GibblerAnnealedExpectedLossCollector(const GainFunction* f, Sampler& sampler) 
      :  ExpectedLossCollector(f) {
        sampler.AddCollector(&m_derivationCollector);
        m_computeScaleGradient = false;
      }
    
    float ComputeEntropy();
    float GetTemperature() { return m_temp;}
    void SetTemperature(float temp) {m_temp = temp;} 
    virtual float UpdateGradient(ScoreComponentCollection* gradient, float* exp_len, float * unreg_exp_gain, float *scaling_gradient);
    virtual float getRegularisationGradientFactor(size_t i);
    virtual float getRegularisation();
    void SetComputeScaleGradient(bool compute) {m_computeScaleGradient = compute;}
    virtual bool ComputeScaleGradient() {return m_computeScaleGradient;}

    
  private:
    float m_temp;
    DerivationCollector m_derivationCollector;
    
    //cache the distribution during gradient calculation
    std::map<const Derivation*,double> m_p;
    
    //whether to compute the scaling gradient
    bool m_computeScaleGradient;
    
  };
  
}