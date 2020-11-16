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
 * Project Version: 3o3a 16-November-2020
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
#ifdef GIA_POS_REL_TRANSLATOR_HYBRID_REFERENCE_SET_PREFERENCE_NLP_PRELIM_POS_TAGS_OVER_LRP_WORD_TYPE_LISTS
	#define LRP_USE_POS_PRELIM_DEFAULT_VALUE (true)
#else
	#define LRP_USE_POS_PRELIM_DEFAULT_VALUE (false)	//CHECKTHIS
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

	private: unordered_map<string,LRPpreprocessorMultiwordReductionWord*>* getWordList(int GIAposType);
		private: unordered_map<string,LRPpreprocessorMultiwordReductionWord*>* getWordListIfExistent(int GIAposType, bool* result);
	private: bool wordListExistent(int GIAposType);
	private: bool transferWordList(int GIAposType1, int GIAposType2, unordered_map<string,LRPpreprocessorMultiwordReductionWord*>** wordList1record);
	
	#ifdef LRP_PREPROCESSOR_INITIALISE_WORD_INDEX_LIST_FROM_LRP_FILES
	public: unordered_map<string,pair<LRPpreprocessorMultiwordReductionWord*,uint64_t>>* getWordListAllTypesWithPOSambiguityInfo();
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FEED_ALL_PERMUTATIONS_INDIVIDUALLY_ASSIGN_WEIGHTS_TO_TRAINED_POS_TYPES
	public: double getLRPpreprocessorPOStypeWeight(int GIAposType);
	#endif
	//private: bool findInstanceInMapWordListAllTypesWithPOSambiguityInfo(unordered_map<string,pair<LRPpreprocessorMultiwordReductionWord*, uchar>>* mapWordListAllTypesWithPOSambiguityInfo, string wordIndex, LRPpreprocessorMultiwordReductionWord* word, uchar* POSambiguityInfo);
	private: void insertInstanceInMapWordListAllTypesWithPOSambiguityInfo(unordered_map<string,pair<LRPpreprocessorMultiwordReductionWord*, uint64_t>>* mapWordListAllTypesWithPOSambiguityInfo, string wordIndex, LRPpreprocessorMultiwordReductionWord* word, uint64_t POSambiguityInfo);
	#endif
	
	public: bool initialiseLRP(string newLRPDataFolderName, bool newUseLRP);
	public: bool getUseLRP();
		private: bool loadWordListWrapper(bool* wordListLoaded, string wordListFileName, unordered_map<string,LRPpreprocessorMultiwordReductionWord*>* wordList);
			private: bool loadWordList(string wordListFileName, unordered_map<string,LRPpreprocessorMultiwordReductionWord*>* wordList);
		private: bool loadStructuredDataListIrregularVerb(string irregularVerbListFileName, unordered_map<string,LRPpreprocessorMultiwordReductionIrregularVerbSentence*>* irregularVerbList);

	#ifdef LRP_PREPROCESSOR_DERIVE_NOUN_VARIANTS
	private: bool generateNounPluralVariantsList(unordered_map<string,LRPpreprocessorMultiwordReductionBasicSentence*>* irregularNounList);
		private: bool generateNounPluralVariants(LRPpreprocessorMultiwordReductionWord* wordTag, unordered_map<string,LRPpreprocessorMultiwordReductionWord*>* nounPluralVariantsList, unordered_map<string,LRPpreprocessorMultiwordReductionBasicSentence*>* irregularNounList);
	#endif
	
	#ifdef LRP_PREPROCESSOR_WORD_LOAD_WORD_LISTS
	public: bool generateVerbCaseStandardAndAdditionalList(bool grammaticallyStrict);
	#endif	
		#ifdef LRP_PREPROCESSOR_WORD_LOAD_WORD_LISTS
		private: bool generateAdditionalTenseVariantsOfVerbBase(unordered_map<string,LRPpreprocessorMultiwordReductionWord*>* verbCaseAdditionalList, LRPpreprocessorMultiwordReductionWord* baseTag, bool irregularVerbFound, bool grammaticallyStrict);
			private: void addVerbCaseAdditional(LRPpreprocessorMultiwordReductionWord* currentTagInVerbList, unordered_map<string,LRPpreprocessorMultiwordReductionWord*>* verbCaseAdditionalList, string baseTenseFormStart, string baseTenseFormAppend, int grammaticalTenseFormNew);
		#endif
		
	#ifdef GIA_TRANSLATOR_CORRECT_IRREGULAR_VERB_LEMMAS
	#ifdef GIA_TRANSLATOR_CORRECT_IRREGULAR_VERB_LEMMAS_LIBERAL
	public: bool determineVerbCaseAdditionalWrapper(string word, string* baseNameFound, int* grammaticalBaseTenseForm);
	#endif
	#ifdef GIA_TRANSLATOR_CORRECT_IRREGULAR_VERB_LEMMAS_CONSERVATIVE
	public: bool determineIfWordIsIrregularVerbContinuousCaseWrapper(string word, string* baseNameFound);
		private: bool determineIfWordIsIrregularVerbContinuousCase(string word, unordered_map<string,LRPpreprocessorMultiwordReductionIrregularVerbSentence*>* irregularVerbList, string* baseNameFound);
	#endif
	#endif

	#ifdef LRP_PREPROCESSOR_WORD_LOAD_INVERSE_PREPOSITIONS_LIST
	private: bool loadStructuredDataList(string structuredDataListFileName, unordered_map<string,LRPpreprocessorMultiwordReductionBasicSentence*>* structuredDataList);
	#endif
	#ifdef LRP_PREPROCESSOR_WORD_NORMALISE_PREPOSITIONS
	public: void detectIfInverseOrTwoWayConditionRequired(string conditionName, bool* inverseConditionRequired, bool* twoWayConditionRequired, string* inverseConditionName);
	#ifdef LRP_PREPROCESSOR_WORD_DETECT_PREPOSITION_TYPE
	public: bool identifyConditionType(GIAentityNode* conditionRelationshipEntity);
	#endif
	#endif

	public: bool checkGrammaticalWordTypeFeaturePrelim(LRPpreprocessorPlainTextWord* wordTag, int GIAposType);

	public: bool determineIsWordType(LRPpreprocessorPlainTextWord* wordTag, bool grammaticallyStrict, int GIAposType);
		public: bool determineIsWordType(LRPpreprocessorPlainTextWord* wordTag, bool usePOSprelim, bool grammaticallyStrict, int GIAposType);
			public: bool determineIsWordType(LRPpreprocessorPlainTextWord* wordTag, bool grammaticallyStrict, int GIAposType, string* baseNameFound, int* grammaticalBaseTenseForm);
				public: bool determineIsWordType(LRPpreprocessorPlainTextWord* wordTag, bool usePOSprelim, bool grammaticallyStrict, int GIAposType, string* baseNameFound, int* grammaticalBaseTenseForm);
					public: bool determineIsWordType(string word, unordered_map<string,LRPpreprocessorMultiwordReductionWord*>* wordTypeList);
	public: bool determineIsVerb(LRPpreprocessorPlainTextWord* wordTag, bool grammaticallyStrict);
		public: bool determineIsVerb(LRPpreprocessorPlainTextWord* wordTag, bool usePOSprelim, bool grammaticallyStrict);	
			public: bool determineIsVerb(LRPpreprocessorPlainTextWord* wordTag, bool grammaticallyStrict, string* baseNameFound, int* grammaticalBaseTenseForm);
				public: bool determineIsVerb(LRPpreprocessorPlainTextWord* wordTag, bool usePOSprelim, bool grammaticallyStrict, string* baseNameFound, int* grammaticalBaseTenseForm);
				public: bool determineIsVerbString(string word);
					public: bool determineVerbCaseStandardWithAdditional(string word, string* baseNameFound, int* grammaticalBaseTenseForm);
						public: bool determineVerbCaseStandard(string word, string* baseNameFound, int* grammaticalBaseTenseForm);
						public: bool determineVerbCaseAdditional(string word, string* baseNameFound, int* grammaticalBaseTenseForm);
						//public: bool convertVerbCaseGrammaticalTenseFormToTenseModifier(int grammaticalTenseForm, int* grammaticalTenseModifier);
						public: bool verbCaseDetectGrammaticallyStrictVariant(int grammaticalTenseForm);
	public: bool determineIsPreposition(LRPpreprocessorPlainTextWord* wordTag);
		public: bool determineIsPreposition(LRPpreprocessorPlainTextWord* wordTag, bool usePOSprelim);
			public: bool determineIsPrepositionString(string word);
	public: bool determineIsAdverb(LRPpreprocessorPlainTextWord* wordTag);
		public: bool determineIsAdverb(LRPpreprocessorPlainTextWord* wordTag, bool usePOSprelim);
			public: bool determineIsAdverbString(string word);
	public: bool determineIsAdjective(LRPpreprocessorPlainTextWord* wordTag);
		public: bool determineIsAdjective(LRPpreprocessorPlainTextWord* wordTag, bool usePOSprelim);
			public: bool determineIsAdjectiveString(string word);
	public: bool determineIsNoun(LRPpreprocessorPlainTextWord* wordTag);
		public: bool determineIsNoun(LRPpreprocessorPlainTextWord* wordTag, bool usePOSprelim);
			public: bool determineIsNoun(LRPpreprocessorPlainTextWord* wordTag, string* baseNameFound, int* grammaticalBaseForm);
				public: bool determineIsNoun(LRPpreprocessorPlainTextWord* wordTag, bool usePOSprelim, string* baseNameFound, int* grammaticalBaseForm);
					public: bool determineIsNounString(string word);
					public: bool determineNounPluralVariant(string word, LRPpreprocessorMultiwordReductionWord** nounBaseFound, int* grammaticalBaseTenseForm);
	public: bool determineIsConjunction(LRPpreprocessorPlainTextWord* wordTag);
		public: bool determineIsConjunction(LRPpreprocessorPlainTextWord* wordTag, bool usePOSprelim);
			//public: bool determineIsConjunctionString(string word);
	public: bool determineIsDeterminer(LRPpreprocessorPlainTextWord* wordTag);
		public: bool determineIsDeterminer(LRPpreprocessorPlainTextWord* wordTag, bool usePOSprelim);
			//public: bool determineIsDeterminerString(string word);
	public: bool determineIsAuxiliaryBeing(LRPpreprocessorPlainTextWord* wordTag);
		public: bool determineIsAuxiliaryBeing(LRPpreprocessorPlainTextWord* wordTag, bool usePOSprelim);
	public: bool determineIsAuxiliaryHaving(LRPpreprocessorPlainTextWord* wordTag);
		public: bool determineIsAuxiliaryHaving(LRPpreprocessorPlainTextWord* wordTag, bool usePOSprelim);
	public: bool determineIsAuxiliaryDoing(LRPpreprocessorPlainTextWord* wordTag);		
		public: bool determineIsAuxiliaryDoing(LRPpreprocessorPlainTextWord* wordTag, bool usePOSprelim);
	public: bool detectAuxiliary(LRPpreprocessorPlainTextWord* wordTag);
		public: bool detectAuxiliary(LRPpreprocessorPlainTextWord* wordTag, bool usePOSprelim);
	public: bool detectModalAuxiliary(LRPpreprocessorPlainTextWord* wordTag);
		public: bool detectModalAuxiliary(LRPpreprocessorPlainTextWord* wordTag, bool usePOSprelim);
	public: bool detectRcmodSameReferenceSetDelimiter(LRPpreprocessorPlainTextWord* wordTag);
		public: bool detectRcmodSameReferenceSetDelimiter(LRPpreprocessorPlainTextWord* wordTag, bool usePOSprelim);
		#ifdef SANI_PARSE_SIMULTANEOUS_SET_WORD_POSTYPE_INFERRED_DYNAMIC_OLD
		public: bool determineIsWordTypeStringBasic(string word, int GIAposType);
		#endif
	
	public: bool findWordInWordList(unordered_map<string,LRPpreprocessorMultiwordReductionWord*>* wordList, string word);
		public: bool findWordInWordList(unordered_map<string,LRPpreprocessorMultiwordReductionWord*>* wordList, string word, LRPpreprocessorMultiwordReductionWord** wordFound);
	public: bool findSentenceInSentenceListBasic(unordered_map<string,LRPpreprocessorMultiwordReductionBasicSentence*>* sentenceList, string word, LRPpreprocessorMultiwordReductionBasicSentence** sentenceFound);
	#ifdef LRP_PREPROCESSOR_INITIALISE_WORD_INDEX_LIST_FROM_LRP_FILES
	public: bool createWordIndexListFromLRPfiles();
	#endif
			
};


#endif
