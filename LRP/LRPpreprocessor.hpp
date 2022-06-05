 /*******************************************************************************
 *
 * License: GNU Affero General Public License version 3 (AGPLv3)
 *
 *******************************************************************************/

/*******************************************************************************
 *
 * File Name: LRPpreprocessor.hpp
 * Author: Richard Bruce Baxter - Copyright (c) 2005-2022 Baxter AI (baxterai.com)
 * Project: Language Reduction Preprocessor
 * Project Version: 3r1a 05-June-2022
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

