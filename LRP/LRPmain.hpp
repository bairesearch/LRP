/*******************************************************************************
 *
 * No License
 * 
 * This work is under exclusive copyright (c) Baxter AI (baxterai.com). 
 * Nobody else can use, copy, distribute, or modify this work without being 
 * at risk of take-downs, shake-downs, or litigation. 
 * 
 * By publishing this source code in a public repository on GitHub, Terms of 
 * Service have been accepted by which Baxter AI have allowed others to view 
 * and fork their repository.
 * 
 * If you find software that doesn't have a license, that generally means you 
 * have no permission from the creators of the software to use, modify, or 
 * share the software. Although a code host such as GitHub may allow you to 
 * view and fork the code, this does not imply that you are permitted to use, 
 * modify, or share the software for any purpose.
 *
 * This notice has been derived from https://choosealicense.com/no-permission 
 * (https://web.archive.org/web/20180312144938/https://choosealicense.com/no-permission)
 *
 *******************************************************************************/

/*******************************************************************************
 *
 * File Name: LRPmain.hpp
 * Author: Richard Bruce Baxter - Copyright (c) 2005-2020 Baxter AI (baxterai.com)
 * Project: Language Reduction Preprocessor
 * Project Version: 3o3b 16-November-2020
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
