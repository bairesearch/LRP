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


#ifndef HEADER_LRP_PREPROCESSOR_POS_TAGGER
#define HEADER_LRP_PREPROCESSOR_POS_TAGGER

#include "SHAREDglobalDefs.hpp"
#include "LRPglobalDefs.hpp"
#ifdef USE_SANI
#include "SANIglobalDefs.hpp"
#endif
#ifdef USE_GIA
#include "GIAglobalDefs.hpp"
#endif

#include "LRPpreprocessorSentenceClass.hpp"
#include "LRPpreprocessorWordClass.hpp"
#include "LRPpreprocessorWordIdentification.hpp"
#ifdef LRP_PREPROCESSOR_WORD_MULTIWORD_REDUCTION
#include "LRPpreprocessorWordReduction.hpp" 
#endif
#ifdef USE_GIA
//#include "GIAtranslatorGrammar.hpp"
#endif
#include "LRPpreprocessorPOStaggerDatabase.hpp"
#ifdef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_NEURAL_NETWORK_SEQUENCE_GRAMMAR
#include "SANIpropagateCompactGenerate.hpp"
#include "SANIrules.hpp"
#endif
#ifdef USE_ANN
#include "ANNexperienceClass.hpp"
#endif	
		


//static int GIAPOStaggerValueCrossReferenceGrammaticalWordTypeArray[LRP_PREPROCESSOR_WORD_LIST_ARRAY_SIZE] = {GRAMMATICAL_WORD_TYPE_VERB, GRAMMATICAL_WORD_TYPE_PREP, GRAMMATICAL_WORD_TYPE_ADV, GRAMMATICAL_WORD_TYPE_ADJ, GRAMMATICAL_WORD_TYPE_NOUN, GRAMMATICAL_WORD_TYPE_CONJUNCTION, GRAMMATICAL_WORD_TYPE_DETERMINER, GRAMMATICAL_WORD_TYPE_PRONOUN};

class LRPpreprocessorPOStaggerClass
{
	private: SHAREDvarsClass SHAREDvars;
	private: LRPpreprocessorSentenceClass LRPpreprocessorSentenceClassObject;
	private: LRPpreprocessorWordClassClass LRPpreprocessorWordClassObject;
	private: LRPpreprocessorWordIdentificationClass LRPpreprocessorWordIdentification;
	#ifdef LRP_PREPROCESSOR_WORD_MULTIWORD_REDUCTION
	private: LRPpreprocessorWordReductionClass LRPpreprocessorWordReduction;
	#endif
	private: LRPpreprocessorPOStaggerDatabaseClass LRPpreprocessorPOStaggerDatabase;
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_NEURAL_NETWORK_SEQUENCE_GRAMMAR
	private: SANIpropagateCompactGenerateClass SANIpropagateCompactGenerate;
	private: SANIrulesClass SANIrules;
	#endif
		
	#ifdef LRP_PREPROCESSOR_POS_TAGGER
	
	#ifdef GIA_POS_REL_TRANSLATOR_HYBRID_EXECUTE_PRELIM_POS_TAGGER
	public: bool executePrelimFeatureProcessingOnSentences(const string outputLRPTextPlainTXTFileName, const string inputTextNLPfeatureXMLfileName, LRPtranslatorVariablesClass* translatorVariables);
		private: bool predictPOStaggerDatabaseEntry(uint64_t centreWordPOSambiguityInfo, const vector<uint64_t>* POSambiguityInfoPermutation, uchar centreWordUnambiguousPOSindex, LRPpreprocessorPlainTextWord* centreWord, bool* foundMatchingCentreWordPOSambiguityInfo, uchar* centreWordPOSindexPrediction, bool* centreWordPOSisAmbiguous, double* experienceBackPropagationPassError, int* maximumNumberOfInstances);
	#endif
	
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_INITIALISE_WORD_INDEX_LIST_FROM_WIKI_DUMP_TEXT
	public: bool createWordIndexListFromWikiDumpText(const bool useLRP);
	#endif	
	#ifndef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_TRAIN_SINGLE_BATCH_ONLY
	public: bool generatePOStaggerDatabaseFromWikiDumpText(string wikiDumpFolderName, const bool useLRP);
	#endif
	public: bool generatePOStaggerDatabaseFromWikiDumpText(string wikiDumpFolderName, int wikiDumpFileBatchIndex, const bool useLRP);
		#ifdef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE_WITHOUT_TARGET_CLASSES
		private: bool addPOStaggerDatabaseEntry1(const vector<uint64_t>* POSambiguityInfoPermutation, ofstream* XtrainBatchFileObject);
		#else
		#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL
		#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_MEMORY_FREE_WRITE_EXPERIENCES_DIRECTLY_TO_FILE
		private: bool addPOStaggerDatabaseEntry2(uint64_t centreWordPOSambiguityInfo, vector<uint64_t>* POSambiguityInfoPermutation, uchar centreWordUnambiguousPOSindex, ofstream* XtrainBatchFileObject, ofstream* YtrainBatchFileObject);
		#else
		private: bool addPOStaggerDatabaseEntry3(uint64_t centreWordPOSambiguityInfo, vector<uint64_t>* POSambiguityInfoPermutation, uchar centreWordUnambiguousPOSindex, ANNexperience** currentExperienceInList);
		#endif
		#else
		private: bool addPOStaggerDatabaseEntry4(uint64_t centreWordPOSambiguityInfo, vector<uint64_t>* POSambiguityInfoPermutation, uchar centreWordUnambiguousPOSindex, int64_t* numberOfNeuralNetworkFeeds);
		#endif
		#endif
			//private: bool addPOStaggerDatabaseEntry(uint64_t centreWordPOSambiguityInfo, vector<uint64_t>* POSambiguityInfoPermutation, uchar centreWordUnambiguousPOSindex, ofstream* XtrainBatchFileObject, ofstream* YtrainBatchFileObject, ANNexperience** currentExperienceInList, int64_t* numberOfNeuralNetworkFeeds);

		private: string generateWikiDumpTextInputFileName(const int wikiDumpFileBatchIndex);
		private: bool generatePreprocessorSentenceObjectsFromText(const string inputFileName, LRPpreprocessorSentence* firstLRPpreprocessorSentenceInText, const bool useLRP);
			private: bool createPreprocessSentencesBasic(string fileContents, LRPpreprocessorSentence* firstLRPpreprocessorSentenceInList, const bool interpretNewLinesAsNewSentences);
	#endif
		#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK
		public: bool generateANNexperienceFromPOSambiguityInfoPermutation(const vector<uint64_t>* POSambiguityInfoPermutation, uchar centreWordUnambiguousPOSindex, ANNexperience* currentExperience);
		#endif
		public: int convertGIAPOStaggerValueToGrammaticalWordType(const int POSvalue);
		public: bool generatePOSambiguityInfoPermutation(vector<LRPpreprocessorPlainTextWord*>* sentenceContents, const int wCentre, bool* identifiedEveryWordInDatabasePOSpermutation, bool* identifiedEveryWordInDatabasePOSpermutationIsUnambiguous, vector<uint64_t>* POSambiguityInfoPermutation);

	private: string convertBoolVectorToString(const vector<bool>* inputNeuronExperienceValuesContextWord);

	#endif
	#ifdef LRP_PREPROCESSOR_INITIALISE_WORD_INDEX_LIST_FROM_LRP_FILES
	public: void generatePOSambiguityInfoUnambiguousPermutationArray(vector<vector<uint64_t>*>* POSambiguityInfoUnambiguousPermutationArray, vector<uint64_t>* POSambiguityInfoPermutation, vector<uint64_t>* POSambiguityInfoUnambiguousPermutationLocal, int wordIndex);
	//#ifdef GIA_POS_REL_TRANSLATOR_RULES_GIA3
	public: bool determinePOSambiguityInfoWrapper(vector<LRPpreprocessorPlainTextWord*>* sentenceContents, vector<uint64_t>* POSambiguityInfoPermutation, vector<string>* explicitWordList);
	//#endif	
		public: bool determinePOSambiguityInfo(constEffective LRPpreprocessorPlainTextWord* contextWord, uint64_t* contextWordPOSambiguityInfo, bool* contextWordPOSisAmbiguous, uchar* contextWordUnambiguousPOSindex, bool* identifiedEveryWordInDatabasePOSpermutation);
			public: bool findWordInWordListAllTypesWithPOSambiguityInfo(const string word, LRPpreprocessorMultiwordReductionWord** wordFound, uint64_t* POSambiguityInfoFound);

	public: bool printPOSambiguityInfoPermutation(const vector<uint64_t>* POSambiguityInfoPermutation);
		public: bool printPOSambiguityInfo(const uint64_t POSambiguityInfo);
	public: bool printPOSambiguityInfoPermutationAssumeUnambiguous(const vector<uint64_t>* POSambiguityInfoPermutation);
	#endif

	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK
	public: bool generateANNexperienceFromPOSambiguityInfoPermutationStandalone(const vector<uint64_t>* POSambiguityInfoPermutation, uchar outputNeuronExperienceValue, ANNexperience* currentExperience);
	#endif
	
	#ifdef SANI_POS_REL_TRANSLATOR_RULES_ITERATE_OVER_UNAMBIGUOUS_POS_PERMUTATIONS_AT_START
	public: bool setSentenceContentsWordsUnambiguousPOSindex(vector<LRPpreprocessorPlainTextWord*>* sentenceContents, const vector<uint64_t>* POSambiguityInfoPermutationTemp);
	#endif
	
	public: uint64_t setPOSambiguityInfoBit(uint64_t wordPOSambiguityInfo, int bitIndex, bool value);
	public: bool getPOSambiguityInfoBit(uint64_t wordPOSambiguityInfo, int bitIndex);
	
	#ifdef SANI_SEQUENCE_GRAMMAR_DETERMINE_POS_AMIGUITY_INFO_AT_START
	public: bool isWordPOSambiguous(const LRPpreprocessorPlainTextWord* currentWord);
	public: bool recordPOSambiguityInfo(LRPpreprocessorPlainTextWord* currentWord);
		#ifdef GIA_POS_REL_TRANSLATOR_RULES_TREAT_UNKNOWN_POSTYPES
		public: void inferAndRecordWordPOStypeFromPOSunknown(LRPpreprocessorPlainTextWord* currentWord);
			public: bool currentWordPOSunknown(const LRPpreprocessorPlainTextWord* currentWord);
		#endif
	#endif
	public: bool getWordPOStypeFromWordPOSunknown(constEffective LRPpreprocessorPlainTextWord* currentWord, int* wordPOStype);

};

#endif

