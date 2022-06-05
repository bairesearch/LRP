 /*******************************************************************************
 *
 * License: GNU Affero General Public License version 3 (AGPLv3)
 *
 *******************************************************************************/

/*******************************************************************************
 *
 * File Name: LRPpreprocessorPOStagger.cpp
 * Author: Richard Bruce Baxter - Copyright (c) 2005-2022 Baxter AI (baxterai.com)
 * Project: Language Reduction Preprocessor
 * Project Version: 3r1a 05-June-2022
 * Requirements: requires plain text file
 * Description: Preprocessor POS tagger
 * /
 *******************************************************************************/


#include "LRPpreprocessorPOStagger.hpp"
#ifdef USE_GIA
#include "GIAsynRelTranslatorDefs.hpp"
#endif


#ifdef LRP_PREPROCESSOR_POS_TAGGER

/*plan: 
execute createPreprocessSentencesSimple on wiki text

DONE read GIA lrp word lists to write to index list (LRP_PREPROCESSOR_INITIALISE_WORD_INDEX_LIST_FROM_LRP_FILES)

[NOTUSED] DONE Convert wiki words to lower case and add to index (LRP_PREPROCESSOR_POS_TAGGER_INITIALISE_WORD_INDEX_LIST_FROM_WIKI_DUMP_TEXT)
	FUTURE: Cross correlate index with that derived from existing GIA lrp word lists

DONE then execute GIA lrp multiword detection on wikitext, 
FUTURE: then save to file, or keep word lists in memory
Planned method:
	Identify the pos/possible pos tags of each word 5 to the left and right of the centre word
		7(word types)^3(possible/ambiguous/alternate word type cases) + (5*2)(number of context words)
	Could even train these experiences (input output cases) using a neural network 
	With 100billiom inputs and 7 outputs
OLD planned method: then for every triplet, find each word in index (ie get ID); A B C
	then add a new instance in the GIA word vec POS tagger database, indexed by a combination of A and C
	Only do this if A and C are pos unambiguous?
*/




#ifdef GIA_POS_REL_TRANSLATOR_HYBRID_EXECUTE_PRELIM_POS_TAGGER
bool LRPpreprocessorPOStaggerClass::executePrelimFeatureProcessingOnSentences(const string outputLRPTextPlainTXTFileName, const string inputTextNLPfeatureXMLfileName, LRPtranslatorVariablesClass* translatorVariables)
{
	bool result = true;
	
	LRPpreprocessorSentence* currentLRPpreprocessorSentenceInList = translatorVariables->firstLRPpreprocessorSentenceInList;
	while(currentLRPpreprocessorSentenceInList->next != NULL)
	{
		vector<LRPpreprocessorPlainTextWord*>* sentenceContents = LRPpreprocessorSentenceClassObject.getSentenceContents(currentLRPpreprocessorSentenceInList);
			
		for(int wCentre=0; wCentre<sentenceContents->size(); wCentre++)
		{
			GIAfeature* currentFeatureInList = new GIAfeature();	//prelim feature

			//centre word calculations
			LRPpreprocessorPlainTextWord* centreWord = (*sentenceContents)[wCentre];
			#ifdef LRP_PREPROCESSOR_POS_TAGGER_POS_DEBUG
			cout << "centreWord = " << centreWord->tagName << endl;
			#endif

			bool identifiedEveryWordInDatabasePOSpermutation = true;
			bool identifiedEveryWordInDatabasePOSpermutationIsUnambiguous = true;

			
			uint64_t centreWordPOSambiguityInfo = LRP_PREPROCESSOR_POS_TAGGER_POS_AMBIGUITY_INFO_UNKNOWN;	//default value
			bool centreWordPOSisAmbiguous = false;
			uchar centreWordUnambiguousPOSindex = INT_DEFAULT_VALUE;
			if(!determinePOSambiguityInfo(centreWord, &centreWordPOSambiguityInfo, &centreWordPOSisAmbiguous, &centreWordUnambiguousPOSindex, &identifiedEveryWordInDatabasePOSpermutation))
			{
				result = false;
			}
								
			if(centreWordPOSisAmbiguous)
			{		
				/*
				cout << "\ncentreWordPOSisAmbiguous" << endl;
				cout << "centreWord = " << centreWord->tagName << endl;
				cout << "DBconvertByteToHex(centreWordPOSambiguityInfo) = " << LRPpreprocessorPOStaggerDatabase.DBconvertByteToBinaryString(centreWordPOSambiguityInfo) << endl;
				*/
				
				vector<uint64_t> POSambiguityInfoPermutation;
				if(!generatePOSambiguityInfoPermutation(sentenceContents, wCentre, &identifiedEveryWordInDatabasePOSpermutation, &identifiedEveryWordInDatabasePOSpermutationIsUnambiguous, &POSambiguityInfoPermutation))
				{
					result = false;
				}
				
				/*
				//debug:
				for(int i=0; i<POSambiguityInfoPermutation.size(); i++)
				{
					cout << "i = " << i << endl;
					cout << "\tPOSambiguityInfoPermutation.at(i) = " << POSambiguityInfoPermutation.at(i) << endl;
					uchar POSpermutationIndexByte = LRPpreprocessorPOStaggerDatabase.convertPOSambiguityInfoToIndex(POSambiguityInfoPermutation.at(i));
					cout << "\tint(POSpermutationIndexByte) = " << int(POSpermutationIndexByte) << endl;
				}
				*/

				#ifdef LRP_PREPROCESSOR_POS_TAGGER_ONLY_ADD_DATABASE_ENTRY_IF_POS_AMBIGUITY_INFO_FOUND_FOR_EVERY_CONTEXT_WORD
				if(identifiedEveryWordInDatabasePOSpermutation)
				{
				#endif

					bool foundMatchingCentreWordPOSambiguityInfo = false;
					uchar centreWordPOSindexPrediction = INT_DEFAULT_VALUE;
					bool centreWordPOSisAmbiguous = false;
					#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK
					double experienceBackPropagationPassError = REALLY_LARGE_DOUBLE;
					#else 
					int maximumNumberOfInstances = 0;
					#endif
					
					//cout << "LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FEED_ALL_PERMUTATIONS_INDIVIDUALLY:" << endl;
	
					#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FEED_ALL_PERMUTATIONS_INDIVIDUALLY
					vector<vector<uint64_t>*> POSambiguityInfoUnambiguousPermutationArray;
					vector<uint64_t>* POSambiguityInfoUnambiguousPermutationNew = new vector<uint64_t>(POSambiguityInfoPermutation.size());
					POSambiguityInfoUnambiguousPermutationArray.push_back(POSambiguityInfoUnambiguousPermutationNew);
					generatePOSambiguityInfoUnambiguousPermutationArray(&POSambiguityInfoUnambiguousPermutationArray, &POSambiguityInfoPermutation, POSambiguityInfoUnambiguousPermutationNew, 0);
					for(int i=0; i<POSambiguityInfoUnambiguousPermutationArray.size(); i++)
					{
						vector<uint64_t>* POSambiguityInfoPermutationTemp = POSambiguityInfoUnambiguousPermutationArray[i];
					#else
						vector<uint64_t>* POSambiguityInfoPermutationTemp = &POSambiguityInfoPermutation;
					#endif

						bool foundMatchingCentreWordPOSambiguityInfoTemp = false;
						uchar centreWordPOSindexPredictionTemp = INT_DEFAULT_VALUE;
						bool centreWordPOSisAmbiguousTemp = false;
						//#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK
						double experienceBackPropagationPassErrorTemp = 0.0; 
						//#else
						int maximumNumberOfInstancesTemp = 0;
						//#endif
						
						//cout << "permutation = " << LRPpreprocessorPOStaggerDatabase.convertPOSambiguityInfoPermutationToPOSambiguityInfoPermutationIndexString(POSambiguityInfoPermutationTemp) << endl;

						predictPOStaggerDatabaseEntry(centreWordPOSambiguityInfo, POSambiguityInfoPermutationTemp, centreWordUnambiguousPOSindex, centreWord, &foundMatchingCentreWordPOSambiguityInfoTemp, &centreWordPOSindexPredictionTemp, &centreWordPOSisAmbiguousTemp, &experienceBackPropagationPassErrorTemp, &maximumNumberOfInstancesTemp);
						if(foundMatchingCentreWordPOSambiguityInfoTemp)
						{
							#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK
							if(experienceBackPropagationPassErrorTemp < experienceBackPropagationPassError)
							#else
							if(maximumNumberOfInstancesTemp > maximumNumberOfInstances)
							#endif
							{
								foundMatchingCentreWordPOSambiguityInfo = foundMatchingCentreWordPOSambiguityInfoTemp;
								centreWordPOSindexPrediction = centreWordPOSindexPredictionTemp;
								centreWordPOSisAmbiguous = centreWordPOSisAmbiguousTemp;
								#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK
								experienceBackPropagationPassError = experienceBackPropagationPassErrorTemp;
								#else
								maximumNumberOfInstances = maximumNumberOfInstancesTemp;
								#endif
							}
							//cout << "predictPOStaggerDatabaseEntry" << endl;
						}
						else
						{
							//cout << "!predictPOStaggerDatabaseEntry" << endl;
						}

					#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FEED_ALL_PERMUTATIONS_INDIVIDUALLY
					}
					#endif


					#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL
					currentFeatureInList->grammaticalWordType = GRAMMATICAL_WORD_TYPE_UNDEFINED;
					#else
					if(foundMatchingCentreWordPOSambiguityInfo)
					{
						if(!centreWordPOSisAmbiguous)
						{
							#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_PREDICTION_VERIFICATION
							uchar centreWordPOSvalueFirstAmbiguousPrediction = 0;
							//cout << "centreWordPOSindexPrediction = " << int(centreWordPOSindexPrediction) << endl;
							if(LRPpreprocessorPOStaggerDatabase.verifyPOStaggerDatabasePredictionAgainstPOSambiguityInfo(centreWordPOSindexPrediction, centreWordPOSambiguityInfo, &centreWordPOSvalueFirstAmbiguousPrediction))
							{
							#endif
								currentFeatureInList->GIAposType = centreWordPOSindexPrediction;
							#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_PREDICTION_VERIFICATION
							}
							else
							{
								currentFeatureInList->GIAposType = centreWordPOSvalueFirstAmbiguousPrediction;

								cerr << "verifyPOStaggerDatabasePredictionAgainstPOSambiguityInfo failed" << endl;
								cerr << "centreWord = " << centreWord->tagName << endl;
								cerr << "wCentre = " << wCentre << endl; 
								cerr << "centreWordPOSindexPrediction =" << centreWordPOSindexPrediction << endl;
								cerr << "centreWord->centreWordPOSambiguityInfo = " << LRPpreprocessorPOStaggerDatabase.DBconvertByteToBinaryString(centreWordPOSambiguityInfo) << endl;
							}
							#endif

						}
						else
						{							
							currentFeatureInList->GIAposType = LRP_PREPROCESSOR_POS_TYPE_UNDEFINED;
							cerr << "LRP_PREPROCESSOR_POS_TAGGER GIAposRelTranslatorHybridClass::executePrelimFeatureProcessingOnSentences{} error: word POS cannot be determined (centreWordPOSisAmbiguous): " << centreWord->tagName << endl;
							exit(EXIT_ERROR);
						}
					}
					else
					{
						currentFeatureInList->GIAposType = LRP_PREPROCESSOR_POS_TYPE_UNDEFINED;
						cout << "LRP_PREPROCESSOR_POS_TAGGER GIAposRelTranslatorHybridClass::executePrelimFeatureProcessingOnSentences{} warning: word POS cannot be determined (!foundMatchingCentreWordPOSambiguityInfo): " << centreWord->tagName << endl;
					}
					#endif


				#ifdef LRP_PREPROCESSOR_POS_TAGGER_ONLY_ADD_DATABASE_ENTRY_IF_POS_AMBIGUITY_INFO_FOUND_FOR_EVERY_CONTEXT_WORD
				}
				#endif
			}
			else
			{
				currentFeatureInList->GIAposType = centreWordUnambiguousPOSindex;
				//cout << "currentFeatureInList->grammaticalWordType = " << currentFeatureInList->grammaticalWordType << endl;
			}


			(*sentenceContents)[wCentre]->featureReferencePrelim = currentFeatureInList;
		}
		
		currentLRPpreprocessorSentenceInList = currentLRPpreprocessorSentenceInList->next;
	}	
	
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_PERFORM_BATCH_PREDICTIONS
	//generate experience list
	ANNexperience* firstExperienceInList = NULL;
	ANNexperience* currentExperienceInList = NULL;
	bool foundFirstExperience = false;
	currentLRPpreprocessorSentenceInList = translatorVariables->firstLRPpreprocessorSentenceInList;
	while(currentLRPpreprocessorSentenceInList->next != NULL)
	{	
		vector<LRPpreprocessorPlainTextWord*>* sentenceContents = LRPpreprocessorSentenceClassObject.getSentenceContents(currentLRPpreprocessorSentenceInList);
	
		for(int wCentre=0; wCentre<sentenceContents->size(); wCentre++)
		{
			LRPpreprocessorPlainTextWord* centreWord = (*sentenceContents)[wCentre];
			
			#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FEED_ALL_PERMUTATIONS_INDIVIDUALLY
			for(int i=0; i<centreWord->POStaggerExperiencePermutations.size(); i++)
			{
				ANNexperience* centreWordPOStaggerExperienceTemp = centreWord->POStaggerExperiencePermutations[i];
			#else
			if(centreWord->POStaggerExperience != NULL)
			{
				ANNexperience* centreWordPOStaggerExperienceTemp = centreWord->POStaggerExperience;
			#endif
				if(!foundFirstExperience)
				{
					foundFirstExperience = true;
					firstExperienceInList = centreWordPOStaggerExperienceTemp;
					currentExperienceInList = centreWordPOStaggerExperienceTemp;
				}
				else
				{
					currentExperienceInList->next = centreWordPOStaggerExperienceTemp;
					currentExperienceInList = currentExperienceInList->next;
				}
			}
		}
		currentLRPpreprocessorSentenceInList = currentLRPpreprocessorSentenceInList->next;
	}	
	if(foundFirstExperience)
	{
		//add blank experience at end of list (required)
		currentExperienceInList->next = new ANNexperience();
	}
		
	//generate predictions	
	LRPpreprocessorPOStaggerDatabase.externalANNpredict(firstExperienceInList);
	
	//apply predictions	
	currentLRPpreprocessorSentenceInList = translatorVariables->firstLRPpreprocessorSentenceInList;
	while(currentLRPpreprocessorSentenceInList->next != NULL)
	{		
		vector<LRPpreprocessorPlainTextWord*>* sentenceContents = LRPpreprocessorSentenceClassObject.getSentenceContents(currentLRPpreprocessorSentenceInList);
	
		for(int wCentre=0; wCentre<sentenceContents->size(); wCentre++)
		{
			LRPpreprocessorPlainTextWord* centreWord = (*sentenceContents)[wCentre];
			GIAfeature* currentFeatureInList = (*sentenceContents)[wCentre]->featureReferencePrelim;
			#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FEED_ALL_PERMUTATIONS_INDIVIDUALLY
			if(centreWord->POStaggerExperiencePermutations.size() > 0)
			{
				double experienceBackPropagationPassError = REALLY_LARGE_DOUBLE;
				ANNexperience* centreWordPOStaggerExperience = NULL;
				for(int i=0; i<centreWord->POStaggerExperiencePermutations.size(); i++)
				{
					ANNexperience* centreWordPOStaggerExperienceTemp2 = centreWord->POStaggerExperiencePermutations[i];
					if(centreWordPOStaggerExperienceTemp2->predictionError < experienceBackPropagationPassError)
					{
						experienceBackPropagationPassError = centreWordPOStaggerExperienceTemp2->predictionError;
						centreWordPOStaggerExperience = centreWordPOStaggerExperienceTemp2;
					}
				}
			#else
			if(centreWord->POStaggerExperience != NULL)
			{
				ANNexperience* centreWordPOStaggerExperience = centreWord->POStaggerExperience;
			#endif
				uchar centreWordPOSindexPrediction = centreWordPOStaggerExperience->classTargetValue;
				uchar centreWordPOSvalueFirstAmbiguousPrediction = 0;
				#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_PREDICTION_VERIFICATION
				if(LRPpreprocessorPOStaggerDatabase.verifyPOStaggerDatabasePredictionAgainstPOSambiguityInfo(centreWordPOSindexPrediction, centreWord->centreWordPOSambiguityInfo, &centreWordPOSvalueFirstAmbiguousPrediction))
				{
				#endif
					currentFeatureInList->GIAposType = centreWordPOSindexPrediction;

					#ifdef GIA_DEBUG_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_PRINT_PREDICTIONS
					cout << "**********************************************************" << endl;
					cout << "GIA POS Tagger neural network prediction:" << endl;
					cout << "centreWord = " << centreWord->tagName << endl;
					cout << "centreWordPOSindexPrediction = " << centreWordPOSindexPrediction << endl;
					cout << "currentFeatureInList->grammaticalWordType = " << currentFeatureInList->grammaticalWordType << endl;
					cout << "grammaticalWordTypeNameArray[currentFeatureInList->grammaticalWordType] = " << grammaticalWordTypeNameArray[currentFeatureInList->grammaticalWordType] << endl;
					cout << "**********************************************************" << endl;
					#endif
				#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_PREDICTION_VERIFICATION
				}
				else
				{
					currentFeatureInList->GIAposType = centreWordPOSvalueFirstAmbiguousPrediction;
					
					cerr << "verifyPOStaggerDatabasePredictionAgainstPOSambiguityInfo failed" << endl;
					cerr << "centreWord = " << centreWord->tagName << endl;
					cerr << "wCentre = " << wCentre << endl; 
					cerr << "centreWordPOSindexPrediction =" << centreWordPOSindexPrediction << endl;
					cerr << "centreWord->centreWordPOSambiguityInfo = " << LRPpreprocessorPOStaggerDatabase.DBconvertByteToBinaryString(centreWord->centreWordPOSambiguityInfo) << endl;
				}
				#endif
			}
		}
		currentLRPpreprocessorSentenceInList = currentLRPpreprocessorSentenceInList->next;
	}
	#endif
}

bool LRPpreprocessorPOStaggerClass::predictPOStaggerDatabaseEntry(uint64_t centreWordPOSambiguityInfo, const vector<uint64_t>* POSambiguityInfoPermutation, uchar centreWordUnambiguousPOSindex, LRPpreprocessorPlainTextWord* centreWord, bool* foundMatchingCentreWordPOSambiguityInfo, uchar* centreWordPOSindexPrediction, bool* centreWordPOSisAmbiguous, double* experienceBackPropagationPassError, int* maximumNumberOfInstances)
{
	bool result = true;
	
	//NB centreWordUnambiguousPOSindex is not set or used (as it is currently ambiguous; it has not yet been predicted)

	*foundMatchingCentreWordPOSambiguityInfo = false;

	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK
	ANNexperience* currentExperience = new ANNexperience();
	generateANNexperienceFromPOSambiguityInfoPermutation(POSambiguityInfoPermutation, centreWordUnambiguousPOSindex, currentExperience);
	*experienceBackPropagationPassError = 0.0;
	int idealClassTarget = INT_DEFAULT_VALUE;
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_INTERNAL
	if(LRPpreprocessorPOStaggerDatabase.calculateIdealClassTargetOfInputExperience(currentExperience, &idealClassTarget, &experienceBackPropagationPassError))
	{
		*foundMatchingCentreWordPOSambiguityInfo = true;
		*centreWordPOSindexPrediction = idealClassTarget;
		*centreWordPOSisAmbiguous = false;
		cout << "centreWord = " << centreWord->tagName << ", idealClassTarget = " << idealClassTarget << ", experienceBackPropagationPassError = " << experienceBackPropagationPassError << endl;
	}
	#elif defined LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_PERFORM_BATCH_PREDICTIONS
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FEED_ALL_PERMUTATIONS_INDIVIDUALLY
	centreWord->POStaggerExperiencePermutations.push_back(currentExperience);
	#else
	centreWord->POStaggerExperience = currentExperience;
	#endif
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_PREDICTION_VERIFICATION
	centreWord->centreWordPOSambiguityInfo = centreWordPOSambiguityInfo;
	#endif
	*foundMatchingCentreWordPOSambiguityInfo = false;	//currentFeatureInList->grammaticalWordType will be temporarily set to GRAMMATICAL_WORD_TYPE_UNDEFINED (until its predicted value has been generated in the batch external ANN operation)
	#else
	currentExperience->next = new ANNexperience();	//required for externalANNpredict:externalANNgenerateBatchDataExperiences
	if(LRPpreprocessorPOStaggerDatabase.externalANNpredict(currentExperience))
	{
		*foundMatchingCentreWordPOSambiguityInfo = true;
		*centreWordPOSindexPrediction = currentExperience->classTargetValue;
		*centreWordPOSisAmbiguous = false;
	}
	#endif
	#endif
	#elif defined LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM
	uchar entryIndexWithMaximumNumberOfInstances = LRP_PREPROCESSOR_POS_TAGGER_POS_AMBIGUITY_INFO_UNKNOWN;
	vector<string> centreWordPOSambiguityInfoList;
	bool POSpermutationEntryExistent = LRPpreprocessorPOStaggerDatabase.DBreadPOSpermutationEstimates(POSambiguityInfoPermutation, &centreWordPOSambiguityInfoList);
	if(POSpermutationEntryExistent)
	{
		*maximumNumberOfInstances = 0;
		for(int i=0; i<centreWordPOSambiguityInfoList.size(); i++)
		{
			string centreWordPOSambiguityReferenceString = centreWordPOSambiguityInfoList[i];
			string centreWordPOSambiguityInfoCurrentByteBaseXstring = centreWordPOSambiguityReferenceString.substr(LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_POS_PERMUTATION_ENTRY_CENTRE_WORD_POS_AMBIGUITY_BYTE_CODED_START_POS, LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_POS_PERMUTATION_ENTRY_CENTRE_WORD_POS_AMBIGUITY_BYTE_CODED_LENGTH);
			#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_AND_MAP_USE_6BIT_INDICES
			char centreWordPOSambiguityInfoCurrentByteBaseXchar = centreWordPOSambiguityInfoCurrentByteBaseXstring[0];
			uchar centreWordPOSambiguityInfoCurrentByte = LRPpreprocessorPOStaggerDatabase.DBconvertBase64ToByte(centreWordPOSambiguityInfoCurrentByteBaseXchar);			
			#else
			uchar centreWordPOSambiguityInfoCurrentByte = LRPpreprocessorPOStaggerDatabase.DBconvertHexToByte(centreWordPOSambiguityInfoCurrentByteBaseXstring);
			#endif
			string centreWordPOSambiguityInfoNumberInstancesCurrentIntString = centreWordPOSambiguityReferenceString.substr(LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_POS_PERMUTATION_ENTRY_CENTRE_WORD_POS_AMBIGUITY_NUMBER_OF_INSTANCES_INT_START_POS, LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_POS_PERMUTATION_ENTRY_CENTRE_WORD_POS_AMBIGUITY_NUMBER_OF_INSTANCES_INT_LENGTH);
			int centreWordPOSambiguityInfoNumberInstancesCurrentInt = SHAREDvars.convertStringToInt(centreWordPOSambiguityInfoNumberInstancesCurrentIntString);

			if(centreWordPOSambiguityInfoNumberInstancesCurrentInt > *maximumNumberOfInstances)
			{						
				*foundMatchingCentreWordPOSambiguityInfo = true;
				*maximumNumberOfInstances = centreWordPOSambiguityInfoNumberInstancesCurrentInt;
				entryIndexWithMaximumNumberOfInstances = centreWordPOSambiguityInfoCurrentByte;
			}
		}
	}
	else
	{	
		//currentFeatureInList->grammaticalWordType = GRAMMATICAL_WORD_TYPE_UNDEFINED;	
		//cout << "LRP_PREPROCESSOR_POS_TAGGER GIAposRelTranslatorHybridClass::predictPOStaggerDatabaseEntry{} warning: word POS cannot be determined (!POSpermutationEntryExistent): " << centreWord->tagName << endl;
	}
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FEED_ALL_PERMUTATIONS_INDIVIDUALLY_ONLY_TRAIN_UNAMBIGUOUS_PERMUTATIONS
	*centreWordPOSisAmbiguous = false;
	*centreWordPOSindexPrediction = entryIndexWithMaximumNumberOfInstances;
	#else
	*centreWordPOSisAmbiguous = LRPpreprocessorPOStaggerDatabase.determinePOSambiguityInfoIsAmbiguous(entryIndexWithMaximumNumberOfInstances, centreWordPOSindexPrediction, true);
	//NB centreWordPOSisAmbiguous should always be false
	#endif
	
	#elif defined LRP_PREPROCESSOR_POS_TAGGER_DATABASE_MAP
	uchar entryIndexWithMaximumNumberOfInstances = LRP_PREPROCESSOR_POS_TAGGER_POS_AMBIGUITY_INFO_UNKNOWN;
	*maximumNumberOfInstances = 0;
	multimap<string, pair<uint64_t, int>>* POStaggerMap = getPOStaggerMap();
	pair<multimap<string, pair<uchar, int>>::iterator, multimap<string, pair<uchar, int>>::iterator> rangeFound = POStaggerMap->equal_range(POSambiguityInfoPermutationString);
	for(multimap<string, pair<uchar, int>>::iterator it = rangeFound.first; it != rangeFound.second; it++)
	{
		uchar centreWordPOSambiguityInfoCurrent = (it->second).first;
		int numberOfInstances = (it->second).second;
		if(numberOfInstances > *maximumNumberOfInstances)
		{
			*foundMatchingCentreWordPOSambiguityInfo = true;
			*maximumNumberOfInstances = numberOfInstances;
			entryIndexWithMaximumNumberOfInstances = centreWordPOSambiguityInfoCurrent;
		}
	}
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FEED_ALL_PERMUTATIONS_INDIVIDUALLY_ONLY_TRAIN_UNAMBIGUOUS_PERMUTATIONS
	*centreWordPOSisAmbiguous = false;
	*centreWordPOSindexPrediction = entryIndexWithMaximumNumberOfInstances;
	#else
	*centreWordPOSisAmbiguous = LRPpreprocessorPOStaggerDatabase.determinePOSambiguityInfoIsAmbiguous(entryIndexWithMaximumNumberOfInstances, centreWordPOSindexPrediction, true);
	//NB centreWordPOSisAmbiguous should always be false
	#endif
	#endif
	
	return result;	
}

#endif


#ifdef LRP_PREPROCESSOR_POS_TAGGER_INITIALISE_WORD_INDEX_LIST_FROM_WIKI_DUMP_TEXT
bool wikiDumpWordIndexListLoaded;
unordered_map<string,LRPpreprocessorMultiwordReductionWord*> wikiDumpWordIndexListGlobal;

bool LRPpreprocessorPOStaggerClass::createWordIndexListFromWikiDumpText(const bool useLRP)
{
	bool result = true;
	
	for(int wikiDumpFileBatchIndex=0; wikiDumpFileBatchIndex<LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE_DOC_XML_OUTPUT_NUMBER_OF_FILES; wikiDumpFileBatchIndex++)
	{
		string inputFileName = generateWikiDumpTextInputFileName(wikiDumpFileBatchIndex);
		LRPpreprocessorSentence* firstLRPpreprocessorSentenceInWikiDumpText = new LRPpreprocessorSentence();
		if(!generatePreprocessorSentenceObjectsFromText(inputFileName, firstLRPpreprocessorSentenceInWikiDumpText, useLRP))
		{
			result = false;
		}

		//add words to wikiDumpWordIndexListGlobal
		LRPpreprocessorSentence* currentLRPpreprocessorSentenceInList = firstLRPpreprocessorSentenceInWikiDumpText;
		while(currentLRPpreprocessorSentenceInList->next != NULL)
		{	
			vector<LRPpreprocessorPlainTextWord*>* sentenceContents = LRPpreprocessorSentenceClassObject.getSentenceContents(currentLRPpreprocessorSentenceInList);

			for(int w=0; w<sentenceContents->size(); w++)
			{		
				LRPpreprocessorPlainTextWord* currentWord = (*sentenceContents)[w];
				string wordLowerCase = SHAREDvars.convertStringToLowerCase(&(currentWord->tagName));	//CHECKTHIS: verify that currentWord->tagName is case sensitive
				wikiDumpWordIndexListGlobal->insert(pair<string, LRPpreprocessorMultiwordReductionWord*>(wordLowerCase, currentWord));
			}
			currentLRPpreprocessorSentenceInList = currentLRPpreprocessorSentenceInList->next;
		}		

		delete firstLRPpreprocessorSentenceInWikiDumpText;
	}
	
	wikiDumpWordIndexListLoaded = true;
	
	return result;	
}
#endif

#ifdef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE

#ifndef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_TRAIN_SINGLE_BATCH_ONLY
//UNTESTED
bool LRPpreprocessorPOStaggerClass::generatePOStaggerDatabaseFromWikiDumpText(string wikiDumpFolderName, const bool useLRP)
{
	bool result = true;

	string currentFolder = SHAREDvarsClass().getCurrentDirectory();
			
	for(int wikiDumpFileBatchIndex=LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE_DOC_XML_OUTPUT_START_FILE; wikiDumpFileBatchIndex<LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE_DOC_XML_OUTPUT_NUMBER_OF_FILES; wikiDumpFileBatchIndex++)
	{
		if(!generatePOStaggerDatabaseFromWikiDumpText(wikiDumpFolderName, wikiDumpFileBatchIndex, useLRP))
		{
			result = false;
		}
	}

	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_INTERNAL
	LRPpreprocessorPOStaggerDatabase.writeDatabaseNeuralNetwork();
	#elif defined LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_TRAIN_EXECUTE_FEED
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_TRAIN_EXECUTE_FEED_SINGLE_EPOCH_ONLY
	LRPpreprocessorPOStaggerDatabase.externalANNtrainEpoch();
	#else
	LRPpreprocessorPOStaggerDatabase.externalANNtrain();
	#endif
	#endif
	#endif
	#endif
	
	SHAREDvarsClass().setCurrentDirectory(currentFolder);

	return result;
}
#endif

bool LRPpreprocessorPOStaggerClass::generatePOStaggerDatabaseFromWikiDumpText(string wikiDumpFolderName, int wikiDumpFileBatchIndex, const bool useLRP)
{
	bool result = true;
	
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_NEURAL_NETWORK_SEQUENCE_GRAMMAR
	LRPtranslatorVariablesClass translatorVariablesNOTUSED;
	#endif
							
	#ifdef LRP_PREPROCESSOR_INITIALISE_WORD_INDEX_LIST_FROM_LRP_FILES
	if(!LRPpreprocessorWordIdentification.createWordIndexListFromLRPfiles())
	{
		result = false;
	}
	#else
	cerr << "LRPpreprocessorPOStaggerClass::generatePOStaggerDatabaseFromWikiDumpText{} error: !LRP_PREPROCESSOR_INITIALISE_WORD_INDEX_LIST_FROM_LRP_FILES; word index list currently needs to be regenerated everytime POS tagger database generation occurs" << endl;
	#endif
	
	string inputFileName = generateWikiDumpTextInputFileName(wikiDumpFileBatchIndex);
	cout << "wikiDumpFileName = " << inputFileName << endl;
	SHAREDvarsClass().setCurrentDirectory(wikiDumpFolderName);
	LRPpreprocessorSentence* firstLRPpreprocessorSentenceInWikiDumpText = new LRPpreprocessorSentence();
	if(!generatePreprocessorSentenceObjectsFromText(inputFileName, firstLRPpreprocessorSentenceInWikiDumpText, useLRP))
	{
		result = false;
	}
	
	//#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_INTERNAL_ANN_DEBUG_WRITE_NN_FREQUENTLY
	int64_t numberOfNeuralNetworkFeeds = 0;
	//#endif
	
	//generate POS tagger database entries
	cout << "finished generatePreprocessorSentenceObjectsFromText" << endl;
	
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_MEMORY_FREE_WRITE_EXPERIENCES_DIRECTLY_TO_FILE
	string XtrainBatchFileName = LRPpreprocessorPOStaggerDatabase.externalANNgenerateBatchFileName(LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_X_TRAIN_BATCH_FILE_NAME_PARTA, wikiDumpFileBatchIndex);
	#ifndef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE_WITHOUT_TARGET_CLASSES
	string YtrainBatchFileName = LRPpreprocessorPOStaggerDatabase.externalANNgenerateBatchFileName(LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_Y_TRAIN_BATCH_FILE_NAME_PARTA, wikiDumpFileBatchIndex);
	#endif
	ofstream XtrainBatchFileObject(XtrainBatchFileName.c_str());
	#ifndef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE_WITHOUT_TARGET_CLASSES
	ofstream YtrainBatchFileObject(YtrainBatchFileName.c_str());
	#endif
	#else
	ANNexperience* firstExperienceInList = new ANNexperience();
	ANNexperience* currentExperienceInList = firstExperienceInList;
	#endif
	#endif
	
	
	LRPpreprocessorSentence* currentLRPpreprocessorSentenceInList = firstLRPpreprocessorSentenceInWikiDumpText;
	while(currentLRPpreprocessorSentenceInList->next != NULL)
	{	
		vector<LRPpreprocessorPlainTextWord*>* sentenceContents = LRPpreprocessorSentenceClassObject.getSentenceContents(currentLRPpreprocessorSentenceInList);
		
		/*
		#ifdef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_NEURAL_NETWORK_SEQUENCE_GRAMMAR_PRINT_SENTENCES
		LRPpreprocessorSentenceClassObject.printSentence(sentenceContents, false);
		#endif
		*/
		
		#ifdef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_NEURAL_NETWORK_SEQUENCE_GRAMMAR_SENTENCE_MIN_WORDS
		if(sentenceContents->size() >= LRP_PREPROCESSOR_POS_TAGGER_GENERATE_NEURAL_NETWORK_SEQUENCE_GRAMMAR_SENTENCE_MIN_WORDS)
		{
		#endif
			/*
			#ifdef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_NEURAL_NETWORK_SEQUENCE_GRAMMAR_PRINT_SENTENCES
			//cout << "\t";
			LRPpreprocessorSentenceClassObject.printSentence(sentenceContents, false);
			#endif
			*/
			
			#ifdef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE_RAW
			int wCentre=-1;	//irrelevant (NOTUSED)
			#else
			#ifdef DEBUG_LRP_PREPROCESSOR_POS_TAGGER_DATABASE_CALCULATE_PERCENTAGE_POS_AMBIGUOUS_WORDS
			int nonAmbiguousWordCount = 0;
			#endif
			for(int wCentre=0; wCentre<sentenceContents->size(); wCentre++)
			{
			#endif
				//centre word calculations
				LRPpreprocessorPlainTextWord* centreWord = (*sentenceContents)[wCentre];
				#ifdef LRP_PREPROCESSOR_POS_TAGGER_POS_DEBUG
				cout << "centreWord = " << centreWord->tagName << endl;
				#endif

				//cout << "centreWord = " << centreWord->tagName << endl;

				bool identifiedEveryWordInDatabasePOSpermutation = true;
				bool identifiedEveryWordInDatabasePOSpermutationIsUnambiguous = true;

				#ifndef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE_RAW
				string centreWordLowerCase = SHAREDvars.convertStringToLowerCase(&(centreWord->tagName));	//CHECKTHIS: verify that currentWord->tagName is case sensitive
				LRPpreprocessorMultiwordReductionWord* centreWordFound = NULL;
				//bool identifiedCentreWordInDatabasePOSpermutation = true;
				uint64_t centreWordPOSambiguityInfo = LRP_PREPROCESSOR_POS_TAGGER_POS_AMBIGUITY_INFO_UNKNOWN;	//default value
				if(!findWordInWordListAllTypesWithPOSambiguityInfo(centreWordLowerCase, &centreWordFound, &centreWordPOSambiguityInfo))
				{
					//identifiedCentreWordInDatabasePOSpermutation = false;
					identifiedEveryWordInDatabasePOSpermutation = false;
				}
				#endif

				vector<uint64_t> POSambiguityInfoPermutation;
				if(generatePOSambiguityInfoPermutation(sentenceContents, wCentre, &identifiedEveryWordInDatabasePOSpermutation, &identifiedEveryWordInDatabasePOSpermutationIsUnambiguous, &POSambiguityInfoPermutation))
				{
					//cout << "identifiedEveryWordInDatabasePOSpermutationIsUnambiguous = " << identifiedEveryWordInDatabasePOSpermutationIsUnambiguous << endl;

					#ifndef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE_RAW
					#ifdef LRP_PREPROCESSOR_POS_TAGGER_ONLY_ADD_DATABASE_ENTRY_IF_POS_AMBIGUITY_INFO_FOUND_FOR_EVERY_CONTEXT_WORD
					if(identifiedEveryWordInDatabasePOSpermutation)
					{
					#endif
						uchar centreWordUnambiguousPOSindex = INT_DEFAULT_VALUE;
						bool centreWordAmbiguous = LRPpreprocessorPOStaggerDatabase.determinePOSambiguityInfoIsAmbiguous(centreWordPOSambiguityInfo, &centreWordUnambiguousPOSindex, true);
						#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FEED_ALL_PERMUTATIONS_INDIVIDUALLY_ONLY_TRAIN_UNAMBIGUOUS_PERMUTATIONS
						if(identifiedEveryWordInDatabasePOSpermutationIsUnambiguous && !centreWordAmbiguous)
						{
						#endif
							#ifndef LRP_PREPROCESSOR_POS_TAGGER_INCLUDE_CENTRE_WORD_IN_POS_PERMUTATION_EVEN_IF_AMBIGUOUS
							if(!centreWordAmbiguous)
							{
								#ifdef DEBUG_LRP_PREPROCESSOR_POS_TAGGER_DATABASE_CALCULATE_PERCENTAGE_POS_AMBIGUOUS_WORDS
								nonAmbiguousWordCount++;
								#endif
							#endif
					#endif
								#ifdef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_NEURAL_NETWORK_SEQUENCE_GRAMMAR_ONLY_TRAIN_UNAMBIGUOUS_PERMUTATIONS
								/*
								bool allWordsHaveUnambigiousPOS = true;
								for(int i=0; i<POSambiguityInfoPermutation.size(); i++)
								{
									bool treatWordAsAmbiguousIfNullPOSvalue = true;
									int unambiguousPOSinfoIndexNOTUSED;
									if(LRPpreprocessorPOStaggerDatabase.determinePOSambiguityInfoIsAmbiguous(POSambiguityInfoPermutation[i], &,unambiguousPOSinfoIndexNOTUSED, treatWordAsAmbiguousIfNullPOSvalue);
									{
										allWordsHaveUnambigiousPOS = false;
									}
								}
								if(allWordsHaveUnambigiousPOS)
								*/
								if(identifiedEveryWordInDatabasePOSpermutationIsUnambiguous)
								{
									setSentenceContentsWordsUnambiguousPOSindex(sentenceContents, &POSambiguityInfoPermutation);

									#ifdef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_NEURAL_NETWORK_SEQUENCE_GRAMMAR_PRINT_SENTENCES
									LRPpreprocessorSentenceClassObject.printSentence(sentenceContents, false);
									#endif
								#else

								#ifdef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_NEURAL_NETWORK_SEQUENCE_GRAMMAR_PRINT_SENTENCES
								LRPpreprocessorSentenceClassObject.printSentence(sentenceContents, false);
								#endif

								#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FEED_ALL_PERMUTATIONS_INDIVIDUALLY
								vector<vector<uint64_t>*> POSambiguityInfoUnambiguousPermutationArray;
								vector<uint64_t>* POSambiguityInfoUnambiguousPermutationNew = new vector<uint64_t>(POSambiguityInfoPermutation.size());
								POSambiguityInfoUnambiguousPermutationArray.push_back(POSambiguityInfoUnambiguousPermutationNew);
								generatePOSambiguityInfoUnambiguousPermutationArray(&POSambiguityInfoUnambiguousPermutationArray, &POSambiguityInfoPermutation, POSambiguityInfoUnambiguousPermutationNew, 0);
								for(int i=0; i<POSambiguityInfoUnambiguousPermutationArray.size(); i++)
								{
									vector<uint64_t>* POSambiguityInfoPermutationTemp = POSambiguityInfoUnambiguousPermutationArray[i];
									#ifdef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_NEURAL_NETWORK_SEQUENCE_GRAMMAR
									setSentenceContentsWordsUnambiguousPOSindex(sentenceContents, POSambiguityInfoPermutationTemp);
									#endif

								#else
									vector<uint64_t>* POSambiguityInfoPermutationTemp = &POSambiguityInfoPermutation;
									#ifdef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_NEURAL_NETWORK_SEQUENCE_GRAMMAR
									(sentenceContents->at(w))->POSambiguityInfo = (*POSambiguityInfoPermutation)[w];
									#endif
								#endif
								#endif

									#ifdef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_NEURAL_NETWORK_SEQUENCE_GRAMMAR
									//cout << "LRP_PREPROCESSOR_POS_TAGGER_GENERATE_NEURAL_NETWORK_SEQUENCE_GRAMMAR: executePosRelTranslatorNeuralNetwork" << endl;
									
									vector<SANIGroupType*>* SANIGroupTypes = SANIrules.getSANIGroupTypesGlobal();
									bool createNewConnections = true; 
									vector<SANIGroupNeuralNetwork*> firstLayer;
									if(SANIpropagateCompactGenerate.executePosRelTranslatorNeuralNetwork(&translatorVariablesNOTUSED, SANIGroupTypes, sentenceContents, createNewConnections))
									{
										result = true;
									}
									#else
									
									#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL
									#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_MEMORY_FREE_WRITE_EXPERIENCES_DIRECTLY_TO_FILE
									#ifdef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE_WITHOUT_TARGET_CLASSES
									if(!addPOStaggerDatabaseEntry1(POSambiguityInfoPermutationTemp, &XtrainBatchFileObject))
									#else
									if(!addPOStaggerDatabaseEntry2(centreWordPOSambiguityInfo, POSambiguityInfoPermutationTemp, centreWordUnambiguousPOSindex, &XtrainBatchFileObject, &YtrainBatchFileObject))
									#endif
									#else
									if(!addPOStaggerDatabaseEntry3(centreWordPOSambiguityInfo, POSambiguityInfoPermutationTemp, centreWordUnambiguousPOSindex, currentExperienceInList))
									#endif
									#else
									if(!addPOStaggerDatabaseEntry4(centreWordPOSambiguityInfo, POSambiguityInfoPermutationTemp, centreWordUnambiguousPOSindex, &numberOfNeuralNetworkFeeds))
									#endif
									{
										result = false;
									}
									
									#endif
								#ifdef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_NEURAL_NETWORK_SEQUENCE_GRAMMAR_ONLY_TRAIN_UNAMBIGUOUS_PERMUTATIONS
								}
								#else
								#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FEED_ALL_PERMUTATIONS_INDIVIDUALLY
								}
								#endif
								#endif
					#ifndef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE_RAW			
							#ifndef LRP_PREPROCESSOR_POS_TAGGER_INCLUDE_CENTRE_WORD_IN_POS_PERMUTATION_EVEN_IF_AMBIGUOUS
							}
							#endif  

							#ifdef LRP_PREPROCESSOR_POS_TAGGER_POS_DEBUG
							cout << "insertInstanceInPOStaggerMap: POSambiguityInfoPermutation = " << POSambiguityInfoPermutation << ", centreWordPOSambiguityInfo = " << centreWordPOSambiguityInfo << endl;
							#endif
						#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FEED_ALL_PERMUTATIONS_INDIVIDUALLY_ONLY_TRAIN_UNAMBIGUOUS_PERMUTATIONS
						}
						#endif
					#ifdef LRP_PREPROCESSOR_POS_TAGGER_ONLY_ADD_DATABASE_ENTRY_IF_POS_AMBIGUITY_INFO_FOUND_FOR_EVERY_CONTEXT_WORD
					}
					#endif
					#endif
				}
				else
				{
					#ifndef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_DO_NOT_TRAIN_POS_INDEX_OUT_OF_SENTENCE_BOUNDS
					result = false;
					#endif
				}
			#ifndef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE_RAW
			}
			#ifdef DEBUG_LRP_PREPROCESSOR_POS_TAGGER_DATABASE_CALCULATE_PERCENTAGE_POS_AMBIGUOUS_WORDS
			int ambiguousWordCount = sentenceContents->size() - nonAmbiguousWordCount;
			cout << "ambiguousWordCount\tsentenceContentsSize\t" << ambiguousWordCount << "\t" << sentenceContents->size() << endl;
			#endif
			#endif

			#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_MEMORY_FREE_DELETE_PREPROCESSOR_SENTENCES_ONCE_PROCESSED
			LRPpreprocessorSentence* previousLRPpreprocessorSentenceInList = currentLRPpreprocessorSentenceInList;
			#endif		
			currentLRPpreprocessorSentenceInList = currentLRPpreprocessorSentenceInList->next;
			#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_MEMORY_FREE_DELETE_PREPROCESSOR_SENTENCES_ONCE_PROCESSED
			previousLRPpreprocessorSentenceInList->next = NULL;	//prevents future sentences from being deleted
			delete previousLRPpreprocessorSentenceInList;
			#endif
		#ifdef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_NEURAL_NETWORK_SEQUENCE_GRAMMAR_SENTENCE_MIN_WORDS
		}
		else
		{
			currentLRPpreprocessorSentenceInList = currentLRPpreprocessorSentenceInList->next;
		}
		#endif
	}		

	#ifndef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_MEMORY_FREE_DELETE_PREPROCESSOR_SENTENCES_ONCE_PROCESSED
	delete firstLRPpreprocessorSentenceInWikiDumpText;
	#endif
	
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_INTERNAL
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_INTERNAL_ANN_DEBUG_WRITE_NN_FREQUENTLY
	LRPpreprocessorPOStaggerDatabase.writeDatabaseNeuralNetwork();
	string neuralNetworkXmlFileName = LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_DEFAULT_XML_FILE_NAME;
	string neuralNetworkXmlFileNameBackup = neuralNetworkXmlFileName + STRING_FULLSTOP + SHAREDvars.convertIntToString(wikiDumpFileBatchIndex);
	SHAREDvars.copyFiles(neuralNetworkXmlFileName, neuralNetworkXmlFileNameBackup);
	cout << "B numberOfNeuralNetworkFeeds = " << numberOfNeuralNetworkFeeds << endl;
	//exit(EXIT_ERROR);
	#endif
	#elif defined LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_MEMORY_FREE_WRITE_EXPERIENCES_DIRECTLY_TO_FILE
	XtrainBatchFileObject.close();
	#ifndef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE_WITHOUT_TARGET_CLASSES
	YtrainBatchFileObject.close();
	#endif
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_TRAIN_EXECUTE_FEED_SINGLE_BATCH_ONLY
	LRPpreprocessorPOStaggerDatabase.externalANNgenerateBatchTrainDataExecuteFeed();
	#endif
	#else
	LRPpreprocessorPOStaggerDatabase.externalANNgenerateBatchTrainData(firstExperienceInList, wikiDumpFileBatchIndex);
	delete firstExperienceInList;
	#endif
	#endif
	#endif
	
	return result;
}


#ifdef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE_WITHOUT_TARGET_CLASSES

#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK
#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL
#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_MEMORY_FREE_WRITE_EXPERIENCES_DIRECTLY_TO_FILE
bool LRPpreprocessorPOStaggerClass::addPOStaggerDatabaseEntry1(const vector<uint64_t>* POSambiguityInfoPermutation, ofstream* XtrainBatchFileObject)
{
	bool result = true;
	
	ANNexperience* currentExperienceInList = new ANNexperience();
	uchar centreWordUnambiguousPOSindexUNUSED = 0; 
	generateANNexperienceFromPOSambiguityInfoPermutation(POSambiguityInfoPermutation, centreWordUnambiguousPOSindexUNUSED, currentExperienceInList);
	string experienceInputString = LRPpreprocessorPOStaggerDatabase.externalANNgenerateBatchDataExperienceInput(currentExperienceInList);
	*XtrainBatchFileObject << experienceInputString << endl;
	delete currentExperienceInList;

	return result;	
}
#endif
#endif
#endif

#else

/*
CS compatibility:
#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL
#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_MEMORY_FREE_WRITE_EXPERIENCES_DIRECTLY_TO_FILE
bool LRPpreprocessorPOStaggerClass::addPOStaggerDatabaseEntry2(uint64_t centreWordPOSambiguityInfo, vector<uint64_t>* POSambiguityInfoPermutation, uchar centreWordUnambiguousPOSindex, ofstream* XtrainBatchFileObject, ofstream* YtrainBatchFileObject)
{
	return addPOStaggerDatabaseEntry(centreWordPOSambiguityInfo, POSambiguityInfoPermutation, centreWordUnambiguousPOSindex, XtrainBatchFileObject, YtrainBatchFileObject, NULL, NULL);
}
#else
bool LRPpreprocessorPOStaggerClass::addPOStaggerDatabaseEntry3(uint64_t centreWordPOSambiguityInfo, vector<uint64_t>* POSambiguityInfoPermutation, uchar centreWordUnambiguousPOSindex, ANNexperience** currentExperienceInList)
{
	return addPOStaggerDatabaseEntry(centreWordPOSambiguityInfo, POSambiguityInfoPermutation, centreWordUnambiguousPOSindex, NULL, NULL, currentExperienceInList, NULL);
}
#endif
#else
bool LRPpreprocessorPOStaggerClass::addPOStaggerDatabaseEntry4(uint64_t centreWordPOSambiguityInfo, vector<uint64_t>* POSambiguityInfoPermutation, uchar centreWordUnambiguousPOSindex, int64_t* numberOfNeuralNetworkFeeds)
{
	return addPOStaggerDatabaseEntry(centreWordPOSambiguityInfo, POSambiguityInfoPermutation, centreWordUnambiguousPOSindex, NULL, NULL, NULL, numberOfNeuralNetworkFeeds);
}
#endif

bool LRPpreprocessorPOStaggerClass::addPOStaggerDatabaseEntry(uint64_t centreWordPOSambiguityInfo, vector<uint64_t>* POSambiguityInfoPermutation, uchar centreWordUnambiguousPOSindex, ofstream* XtrainBatchFileObject, ofstream* YtrainBatchFileObject, ANNexperience** currentExperienceInList, int64_t* numberOfNeuralNetworkFeeds)
*/

#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL
#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_MEMORY_FREE_WRITE_EXPERIENCES_DIRECTLY_TO_FILE
bool LRPpreprocessorPOStaggerClass::addPOStaggerDatabaseEntry2(uint64_t centreWordPOSambiguityInfo, vector<uint64_t>* POSambiguityInfoPermutation, uchar centreWordUnambiguousPOSindex, ofstream* XtrainBatchFileObject, ofstream* YtrainBatchFileObject)
#else
bool LRPpreprocessorPOStaggerClass::addPOStaggerDatabaseEntry3(uint64_t centreWordPOSambiguityInfo, vector<uint64_t>* POSambiguityInfoPermutation, uchar centreWordUnambiguousPOSindex, ANNexperience** currentExperienceInList)
#endif
#else
bool LRPpreprocessorPOStaggerClass::addPOStaggerDatabaseEntry4(uint64_t centreWordPOSambiguityInfo, vector<uint64_t>* POSambiguityInfoPermutation, uchar centreWordUnambiguousPOSindex, int64_t* numberOfNeuralNetworkFeeds)
#endif
{
	bool result = true;	
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK
	//cout << "LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK" << endl;

	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_INTERNAL

	//cout << "LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_INTERNAL" << endl;
	ANNexperience* currentExperienceInList = new ANNexperience();
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_INTERNAL_CLASSIFICATION_NET
	uint64_t centreWordUnambiguousPOSindexTemp = LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_INTERNAL_CLASSIFICATION_NET_TRAIN_EXPERIENCE_OUTPUT_VALUE_NOT_USED;
	#else
	uint64_t centreWordUnambiguousPOSindexTemp = centreWordUnambiguousPOSindex; 
	#endif
	generateANNexperienceFromPOSambiguityInfoPermutation(POSambiguityInfoPermutation, centreWordUnambiguousPOSindexTemp, currentExperienceInList);
	LRPpreprocessorPOStaggerDatabase.feedNeuralNetworkWithExperience(currentExperienceInList);
	delete currentExperienceInList;
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_INTERNAL_ANN_DEBUG_WRITE_NN_FREQUENTLY
	(*numberOfNeuralNetworkFeeds) = (*numberOfNeuralNetworkFeeds) + 1;
	if((*numberOfNeuralNetworkFeeds)%100000 == 0)
	{
		LRPpreprocessorPOStaggerDatabase.writeDatabaseNeuralNetwork();
		cout << "A numberOfNeuralNetworkFeeds = " << numberOfNeuralNetworkFeeds << endl;
	}
	#endif

	#elif defined LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL

	//cout << "LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL" << endl;
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_MEMORY_FREE_WRITE_EXPERIENCES_DIRECTLY_TO_FILE
	ANNexperience* currentExperienceInList = new ANNexperience();
	#endif
	generateANNexperienceFromPOSambiguityInfoPermutation(POSambiguityInfoPermutation, centreWordUnambiguousPOSindex, currentExperienceInList);
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_MEMORY_FREE_WRITE_EXPERIENCES_DIRECTLY_TO_FILE
	string experienceInputString = LRPpreprocessorPOStaggerDatabase.externalANNgenerateBatchDataExperienceInput(currentExperienceInList);
	string experienceOutputString = LRPpreprocessorPOStaggerDatabase.externalANNgenerateBatchDataExperienceOutput(currentExperienceInList);
	*XtrainBatchFileObject << experienceInputString << endl;
	*YtrainBatchFileObject << experienceOutputString << endl;
	delete currentExperienceInList;
	#else
	(*currentExperienceInList)->next = new ANNexperience();
	(*currentExperienceInList) = (*currentExperienceInList)->next;
	#endif

	#endif
		
	#elif defined LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM
	//cout << "LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM" << endl;
	for(int i=0; i<POSambiguityInfoPermutation->size(); i++)
	{
		uint64_t val = (*POSambiguityInfoPermutation)[i]; 
		//cout << "val = " << val << endl;
	}
	//cout << "centreWordPOSambiguityInfoString = " << centreWordPOSambiguityInfoString << endl;
	if(!LRPpreprocessorPOStaggerDatabase.DBwritePOSpermutationEstimate(POSambiguityInfoPermutation, centreWordPOSambiguityInfo))
	{
		result = false;
	}
	
	#elif defined LRP_PREPROCESSOR_POS_TAGGER_DATABASE_MAP
	//cout << "LRP_PREPROCESSOR_POS_TAGGER_DATABASE_MAP" << endl;
	int numberOfInstances = 0;
	bool incrementIfFound = true;
	if(!LRPpreprocessorPOStaggerDatabase.findInstancePOStaggerMap(POSambiguityInfoPermutation, centreWordPOSambiguityInfo, &numberOfInstances, incrementIfFound))
	{
		numberOfInstances = 1;
		LRPpreprocessorPOStaggerDatabase.insertInstanceInPOStaggerMap(POSambiguityInfoPermutation, centreWordPOSambiguityInfo, numberOfInstances);
	}	
	
	#endif
	
	return result;	
}
#endif

string LRPpreprocessorPOStaggerClass::generateWikiDumpTextInputFileName(const int wikiDumpFileBatchIndex)
{
	string formatString = LRPpreprocessorPOStaggerDatabase.generateIntFormatString(LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE_DOC_XML_OUTPUT_NAME_PART_B_INDEX_NUMBER_OF_CHARACTERS);
	string inputFileName = string(LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE_DOC_XML_OUTPUT_NAME_PART_A) + SHAREDvars.convertIntToString(wikiDumpFileBatchIndex, formatString) + LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE_DOC_XML_OUTPUT_NAME_PART_C;
	return inputFileName;
}


bool LRPpreprocessorPOStaggerClass::generatePreprocessorSentenceObjectsFromText(const string inputFileName, LRPpreprocessorSentence* firstLRPpreprocessorSentenceInText, const bool useLRP)
{
	bool result = true;
	
	//generate preprocessor sentence objects from text
	string fileContents = SHAREDvars.getFileContents(inputFileName);
	bool interpretNewLinesAsNewSentences = true;	//CHECKTHIS
	//cout << "fileContents = " << fileContents << endl;
	if(!createPreprocessSentencesBasic(fileContents, firstLRPpreprocessorSentenceInText, interpretNewLinesAsNewSentences))
	{
		result = false;
	}	
	//code replicated from LRPpreprocessorClass::createPreprocessSentencesForGIA
	LRPpreprocessorSentence* currentLRPpreprocessorSentenceInList = firstLRPpreprocessorSentenceInText;
	while(currentLRPpreprocessorSentenceInList->next != NULL)
	{	
		currentLRPpreprocessorSentenceInList->sentenceContentsLRP = currentLRPpreprocessorSentenceInList->sentenceContentsOriginal;
		
		#ifdef LRP_PREPROCESSOR_RECORD_REFERENCES
		for(int w=0; w<currentLRPpreprocessorSentenceInList->sentenceContentsLRP.size(); w++)
		{		
			LRPpreprocessorMultiwordReductionPlainTextWord* sentenceContentsLRPcurrentWord = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>((currentLRPpreprocessorSentenceInList->sentenceContentsLRP)[w]);
			LRPpreprocessorMultiwordReductionPlainTextWord* sentenceContentsOriginalcurrentWord = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>((currentLRPpreprocessorSentenceInList->sentenceContentsLRP)[w]);
			sentenceContentsLRPcurrentWord->preprocessorUpperLevelWordReference = sentenceContentsOriginalcurrentWord;
			sentenceContentsLRPcurrentWord->preprocessorUpperLevelWordReferenceSize = 1;
		}
		#endif
		
		currentLRPpreprocessorSentenceInList = currentLRPpreprocessorSentenceInList->next;
	}
	
	if(useLRP)
	{
		//perform multiword reduction
		#ifdef LRP_PREPROCESSOR_WORD_MULTIWORD_REDUCTION
		bool isQuery = false;

		string outputLRPTextPlainTXTFileName = LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE_WIKIDUMP_MULTIWORD_FILE_NAME;
		string outputLRPTextForNLPonlyPlainTXTFileNameBase = outputLRPTextPlainTXTFileName + LRP_PREPROCESSOR_OUTPUT_FOR_NLP_ONLY_FILE_EXTENSION;

		string outputLRPTextPlainTXTFileNameWikiDumpMultiword = string(outputLRPTextPlainTXTFileName) + LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE_WIKIDUMP_MULTIWORD_FILE_EXTENSION;
		string outputLRPTextForNLPonlyPlainTXTFileNameWikiDumpMultiword = string(outputLRPTextForNLPonlyPlainTXTFileNameBase) + LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE_WIKIDUMP_MULTIWORD_FILE_EXTENSION;
		LRPpreprocessorWordReduction.initialiseActiveLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo(isQuery);
		LRPpreprocessorWordReduction.setActiveLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo(isQuery);	//required for local variable access
		if(!LRPpreprocessorWordReduction.parseTextFileAndReduceLanguage(firstLRPpreprocessorSentenceInText, outputLRPTextPlainTXTFileNameWikiDumpMultiword, outputLRPTextForNLPonlyPlainTXTFileNameWikiDumpMultiword))
		{
			result = false;
		}
		LRPpreprocessorWordReduction.deinitialiseActiveLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo(isQuery);	//required so as not to intefere with GIA preprocessor
		#endif
	}
	
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_NEURAL_NETWORK_SEQUENCE_GRAMMAR
	//code replicated from GIAposRelTranslatorClass::parseTxtfileAndCreateSemanticNetworkBasedUponSemanticDependencyParsedSentences
	#ifdef LRP_DEFINE_WORD_TRANSLATOR_SENTENCE_ENTITY_INDEX_AT_START
	currentLRPpreprocessorSentenceInList = firstLRPpreprocessorSentenceInText;
	while(currentLRPpreprocessorSentenceInList->next != NULL)
	{
		vector<LRPpreprocessorPlainTextWord*>* sentenceContents = LRPpreprocessorSentenceClassObject.getSentenceContents(currentLRPpreprocessorSentenceInList);
		for(int w=0; w<sentenceContents->size(); w++)
		{	
			LRPpreprocessorPlainTextWord* currentWord = sentenceContents->at(w);
			currentWord->translatorSentenceEntityIndex = LRPpreprocessorWordClassObject.convertSentenceContentsIndexToEntityIndex(w);
			#ifdef SANI_FORWARD
			currentWord->translatorSentenceWordIndex = w;
			#endif
		}
		currentLRPpreprocessorSentenceInList = currentLRPpreprocessorSentenceInList->next;
	}
	#endif
	#endif
	
	return result;
}	

bool LRPpreprocessorPOStaggerClass::createPreprocessSentencesBasic(string fileContents, LRPpreprocessorSentence* firstLRPpreprocessorSentenceInList, const bool interpretNewLinesAsNewSentences)
{	
	bool result = true;
	
	int charCount = 0;
	char currentToken;
	string currentWord = "";
	LRPpreprocessorSentence* currentLRPpreprocessorSentenceInList = firstLRPpreprocessorSentenceInList;
	LRPpreprocessorMultiwordReductionPlainTextWord* firstWordInSentence = new LRPpreprocessorMultiwordReductionPlainTextWord();
	LRPpreprocessorMultiwordReductionPlainTextWord* currentWordInSentence = firstWordInSentence;
	string sentenceContentsOriginalText = "";
	int entityIndex = LRP_NLP_START_ENTITY_INDEX;	//only assigned after collapse?
	int sentenceIndex = LRP_NLP_START_SENTENCE_INDEX;
	
	while(charCount < fileContents.length())
	{		
		currentToken = fileContents[charCount];
		sentenceContentsOriginalText = sentenceContentsOriginalText + currentToken;
		
		bool punctuationMarkFound = false;
		if(SHAREDvars.charInCharArray(currentToken, nlpPunctionMarkCharacterArray, LRP_NLP_NUMBER_OF_PUNCTUATION_MARK_CHARACTERS))
		{
			#ifdef LRP_PREPROCESSOR_WORD_NLP_PARSABLE_PHRASE_SUPPORT_INTRAWORD_PUNCTUATION_MARK
			if(!LRPpreprocessorWordClassObject.isIntrawordPunctuationMark(charCount, &fileContents))
			{
			#endif
				punctuationMarkFound = true;
			#ifdef LRP_PREPROCESSOR_WORD_NLP_PARSABLE_PHRASE_SUPPORT_INTRAWORD_PUNCTUATION_MARK
			}
			#endif
		}
		
		bool whiteSpaceFound = false;
		if(SHAREDvars.charInCharArray(currentToken, nlpWhitespaceCharacterArray, LRP_NLP_NUMBER_OF_WHITESPACE_CHARACTERS))	//NB this includes CHAR_NEWLINE
		{
			whiteSpaceFound = true;
		}
		
		bool newlineFound = false;
		if(currentToken == CHAR_NEWLINE)
		{
			newlineFound = true;
		}
		
		bool specialCharFound = false;
		if(!SHAREDvars.charInCharArray(currentToken, preprocessorLowerOrUpperCaseLettersArray, LRP_PREPROCESSOR_WORD_LOWER_OR_UPPER_CASE_LETTERS_NUMBER_OF_TYPES))
		{
			specialCharFound = true;
		}
		
		/*
		//not required because it will be interpreted as specialCharFound
		bool quotationCharFound = false;
		if(!SHAREDvars.charInCharArray(currentToken, nlpQuotationMarkCharacterArray, LRP_NLP_NUMBER_OF_QUOTATIONMARK_CHARACTERS))
		{
			quotationCharFound = true;
		}
		*/
		
		bool endOfSentencePunctuationMarkFound = false;

		if(whiteSpaceFound || newlineFound || punctuationMarkFound || specialCharFound)
		{
			bool lastWordBlank = true;
			
			int lastCharacterIndexOfLastWordBeingFilled = charCount-1; 
			//cout << "currentWord = " << currentWord << ", currentToken = " << currentToken << endl;
			
			if(currentWord != "")
			{//do not add empty tag after closing quotation marks	//e.g. LRP_PREPROCESSOR_WORD_REDUCE_QUOTES_TO_SINGLE_WORDS or (newlineFound && interpretNewLinesAsNewSentences && previousCharacter==whitespace)
				lastWordBlank = false;
				LRPpreprocessorWordClassObject.preprocessorFillCurrentWord(&currentWordInSentence, &currentWord, &entityIndex, lastCharacterIndexOfLastWordBeingFilled);
			}
			
			if(punctuationMarkFound)
			{
				string punctuationMark = ""; 
				punctuationMark = punctuationMark + currentToken;
				LRPpreprocessorWordClassObject.preprocessorFillCurrentWord(&currentWordInSentence, &punctuationMark, &entityIndex, charCount);

				if(SHAREDvars.charInCharArray(currentToken, nlpPunctionMarkCharacterEndOfSentenceArray, LRP_NLP_NUMBER_OF_PUNCTUATION_MARK_CHARACTERS_END_OF_SENTENCE))
				{
					#ifdef LRP_PREPROCESSOR_WORD_NLP_PARSABLE_PHRASE_SUPPORT_INTRAWORD_PUNCTUATION_MARK
					if(!LRPpreprocessorWordClassObject.isIntrawordPunctuationMark(charCount, &fileContents))
					{
					#endif
						endOfSentencePunctuationMarkFound = true;
					#ifdef LRP_PREPROCESSOR_WORD_NLP_PARSABLE_PHRASE_SUPPORT_INTRAWORD_PUNCTUATION_MARK
					}
					#endif
				}
				
				bool lastCharacterInFile = false;
				#ifndef GIA_EXPECT_NEWLINE_AT_END_OF_INPUT_TEXT_FILE
				if(charCount == fileContents.length()-1)
				{
					lastCharacterInFile = true;
				}
				#endif
				if(endOfSentencePunctuationMarkFound)
				{
					LRPpreprocessorWordClassObject.generateSentenceWordList(firstWordInSentence, &(currentLRPpreprocessorSentenceInList->sentenceContentsOriginal));
					currentLRPpreprocessorSentenceInList->sentenceIndexOriginal = sentenceIndex;
					currentLRPpreprocessorSentenceInList->sentenceContentsOriginalText = sentenceContentsOriginalText;
					currentLRPpreprocessorSentenceInList->next = new LRPpreprocessorSentence();
					currentLRPpreprocessorSentenceInList = currentLRPpreprocessorSentenceInList->next;
					firstWordInSentence = new LRPpreprocessorMultiwordReductionPlainTextWord();
					currentWordInSentence = firstWordInSentence;
					sentenceContentsOriginalText = "";
					entityIndex = LRP_NLP_START_ENTITY_INDEX;
					sentenceIndex++;
				}
			}
			else if(newlineFound && interpretNewLinesAsNewSentences)
			{
				#ifdef LRP_PREPROCESSOR_DISALLOW_EMPTY_SENTENCE_OBJECTS
				if(firstWordInSentence->nextTag != NULL)
				{
				#endif
					LRPpreprocessorWordClassObject.generateSentenceWordList(firstWordInSentence, &(currentLRPpreprocessorSentenceInList->sentenceContentsOriginal));
					currentLRPpreprocessorSentenceInList->sentenceIndexOriginal = sentenceIndex;
					currentLRPpreprocessorSentenceInList->sentenceContentsOriginalText = sentenceContentsOriginalText;
					currentLRPpreprocessorSentenceInList->next = new LRPpreprocessorSentence();
					currentLRPpreprocessorSentenceInList = currentLRPpreprocessorSentenceInList->next;
					firstWordInSentence = new LRPpreprocessorMultiwordReductionPlainTextWord();
					currentWordInSentence = firstWordInSentence;
					sentenceContentsOriginalText = "";
					entityIndex = LRP_NLP_START_ENTITY_INDEX;
					sentenceIndex++;
				#ifdef LRP_PREPROCESSOR_DISALLOW_EMPTY_SENTENCE_OBJECTS
				}
				#endif
			}
			
		}
		else
		{
			currentWord = currentWord + currentToken;
		}
		
		charCount++;
	}
		
	return result;
}
#endif


#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK
bool LRPpreprocessorPOStaggerClass::generateANNexperienceFromPOSambiguityInfoPermutation(const vector<uint64_t>* POSambiguityInfoPermutation, uchar centreWordUnambiguousPOSindex, ANNexperience* currentExperience)
{
	return generateANNexperienceFromPOSambiguityInfoPermutationStandalone(POSambiguityInfoPermutation, centreWordUnambiguousPOSindex, currentExperience);
}

#endif

/*
int LRPpreprocessorPOStaggerClass::convertGIAPOStaggerValueToGrammaticalWordType(const int POSvalue)
{
	int grammaticalWordType = GIAPOStaggerValueCrossReferenceGrammaticalWordTypeArray[POSvalue];
	return grammaticalWordType;
}
*/
	
bool LRPpreprocessorPOStaggerClass::generatePOSambiguityInfoPermutation(vector<LRPpreprocessorPlainTextWord*>* sentenceContents, const int wCentre, bool* identifiedEveryWordInDatabasePOSpermutation, bool* identifiedEveryWordInDatabasePOSpermutationIsUnambiguous, vector<uint64_t>* POSambiguityInfoPermutation)
{
	bool result = true;
	
	LRPpreprocessorPlainTextWord* centreWord = (*sentenceContents)[wCentre];

	//context word calculations
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE_RAW
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE_EXCLUDE_POS_INDEX_OUT_OF_SENTENCE_BOUNDS
	int wMin = 0;
	int wMax = sentenceContents->size()-1;
	int wSize = wMax - wMin;
	POSambiguityInfoPermutation->resize(sentenceContents->size(), LRP_PREPROCESSOR_POS_TAGGER_POS_AMBIGUITY_INFO_UNKNOWN);	//LRP_PREPROCESSOR_POS_TAGGER_MAX_WORDS_IN_DATABASE_POS_PERMUTATION = max size of wSize	
	#else	
	int wMin = 0;
	int wMax = LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE_RAW_SENTENCE_MAX_NUM_WORDS;
	int wSize = wMax - wMin;	//LRP_PREPROCESSOR_POS_TAGGER_MAX_CONTEXT_WORDS_IN_DATABASE_POS_PERMUTATION
	POSambiguityInfoPermutation->resize(LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE_RAW_SENTENCE_MAX_NUM_WORDS, LRP_PREPROCESSOR_POS_TAGGER_POS_AMBIGUITY_INFO_UNKNOWN);	//LRP_PREPROCESSOR_POS_TAGGER_MAX_WORDS_IN_DATABASE_POS_PERMUTATION = max size of wSize
	#endif
	#else
	int wMin = wCentre - (LRP_PREPROCESSOR_POS_TAGGER_MAX_CONTEXT_WORDS_IN_DATABASE_POS_PERMUTATION/2);
	int wMax = wCentre + (LRP_PREPROCESSOR_POS_TAGGER_MAX_CONTEXT_WORDS_IN_DATABASE_POS_PERMUTATION/2);
	int wSize = wMax - wMin;	//LRP_PREPROCESSOR_POS_TAGGER_MAX_CONTEXT_WORDS_IN_DATABASE_POS_PERMUTATION
	POSambiguityInfoPermutation->resize(LRP_PREPROCESSOR_POS_TAGGER_MAX_WORDS_IN_DATABASE_POS_PERMUTATION, LRP_PREPROCESSOR_POS_TAGGER_POS_AMBIGUITY_INFO_UNKNOWN);	//LRP_PREPROCESSOR_POS_TAGGER_MAX_WORDS_IN_DATABASE_POS_PERMUTATION = max size of wSize
	#endif
	int POSambiguityInfoPermutationIndex = 0;	

	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_DO_NOT_TRAIN_POS_INDEX_OUT_OF_SENTENCE_BOUNDS
	bool sequenceContainsPosIndexOutOfSentenceBounds = false;
	if((wMin < 0) || (wMax >= sentenceContents->size()))
	{
		sequenceContainsPosIndexOutOfSentenceBounds = true;
		result = false;
	}
	if(!sequenceContainsPosIndexOutOfSentenceBounds)
	{
	#endif
		for(int w=wMin; w<=wMax; w++)
		{	
			/*
			#ifdef LRP_PREPROCESSOR_POS_TAGGER_INCLUDE_CENTRE_WORD_IN_POS_PERMUTATION_OLD
			if(w == wCentre)	
			{
				(*POSambiguityInfoPermutation)[POSambiguityInfoPermutationIndex] = setPOSambiguityInfoBit(LRP_PREPROCESSOR_POS_TAGGER_POS_AMBIGUITY_INFO_UNKNOWN, LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_INPUT_CENTRE_WORD_NOTUSED, true);
				POSambiguityInfoPermutationIndex++;	//?
			}
			#else
			*/
			#ifndef LRP_PREPROCESSOR_POS_TAGGER_INCLUDE_CENTRE_WORD_IN_POS_PERMUTATION
			if(w == wCentre)	
			{
				//ignore centre word
			}
			else 
			#endif
			/*
			#endif
			*/
			if((w < 0) || (w >= sentenceContents->size()))
			{
				(*POSambiguityInfoPermutation)[POSambiguityInfoPermutationIndex] = setPOSambiguityInfoBit(LRP_PREPROCESSOR_POS_TAGGER_POS_AMBIGUITY_INFO_UNKNOWN, LRP_PREPROCESSOR_POS_TAGGER_DATABASE_POS_INDEX_OUT_OF_SENTENCE_BOUNDS, true);	
				POSambiguityInfoPermutationIndex++;					
			}
			else
			{
				LRPpreprocessorPlainTextWord* contextWord = sentenceContents->at(w);

				bool contextWordPOSisAmbiguous = false;
				uint64_t contextWordPOSambiguityInfo = LRP_PREPROCESSOR_POS_TAGGER_POS_AMBIGUITY_INFO_UNKNOWN;	//default value
				uchar contextWordUnambiguousPOSindex = INT_DEFAULT_VALUE;	//LRP_PREPROCESSOR_POS_TYPE_UNDEFINED;
				if(!determinePOSambiguityInfo(contextWord, &contextWordPOSambiguityInfo, &contextWordPOSisAmbiguous, &contextWordUnambiguousPOSindex, identifiedEveryWordInDatabasePOSpermutation))
				{
					result = false;
				}
				if(contextWordPOSisAmbiguous)
				{
					*identifiedEveryWordInDatabasePOSpermutationIsUnambiguous = false;
				}

				(*POSambiguityInfoPermutation)[POSambiguityInfoPermutationIndex] = contextWordPOSambiguityInfo;
				/*
				cout << "\t\tcontextWordLowerCase = " << contextWordLowerCase << endl;
				cout << "\t\tPOSambiguityInfoPermutationIndex = " << POSambiguityInfoPermutationIndex << endl;
				cout << "\t\tcontextWordPOSambiguityInfo = " << contextWordPOSambiguityInfo <<endl;
				*/
				POSambiguityInfoPermutationIndex++;
			}
		}
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_DO_NOT_TRAIN_POS_INDEX_OUT_OF_SENTENCE_BOUNDS
	}
	#endif
		
	return result;
}	


string LRPpreprocessorPOStaggerClass::convertBoolVectorToString(const vector<bool>* inputNeuronExperienceValuesContextWord)
{
	string boolVectorString = "";
	for(int i=0; i<inputNeuronExperienceValuesContextWord->size(); i++)
	{
		boolVectorString = boolVectorString + SHAREDvars.convertIntToString(int(inputNeuronExperienceValuesContextWord->at(i)));
	}
	return boolVectorString;
}




#endif

#ifdef LRP_PREPROCESSOR_INITIALISE_WORD_INDEX_LIST_FROM_LRP_FILES

void LRPpreprocessorPOStaggerClass::generatePOSambiguityInfoUnambiguousPermutationArray(vector<vector<uint64_t>*>* POSambiguityInfoUnambiguousPermutationArray, vector<uint64_t>* POSambiguityInfoPermutation, vector<uint64_t>* POSambiguityInfoUnambiguousPermutationLocal, int wordIndex)
{
	if(wordIndex < POSambiguityInfoPermutation->size())	//OLD: (wordIndex < LRP_PREPROCESSOR_POS_TAGGER_MAX_WORDS_IN_DATABASE_POS_PERMUTATION)
	{
		uint64_t POSambiguityInfo = (*POSambiguityInfoPermutation)[wordIndex];
		int64_t maxValForPOSambiguousEntry = int64_t(1U) << LRP_PREPROCESSOR_POS_TYPE_ARRAY_NUMBER_OF_TYPES;
		//cout << "maxValForPOSambiguousEntry = " << maxValForPOSambiguousEntry << endl;
		//cout << "POSambiguityInfo = " << POSambiguityInfo << endl;
				
		if((POSambiguityInfo > 0) && (POSambiguityInfo < maxValForPOSambiguousEntry))
		{
			//remove POSambiguityInfoUnambiguousPermutationLocal from POSambiguityInfoUnambiguousPermutationArray as it will be replicated
			vector<vector<uint64_t>*>::iterator it = find(POSambiguityInfoUnambiguousPermutationArray->begin(), POSambiguityInfoUnambiguousPermutationArray->end(), POSambiguityInfoUnambiguousPermutationLocal);
  			if(it != POSambiguityInfoUnambiguousPermutationArray->end())
  			{
				POSambiguityInfoUnambiguousPermutationArray->erase(it);
				//cout << "remove POSambiguityInfoUnambiguousPermutationLocal from POSambiguityInfoUnambiguousPermutationArray as it will be replicated" << endl;
			}
			else
			{
				cerr << "LRPpreprocessorPOStaggerClass::generatePOSambiguityInfoUnambiguousPermutationArray error{}: POSambiguityInfoUnambiguousPermutationLocal not found in POSambiguityInfoUnambiguousPermutationArray" << endl;
				exit(EXIT_ERROR);
			}
			
			for(int x=0; x<LRP_PREPROCESSOR_POS_TYPE_ARRAY_NUMBER_OF_TYPES; x++)
			{
				if(SHAREDvars.getBitValue(POSambiguityInfo, x))
				{
					//cout << "x = " << x << endl;
					vector<uint64_t>* POSambiguityInfoUnambiguousPermutationNew = new vector<uint64_t>(*POSambiguityInfoUnambiguousPermutationLocal);
					POSambiguityInfoUnambiguousPermutationArray->push_back(POSambiguityInfoUnambiguousPermutationNew);
					(*POSambiguityInfoUnambiguousPermutationNew)[wordIndex] = setPOSambiguityInfoBit(LRP_PREPROCESSOR_POS_TAGGER_POS_AMBIGUITY_INFO_UNKNOWN, x, true);	//create a new unambigious hypothetical POS value
					generatePOSambiguityInfoUnambiguousPermutationArray(POSambiguityInfoUnambiguousPermutationArray, POSambiguityInfoPermutation, POSambiguityInfoUnambiguousPermutationNew, wordIndex+1);
				}
			}
			
			delete POSambiguityInfoUnambiguousPermutationLocal;
		}
		else
		{
			//special POS type (e.g. punctuation)
			(*POSambiguityInfoUnambiguousPermutationLocal)[wordIndex] = POSambiguityInfo;
			generatePOSambiguityInfoUnambiguousPermutationArray(POSambiguityInfoUnambiguousPermutationArray, POSambiguityInfoPermutation, POSambiguityInfoUnambiguousPermutationLocal, wordIndex+1);
		}
	}
}

//#ifdef GIA_POS_REL_TRANSLATOR_RULES_GIA3
bool LRPpreprocessorPOStaggerClass::determinePOSambiguityInfoWrapper(vector<LRPpreprocessorPlainTextWord*>* sentenceContents, vector<uint64_t>* POSambiguityInfoPermutation, vector<string>* explicitWordList)
{
	bool result = true;
		
	//resetting of word->alreadyFoundMatch is required in case a higher level app (e.g. NLC) shares word objects between sentences:
	for(int w=0; w<sentenceContents->size(); w++)
	{
		LRPpreprocessorPlainTextWord* currentWord = sentenceContents->at(w);
		currentWord->alreadyFoundMatch = false;
	}

	for(int w=0; w<sentenceContents->size(); w++)
	{
		LRPpreprocessorPlainTextWord* contextWord = sentenceContents->at(w);

		#ifdef GIA_POS_REL_TRANSLATOR_RULES_CODE_COMPONENT_WORD_NOUN_VERB_VARIANT
		string wordTextLowerCase = SHAREDvars.convertStringToLowerCase(&(contextWord->tagName));
		LRPpreprocessorMultiwordReductionWord* nounBaseFound = NULL;
		int nounGrammaticalBaseTenseForm = LRP_PREPROCESSOR_WORD_NOUN_DATABASE_TAG_BASE_TENSE_FORM_UNKNOWN;
		if(LRPpreprocessorWordIdentification.determineNounPluralVariant(wordTextLowerCase, &nounBaseFound, &nounGrammaticalBaseTenseForm))
		{
			#ifdef GIA_POS_REL_TRANSLATOR_RULES_CODE_COMPONENT_WORD_NOUN_VERB_VARIANT_DETECT_IRREGULAR_NOUN_FORMS
			contextWord->wordNounVariantGrammaticalTenseForm = nounGrammaticalBaseTenseForm;	//will only be valid if wordPOStype == noun
			#else
			contextWord->wordNounVariantGrammaticalTenseForm = LRP_PREPROCESSOR_WORD_NOUN_DATABASE_TAG_BASE_TENSE_FORM_PLURAL;
			#endif
			//cout << "contextWord->wordNounVariantGrammaticalTenseForm == " << contextWord->wordNounVariantGrammaticalTenseForm << ", contextWord = " << contextWord->tagName << endl;
		}
		string verbBaseNameFound = "";
		int verbGrammaticalBaseTenseForm = LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_UNKNOWN;
		if(LRPpreprocessorWordIdentification.determineVerbCaseStandardWithAdditional(wordTextLowerCase, &verbBaseNameFound, &verbGrammaticalBaseTenseForm))
		{
			contextWord->wordVerbVariantGrammaticalTenseForm = verbGrammaticalBaseTenseForm;	//will only be valid if wordPOStype == verb
			//cout << "contextWord->wordVerbVariantGrammaticalTenseForm == " << contextWord->verbGrammaticalBaseTenseForm << ", contextWord = " << contextWord->tagName << endl;
		}			
		#endif

		#ifdef SANI_DEBUG_RULES_PRINT_PARSE_PROCESS
		cout << "contextWord = " << contextWord->tagName << endl;
		#endif
		bool contextWordPOSisAmbiguous = false;
		bool identifiedEveryWordInDatabasePOSpermutationNOTUSED = true;
		uint64_t contextWordPOSambiguityInfo = LRP_PREPROCESSOR_POS_TAGGER_POS_AMBIGUITY_INFO_UNKNOWN;	//default value
		uchar contextWordUnambiguousPOSindex = INT_DEFAULT_VALUE;	//LRP_PREPROCESSOR_POS_TYPE_UNDEFINED;
		if(!determinePOSambiguityInfo(contextWord, &contextWordPOSambiguityInfo, &contextWordPOSisAmbiguous, &contextWordUnambiguousPOSindex, &identifiedEveryWordInDatabasePOSpermutationNOTUSED))
		{
			result = false;
		}

		#ifdef SANI_ADD_EXPLICIT_WORD_REFERENCES_AS_INDEPENDENT_POS_PERMUTATIONS
		vector<string>::iterator explicitWordListIterator = find(explicitWordList->begin(), explicitWordList->end(), wordTextLowerCase);
		if(explicitWordListIterator != explicitWordList->end())
		{
			//cout << "found explicit word; wordTextLowerCase = " << wordTextLowerCase << endl;
			//add explicit case to contextWordPOSambiguityInfo
			contextWordPOSambiguityInfo = setPOSambiguityInfoBit(contextWordPOSambiguityInfo, LRP_PREPROCESSOR_POS_TYPE_EXPLICITWORDTEMP, true);	//set as explicitWordTemp
		}
		#endif
  
		POSambiguityInfoPermutation->push_back(contextWordPOSambiguityInfo);
	}
	
	return result;
}
//#endif

bool LRPpreprocessorPOStaggerClass::determinePOSambiguityInfo(constEffective LRPpreprocessorPlainTextWord* contextWord, uint64_t* contextWordPOSambiguityInfo, bool* contextWordPOSisAmbiguous, uchar* contextWordUnambiguousPOSindex, bool* identifiedEveryWordInDatabasePOSpermutation)
{
	bool result = true;
	
	int entityIndex = (static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(contextWord))->entityIndex;
	//cout << "entityIndex = " << entityIndex << endl;
	string wordText = contextWord->tagName;
	
	bool foundWordInLists = false;
	LRPpreprocessorMultiwordReductionWord* contextWordFound = NULL;
	#ifdef LRP_PREPROCESSOR_INITIALISE_WORD_INDEX_LIST_FROM_LRP_FILES_SUPPORT_UPPERCASE_PROPERNOUN_WORD_LISTS
	//CHECKTHIS: verify that currentWord->tagName is case sensitive
	//cout << "wordText = " << wordText << endl;
	if(findWordInWordListAllTypesWithPOSambiguityInfo(wordText, &contextWordFound, contextWordPOSambiguityInfo))
	{
		//cout << "1 wordText = " << wordText << endl;
		foundWordInLists = true;
	}
	#ifdef GIA_POS_REL_TRANSLATOR_RULES_TREAT_UNKNOWN_POSTYPES_MID_SENTENCE_CAPITALISED_WORDS_AS_PROPERNOUNS_METHOD1
	if((!foundWordInLists) && LRPpreprocessorWordClassObject.isMidSentenceUppercaseWordLikelyProperNoun(contextWord))
	{
		foundWordInLists = true;
		*contextWordPOSambiguityInfo = setPOSambiguityInfoBit(LRP_PREPROCESSOR_POS_TAGGER_POS_AMBIGUITY_INFO_UNKNOWN, LRP_PREPROCESSOR_POS_TYPE_PROPERNOUN_DEFAULT, true);	//set as propernoun
	}
	else
	{
	#endif
		#ifdef LRP_PREPROCESSOR_INITIALISE_WORD_INDEX_LIST_FROM_LRP_FILES_SUPPORT_UPPERCASE_PROPERNOUN_WORD_LISTS_IF_FIRST_WORD_OF_SENTENCE_IS_IN_PROPERNOUN_LIST_THEN_CHECK_OTHER_LISTS_ALSO
		if((entityIndex == LRP_NLP_START_ENTITY_INDEX) || (!foundWordInLists))
		{
			if(LRPpreprocessorWordClassObject.wordIsUpperCase(wordText))
			{
				string contextWordLowerCase = SHAREDvars.convertStringToLowerCase(&(wordText));
				uint64_t contextWordLowerCasePOSambiguityInfo = 0;
				//cout << "contextWordLowerCase = " << contextWordLowerCase << endl;
				if(findWordInWordListAllTypesWithPOSambiguityInfo(contextWordLowerCase, &contextWordFound, &contextWordLowerCasePOSambiguityInfo))
				{
					if((entityIndex == LRP_NLP_START_ENTITY_INDEX) && foundWordInLists)
					{
						//cout << "2 wordText = " << wordText << endl;
						//cout << "*contextWordPOSambiguityInfo = " << *contextWordPOSambiguityInfo << endl;
						//cout << "contextWordLowerCasePOSambiguityInfo = " << contextWordLowerCasePOSambiguityInfo << endl;
						*contextWordPOSambiguityInfo = (*contextWordPOSambiguityInfo | contextWordLowerCasePOSambiguityInfo);	//combine the potential pos types for upper case version and lower case version of word
					}
					else
					{
						*contextWordPOSambiguityInfo = contextWordLowerCasePOSambiguityInfo;
					}
					foundWordInLists = true;
				}
			}
		}
		#else
		if(!foundWordInLists)
		{
			#ifdef GIA_POS_REL_TRANSLATOR_RULES_TREAT_UNKNOWN_POSTYPES_MID_SENTENCE_CAPITALISED_WORDS_AS_PROPERNOUNS_METHOD1
			if(midSentenceUppercaseWordSetToProperNoun(wordText, contextWordPOSambiguityInfo))
			{
				foundWordInLists = true;
			}
			else
			{
			#endif
				string contextWordLowerCase = SHAREDvars.convertStringToLowerCase(&(wordText));
				//cout << "contextWordLowerCase = " << contextWordLowerCase << endl;
				if(findWordInWordListAllTypesWithPOSambiguityInfo(contextWordLowerCase, &contextWordFound, contextWordPOSambiguityInfo))
				{
					foundWordInLists = true;
				}
			#ifdef GIA_POS_REL_TRANSLATOR_RULES_TREAT_UNKNOWN_POSTYPES_MID_SENTENCE_CAPITALISED_WORDS_AS_PROPERNOUNS_METHOD1
			}
			#endif
		}
		#endif
	#ifdef GIA_POS_REL_TRANSLATOR_RULES_TREAT_UNKNOWN_POSTYPES_MID_SENTENCE_CAPITALISED_WORDS_AS_PROPERNOUNS_METHOD1
	}
	#endif
	#else
	string contextWordLowerCase = SHAREDvars.convertStringToLowerCase(&(wordText));
	if(findWordInWordListAllTypesWithPOSambiguityInfo(contextWordLowerCase, &contextWordFound, contextWordPOSambiguityInfo))
	{	
		foundWordInLists = true;
	}
	#ifdef GIA_POS_REL_TRANSLATOR_RULES_TREAT_UNKNOWN_POSTYPES_MID_SENTENCE_CAPITALISED_WORDS_AS_PROPERNOUNS_METHOD1
	if((!foundWordInLists) && LRPpreprocessorWordClassObject.isMidSentenceUppercaseWordLikelyProperNoun(contextWord))
	{
		foundWordInLists = true;
		*contextWordPOSambiguityInfo = setPOSambiguityInfoBit(LRP_PREPROCESSOR_POS_TAGGER_POS_AMBIGUITY_INFO_UNKNOWN, LRP_PREPROCESSOR_POS_TYPE_PROPERNOUN_DEFAULT, true);	//set as propernoun
	}
	#endif
	#endif

	#ifdef LRP_PREPROCESSOR_WORD_MULTIWORD_REDUCTION
	LRPpreprocessorMultiwordReductionPlainTextWord* contextWordMultiwordReductionPlainTextWord = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(contextWord);
	if(contextWordMultiwordReductionPlainTextWord->collapsedMultiwordWord || contextWordMultiwordReductionPlainTextWord->collapsedPhrasalVerbExactDefinedSection)
	{
		*contextWordUnambiguousPOSindex = contextWordMultiwordReductionPlainTextWord->collapsedMultiwordWordType;
		*contextWordPOSambiguityInfo = setPOSambiguityInfoBit(LRP_PREPROCESSOR_POS_TAGGER_POS_AMBIGUITY_INFO_UNKNOWN, *contextWordUnambiguousPOSindex, true);
		*contextWordPOSisAmbiguous = false;
		foundWordInLists = true;
	}
	#endif

	if(!foundWordInLists)
	{
		/*
		cout << "!foundWordInLists" << endl;
		cout << "wordText = " << wordText << endl;
		exit(0);
		*/
		
		*identifiedEveryWordInDatabasePOSpermutation = false;
		*contextWordPOSisAmbiguous = true;
		
		#ifdef LRP_PREPROCESSOR_DETECT_NUMBERS
		if(LRPpreprocessorWordClassObject.isStringNumber(contextWordMultiwordReductionPlainTextWord->tagName))
		{
			//cout << "isStringNumber" << endl;
			*contextWordUnambiguousPOSindex = LRP_PREPROCESSOR_POS_TYPE_NUMBER;
			*contextWordPOSambiguityInfo = setPOSambiguityInfoBit(LRP_PREPROCESSOR_POS_TAGGER_POS_AMBIGUITY_INFO_UNKNOWN, *contextWordUnambiguousPOSindex, true);
			*contextWordPOSisAmbiguous = false;	
		}
		#endif
	}
	else
	{
		*contextWordPOSisAmbiguous = LRPpreprocessorPOStaggerDatabase.determinePOSambiguityInfoIsAmbiguous(*contextWordPOSambiguityInfo, contextWordUnambiguousPOSindex, false);
	}
	
	return result;
}

		
bool LRPpreprocessorPOStaggerClass::findWordInWordListAllTypesWithPOSambiguityInfo(const string word, LRPpreprocessorMultiwordReductionWord** wordFound, uint64_t* POSambiguityInfoFound)
{	
	bool result = false;
		
	unordered_map<string,pair<LRPpreprocessorMultiwordReductionWord*, uint64_t>>* wordListAllTypesWithPOSambiguityInfo = LRPpreprocessorWordIdentification.getWordListAllTypesWithPOSambiguityInfo();

	unordered_map<string,pair<LRPpreprocessorMultiwordReductionWord*, uint64_t>>::iterator it;
	it = wordListAllTypesWithPOSambiguityInfo->find(word);
	if(it != wordListAllTypesWithPOSambiguityInfo->end())
	{
		result = true;
		*wordFound = (it->second).first;
		*POSambiguityInfoFound = (it->second).second;
	}
	
	return result;
}	


bool LRPpreprocessorPOStaggerClass::printPOSambiguityInfoPermutation(const vector<uint64_t>* POSambiguityInfoPermutation)
{
	bool result = true;
	
	for(int i=0; i<POSambiguityInfoPermutation->size(); i++)
	{
		uint64_t POSambiguityInfo = (*POSambiguityInfoPermutation)[i];	
		printPOSambiguityInfo(POSambiguityInfo);
	}
	
	return result;
}

bool LRPpreprocessorPOStaggerClass::printPOSambiguityInfo(const uint64_t POSambiguityInfo)
{
	bool result = true;

	for(int x=0; x<LRP_PREPROCESSOR_POS_TYPE_ARRAY_NUMBER_OF_TYPES; x++)
	{
		if(SHAREDvars.getBitValue(POSambiguityInfo, x))
		{
			cout << "1";
		}
		else
		{
			cout << "0";
		}
	}		
	cout << "\n" <<endl;
	
	return result;
}


bool LRPpreprocessorPOStaggerClass::printPOSambiguityInfoPermutationAssumeUnambiguous(const vector<uint64_t>* POSambiguityInfoPermutation)
{
	bool result = true;
	
	for(int i=0; i<POSambiguityInfoPermutation->size(); i++)
	{
		uint64_t POSambiguityInfo = (*POSambiguityInfoPermutation)[i];	
		uchar unambiguousPOSinfoIndex = LRP_PREPROCESSOR_POS_TYPE_UNDEFINED;
		bool wordAmbiguous = LRPpreprocessorPOStaggerDatabase.determinePOSambiguityInfoIsAmbiguous(POSambiguityInfoPermutation->at(i), &unambiguousPOSinfoIndex, false);
		cout << "LRPpreprocessorPOStypeNameArray[" << i << "] = " << LRPpreprocessorPOStypeNameArray[unambiguousPOSinfoIndex] << endl;
	}
	
	return result;
}



#endif


#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK
bool LRPpreprocessorPOStaggerClass::generateANNexperienceFromPOSambiguityInfoPermutationStandalone(const vector<uint64_t>* POSambiguityInfoPermutation, uchar outputNeuronExperienceValue, ANNexperience* currentExperience)
{
	bool result = true;
	
	//cout << "POSambiguityInfoPermutation->size() = " << POSambiguityInfoPermutation->size() << endl;
	
	int numberOfInputNeurons = LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_NUMBER_OF_INPUT_NEURONS_PER_CONTEXT_WORD*POSambiguityInfoPermutation->size();	//eg *LRP_PREPROCESSOR_POS_TAGGER_MAX_CONTEXT_WORDS_IN_DATABASE_POS_PERMUTATION
	
	vector<bool> inputNeuronExperienceValues(numberOfInputNeurons, false);
	for(int i=0; i<POSambiguityInfoPermutation->size(); i++)
	{
		int POSambiguityInfoNeuralNetworkInputNeuron = INT_DEFAULT_VALUE;
		vector<bool> inputNeuronExperienceValuesContextWord(LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_NUMBER_OF_INPUT_NEURONS_PER_CONTEXT_WORD, false);	

		int numberOfPOStypesToCheck = LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_NUMBER_OF_INPUT_NEURONS_PER_CONTEXT_WORD;
		for(int POStype = 0; POStype<numberOfPOStypesToCheck; POStype++)
		{
			bool bitValue = SHAREDvars.getBitValue((*POSambiguityInfoPermutation)[i], POStype);
			inputNeuronExperienceValuesContextWord[POStype] = bitValue;
		}

		for(int j=0; j<LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_NUMBER_OF_INPUT_NEURONS_PER_CONTEXT_WORD; j++)
		{
			inputNeuronExperienceValues[i*LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_NUMBER_OF_INPUT_NEURONS_PER_CONTEXT_WORD+j] = inputNeuronExperienceValuesContextWord[j];	
		}
	}
	
	/*
	for(int i=0; i<POSambiguityInfoPermutation.size(); i++)
	{
		cout << int((*POSambiguityInfoPermutation)[i]);
	}
	cout << endl;
	cout << "inputNeuronExperienceValues = " << convertBoolVectorToString(&inputNeuronExperienceValues) << endl;
	*/
	
	//only feed neural network with experience if centre word POS is not ambiguous
	//now create an experience and train the network with the experience
	currentExperience->classTargetValue = outputNeuronExperienceValue;
	ANNexperienceInput* currentExperienceInput = currentExperience->firstExperienceInput;
	for(int i=0; i<numberOfInputNeurons; i++)
	{
		currentExperienceInput->inputValue = inputNeuronExperienceValues[i];
		currentExperienceInput->next = new ANNexperienceInput();
		currentExperienceInput = currentExperienceInput->next;
	}
	
	return result;
}
#endif

#ifdef SANI_POS_REL_TRANSLATOR_RULES_ITERATE_OVER_UNAMBIGUOUS_POS_PERMUTATIONS_AT_START
bool LRPpreprocessorPOStaggerClass::setSentenceContentsWordsUnambiguousPOSindex(vector<LRPpreprocessorPlainTextWord*>* sentenceContents, const vector<uint64_t>* POSambiguityInfoPermutationTemp)
{
	bool result = true;
	
	for(int w=0; w<sentenceContents->size(); w++)
	{
		LRPpreprocessorPlainTextWord* contextWord = sentenceContents->at(w);
		uchar unambiguousPOSinfoIndex = LRP_PREPROCESSOR_POS_TYPE_UNDEFINED;
		bool wordAmbiguous = LRPpreprocessorPOStaggerDatabase.determinePOSambiguityInfoIsAmbiguous(POSambiguityInfoPermutationTemp->at(w), &unambiguousPOSinfoIndex, false);
		contextWord->unambiguousPOSindex = unambiguousPOSinfoIndex;
		//#ifdef SANI_DEBUG_PROPAGATE
		#ifdef SANI_DEBUG_RULES_PRINT_PARSE_PROCESS_POS_TYPES
		cout << "unambiguousPOSinfoIndex = " << int(unambiguousPOSinfoIndex) << endl;
		cout << "LRPpreprocessorPOStypeNameArray[unambiguousPOSinfoIndex] = " << LRPpreprocessorPOStypeNameArray[unambiguousPOSinfoIndex] << endl;
		#endif
	}

	#ifdef SANI_DEBUG_RULES_PRINT_SENTENCES
	LRPpreprocessorSentenceClassObject.printSentenceUnambiguousPOSindex(sentenceContents);
	#endif

	return result;
}
#endif

uint64_t LRPpreprocessorPOStaggerClass::setPOSambiguityInfoBit(uint64_t wordPOSambiguityInfo, int bitIndex, bool value)
{
	wordPOSambiguityInfo = SHAREDvars.setBitValue(wordPOSambiguityInfo, bitIndex, value);
	return wordPOSambiguityInfo;
}
bool LRPpreprocessorPOStaggerClass::getPOSambiguityInfoBit(uint64_t wordPOSambiguityInfo, int bitIndex)
{
	bool bitValue = SHAREDvars.getBitValue(wordPOSambiguityInfo, bitIndex);
	return bitValue;	
}
	

#ifdef SANI_SEQUENCE_GRAMMAR_DETERMINE_POS_AMIGUITY_INFO_AT_START
//preconditions: assumes LRPpreprocessorPOStaggerClass::determinePOSambiguityInfo has already been executed and currentWord->unambiguousPOSindex has been recorded
bool LRPpreprocessorPOStaggerClass::isWordPOSambiguous(const LRPpreprocessorPlainTextWord* currentWord)
{
	bool contextWordPOSisAmbiguous = false;
	#ifdef GIA_POS_REL_TRANSLATOR_RULES_TREAT_UNKNOWN_POSTYPES	//required
	if(currentWord->unambiguousPOSindex == LRP_PREPROCESSOR_POS_TYPE_UNDEFINED)	//the only words not to be assigned a specific contextWordUnambiguousPOSindex will contain ambiguous POSambiguityInfo
	#else
	if(LRPpreprocessorPOStaggerDatabase.determinePOSambiguityInfoIsAmbiguous(currentWord->POSambiguityInfo, currentWord->unambiguousPOSindex, false)
	#endif
	{
		contextWordPOSisAmbiguous = true;
	}
	
	#ifndef GIA_POS_REL_TRANSLATOR_RULES_TREAT_UNKNOWN_POSTYPES
	cerr << "LRPpreprocessorPOStaggerClass::isWordPOSambiguous warning: GIA_POS_REL_TRANSLATOR_RULES_TREAT_UNKNOWN_POSTYPES is currently required by !SANI_SEMANTIC_NETWORK" << endl;
	#endif
	
	return contextWordPOSisAmbiguous;
}

bool LRPpreprocessorPOStaggerClass::recordPOSambiguityInfo(LRPpreprocessorPlainTextWord* currentWord)
{
	bool result = true;
	
	bool contextWordPOSisAmbiguous = false;
	bool identifiedEveryWordInDatabasePOSpermutationNOTUSED = true;
	uint64_t contextWordPOSambiguityInfo = LRP_PREPROCESSOR_POS_TAGGER_POS_AMBIGUITY_INFO_UNKNOWN;	//default value
	uchar contextWordUnambiguousPOSindex = INT_DEFAULT_VALUE;	//LRP_PREPROCESSOR_POS_TYPE_UNDEFINED;
	if(!determinePOSambiguityInfo(currentWord, &contextWordPOSambiguityInfo, &contextWordPOSisAmbiguous, &contextWordUnambiguousPOSindex, &identifiedEveryWordInDatabasePOSpermutationNOTUSED))
	{
		result = false;
	}
	
	currentWord->POSambiguityInfo = contextWordPOSambiguityInfo;		
	if(!contextWordPOSisAmbiguous)
	{
		currentWord->unambiguousPOSindex = contextWordUnambiguousPOSindex;
	}
	
	#ifdef GIA_POS_REL_TRANSLATOR_RULES_TREAT_UNKNOWN_POSTYPES
	inferAndRecordWordPOStypeFromPOSunknown(currentWord);
	#endif
	
	return result;
}

#ifdef GIA_POS_REL_TRANSLATOR_RULES_TREAT_UNKNOWN_POSTYPES
void LRPpreprocessorPOStaggerClass::inferAndRecordWordPOStypeFromPOSunknown(LRPpreprocessorPlainTextWord* currentWord)
{
	if(currentWordPOSunknown(currentWord))
	{
		int wordPOStype = INT_DEFAULT_VALUE;
		if(getWordPOStypeFromWordPOSunknown(currentWord, &wordPOStype))
		{
			currentWord->unambiguousPOSindex = wordPOStype;
		}
	}
}
//based on SANInodes::currentWordPOSunknown, but executed higher in execution hierarchy (independent of SANI_POS_REL_TRANSLATOR_RULES_ITERATE_OVER_UNAMBIGUOUS_POS_PERMUTATIONS_AT_START)
bool LRPpreprocessorPOStaggerClass::currentWordPOSunknown(const LRPpreprocessorPlainTextWord* currentWord)
{
	bool result = false;
	
	if(currentWord->POSambiguityInfo == LRP_PREPROCESSOR_POS_TAGGER_POS_AMBIGUITY_INFO_UNKNOWN)
	{
		result = true;
	}
	
	return result;
}
#endif
#endif
	
bool LRPpreprocessorPOStaggerClass::getWordPOStypeFromWordPOSunknown(constEffective LRPpreprocessorPlainTextWord* currentWord, int* wordPOStype)
{
	bool result = false;
	
	#ifdef GIA_POS_REL_TRANSLATOR_RULES_TREAT_UNKNOWN_POSTYPES_MID_SENTENCE_CAPITALISED_WORDS_AS_PROPERNOUNS_METHOD2
	if(LRPpreprocessorWordClassObject.isMidSentenceUppercaseWordLikelyProperNoun(currentWord))
	{	
		*wordPOStype = LRP_PREPROCESSOR_POS_TYPE_PROPERNOUN_DEFAULT;
		result = true;
	}
	else
	{
	#endif
		#ifdef GIA_POS_REL_TRANSLATOR_RULES_TREAT_UNKNOWN_POSTYPES_AS_NOUNS
		*wordPOStype = LRP_PREPROCESSOR_POS_TYPE_NOUN;
		result = true;		
		#endif
	#ifdef GIA_POS_REL_TRANSLATOR_RULES_TREAT_UNKNOWN_POSTYPES_MID_SENTENCE_CAPITALISED_WORDS_AS_PROPERNOUNS_METHOD2
	}
	#endif
	
	return result;
}
