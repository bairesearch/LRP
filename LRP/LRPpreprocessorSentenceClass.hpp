 /*******************************************************************************
 *
 * License: GNU Affero General Public License version 3 (AGPLv3)
 *
 *******************************************************************************/

/*******************************************************************************
 *
 * File Name: LRPpreprocessorSentenceClass.hpp
 * Author: Richard Bruce Baxter - Copyright (c) 2005-2022 Baxter AI (baxterai.com)
 * Project: Language Reduction Preprocessor
 * Project Version: 3r1a 05-June-2022
 * Requirements: requires plain text file
 * Description: Preprocessor Sentence Class
 * /
 *******************************************************************************/


#ifndef HEADER_LRP_PREPROCESSOR_SENTENCE_CLASS
#define HEADER_LRP_PREPROCESSOR_SENTENCE_CLASS

#include "SHAREDglobalDefs.hpp"
#include "LRPglobalDefs.hpp"
#ifdef USE_SANI
#include "SANIglobalDefs.hpp"
#endif
#ifdef USE_GIA
#include "GIAglobalDefs.hpp"
#endif

#include "SHAREDvars.hpp"
#ifdef USE_GIA
#include "GIAentityNodeClass.hpp"	//required for primaryEntityTemp, LRP_PREPROCESSOR_RECORD_REFERENCES
#include "GIAsentenceClass.hpp"	//required for LRP_PREPROCESSOR_RECORD_REFERENCES
#endif
#include "LRPpreprocessorWordClass.hpp"
#ifdef GIA_POS_REL_TRANSLATOR_HYBRID
#include "GIAposRelTranslatorHybridSentenceClass.hpp"
#endif
#ifdef USE_SANI	//GIA_POS_REL_TRANSLATOR_RULES
#include "SANInodesGroupClass.hpp"
#endif


class LRPpreprocessorSentence
{
public:
	LRPpreprocessorSentence(void);
	~LRPpreprocessorSentence(void);

	int sentenceIndexOriginal;
	string sentenceContentsOriginalText;
	//#ifdef LRP_PREPROCESSOR_RECORD_REFERENCES
	vector<LRPpreprocessorPlainTextWord*> sentenceContentsOriginal;
	vector<LRPpreprocessorPlainTextWord*> sentenceContentsLRP;
	//#endif
	
	#ifdef GIA_POS_REL_TRANSLATOR_HYBRID
	bool hasLogicReference;	//if false, then firstLogicReferenceInList will only have a single GIAposRelTranslatorHybridLogicReference
	GIAposRelTranslatorHybridLogicReference* firstLogicReferenceInList;
	//int logicReferenceTotal;
	#ifdef LRP_PREPROCESSOR_ASSIGN_UNIQUE_SENTENCE_INDICES_FOR_SENTENCES
	int sentenceIndex;
	#endif
	#ifdef GIA_POS_REL_TRANSLATOR_HYBRID_EXECUTE_PRELIM_POS_TAGGER
	GIAsentence* sentenceReferencePrelim;		//temporary GIA sentence data (POS tags) required for execution of GIA preprocessor
	#endif
	#else
	#ifdef LRP_PREPROCESSOR_RECORD_REFERENCES
	GIAsentence* sentenceReference;
	#endif
	#endif
	#ifdef USE_SANI	//GIA_POS_REL_TRANSLATOR_RULES
	SANIGroupParseTree* firstParseTreeGroup;
	#endif
	#ifdef LRP_PREPROCESSOR_WORD_EXTRACT_INDENTATION
	int indentation;
	#endif
	
	#ifdef GIA_POS_REL_TRANSLATOR_RULES_PARSE_ISOLATED_SUBREFERENCE_SETS_OPTIMISED
	bool parseIsolatedSubreferenceSets;
	bool parseIsolatedSubreferenceSetsOnly;
	bool parseIsolatedSubreferenceSetsFirst;
	#endif

	LRPpreprocessorSentence* next;
};

class LRPtranslatorVariablesClass
{
public:

	LRPtranslatorVariablesClass(void);
	~LRPtranslatorVariablesClass(void);
	
	LRPpreprocessorSentence* firstLRPpreprocessorSentenceInList;
};


class LRPpreprocessorSentenceClass
{
	private: LRPpreprocessorWordClassClass LRPpreprocessorWordClassObject;

	public: void copyPreprocessorSentence(LRPpreprocessorSentence* sentenceToCopy, LRPpreprocessorSentence* newSentence);
	public: vector<LRPpreprocessorPlainTextWord*>* getSentenceContents(LRPpreprocessorSentence* currentLRPpreprocessorSentenceInList);
	#ifdef GIA_POS_REL_TRANSLATOR_RULES_PARSE_ISOLATED_SUBREFERENCE_SETS_OPTIMISED
	public: bool calculateParseIsolatedSubreferenceSets1(const LRPpreprocessorSentence* currentLRPpreprocessorSentenceInList, bool* parseIsolatedSubreferenceSets1);
	public: bool calculateParseIsolatedSubreferenceSets2(const LRPpreprocessorSentence* currentLRPpreprocessorSentenceInList, bool* parseIsolatedSubreferenceSets2);
	#endif
	
	public: void printSentence(vector<LRPpreprocessorPlainTextWord*>* sentenceContents, const bool error);
	#ifdef SANI_POS_REL_TRANSLATOR_RULES_ITERATE_OVER_UNAMBIGUOUS_POS_PERMUTATIONS_AT_START
	public: void printSentenceUnambiguousPOSindex(vector<LRPpreprocessorPlainTextWord*>* sentenceContents);
	#endif

};



#endif
