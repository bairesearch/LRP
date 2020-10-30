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
 * File Name: LRPpreprocessorWordIdentification.hpp
 * Author: Richard Bruce Baxter - Copyright (c) 2005-2020 Baxter AI (baxterai.com)
 * Project: Language Reduction Preprocessor
 * Project Version: 3n3a 29-October-2020
 * Requirements: requires plain text file
 * Description: Preprocessor Word Identification
 * /
 *******************************************************************************/


#ifndef HEADER_LRP_PREPROCESSOR_WORD_IDENTIFICATION
#define HEADER_LRP_PREPROCESSOR_WORD_IDENTIFICATION

#include "SHAREDglobalDefs.hpp"
#include "LRPglobalDefs.hpp"
#ifdef USE_SANI
#include "SANIglobalDefs.hpp"
#endif
#ifdef USE_GIA
#include "GIAglobalDefs.hpp"
#endif

#ifdef USE_GIA
#include "GIAsentenceClass.hpp"
#endif
#include "LRPpreprocessorSentenceClass.hpp"
#include "LRPpreprocessorWordClass.hpp" 
#ifdef LRP_PREPROCESSOR_WORD_MULTIWORD_REDUCTION
#include "LRPpreprocessorWordReduction.hpp" 
#endif
#include "SHAREDvars.hpp"	//file io





class LRPpreprocessorWordIdentificationClass
{
	private: SHAREDvarsClass SHAREDvars;
	private: LRPpreprocessorWordClassClass LRPpreprocessorWordClassObject;
	#ifdef LRP_PREPROCESSOR_WORD_MULTIWORD_REDUCTION
	private: LRPpreprocessorWordReductionClass LRPpreprocessorWordReduction;
	#endif

	private: unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* getWordList(const int GIAposType);
		private: unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* getWordListIfExistent(const int GIAposType, bool* result);
	private: bool wordListExistent(int GIAposType);
	private: bool transferWordList(int GIAposType1, int GIAposType2, unordered_map<string, LRPpreprocessorMultiwordReductionWord*>** wordList1record);
	
	#ifdef LRP_PREPROCESSOR_INITIALISE_WORD_INDEX_LIST_FROM_LRP_FILES
	public: unordered_map<string, pair<LRPpreprocessorMultiwordReductionWord*, uint64_t>>* getWordListAllTypesWithPOSambiguityInfo();
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FEED_ALL_PERMUTATIONS_INDIVIDUALLY_ASSIGN_WEIGHTS_TO_TRAINED_POS_TYPES
	public: double getLRPpreprocessorPOStypeWeight(int GIAposType);
	#endif
	//private: bool findInstanceInMapWordListAllTypesWithPOSambiguityInfo(unordered_map<string, pair<LRPpreprocessorMultiwordReductionWord*, unsigned char>>* mapWordListAllTypesWithPOSambiguityInfo, const string wordIndex, LRPpreprocessorMultiwordReductionWord* word, unsigned char* POSambiguityInfo);
	private: void insertInstanceInMapWordListAllTypesWithPOSambiguityInfo(unordered_map<string, pair<LRPpreprocessorMultiwordReductionWord*, uint64_t>>* mapWordListAllTypesWithPOSambiguityInfo, const string wordIndex, LRPpreprocessorMultiwordReductionWord* word, const uint64_t POSambiguityInfo);
	#endif
	
	public: bool initialiseLRP(const string newLRPDataFolderName, const bool newUseLRP);
	public: bool getUseLRP();
		private: bool loadWordListWrapper(bool* wordListLoaded, const string wordListFileName, unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* wordList);
			private: bool loadWordList(const string wordListFileName, unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* wordList);
		private: bool loadStructuredDataListIrregularVerb(const string irregularVerbListFileName, unordered_map<string, LRPpreprocessorMultiwordReductionIrregularVerbSentence*>* irregularVerbList);

	#ifdef LRP_PREPROCESSOR_DERIVE_NOUN_VARIANTS
	private: bool generateNounPluralVariantsList(unordered_map<string, LRPpreprocessorMultiwordReductionBasicSentence*>* irregularNounList);
		private: bool generateNounPluralVariants(LRPpreprocessorMultiwordReductionWord* wordTag, unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* nounPluralVariantsList, unordered_map<string, LRPpreprocessorMultiwordReductionBasicSentence*>* irregularNounList);
	#endif
	
	#ifdef LRP_PREPROCESSOR_WORD_LOAD_WORD_LISTS
	public: bool generateVerbCaseStandardAndAdditionalList(bool grammaticallyStrict);
	#endif	
		#ifdef LRP_PREPROCESSOR_WORD_LOAD_WORD_LISTS
		private: bool generateAdditionalTenseVariantsOfVerbBase(unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* verbCaseAdditionalList, LRPpreprocessorMultiwordReductionWord* baseTag, bool irregularVerbFound, bool grammaticallyStrict);
			private: void addVerbCaseAdditional(LRPpreprocessorMultiwordReductionWord* currentTagInVerbList, unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* verbCaseAdditionalList, const string baseTenseFormStart, string baseTenseFormAppend, int grammaticalTenseFormNew);
		#endif
		
	#ifdef GIA_TRANSLATOR_CORRECT_IRREGULAR_VERB_LEMMAS
	#ifdef GIA_TRANSLATOR_CORRECT_IRREGULAR_VERB_LEMMAS_LIBERAL
	public: bool determineVerbCaseAdditionalWrapper(const string word, string* baseNameFound, int* grammaticalBaseTenseForm);
	#endif
	#ifdef GIA_TRANSLATOR_CORRECT_IRREGULAR_VERB_LEMMAS_CONSERVATIVE
	public: bool determineIfWordIsIrregularVerbContinuousCaseWrapper(const string word, string* baseNameFound);
		private: bool determineIfWordIsIrregularVerbContinuousCase(const string word, unordered_map<string, LRPpreprocessorMultiwordReductionIrregularVerbSentence*>* irregularVerbList, string* baseNameFound);
	#endif
	#endif

	#ifdef LRP_PREPROCESSOR_WORD_LOAD_INVERSE_PREPOSITIONS_LIST
	private: bool loadStructuredDataList(const string prepositionsInverseListFileName, unordered_map<string, LRPpreprocessorMultiwordReductionBasicSentence*>* prepositionInverseList);
	#endif
	#ifdef LRP_PREPROCESSOR_WORD_NORMALISE_PREPOSITIONS
	public: void detectIfInverseOrTwoWayConditionRequired(const string conditionName, bool* inverseConditionRequired, bool* twoWayConditionRequired, string* inverseConditionName);
	#ifdef LRP_PREPROCESSOR_WORD_DETECT_PREPOSITION_TYPE
	public: bool identifyConditionType(GIAentityNode* conditionRelationshipEntity);
	#endif
	#endif

	public: bool checkGrammaticalWordTypeFeaturePrelim(LRPpreprocessorPlainTextWord* wordTag, const int grammaticalWordType);

	public: bool determineIsWordType(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim, const bool grammaticallyStrict, const int grammaticalWordType);
		public: bool determineIsWordType(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim, const bool grammaticallyStrict, const int grammaticalWordType, string* baseNameFound, int* grammaticalBaseTenseForm);
			public: bool determineIsWordType(const string word, unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* wordTypeList);
	public: bool determineIsVerb(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim, const bool grammaticallyStrict);	
		public: bool determineIsVerb(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim, const bool grammaticallyStrict, string* baseNameFound, int* grammaticalBaseTenseForm);
		public: bool determineIsVerbString(const string word);
			public: bool determineVerbCaseStandardWithAdditional(const string word, string* baseNameFound, int* grammaticalBaseTenseForm);
				public: bool determineVerbCaseStandard(const string word, string* baseNameFound, int* grammaticalBaseTenseForm);
				public: bool determineVerbCaseAdditional(const string word, string* baseNameFound, int* grammaticalBaseTenseForm);
				//public: bool convertVerbCaseGrammaticalTenseFormToTenseModifier(const int grammaticalTenseForm, int* grammaticalTenseModifier);
				public: bool verbCaseDetectGrammaticallyStrictVariant(const int grammaticalTenseForm);
	public: bool determineIsPreposition(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim);
		public: bool determineIsPrepositionString(const string word);
	public: bool determineIsAdverb(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim);
		public: bool determineIsAdverbString(const string word);
	public: bool determineIsAdjective(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim);
		public: bool determineIsAdjectiveString(const string word);
	public: bool determineIsNoun(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim);
		public: bool determineIsNoun(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim, string* baseNameFound, int* grammaticalBaseForm);
			public: bool determineIsNounString(const string word);
			public: bool determineNounPluralVariant(const string word, LRPpreprocessorMultiwordReductionWord** nounBaseFound, int* grammaticalBaseTenseForm);
	public: bool determineIsConjunction(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim);
		//public: bool determineIsConjunctionString(const string word);
	public: bool determineIsDeterminer(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim);
		//public: bool determineIsDeterminerString(const string word);
	public: bool determineIsAuxiliaryBeing(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim);
	public: bool determineIsAuxiliaryHaving(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim);
	public: bool determineIsAuxiliaryDoing(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim);
	public: bool detectAuxiliary(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim);
	public: bool detectModalAuxiliary(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim);
	public: bool detectRcmodSameReferenceSetDelimiter(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim);
	#ifdef SANI_PARSE_SIMULTANEOUS_SET_WORD_POSTYPE_INFERRED_DYNAMIC_OLD
	public: bool determineIsWordTypeStringBasic(const string word, const int GIAposType);
	#endif
	
	public: bool findWordInWordList(unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* wordList, const string word);
		public: bool findWordInWordList(unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* wordList, const string word, LRPpreprocessorMultiwordReductionWord** wordFound);
	public: bool findSentenceInSentenceListBasic(unordered_map<string, LRPpreprocessorMultiwordReductionBasicSentence*>* sentenceList, const string word, LRPpreprocessorMultiwordReductionBasicSentence** sentenceFound);
	#ifdef LRP_PREPROCESSOR_INITIALISE_WORD_INDEX_LIST_FROM_LRP_FILES
	public: bool createWordIndexListFromLRPfiles();
	#endif
			
};


#endif
