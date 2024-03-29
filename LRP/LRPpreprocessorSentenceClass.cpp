 /*******************************************************************************
 *
 * License: GNU Affero General Public License version 3 (AGPLv3)
 *
 *******************************************************************************/

/*******************************************************************************
 *
 * File Name: LRPpreprocessorSentenceClass.cpp
 * Author: Richard Bruce Baxter - Copyright (c) 2005-2022 Baxter AI (baxterai.com)
 * Project: Language Reduction Preprocessor
 * Project Version: 3r1a 05-June-2022
 * Requirements: requires plain text file
 * Description: Preprocessor Sentence Class
 * /
 *******************************************************************************/


#include "LRPpreprocessorSentenceClass.hpp"
#ifdef USE_GIA
#include "GIAentityNodeClass.hpp"
#include "GIAentityConnectionClass.hpp"
#endif

LRPpreprocessorSentence::LRPpreprocessorSentence(void)
{
	sentenceIndexOriginal = 0;	
	sentenceContentsOriginalText = "";
	//#ifdef LRP_PREPROCESSOR_RECORD_REFERENCES
	//sentenceContentsOriginal = "";
	//sentenceContentsLRP = "";
	//#endif
	
	#ifdef GIA_POS_REL_TRANSLATOR_HYBRID
	hasLogicReference = false;
	firstLogicReferenceInList = new GIAposRelTranslatorHybridLogicReference();
	//logicReferenceTotal = 0;
	#ifdef LRP_PREPROCESSOR_ASSIGN_UNIQUE_SENTENCE_INDICES_FOR_SENTENCES
	sentenceIndex = INT_DEFAULT_VALUE;
	#endif
	#ifdef GIA_POS_REL_TRANSLATOR_HYBRID_EXECUTE_PRELIM_POS_TAGGER
	sentenceReferencePrelim = NULL;
	#endif	
	#else
	#ifdef LRP_PREPROCESSOR_RECORD_REFERENCES
	sentenceReference = NULL;
	#endif	
	#endif
	#ifdef USE_SANI	//GIA_POS_REL_TRANSLATOR_RULES
	firstParseTreeGroup = NULL;
	#endif
	#ifdef LRP_PREPROCESSOR_WORD_EXTRACT_INDENTATION
	indentation = 0;
	#endif		

	#ifdef GIA_POS_REL_TRANSLATOR_RULES_PARSE_ISOLATED_SUBREFERENCE_SETS_OPTIMISED
	parseIsolatedSubreferenceSets = false;
	parseIsolatedSubreferenceSetsOnly = false;
	parseIsolatedSubreferenceSetsFirst = false;
	#endif	
	
	next = NULL;
}
LRPpreprocessorSentence::~LRPpreprocessorSentence(void)
{
	/*
	//doesn't work because word->next pointers in words will have changed based on lrp
	if(sentenceContentsOriginal.size() > 0)
	{
		delete sentenceContentsOriginal[0];
	}
	*/
	for(vector<LRPpreprocessorPlainTextWord*>::iterator it = sentenceContentsOriginal.begin(); it != sentenceContentsOriginal.end(); it++)
	{
		LRPpreprocessorPlainTextWord* currentWord = *it;
		LRPpreprocessorMultiwordReductionPlainTextWord* plaintextWord = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(currentWord);	//require to cast back to plaintextword to ensure that all memory is deleted	//CHECKTHIS
		currentWord->nextTag = NULL;	//prevents future words from being deleted
		delete plaintextWord;
	}
	sentenceContentsOriginal.clear();
	sentenceContentsLRP.clear();
	
	#ifdef GIA_POS_REL_TRANSLATOR_HYBRID
	delete firstLogicReferenceInList;
	#endif
	
	if(next != NULL)
	{
		delete next;
	}
}

LRPtranslatorVariablesClass::LRPtranslatorVariablesClass(void)
{
	firstLRPpreprocessorSentenceInList = NULL;
}
LRPtranslatorVariablesClass::~LRPtranslatorVariablesClass(void)
{
}

void LRPpreprocessorSentenceClass::copyPreprocessorSentence(LRPpreprocessorSentence* sentenceToCopy, LRPpreprocessorSentence* newSentence)
{
	*newSentence = *sentenceToCopy;
}

vector<LRPpreprocessorPlainTextWord*>* LRPpreprocessorSentenceClass::getSentenceContents(LRPpreprocessorSentence* currentLRPpreprocessorSentenceInList)
{
	return  &(currentLRPpreprocessorSentenceInList->sentenceContentsLRP);
}

#ifdef GIA_POS_REL_TRANSLATOR_RULES_PARSE_ISOLATED_SUBREFERENCE_SETS_OPTIMISED
bool LRPpreprocessorSentenceClass::calculateParseIsolatedSubreferenceSets1(const LRPpreprocessorSentence* currentLRPpreprocessorSentenceInList, bool* parseIsolatedSubreferenceSets1)
{
	bool result = true;
	if((currentLRPpreprocessorSentenceInList->parseIsolatedSubreferenceSets) && ((currentLRPpreprocessorSentenceInList->parseIsolatedSubreferenceSetsFirst) || (currentLRPpreprocessorSentenceInList->parseIsolatedSubreferenceSetsOnly)))
	{
		*parseIsolatedSubreferenceSets1 = true;
	}
	return result;
}
bool LRPpreprocessorSentenceClass::calculateParseIsolatedSubreferenceSets2(const LRPpreprocessorSentence* currentLRPpreprocessorSentenceInList, bool* parseIsolatedSubreferenceSets2)
{
	bool result = false;
	if(currentLRPpreprocessorSentenceInList->parseIsolatedSubreferenceSets)
	{
		if(!(currentLRPpreprocessorSentenceInList->parseIsolatedSubreferenceSetsOnly))
		{
			result = true;
			if(currentLRPpreprocessorSentenceInList->parseIsolatedSubreferenceSetsFirst)
			{
				*parseIsolatedSubreferenceSets2 = false;
			}
		}
	}			
	return result;
}
#endif	

void LRPpreprocessorSentenceClass::printSentence(vector<LRPpreprocessorPlainTextWord*>* sentenceContents, const bool error)
{
	if(error)
	{
		cerr << "sentenceContents = " << LRPpreprocessorWordClassObject.printWordListString(sentenceContents) << endl;
	}
	else
	{
		cout << "sentenceContents = " << LRPpreprocessorWordClassObject.printWordListString(sentenceContents) << endl;
	}
	
	/*
	for(int w=0; w<sentenceContents->size(); w++)
	{
		cout << sentenceContents->at(w)->tagName << " ";
	}
	cout << endl;
	*/
}

#ifdef SANI_POS_REL_TRANSLATOR_RULES_ITERATE_OVER_UNAMBIGUOUS_POS_PERMUTATIONS_AT_START
//assumes LRPpreprocessorPOStaggerClass::setSentenceContentsWordsUnambiguousPOSindex has already been executed
void LRPpreprocessorSentenceClass::printSentenceUnambiguousPOSindex(vector<LRPpreprocessorPlainTextWord*>* sentenceContents)
{
	cout << "sentencePOS = ";	
	for(int w=0; w<sentenceContents->size(); w++)
	{
		LRPpreprocessorPlainTextWord* contextWord = sentenceContents->at(w);
		uchar unambiguousPOSinfoIndex = contextWord->unambiguousPOSindex;
		cout << LRPpreprocessorPOStypeCrossReferenceStanfordPos[unambiguousPOSinfoIndex] << " ";	//int(unambiguousPOSinfoIndex)
	}
	cout << endl;
}
#endif








