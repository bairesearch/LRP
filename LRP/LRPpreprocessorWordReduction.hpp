 /*******************************************************************************
 *
 * License: GNU Affero General Public License version 3 (AGPLv3)
 *
 *******************************************************************************/

/*******************************************************************************
 *
 * File Name: LRPpreprocessorWordReduction.hpp
 * Author: Richard Bruce Baxter - Copyright (c) 2005-2022 Baxter AI (baxterai.com)
 * Project: Language Reduction Preprocessor
 * Project Version: 3r1a 05-June-2022
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
		private: bool loadPhrasalVerbDataAndGenerateAllTenseVariants(const string phrasalVerbDatabaseFileName, multimap<string, LRPpreprocessorMultiwordReductionPhrasalVerbSentence*>* phrasalVerbList, unordered_map<string,LRPpreprocessorMultiwordReductionIrregularVerbSentence*>* irregularVerbList);
	
	/*
	public: multimap<string, LRPpreprocessorMultiwordReductionPhrasalVerbSentence*>* getPhrasalVerbListGlobal();
	public: unordered_map<string,multimap<string, LRPpreprocessorMultiwordReductionBasicSentence*>*>* getMultiwordListsGlobal();
	*/
	
	public: LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo* getActiveLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo();
	public: void setActiveLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo(const bool isQuery);
	public: void initialiseActiveLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo(const bool isQuery);
	public: void deinitialiseActiveLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo(const bool isQuery);
	
	public: bool parseTextFileAndReduceLanguage(LRPpreprocessorSentence* firstLRPpreprocessorSentenceInList, const string plainTextLRPoutputFileName, const string plainTextLRPforNLPoutputFileName);
		//public: bool loadPlainTextFile(string plainTextInputFileName, LRPpreprocessorMultiwordReductionSentence* firstTagInPlainText);
		private: bool searchAndReplacePhrasalVerbs(LRPpreprocessorSentence* firstLRPpreprocessorSentenceInList, multimap<string, LRPpreprocessorMultiwordReductionPhrasalVerbSentence*>* phrasalVerbList, LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo* firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo);
		private: bool loadMultiwordWordList(const string multiwordWordListFileName, multimap<string, LRPpreprocessorMultiwordReductionBasicSentence*>* multiwordWordList);
		private: bool searchAndReplaceMultiwordWordList(LRPpreprocessorSentence* firstLRPpreprocessorSentenceInList, multimap<string, LRPpreprocessorMultiwordReductionBasicSentence*>* multiwordWordList, LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo* firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo, const int wordListType);
		#ifdef LRP_PREPROCESSOR_WORD_DYNAMIC
		private: bool searchAndReplaceMultiwordWordListDynamic(LRPpreprocessorSentence* firstLRPpreprocessorSentenceInList, unordered_map<string,LRPpreprocessorMultiwordReductionWord*>* wordList, LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo* firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo, const int wordListType);
		#endif		
			private: void createNewCorrespondenceInfo(LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo* firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo, LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo** currentCorrespondenceInfo, const LRPpreprocessorSentence* currentLRPpreprocessorSentenceInList, LRPpreprocessorMultiwordReductionPlainTextWord* firstTagInPlainTextSentence, constEffective LRPpreprocessorMultiwordReductionPlainTextWord** currentTagInPlainTextSentence, const int entityIndex, int numberWordsInMultiwordMatched);
			private: void renumberEntityIndiciesInCorrespondenceInfo(LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo* firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo, const int sentenceIndex, const int entityIndex, int numberWordsInMultiwordMatched);
		public: bool writeTagListToFile(LRPpreprocessorSentence* firstLRPpreprocessorSentenceInList, const string plainTextLRPoutputFileName, const string plainTextLRPforNLPoutputFileName, const bool performLRPoutput, const bool performLRPforNLPoutput);
			private: string generateWordWithLRPforNLPonly(const LRPpreprocessorMultiwordReductionPlainTextWord* currentTagInPlainTextSentence);
	#ifdef LRP_PREPROCESSOR_WORD_REPLACE_OUTPUT_FOR_NLP_TEMPORARILY
	public: void revertNLPtagNameToOfficialLRPtagName(GIAfeature* feature, const GIAsentence* currentSentenceInList, const GIArelation* currentRelationInListForPrepositionsOnly, const bool isPreposition, bool* foundOfficialLRPreplacementString);
	#endif
	#endif
	
	public: bool generateIrregularTenseVariantsOfVerbBase(LRPpreprocessorMultiwordReductionWord* baseTag, unordered_map<string,LRPpreprocessorMultiwordReductionIrregularVerbSentence*>* irregularVerbList, const bool grammaticallyStrict);	
	public: bool generateStandardTenseVariantsOfVerbBase(LRPpreprocessorMultiwordReductionWord* baseTag, const bool irregularVerbFound, const bool grammaticallyStrict);
	public: bool findSentenceInSentenceListIrregularVerb(unordered_map<string,LRPpreprocessorMultiwordReductionIrregularVerbSentence*>* sentenceList, const string word, LRPpreprocessorMultiwordReductionIrregularVerbSentence** sentenceFound);

	#ifdef LRP_PREPROCESSOR_WORD_COLLAPSE_NUMERICAL_WORDS_TO_NUMBERS
	private: bool replaceAllNumericalWordsWithNumbers(LRPpreprocessorSentence* firstLRPpreprocessorSentenceInList);
		private: int64_t convertWordToNumber(vector<LRPpreprocessorPlainTextWord*>* numericalWordList);
			private: int64_t parseNumerals(const vector<LRPpreprocessorPlainTextWord*>* numericalWordListSubset);
				private: int64_t getValueOf(const string wordText);
	#endif
			
};


#endif
