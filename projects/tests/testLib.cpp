
#include "../CAPI/common.h"
#include "../CAPI/exports.h"
#include "../CAPI/streams.h"
#include "../CAPI/generators.h"
#include "../CAPI/arythmetics.h"
#include <vector>
#include <cstdlib>
#include <iostream>

bool running = false;
float getRandomFrequency(){
    return ((float)rand() / (float)RAND_MAX) * 256.f + 128.f;
}

STKNode* getGenerator(STKStream* stream){
    STKNode* generator1 = STKGeneratorCreate(STKGeneratorType::GENERATOR_SINEWAVE,
      getRandomFrequency());
    STKStreamAddNode(stream, generator1, false);
    STKNode* generator2 = STKGeneratorCreate(STKGeneratorType::GENERATOR_SINEWAVE,
      getRandomFrequency());
    STKStreamAddNode(stream, generator2, false);
    STKNode* effect1 = STKStreamAddEffect(stream, STKEffectType::EFFECT_NREV, generator1, true);
    STKNode* effect2 = STKStreamAddEffect(stream, STKEffectType::EFFECT_NREV, generator2, false);
    //STKNode* blender = STKStreamAddArythmetic(stream, STKArythmeticMode::ARYTHMETIC_BLEND, effect1, effect2, true);
    //STKSetArythmeticScalar((STKArythmetic*)blender, 0.5f);
    return effect1;
}

int getNumGenerators(){
    int numGenerators;
    std::cout << "How many generator do you want ?" << std::endl;
    std::cin >> numGenerators;
    return numGenerators;
}

void setupGenerators(STKStream* stream, int numGenerators){
    if(running){
        STKStreamStop(stream);
        STKStreamRemoveAllNodes(stream);
    }
    
    for(int i=0;i<numGenerators;++i){
        STKNode* node = getGenerator(stream);
    }
    STKStreamStart(stream);
    running = true;
    
    int counter = 0;
    while(counter < 2000){
        counter ++;
    }
    numGenerators = getNumGenerators();
    if(numGenerators>0){
        setupGenerators(stream, numGenerators);
    }
};

int main(){
    RtAudio* DAC = STKInit();
    STKStream* stream = STKStreamSetup(DAC, 2);
    
    int numGenerators = getNumGenerators();
    if(numGenerators > 0){
        setupGenerators(stream, numGenerators);
    }
}
