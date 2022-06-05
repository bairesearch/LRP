 /*******************************************************************************
 *
 * License: GNU Affero General Public License version 3 (AGPLv3)
 *
 *******************************************************************************/

/*******************************************************************************
 *
 * File Name: LRPmain.hpp
 * Author: Richard Bruce Baxter - Copyright (c) 2005-2022 Baxter AI (baxterai.com)
 * Project: Language Reduction Preprocessor
 * Project Version: 3r1a 05-June-2022
 * Requirements: requires plain text file
 * Description: Main
 * /
 *******************************************************************************/


#ifndef HEADER_LRP_MAIN
#define HEADER_LRP_MAIN



#include "SHAREDglobalDefs.hpp"
#include "LRPglobalDefs.hpp"
#ifdef USE_SANI
#include "SANIglobalDefs.hpp"
#endif
#ifdef USE_GIA
#include "GIAglobalDefs.hpp"
#endif

#include "XMLrulesClass.hpp"

#include "LRPpreprocessor.hpp"
#include "LRPpreprocessorWordIdentification.hpp"
#ifdef LRP_PREPROCESSOR_WORD_MULTIWORD_REDUCTION
#include "LRPpreprocessorWordReduction.hpp" 
#endif
#include "LRPpreprocessorPOStaggerDatabase.hpp" 

#ifdef SANI_NEURAL_NETWORK
#include "ANNneuronClass.hpp"
#include "ANNdisplay.hpp"
#include "ANNdraw.hpp"
#include "ANNxmlConversion.hpp"
#include "SANIneuralNetworkOperations.hpp"
#endif



#ifdef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE

int main(const int argc, const char** argv);

#endif




#endif
