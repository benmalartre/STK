
#include "../CAPI/common.h"
#include "../CAPI/streams.h"
#include "../CAPI/generators.h"
#include <vector>
#include <cstdlib>
#include <iostream>

std::vector<STKNode*> nodes;
bool running = false;
float getRandomFrequency(){
    return ((float)rand() / (float)RAND_MAX) * 256.f + 128.f;
}

STKNode* getGenerator(STKStream* stream){
    STKNode* generator = STKAddGenerator(stream,
                                        STKGenerator::SINEWAVE_GENERATOR,
                                        getRandomFrequency());
    return generator;
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
        for(int i=0;i<nodes.size();++i){
            STKRemoveNode(stream, nodes[i]);
        }
        nodes.clear();
    }
    
    for(int i=0;i<numGenerators;++i){
        STKNode* node = getGenerator(stream);
        nodes.push_back(node);
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
    STKStream* stream = STKStreamSetup(DAC, 1);
    
    int numGenerators = getNumGenerators();
    if(numGenerators > 0){
        setupGenerators(stream, numGenerators);
    }
}
