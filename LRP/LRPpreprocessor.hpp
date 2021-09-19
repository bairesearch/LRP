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
 * File Name: LRPpreprocessor.hpp
 * Author: Richard Bruce Baxter - Copyright (c) 2005-2021 Baxter AI (baxterai.com)
 * Project: Language Reduction Preprocessor
 * Project Version: 3q2a 19-September-2021
 * Requirements: requires plain text file
 * Description: Preprocessor
 * /
 *******************************************************************************/


#ifndef HEADER_LRP_PREPROCESSOR
#define HEADER_LRP_PREPROCESSOR

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

#include "XMLparserClass.hpp"
#ifdef LRP_PREPROCESSOR_POS_TAGGER
#include "LRPpreprocessorPOStagger.hpp"
#include "LRPpreprocessorPOStaggerDatabase.hpp"
#endif
#ifdef GIA_POS_REL_TRANSLATOR_HYBRID
#include "GIAposRelTranslatorHybrid.hpp"
#endif



class LRPpreprocessorClass
{
	private: XMLparserClassClass XMLparserClass;
	private: SHAREDvarsClass SHAREDvars;
	private: LRPpreprocessorWordClassClass LRPpreprocessorWordClassObject;
	private: LRPpreprocessorWordIdentificationClass LRPpreprocessorWordIdentification;
	#ifdef LRP_PREPROCESSOR_WORD_MULTIWORD_REDUCTION
	private: LRPpreprocessorWordReductionClass LRPpreprocessorWordReduction;
	#endif
	private: LRPpreprocessorSentenceClass LRPpreprocessorSentenceClassObject;
	#ifdef GIA_POS_REL_TRANSLATOR_HYBRID
	private: GIAposRelTranslatorHybridClass GIAposRelTranslatorHybrid;
	#endif
	#ifdef LRP_PREPROCESSOR_POS_TAGGER
	private: LRPpreprocessorPOStaggerClass LRPpreprocessorPOStagger;
	private: LRPpreprocessorPOStaggerDatabaseClass LRPpreprocessorPOStaggerDatabase;
	#endif
	#ifdef LRP_PREPROCESSOR
	public: bool preprocessTextForGIAwrapper(const bool useLRP, string* inputTextPlainTXTfileName, const string outputLRPTextPlainTXTFileName, bool isQuery, LRPtranslatorVariablesClass* translatorVariables, bool* useInputTextPlainTXTFile, const string inputTextNLPfeatureXMLfileName);
		public: bool preprocessTextForGIA(string* inputTextPlainTXTfileName, const string outputLRPTextPlainTXTFileName, bool isQuery, LRPtranslatorVariablesClass* translatorVariables, const string inputTextNLPfeatureXMLfileName);
			#ifdef GIA_POS_REL_TRANSLATOR_HYBRID
			bool preprocessSentencesForGIAwrapper(LRPtranslatorVariablesClass* translatorVariables, const string outputLRPTextPlainTXTFileName, const string inputTextNLPfeatureXMLfileName, string outputFileName, string outputFileNameLRPforNLP, bool isQuery);
			#endif
		public: bool regenerateFileFromPreprocessedTextWithoutLRP(string* inputTextPlainTXTfileName, const string outputLRPTextPlainTXTFileName, const LRPtranslatorVariablesClass* translatorVariables);
			public: bool createPreprocessSentencesForGIA(const string inputFileName, LRPtranslatorVariablesClass* translatorVariables);
				public: bool createPreprocessSentences(string fileContents, LRPpreprocessorSentence* firstLRPpreprocessorSentenceInList, const bool interpretNewLinesAsNewSentences, const bool splitMultisentenceLines);
					#ifdef LRP_PREPROCESSOR_WORD_EXTRACT_INDENTATION
					private: bool extractIndentationFromCurrentLine(string* lineContents, int* indexOfCurrentToken, int* currentIndentation, string* indentationContents);
					public: string generateIndentationContents(const int currentIndentation);
					#endif
				private: void copySentenceContentsPreprocessor(LRPpreprocessorMultiwordReductionPlainTextWord* sentenceContents1FirstWord, LRPpreprocessorMultiwordReductionPlainTextWord* sentenceContents2firstWord);	
	#endif
		
};

#endif

