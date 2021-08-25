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
 * Author: Richard Bruce Baxter - Copyright (c) 2005-2021 Baxter AI (baxterai.com)
 * Project: Language Reduction Preprocessor
 * Project Version: 3q1a 25-August-2021
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

	private: unordered_map<string,LRPpreprocessorMultiwordReductionWord*>* getWordList(const int GIAposType);
		private: unordered_map<string,LRPpreprocessorMultiwordReductionWord*>* getWordListIfExistent(const int GIAposType, bool* result);
	private: bool wordListExistent(const int GIAposType);
	private: bool transferWordList(const int GIAposType1, const int GIAposType2, unordered_map<string,LRPpreprocessorMultiwordReductionWord*>** wordList1record);
	
	#ifdef LRP_PREPROCESSOR_INITIALISE_WORD_INDEX_LIST_FROM_LRP_FILES
	public: unordered_map<string,pair<LRPpreprocessorMultiwordReductionWord*,uint64_t>>* getWordListAllTypesWithPOSambiguityInfo();
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FEED_ALL_PERMUTATIONS_INDIVIDUALLY_ASSIGN_WEIGHTS_TO_TRAINED_POS_TYPES
	public: double getLRPpreprocessorPOStypeWeight(const int GIAposType);
	#endif
	//private: bool findInstanceInMapWordListAllTypesWithPOSambiguityInfo(unordered_map<string,pair<LRPpreprocessorMultiwordReductionWord*, uchar>>* mapWordListAllTypesWithPOSambiguityInfo, string wordIndex, LRPpreprocessorMultiwordReductionWord* word, uchar* POSambiguityInfo);
	private: void insertInstanceInMapWordListAllTypesWithPOSambiguityInfo(unordered_map<string,pair<LRPpreprocessorMultiwordReductionWord*, uint64_t>>* mapWordListAllTypesWithPOSambiguityInfo, string wordIndex, constEffective LRPpreprocessorMultiwordReductionWord* word, constEffective uint64_t POSambiguityInfo);
	#endif
	
	public: bool initialiseLRP(const string newLRPDataFolderName, const bool newUseLRP);
	public: bool getUseLRP();
		private: bool loadWordListWrapper(bool* wordListLoaded, const string wordListFileName, unordered_map<string,LRPpreprocessorMultiwordReductionWord*>* wordList);
			private: bool loadWordList(const string wordListFileName, unordered_map<string,LRPpreprocessorMultiwordReductionWord*>* wordList);
		private: bool loadStructuredDataListIrregularVerb(const string irregularVerbListFileName, unordered_map<string,LRPpreprocessorMultiwordReductionIrregularVerbSentence*>* irregularVerbList);

	#ifdef LRP_PREPROCESSOR_DERIVE_NOUN_VARIANTS
	private: bool generateNounPluralVariantsList(unordered_map<string,LRPpreprocessorMultiwordReductionBasicSentence*>* irregularNounList);
		private: bool generateNounPluralVariants(LRPpreprocessorMultiwordReductionWord* wordTag, unordered_map<string,LRPpreprocessorMultiwordReductionWord*>* nounPluralVariantsList, unordered_map<string,LRPpreprocessorMultiwordReductionBasicSentence*>* irregularNounList);
	#endif
	
	#ifdef LRP_PREPROCESSOR_WORD_LOAD_WORD_LISTS
	public: bool generateVerbCaseStandardAndAdditionalList(bool grammaticallyStrict);
	#endif	
		#ifdef LRP_PREPROCESSOR_WORD_LOAD_WORD_LISTS
		private: bool generateAdditionalTenseVariantsOfVerbBase(unordered_map<string,LRPpreprocessorMultiwordReductionWord*>* verbCaseAdditionalList, const LRPpreprocessorMultiwordReductionWord* baseTag, const bool irregularVerbFound, const bool grammaticallyStrict);
			private: void addVerbCaseAdditional(const LRPpreprocessorMultiwordReductionWord* currentTagInVerbList, unordered_map<string,LRPpreprocessorMultiwordReductionWord*>* verbCaseAdditionalList, const string baseTenseFormStart, const string baseTenseFormAppend, const int grammaticalTenseFormNew);
		#endif
		
	#ifdef GIA_TRANSLATOR_CORRECT_IRREGULAR_VERB_LEMMAS
	#ifdef GIA_TRANSLATOR_CORRECT_IRREGULAR_VERB_LEMMAS_LIBERAL
	public: bool determineVerbCaseAdditionalWrapper(const string word, string* baseNameFound, int* grammaticalBaseTenseForm);
	#endif
	#ifdef GIA_TRANSLATOR_CORRECT_IRREGULAR_VERB_LEMMAS_CONSERVATIVE
	public: bool determineIfWordIsIrregularVerbContinuousCaseWrapper(const string word, string* baseNameFound);
		private: bool determineIfWordIsIrregularVerbContinuousCase(const string word, const unordered_map<string,LRPpreprocessorMultiwordReductionIrregularVerbSentence*>* irregularVerbList, string* baseNameFound);
	#endif
	#endif

	#ifdef LRP_PREPROCESSOR_WORD_LOAD_INVERSE_PREPOSITIONS_LIST
	private: bool loadStructuredDataList(const string structuredDataListFileName, unordered_map<string,LRPpreprocessorMultiwordReductionBasicSentence*>* structuredDataList);
	#endif
	#ifdef LRP_PREPROCESSOR_WORD_NORMALISE_PREPOSITIONS
	public: void detectIfInverseOrTwoWayConditionRequired(const string conditionName, bool* inverseConditionRequired, bool* twoWayConditionRequired, string* inverseConditionName);
	#ifdef LRP_PREPROCESSOR_WORD_DETECT_PREPOSITION_TYPE
	public: bool identifyConditionType(GIAentityNode* conditionRelationshipEntity);
	#endif
	#endif

	public: bool checkGrammaticalWordTypeFeaturePrelim(const LRPpreprocessorPlainTextWord* wordTag, const int GIAposType);

	public: bool determineIsWordType(constEffective LRPpreprocessorPlainTextWord* wordTag, const bool grammaticallyStrict, const int GIAposType);
		public: bool determineIsWordType(constEffective LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim, const bool grammaticallyStrict, const int GIAposType);
			public: bool determineIsWordType(constEffective LRPpreprocessorPlainTextWord* wordTag, const bool grammaticallyStrict, const int GIAposType, string* baseNameFound, int* grammaticalBaseTenseForm);
				public: bool determineIsWordType(constEffective LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim, const bool grammaticallyStrict, const int GIAposType, string* baseNameFound, int* grammaticalBaseTenseForm);
					public: bool determineIsWordType(const string word, unordered_map<string,LRPpreprocessorMultiwordReductionWord*>* wordTypeList);
	public: bool determineIsVerb(constEffective LRPpreprocessorPlainTextWord* wordTag, const bool grammaticallyStrict);
		public: bool determineIsVerb(constEffective LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim, const bool grammaticallyStrict);	
			public: bool determineIsVerb(constEffective LRPpreprocessorPlainTextWord* wordTag, const bool grammaticallyStrict, string* baseNameFound, int* grammaticalBaseTenseForm);
				public: bool determineIsVerb(constEffective LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim, const bool grammaticallyStrict, string* baseNameFound, int* grammaticalBaseTenseForm);
				public: bool determineIsVerbString(const string word);
					public: bool determineVerbCaseStandardWithAdditional(const string word, string* baseNameFound, int* grammaticalBaseTenseForm);
						public: bool determineVerbCaseStandard(const string word, string* baseNameFound, int* grammaticalBaseTenseForm);
						public: bool determineVerbCaseAdditional(const string word, string* baseNameFound, int* grammaticalBaseTenseForm);
						//public: bool convertVerbCaseGrammaticalTenseFormToTenseModifier(int grammaticalTenseForm, int* grammaticalTenseModifier);
						public: bool verbCaseDetectGrammaticallyStrictVariant(const int grammaticalTenseForm);
	public: bool determineIsPreposition(constEffective LRPpreprocessorPlainTextWord* wordTag);
		public: bool determineIsPreposition(constEffective LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim);
			public: bool determineIsPrepositionString(const string word);
	public: bool determineIsAdverb(constEffective LRPpreprocessorPlainTextWord* wordTag);
		public: bool determineIsAdverb(constEffective LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim);
			public: bool determineIsAdverbString(const string word);
	public: bool determineIsAdjective(constEffective LRPpreprocessorPlainTextWord* wordTag);
		public: bool determineIsAdjective(constEffective LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim);
			public: bool determineIsAdjectiveString(const string word);
	public: bool determineIsNoun(constEffective LRPpreprocessorPlainTextWord* wordTag);
		public: bool determineIsNoun(constEffective LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim);
			public: bool determineIsNoun(constEffective LRPpreprocessorPlainTextWord* wordTag, string* baseNameFound, int* grammaticalBaseForm);
				public: bool determineIsNoun(constEffective LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim, string* baseNameFound, int* grammaticalBaseForm);
					public: bool determineIsNounString(const string word);
					public: bool determineNounPluralVariant(const string word, constEffective LRPpreprocessorMultiwordReductionWord** nounBaseFound, int* grammaticalBaseTenseForm);
	public: bool determineIsConjunction(constEffective LRPpreprocessorPlainTextWord* wordTag);
		public: bool determineIsConjunction(constEffective LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim);
			//public: bool determineIsConjunctionString(string word);
	public: bool determineIsDeterminer(constEffective LRPpreprocessorPlainTextWord* wordTag);
		public: bool determineIsDeterminer(constEffective LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim);
			//public: bool determineIsDeterminerString(string word);
	
	public: bool determineIsAuxiliaryBeing(constEffective LRPpreprocessorPlainTextWord* wordTag);
		public: bool determineIsAuxiliaryBeing(constEffective LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim);
	public: bool determineIsAuxiliaryHaving(constEffective LRPpreprocessorPlainTextWord* wordTag);
		public: bool determineIsAuxiliaryHaving(constEffective LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim);
	public: bool determineIsAuxiliaryDoing(constEffective LRPpreprocessorPlainTextWord* wordTag);		
		public: bool determineIsAuxiliaryDoing(constEffective LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim);
	
	public: bool detectAuxiliary(constEffective LRPpreprocessorPlainTextWord* wordTag);
		public: bool detectAuxiliary(constEffective LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim);
	public: bool detectModalAuxiliary(constEffective LRPpreprocessorPlainTextWord* wordTag);
		public: bool detectModalAuxiliary(constEffective LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim);
	public: bool detectRcmodSameReferenceSetDelimiter(constEffective LRPpreprocessorPlainTextWord* wordTag);
		public: bool detectRcmodSameReferenceSetDelimiter(constEffective LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim);
		#ifdef SANI_PARSE_SIMULTANEOUS_SET_WORD_POSTYPE_INFERRED_DYNAMIC_OLD
		public: bool determineIsWordTypeStringBasic(const string word, const int GIAposType);
		#endif
	
	/*	
	public: bool determineIsPossessiveEnding(constEffective LRPpreprocessorPlainTextWord* wordTag);		
		public: bool determineIsPossessiveEnding(constEffective LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim);
	*/
	public: bool determineIsPredeterminer(constEffective LRPpreprocessorPlainTextWord* wordTag);
		public: bool determineIsPredeterminer(constEffective LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim);
	public: bool determineIsPropernoun(constEffective LRPpreprocessorPlainTextWord* wordTag);
		public: bool determineIsPropernoun(constEffective LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim);

	
	public: bool determineIsDeterminerDefinite(const string currentWordText);
	public: bool determineIsDeterminerIndefinite(const string currentWordText);
	
	#ifdef LRP_PREPROCESSOR_INITIALISE_WORD_INDEX_LIST_FROM_LRP_FILES
	#ifdef GIA_POS_REL_TRANSLATOR_RULES_TREAT_UNKNOWN_POSTYPES_MID_SENTENCE_CAPITALISED_WORDS_AS_PROPERNOUNS
	public: bool determineIsLikelyPropernoun(constEffective LRPpreprocessorPlainTextWord* contextWord);
	#endif
		public: bool isFirstSentenceWordLikelyPropernoun(constEffective LRPpreprocessorPlainTextWord* contextWord);
		public: bool isNounLikelyPlural(constEffective LRPpreprocessorPlainTextWord* nounWord);
	#endif
	
	public: bool findWordInWordList(unordered_map<string,LRPpreprocessorMultiwordReductionWord*>* wordList, const string word);
		public: bool findWordInWordList(unordered_map<string,LRPpreprocessorMultiwordReductionWord*>* wordList, const string word, LRPpreprocessorMultiwordReductionWord** wordFound);
	public: bool findSentenceInSentenceListBasic(unordered_map<string,LRPpreprocessorMultiwordReductionBasicSentence*>* sentenceList, const string word, LRPpreprocessorMultiwordReductionBasicSentence** sentenceFound);
	#ifdef LRP_PREPROCESSOR_INITIALISE_WORD_INDEX_LIST_FROM_LRP_FILES
	public: bool createWordIndexListFromLRPfiles();
	#endif
			
};


#endif
