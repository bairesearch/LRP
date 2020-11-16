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
 * File Name: LRPpreprocessorWordReduction.hpp
 * Author: Richard Bruce Baxter - Copyright (c) 2005-2020 Baxter AI (baxterai.com)
 * Project: Language Reduction Preprocessor
 * Project Version: 3o3a 16-November-2020
 * Requirements: requires plain text file
 * Description: Preprocessor Word Reduction
 * /
 *******************************************************************************/


#ifndef HEADER_LRP_PREPROCESSOR_WORD_REDUCTION
#define HEADER_LRP_PREPROCESSOR_WORD_REDUCTION

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
#include "SHAREDvars.hpp"	//file io




class LRPpreprocessorWordReductionClass
{
	private: SHAREDvarsClass SHAREDvars;
	private: LRPpreprocessorWordClassClass LRPpreprocessorWordClassObject;
	private: LRPpreprocessorSentenceClass LRPpreprocessorSentenceClassObject;

	#ifdef LRP_PREPROCESSOR_WORD_MULTIWORD_REDUCTION

	public: bool initialiseLRPreduction(unordered_map<string,LRPpreprocessorMultiwordReductionIrregularVerbSentence*>* irregularVerbList);
		private: bool loadPhrasalVerbDataAndGenerateAllTenseVariants(string phrasalVerbDatabaseFileName, multimap<string, LRPpreprocessorMultiwordReductionPhrasalVerbSentence*>* phrasalVerbList, unordered_map<string,LRPpreprocessorMultiwordReductionIrregularVerbSentence*>* irregularVerbList);
	
	/*
	public: multimap<string, LRPpreprocessorMultiwordReductionPhrasalVerbSentence*>* getPhrasalVerbListGlobal();
	public: unordered_map<string,multimap<string, LRPpreprocessorMultiwordReductionBasicSentence*>*>* getMultiwordListsGlobal();
	*/
	
	public: LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo* getActiveLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo();
	public: void setActiveLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo(bool isQuery);
	public: void initialiseActiveLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo(bool isQuery);
	public: void deinitialiseActiveLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo(bool isQuery);
	
	public: bool parseTextFileAndReduceLanguage(LRPpreprocessorSentence* firstLRPpreprocessorSentenceInList, string plainTextLRPoutputFileName, string plainTextLRPforNLPoutputFileName);
		//public: bool loadPlainTextFile(string plainTextInputFileName, LRPpreprocessorMultiwordReductionSentence* firstTagInPlainText);
		private: bool searchAndReplacePhrasalVerbs(LRPpreprocessorSentence* firstLRPpreprocessorSentenceInList, multimap<string, LRPpreprocessorMultiwordReductionPhrasalVerbSentence*>* phrasalVerbList, LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo* firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo);
		private: bool loadMultiwordWordList(string multiwordWordListFileName, multimap<string, LRPpreprocessorMultiwordReductionBasicSentence*>* multiwordWordList);
		private: bool searchAndReplaceMultiwordWordList(LRPpreprocessorSentence* firstLRPpreprocessorSentenceInList, multimap<string, LRPpreprocessorMultiwordReductionBasicSentence*>* multiwordWordList, LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo* firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo, int wordListType);
		#ifdef LRP_PREPROCESSOR_WORD_DYNAMIC
		private: bool searchAndReplaceMultiwordWordListDynamic(LRPpreprocessorSentence* firstLRPpreprocessorSentenceInList, unordered_map<string,LRPpreprocessorMultiwordReductionWord*>* wordList, LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo* firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo, int wordListType);
		#endif		
			private: void createNewCorrespondenceInfo(LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo* firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo, LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo** currentCorrespondenceInfo, LRPpreprocessorSentence* currentLRPpreprocessorSentenceInList, LRPpreprocessorMultiwordReductionPlainTextWord* firstTagInPlainTextSentence, LRPpreprocessorMultiwordReductionPlainTextWord** currentTagInPlainTextSentence, int entityIndex, int numberWordsInMultiwordMatched);
			private: void renumberEntityIndiciesInCorrespondenceInfo(LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo* firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo, int sentenceIndex, int entityIndex, int numberWordsInMultiwordMatched);
		public: bool writeTagListToFile(LRPpreprocessorSentence* firstLRPpreprocessorSentenceInList, string plainTextLRPoutputFileName, string plainTextLRPforNLPoutputFileName, bool performLRPoutput, bool performLRPforNLPoutput);
			private: string generateWordWithLRPforNLPonly(LRPpreprocessorMultiwordReductionPlainTextWord* currentTagInPlainTextSentence);
	#ifdef LRP_PREPROCESSOR_WORD_REPLACE_OUTPUT_FOR_NLP_TEMPORARILY
	public: void revertNLPtagNameToOfficialLRPtagName(GIAfeature* feature, GIAsentence* currentSentenceInList, GIArelation* currentRelationInListForPrepositionsOnly, bool isPreposition, bool* foundOfficialLRPreplacementString);
	#endif
	#endif
	
	public: bool generateIrregularTenseVariantsOfVerbBase(LRPpreprocessorMultiwordReductionWord* baseTag, unordered_map<string,LRPpreprocessorMultiwordReductionIrregularVerbSentence*>* irregularVerbList, bool grammaticallyStrict);	
	public: bool generateStandardTenseVariantsOfVerbBase(LRPpreprocessorMultiwordReductionWord* baseTag, bool irregularVerbFound, bool grammaticallyStrict);
	public: bool findSentenceInSentenceListIrregularVerb(unordered_map<string,LRPpreprocessorMultiwordReductionIrregularVerbSentence*>* sentenceList, string word, LRPpreprocessorMultiwordReductionIrregularVerbSentence** sentenceFound);

	#ifdef LRP_PREPROCESSOR_WORD_COLLAPSE_NUMERICAL_WORDS_TO_NUMBERS
	private: bool replaceAllNumericalWordsWithNumbers(LRPpreprocessorSentence* firstLRPpreprocessorSentenceInList);
		private: int64_t convertWordToNumber(vector<LRPpreprocessorPlainTextWord*>* numericalWordList);
			private: int64_t parseNumerals(vector<LRPpreprocessorPlainTextWord*>* numericalWordListSubset);
				private: int64_t getValueOf(string wordText);
	#endif
			
};


#endif
