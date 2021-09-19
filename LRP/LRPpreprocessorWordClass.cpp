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
 * File Name: LRPpreprocessorWordClass.cpp
 * Author: Richard Bruce Baxter - Copyright (c) 2005-2021 Baxter AI (baxterai.com)
 * Project: Language Reduction Preprocessor
 * Project Version: 3q2a 19-September-2021
 * Requirements: requires plain text file
 * Description: Preprocessor Word Class
 * /
 *******************************************************************************/


#include "LRPpreprocessorWordClass.hpp"



LRPpreprocessorWord::LRPpreprocessorWord(void)
{
	initialiseLRPpreprocessorWord();
}
LRPpreprocessorWord::LRPpreprocessorWord(string tagNameNew)
{
	initialiseLRPpreprocessorWord();
	tagName = tagNameNew;
}
LRPpreprocessorWord::~LRPpreprocessorWord(void)
{
	#ifdef GIA_POS_REL_TRANSLATOR_HYBRID_EXECUTE_PRELIM_POS_TAGGER
	#ifdef LRP_PREPROCESSOR_POS_TAGGER
	if(featureReferencePrelim != NULL)
	{
		delete featureReferencePrelim;
	}
	#else
	//featureReferencePrelim will be deleted when deleting prelim sentence objects created by parsing of prelim extenal nlp pos tagger output
	#endif
	#endif
		
	if(nextTag != NULL)
	{
		delete nextTag;
	}
}
void LRPpreprocessorWord::initialiseLRPpreprocessorWord()
{
	tagName = "";

	plainTextWord = false;	

	nextTag = NULL;
}


LRPpreprocessorMultiwordReductionWord::LRPpreprocessorMultiwordReductionWord(void)
{
	base = false;
	
	grammaticalTenseForm = LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_INFINITIVE;
	
	grammaticalTenseModifier = GRAMMATICAL_TENSE_MODIFIER_INFINITIVE_OR_IMPERATIVE_OR_PRESENT_NOT_THIRD_PERSON_SINGULAR_OR_STATE_TEMP;
	baseName = "";
	
	alternateTagBasic = NULL;

}
LRPpreprocessorMultiwordReductionWord::~LRPpreprocessorMultiwordReductionWord(void)
{
}

LRPpreprocessorMultiwordReductionPhrasalVerbWord::LRPpreprocessorMultiwordReductionPhrasalVerbWord(void)
{
	tagSpecialArbitraryName = false;
	tagSpecialArbitraryNameType = LRP_PREPROCESSOR_WORD_PHRASALVERB_DATABASE_TAG_ARBITRARYNAME_UNDEFINED;
	optional = false;
	/*
	primary = false;
	primaryPhrasalVerbReplacementString = "";
	primaryPhrasalVerbReplacementStringNLPonly = "";
	*/
	alternateTag = NULL;
}
LRPpreprocessorMultiwordReductionPhrasalVerbWord::~LRPpreprocessorMultiwordReductionPhrasalVerbWord(void)
{
}

LRPpreprocessorMultiwordReductionIrregularVerbWord::LRPpreprocessorMultiwordReductionIrregularVerbWord(void)
{
	alternateTag = NULL;
}
LRPpreprocessorMultiwordReductionIrregularVerbWord::~LRPpreprocessorMultiwordReductionIrregularVerbWord(void)
{
	if(alternateTag != NULL)
	{
		delete alternateTag;	//CHECKTHIS
	}	
}

LRPpreprocessorPlainTextWord::LRPpreprocessorPlainTextWord(void)
{	
	initialiseLRPpreprocessorPlainTextWord();	
}
LRPpreprocessorPlainTextWord::~LRPpreprocessorPlainTextWord(void)
{
}
void LRPpreprocessorPlainTextWord::initialiseLRPpreprocessorPlainTextWord()
{
	#ifdef GIA_POS_REL_TRANSLATOR_HYBRID_EXECUTE_PRELIM_POS_TAGGER
	featureReferencePrelim = NULL;
	#endif
	#ifdef LRP_PREPROCESSOR_RECORD_REFERENCES
	featureReferenceOriginal = NULL;
	featureReference = NULL;
	entityReference = NULL;
	#endif

	#ifdef GIA_NEURAL_NETWORK
	wordShortcutToConceptNeuron = NULL;
	#ifdef GIA_NEURAL_NETWORK_NON_SEMANTIC
	neuralNetworkPreprocessorWordType = GIA_NEURAL_NETWORK_NON_SEMANTIC_PREPROCESSOR_WORD_TYPE_UNDEFINED;
	tagNameOriginalNonLemma = "";
	#endif
	#endif

	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_PERFORM_BATCH_PREDICTIONS
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FEED_ALL_PERMUTATIONS_INDIVIDUALLY
	//POStaggerExperiencePermutations = NULL;
	#else
	POStaggerExperience = NULL;
	#endif
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_PREDICTION_VERIFICATION
	centreWordPOSambiguityInfo = 0;
	#endif
	#endif
	
	alreadyFoundMatch = false;	//moved LRP3m7a
	#ifdef LRP_PREPROCESSOR_INITIALISE_WORD_INDEX_LIST_FROM_LRP_FILES
	POSambiguityInfo = 0;
	#endif
	#ifdef SANI_NODES
	//#ifdef SANI_POS_REL_TRANSLATOR_RULES_ITERATE_OVER_UNAMBIGUOUS_POS_PERMUTATIONS_AT_START	//also required for SANI_SEQUENCE_GRAMMAR_REFERENCE_SET_IDENTIFICATION
	unambiguousPOSindex = LRP_SHARED_POS_TYPE_UNDEFINED;	//LRP_PREPROCESSOR_POS_TYPE_UNDEFINED is out of scope
	//#else	//also required for SANI_SEQUENCE_GRAMMAR_REFERENCE_SET_IDENTIFICATION
	//#endif
	wordPOStypeInferred = LRP_SHARED_POS_TYPE_UNDEFINED;
	translatorSentenceEntityIndex = LRP_WORD_INDEX_UNDEFINED;
	#ifdef SANI_FORWARD
	translatorSentenceWordIndex = LRP_WORD_INDEX_UNDEFINED;
	#endif
	#ifdef GIA_POS_REL_TRANSLATOR_RULES
	translatorEntity = NULL;
	#ifdef GIA_POS_REL_TRANSLATOR_RULES_CODE_COMPONENT_WORD_NOUN_VERB_VARIANT
	wordVerbVariantGrammaticalTenseForm = LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_UNKNOWN;
	wordNounVariantGrammaticalTenseForm = LRP_PREPROCESSOR_WORD_NOUN_DATABASE_TAG_BASE_TENSE_FORM_SINGULAR;
	#endif
	#endif
	#endif	
	
	//#ifdef SANI_SEQUENCE_GRAMMAR_REFERENCE_SET_IDENTIFICATION_VIA_DETERMINERS
	referenceSetStartCodonDeterminerType = INT_DEFAULT_VALUE;
	//#ifdef SANI_SEQUENCE_GRAMMAR_REFERENCE_SET_IDENTIFICATION_VIA_DETERMINERS_SUPPORT_PRONOUNS
	pronounDeterminerDetected = false;
	pronounReferenceDetected = false;
	//#endif
	//#endif
}

LRPpreprocessorMultiwordReductionPlainTextWord::LRPpreprocessorMultiwordReductionPlainTextWord(void)
{	
	initialiseLRPpreprocessorMultiwordReductionPlainTextWord();	
}
LRPpreprocessorMultiwordReductionPlainTextWord::LRPpreprocessorMultiwordReductionPlainTextWord(string tagNameNew)
{	
	initialiseLRPpreprocessorMultiwordReductionPlainTextWord();
	
	tagName = tagNameNew;
}
LRPpreprocessorMultiwordReductionPlainTextWord::~LRPpreprocessorMultiwordReductionPlainTextWord(void)
{
}
void LRPpreprocessorMultiwordReductionPlainTextWord::initialiseLRPpreprocessorMultiwordReductionPlainTextWord()
{
	tagNameLemma = "";

	entityIndex = false;
	collapsedPhrasalVerbExactDefinedSection = false;
	collapsedMultiwordWord = false;
	collapsedMultiwordWordType = LRP_PREPROCESSOR_WORD_DUMMY_COLLAPSED_MULTIWORD_UNDEFINED_TYPE;
	collapsedMultiwordWordTemp = false;
	grammaticalTenseFormDetected = LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_INFINITIVE;
	
	#ifdef LRP_PREPROCESSOR_RECORD_REFERENCES
	preprocessorUpperLevelWordReference = NULL;
	preprocessorUpperLevelWordReferenceSize = INT_DEFAULT_VALUE;
	preprocessorLowerLevelWordReference = NULL;
	characterIndexInSentenceContentsOriginalText = INT_DEFAULT_VALUE;
	#endif
	
	#ifdef LRP_PREPROCESSOR_WORD_MULTIWORD_REDUCTION
	tagNameLRPforNLP = "";
	#endif
	
	plainTextWord = true;
}



/*
LRPpreprocessorMultiwordReductionSentence::LRPpreprocessorMultiwordReductionSentence(void)
{
	nextSentence = NULL;
}
LRPpreprocessorMultiwordReductionSentence::~LRPpreprocessorMultiwordReductionSentence(void)
{
}
*/

LRPpreprocessorMultiwordReductionBasicSentence::LRPpreprocessorMultiwordReductionBasicSentence(void)
{
	firstTagInSentence = new LRPpreprocessorMultiwordReductionWord();
	
	nextSentence = NULL;
}
LRPpreprocessorMultiwordReductionBasicSentence::~LRPpreprocessorMultiwordReductionBasicSentence(void)
{
}

LRPpreprocessorMultiwordReductionPhrasalVerbSentence::LRPpreprocessorMultiwordReductionPhrasalVerbSentence(void)
{
	firstTagInSentence = new LRPpreprocessorMultiwordReductionPhrasalVerbWord();
	alternateSentence = NULL;
	
	nextSentence = NULL;
}
LRPpreprocessorMultiwordReductionPhrasalVerbSentence::~LRPpreprocessorMultiwordReductionPhrasalVerbSentence(void)
{
}

LRPpreprocessorMultiwordReductionIrregularVerbSentence::LRPpreprocessorMultiwordReductionIrregularVerbSentence(void)
{
	firstTagInSentence = new LRPpreprocessorMultiwordReductionIrregularVerbWord();
	
	nextSentence = NULL;
}
LRPpreprocessorMultiwordReductionIrregularVerbSentence::~LRPpreprocessorMultiwordReductionIrregularVerbSentence(void)
{
}



LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo::LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo(void)
{
	wordWithLRP = "";
	wordWithLRPforNLPonly = "";

	entityIndex = INT_DEFAULT_VALUE;
	sentenceIndex = INT_DEFAULT_VALUE;

	next = NULL;
}
LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo::~LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo(void)
{
}




//#ifdef LRP_PREPROCESSOR_RECORD_REFERENCES

string LRPpreprocessorWordClassClass::generateTextFromPreprocessorSentenceWordList(constEffective LRPpreprocessorPlainTextWord* firstWordInSentence)
{
	return generateTextFromPreprocessorSentenceWordList(firstWordInSentence, false);
}
string LRPpreprocessorWordClassClass::generateTextFromPreprocessorSentenceWordList(constEffective LRPpreprocessorPlainTextWord* firstWordInSentence, const bool LRPforNLP)
{
	string sentenceText = "";
	LRPpreprocessorPlainTextWord* currentWordInSentence = firstWordInSentence;
	bool isFirstWordInSentence = true;
	while(currentWordInSentence->nextTag != NULL)
	{
		constEffective LRPpreprocessorPlainTextWord* word = currentWordInSentence;
		sentenceText = sentenceText + generateTextFromPreprocessorSentenceWord(word, LRPforNLP, isFirstWordInSentence);
		isFirstWordInSentence = false;
		
		currentWordInSentence = static_cast<LRPpreprocessorPlainTextWord*>(currentWordInSentence->nextTag);
	}
	return sentenceText;
}

string LRPpreprocessorWordClassClass::generateTextFromVectorWordList(const vector<LRPpreprocessorPlainTextWord*>* wordList)
{
	return generateTextFromVectorWordList(wordList, false);
}
string LRPpreprocessorWordClassClass::generateTextFromVectorWordList(const vector<LRPpreprocessorPlainTextWord*>* wordList, const bool LRPforNLP)
{
	string sentenceText = "";
	bool isFirstWordInSentence = true;
	for(int wordIndex = 0; wordIndex<wordList->size(); wordIndex++)
	{
		constEffective LRPpreprocessorPlainTextWord* word = (*wordList)[wordIndex];
		#ifdef LRP_PREPROCESSOR_WORD_NLP_PARSABLE_PHRASE_SUPPORT_APOSTROPHES_POSSESSION_AND_OMISSION	
		if(isApostrophePossessionOrOmission(word))
		{
			isFirstWordInSentence = true;	//do not prepend a space
		}
		#endif
		sentenceText = sentenceText + generateTextFromPreprocessorSentenceWord(word, LRPforNLP, isFirstWordInSentence);
		isFirstWordInSentence = false;
	}
	return sentenceText;
}

string LRPpreprocessorWordClassClass::generateTextFromPreprocessorSentenceWord(constEffective LRPpreprocessorPlainTextWord* word, const bool LRPforNLP, const bool isFirstWordInSentence)
{
	string wordText = word->tagName;
	if(LRPforNLP)
	{
		if(word->plainTextWord)
		{	
			//cout << "word->plainTextWord: " << word->tagName << endl;
			LRPpreprocessorMultiwordReductionPlainTextWord* plaintextWord = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(word);
			#ifdef LRP_PREPROCESSOR_WORD_MULTIWORD_REDUCTION
			if(plaintextWord->tagNameLRPforNLP != "")
			{
				wordText = plaintextWord->tagNameLRPforNLP;
			}
			#endif
		}
	}
	bool punctuationMarkFound = false;
	for(int i=0; i<LRP_NLP_NUMBER_OF_PUNCTUATION_MARK_CHARACTERS; i++)
	{
		string nlpPunctionMarkCharacterString = "";
		nlpPunctionMarkCharacterString = nlpPunctionMarkCharacterString + nlpPunctionMarkCharacterArray[i];
		if(wordText == nlpPunctionMarkCharacterString)
		{
			punctuationMarkFound = true;
		}
	}
	if(!punctuationMarkFound && !isFirstWordInSentence)
	{
		wordText = string(STRING_SPACE) + wordText;
	}
	return wordText;
}

int LRPpreprocessorWordClassClass::calculateLengthOfGeneratedVectorWordListText(const vector<LRPpreprocessorPlainTextWord*>* wordList)
{
	string sentenceText = generateTextFromVectorWordList(wordList);
	return sentenceText.length();
}

bool LRPpreprocessorWordClassClass::generateSentenceWordList(LRPpreprocessorPlainTextWord* sentenceContentsFirstWord, vector<LRPpreprocessorPlainTextWord*>* wordList)
{
	return generateSentenceWordList(sentenceContentsFirstWord, wordList, LRP_PREPROCESSOR_WORD_LIST_INDEX_NO_REQUIREMENT, LRP_PREPROCESSOR_WORD_LIST_INDEX_NO_REQUIREMENT);
}
bool LRPpreprocessorWordClassClass::generateSentenceWordList(LRPpreprocessorPlainTextWord* sentenceContentsFirstWord, vector<LRPpreprocessorPlainTextWord*>* wordList, const int startIndex, const int endIndex)
{
	bool result = true;
	
	wordList->clear();	//added 3b3i
	
	int index = 0;
	LRPpreprocessorPlainTextWord* currentWordInSentence = sentenceContentsFirstWord;
	while(currentWordInSentence->nextTag != NULL)
	{
		if((startIndex == LRP_PREPROCESSOR_WORD_LIST_INDEX_NO_REQUIREMENT) || (index >= startIndex))
		{
			if((endIndex == LRP_PREPROCESSOR_WORD_LIST_INDEX_NO_REQUIREMENT) || (index < endIndex))
			{
				wordList->push_back(currentWordInSentence);
			}
		}
		
		currentWordInSentence = static_cast<LRPpreprocessorPlainTextWord*>(currentWordInSentence->nextTag);
		index++;
	}
	
	#ifdef GIA_DEBUG_PREPROCESSOR_SENTENCE_REFERENCE_SET
	cout << "GIAposRelTranslatorHybridReferenceSetClass::generateSentenceWordList{}: " << endl;
	for(int i=0; i<wordList->size(); i++)
	{
		cout << ((*wordList)[i])->tagName << STRING_SPACE;
	}
	cout << endl;
	#endif
	
	return result;
}

//precondition: words in wordList are unique across all logicReferenceVariableWordLists in memory
bool LRPpreprocessorWordClassClass::generateFlatSentenceWordList(vector<LRPpreprocessorPlainTextWord*>* wordList, LRPpreprocessorMultiwordReductionPlainTextWord** sentenceContentsFirstWord)
{
	bool result = true;
	
	if(wordList->size() > 0)
	{
		*sentenceContentsFirstWord = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>((*wordList)[0]);
		LRPpreprocessorPlainTextWord* currentWordInSentence = *sentenceContentsFirstWord;
		for(int i=1; i<wordList->size(); i++)
		{
			LRPpreprocessorPlainTextWord* word = (*wordList)[i];
			if(!(word->plainTextWord))
			{
				cout << "LRPpreprocessorWordClassClass::generateFlatSentenceWordList{} warning: (!(word->plainTextWord))" << endl;
			}
			currentWordInSentence->nextTag = word;
			currentWordInSentence = static_cast<LRPpreprocessorPlainTextWord*>(currentWordInSentence->nextTag);
		}
		if(currentWordInSentence->nextTag == NULL)
		{
			//cout << "LRPpreprocessorWordClassClass::generateFlatSentenceWordList{}: final (currentWordInSentence->nextTag == NULL)" << endl;
			//cout << "currentWordInSentence->tagName = " << currentWordInSentence->tagName << endl;
			currentWordInSentence->nextTag = new LRPpreprocessorMultiwordReductionPlainTextWord();	//add a blank word to end of list (as per standard object list specification)
		}
	}
	
	return result;
}


bool LRPpreprocessorWordClassClass::addWordListToWordList(vector<LRPpreprocessorPlainTextWord*>* wordList, vector<LRPpreprocessorPlainTextWord*>* wordListToAdd)
{
	bool result = true;
	for(int i=0; i<wordListToAdd->size(); i++)
	{
		wordList->push_back((*wordListToAdd)[i]);
	}
	return result;
}
bool LRPpreprocessorWordClassClass::addStringArrayToWordList(vector<LRPpreprocessorPlainTextWord*>* wordList, const string* stringArrayToAdd, const int arraySize)
{
	bool result = true;
	for(int i=0; i<arraySize; i++)
	{
		LRPpreprocessorPlainTextWord* newWord = new LRPpreprocessorMultiwordReductionPlainTextWord(stringArrayToAdd[i]);
		wordList->push_back(newWord);
	}
	return result;
}
bool LRPpreprocessorWordClassClass::addStringToWordList(vector<LRPpreprocessorPlainTextWord*>* wordList, const string stringToAdd)
{
	bool result = true;
	LRPpreprocessorPlainTextWord* newWord = new LRPpreprocessorMultiwordReductionPlainTextWord(stringToAdd);
	wordList->push_back(newWord);
	return result;
}


#ifdef LRP_PREPROCESSOR_WORD_NLP_PARSABLE_PHRASE_SUPPORT_INTRAWORD_PUNCTUATION_MARK
bool LRPpreprocessorWordClassClass::isIntrawordPunctuationMark(int indexOfCurrentToken, string* lineContents)
{
	bool intrawordPunctuationMark = false;
	char currentToken = (*lineContents)[indexOfCurrentToken];
	if((currentToken == CHAR_FULLSTOP) || (currentToken == CHAR_COLON))	//updated 2j6e (added CHAR_COLON for times, eg 06:45)
	{
		if(indexOfCurrentToken < lineContents->length()-1)	//ensure fullstop is not immediately succeded by an alphabetical character, which indicates that the fullstop is part of a filename, eg "people.xml"
		{
			char characterImmediatelySucceedingPunctuationMark = (*lineContents)[indexOfCurrentToken+1];
			#ifdef LRP_PREPROCESSOR_WORD_NLP_PARSABLE_PHRASE_SUPPORT_FILENAMES_WITH_FULLSTOPS_AND_FLOATS_AND_TIMES
			bool isPunctuationMarkImmediatelySucceededByAlphanumericCharacter = SHAREDvars.charInCharArray(characterImmediatelySucceedingPunctuationMark, LRPpreprocessorMultiwordReductionNLPparsableCharacters, LRP_PREPROCESSOR_WORD_NLP_PARSABLE_PHRASE_CHARACTERS_NUMBER_OF_TYPES);
			if(isPunctuationMarkImmediatelySucceededByAlphanumericCharacter)
			{
				//e.g. "thefile.exe ..."
				intrawordPunctuationMark = true;
			}
			#endif
			
			#ifdef LRP_PREPROCESSOR_WORD_NLP_PARSABLE_PHRASE_SUPPORT_ABBREVIATIONS
			if(currentToken == CHAR_FULLSTOP)
			{
				if(indexOfCurrentToken < lineContents->length()-2)
				{
					if(characterImmediatelySucceedingPunctuationMark == CHAR_SPACE)
					{
						bool characterPreceededByTitlePrefixAbbreviation = false;
						for(int i=0; i<LRP_PREPROCESSOR_WORD_NLP_PARSABLE_PHRASE_SUPPORT_ABBREVIATIONS_TITLE_PREFIXES_NUMBER_OF_TYPES; i++)
						{
							int lengthOfPrefix = preprocessorSupportAbbreviationsTitlePrefixesArray[i].length();
							if(indexOfCurrentToken >= lengthOfPrefix-1)	//-1 to take into account fact that prefixes are defined with a succeeding full stop character
							{
								if(lineContents->substr(indexOfCurrentToken-lengthOfPrefix+1, lengthOfPrefix) == preprocessorSupportAbbreviationsTitlePrefixesArray[i])	//+1 to take into account fact that prefixes are defined with a succeeding full stop character
								{
									characterPreceededByTitlePrefixAbbreviation = true;
									//cout << "characterPreceededByTitlePrefixAbbreviation: preprocessorSupportAbbreviationsTitlePrefixesArray[i] = " << preprocessorSupportAbbreviationsTitlePrefixesArray[i] << endl;
								}
							}
						}
						if(characterPreceededByTitlePrefixAbbreviation)
						{
							//e.g. "Ms. House ..."
							intrawordPunctuationMark = true;
						}

						char characterImmediatelySucceedingSpace = (*lineContents)[indexOfCurrentToken+2];
						bool isPunctuationMarkSucceededBySpaceAndUncapitalisedCharacter = SHAREDvars.charInCharArray(characterImmediatelySucceedingSpace, preprocessorLowerCaseLettersArray, LRP_PREPROCESSOR_WORD_LOWER_CASE_LETTERS_NUMBER_OF_TYPES);
						if(isPunctuationMarkSucceededBySpaceAndUncapitalisedCharacter)
						{
							//e.g. "e.g. the house ..."
							intrawordPunctuationMark = true;
						}
					}
				}
			}
			#endif
		}
	}
	return intrawordPunctuationMark;
}
#endif

#ifdef LRP_PREPROCESSOR_WORD_NLP_PARSABLE_PHRASE_SUPPORT_APOSTROPHES_POSSESSION_AND_OMISSION
bool LRPpreprocessorWordClassClass::isApostrophePossessionOrOmission(int indexOfCurrentToken, string* lineContents)
{
	bool apostrophePossessionOrOmissionFound = false;
	
	char currentToken = (*lineContents)[indexOfCurrentToken];
	if(currentToken == CHAR_APOSTROPHE)
	{
		if((indexOfCurrentToken >= 1) && (indexOfCurrentToken < lineContents->length()-1))
		{
			char characterImmediatelyPreceedingApostrophe = (*lineContents)[indexOfCurrentToken-1];
			char characterImmediatelySucceedingApostrophe = (*lineContents)[indexOfCurrentToken+1];
			bool characterImmediatelyPreceedingApostropheIsLetter = SHAREDvars.charInCharArray(characterImmediatelyPreceedingApostrophe, preprocessorLowerOrUpperCaseLettersArray, LRP_PREPROCESSOR_WORD_LOWER_OR_UPPER_CASE_LETTERS_NUMBER_OF_TYPES);
			bool characterImmediatelySucceedingApostropheIsLetter = SHAREDvars.charInCharArray(characterImmediatelySucceedingApostrophe, preprocessorLowerOrUpperCaseLettersArray, LRP_PREPROCESSOR_WORD_LOWER_OR_UPPER_CASE_LETTERS_NUMBER_OF_TYPES);			
			if(characterImmediatelyPreceedingApostropheIsLetter && characterImmediatelySucceedingApostropheIsLetter)
			{
				apostrophePossessionOrOmissionFound = true;
			}
		}
	}
	
	return apostrophePossessionOrOmissionFound;	
}
bool LRPpreprocessorWordClassClass::isApostrophePossessionOrOmission(const LRPpreprocessorPlainTextWord* word)
{
	bool apostrophePossessionOrOmissionFound = false;
	
	string wordText = word->tagName;
	if(wordText.length() > 0)
	{
		if((wordText[0] == CHAR_APOSTROPHE) && (wordText[wordText.length()-1] != CHAR_APOSTROPHE))
		{
			apostrophePossessionOrOmissionFound = true;
		}
	}
	
	return apostrophePossessionOrOmissionFound;	
}
#endif			
		
		
#ifdef LRP_PREPROCESSOR_RECORD_REFERENCES

bool LRPpreprocessorWordClassClass::findAndReplaceAllOccurancesSimpleSubstringInWordListWithSimpleSubstring(vector<LRPpreprocessorPlainTextWord*>* wordList, string stringSimpleToFind,  string stringSimpleReplacement)
{
	return findAndReplaceAllOccurancesSimpleSubstringInWordListWithSimpleSubstring(wordList, &stringSimpleToFind, &stringSimpleReplacement);
}
bool LRPpreprocessorWordClassClass::findAndReplaceAllOccurancesSimpleSubstringInWordListWithSimpleSubstring(vector<LRPpreprocessorPlainTextWord*>* wordList, string* stringSimpleToFind,  string* stringSimpleReplacement)
{
	bool result = false;
	
	//cout << "findAndReplaceAllOccurancesSimpleSubstringInWordListWithSimpleSubstring{}: wordList before: " << generateTextFromVectorWordList(wordList) << ", stringSimpleToFind = " << *stringSimpleToFind << ", stringSimpleReplacement = " << *stringSimpleReplacement  << endl;

	vector<LRPpreprocessorPlainTextWord*> wordListToFind;
	generateSentenceWordListFromStringSimple(&wordListToFind, stringSimpleToFind);
	vector<LRPpreprocessorPlainTextWord*> wordListReplacement;
	generateSentenceWordListFromStringSimple(&wordListReplacement, stringSimpleReplacement);
	
	for(int w = 0; w < wordList->size(); w++)
	{
		if(findAndReplaceWordListInWordListAtIndexWithWordList(wordList, &wordListToFind, w, &wordListReplacement))
		{
			w = w + wordListReplacement.size()-1;
			result = true;
		}
	}
		
	return result;
}
bool LRPpreprocessorWordClassClass::findAndReplaceSimpleSubstringInWordListAtIndexWithSimpleSubstring(vector<LRPpreprocessorPlainTextWord*>* wordList, string stringSimpleToFind, int indexToPerformFind, string stringSimpleReplacement)
{
	bool result = false;
	
	vector<LRPpreprocessorPlainTextWord*> wordListToFind;
	generateSentenceWordListFromStringSimple(&wordListToFind, &stringSimpleToFind);
	vector<LRPpreprocessorPlainTextWord*> wordListReplacement;
	generateSentenceWordListFromStringSimple(&wordListReplacement, &stringSimpleReplacement);
	
	//cout << "stringSimpleToFind = " << stringSimpleToFind << endl;
	//cout << "stringSimpleReplacement = " << stringSimpleReplacement << endl;
	
	if(findAndReplaceWordListInWordListAtIndexWithWordList(wordList, &wordListToFind, indexToPerformFind, &wordListReplacement))
	{
		result = true;
	}
		
	return result;
}
bool LRPpreprocessorWordClassClass::findAndReplaceWordListInWordListAtIndexWithWordList(vector<LRPpreprocessorPlainTextWord*>* wordList, vector<LRPpreprocessorPlainTextWord*>* wordListToFind, int indexToPerformFind, vector<LRPpreprocessorPlainTextWord*>* wordListReplacement)
{
	bool result = false;
	
	//cout << "findAndReplaceWordListInWordListAtIndexWithWordList{}: wordList before: " << generateTextFromVectorWordList(wordList) << ", wordListToFind = " << generateTextFromVectorWordList(wordListToFind) << ", wordListReplacement = " << generateTextFromVectorWordList(wordListReplacement)  << endl;
	
	bool caseInsensitive = false;
	if(findSubWordListInWordListAtIndex(wordList, wordListToFind, indexToPerformFind, caseInsensitive))
	{
		removeWordsFromWordList(wordList, indexToPerformFind, wordListToFind->size());
		insertWordListIntoWordList(wordList, wordListReplacement, indexToPerformFind);
		result = true;
	}
	
	//cout << "findAndReplaceWordListInWordListAtIndexWithWordList{}: wordList after: " << generateTextFromVectorWordList(wordList) << endl;
	
	return result;
}


bool LRPpreprocessorWordClassClass::findSimpleSubstringInWordList(const vector<LRPpreprocessorPlainTextWord*>* wordList, string stringSimpleToFind)
{
	int startIndexToPerformFind = 0;
	return findSimpleSubstringInWordList(wordList, stringSimpleToFind, startIndexToPerformFind);
}
bool LRPpreprocessorWordClassClass::findSimpleSubstringInWordList(const vector<LRPpreprocessorPlainTextWord*>* wordList, string stringSimpleToFind, const int startIndexToPerformFind)
{
	bool result = false;
	
	vector<LRPpreprocessorPlainTextWord*> wordListToFind;
	generateSentenceWordListFromStringSimple(&wordListToFind, &stringSimpleToFind);
	
	if(findSubWordListInWordList(wordList, &wordListToFind, startIndexToPerformFind))
	{
		result = true;
	}
		
	return result;
}
bool LRPpreprocessorWordClassClass::findSubWordListInWordList(const vector<LRPpreprocessorPlainTextWord*>* wordList, vector<LRPpreprocessorPlainTextWord*>* wordListToFind, const int startIndexToPerformFind)
{
	bool result = false;
	
	for(int i=startIndexToPerformFind; i<wordList->size(); i++)
	{
		bool caseInsensitive = false;
		if(findSubWordListInWordListAtIndex(wordList, wordListToFind, i, caseInsensitive))
		{
			result = true;
		}
	}
		
	return result;
}
bool LRPpreprocessorWordClassClass::findSimpleSubstringInWordListAtIndex(const vector<LRPpreprocessorPlainTextWord*>* wordList, string stringSimpleToFind, int indexToPerformFind, const bool caseInsensitive)
{
	bool result = false;
	
	vector<LRPpreprocessorPlainTextWord*> wordListToFind;
	generateSentenceWordListFromStringSimple(&wordListToFind, &stringSimpleToFind);
	
	if(findSubWordListInWordListAtIndex(wordList, &wordListToFind, indexToPerformFind, caseInsensitive))
	{
		result = true;
	}
		
	return result;
}
bool LRPpreprocessorWordClassClass::findSubWordListInWordListAtIndex(const vector<LRPpreprocessorPlainTextWord*>* wordList, vector<LRPpreprocessorPlainTextWord*>* wordListToFind, int indexToPerformFind, const bool caseInsensitive)
{
	bool result = false;
	
	if(indexToPerformFind+wordListToFind->size() <= wordList->size())
	{
		bool stillFindingWordMatches = true;
		int i = 0;
		while((i < wordListToFind->size()) && stillFindingWordMatches)
		{
			string string1 = (*wordList)[i+indexToPerformFind]->tagName;
			string string2 = (*wordListToFind)[i]->tagName;
			if(caseInsensitive)
			{
				string1 = SHAREDvars.convertStringToLowerCase(&string1);
				string2 = SHAREDvars.convertStringToLowerCase(&string2);
			}
			if(string1 != string2)
			{
				stillFindingWordMatches = false;
			}
			i++;
		}
		if(stillFindingWordMatches)
		{
			result = true;
		}
	}
	/*
	else
	{
		cerr << "LRPpreprocessorWordClassClass::findSubWordListInWordListAtIndex error{}: !(indexToPerformFind+wordListToFind->size() <= wordList->size())" << endl;
		exit(EXIT_ERROR);
	}
	*/
			
	return result;
}

bool LRPpreprocessorWordClassClass::generateSentenceWordListFromStringSimple(vector<LRPpreprocessorPlainTextWord*>* wordList, string* stringSimple)
{
	bool result = true;
	
	string currentWord = "";
	for(int i=0; i < stringSimple->length(); i++)
	{
		char c = (*stringSimple)[i];
		if((c == CHAR_SPACE) || (i == stringSimple->length()-1))
		{
			if(i == stringSimple->length()-1)
			{
				currentWord = currentWord + c;
			}
			addStringToWordList(wordList, currentWord);
			//cout << "generateSentenceWordListFromStringSimple: currentWord = " << currentWord << endl;
			currentWord = "";
		}
		else
		{
			currentWord = currentWord + c;
		}
	}
	
	return result;
}


int LRPpreprocessorWordClassClass::findStringInWordList(vector<LRPpreprocessorPlainTextWord*>* wordList, const string stringToFind)
{
	int startIndexToPerformFind = 0;
	return findStringInWordList(wordList, stringToFind, startIndexToPerformFind);
}
int LRPpreprocessorWordClassClass::findStringInWordList(vector<LRPpreprocessorPlainTextWord*>* wordList, const string stringToFind, int startIndexToPerformFind)
{
	bool result  = false;
	int indexOfWordFound = CPP_STRING_FIND_RESULT_FAIL_VALUE;
	
	if(startIndexToPerformFind < wordList->size())
	{
		for(int i=startIndexToPerformFind; i<wordList->size(); i++)
		{
			if(!result)
			{//return first instance
				string currentWord = (*wordList)[i]->tagName;
				if(currentWord == stringToFind)
				{
					result = true;
					indexOfWordFound = i;
				}
			}
		}
	}
	/*
	else
	{
		cerr << "LRPpreprocessorWordClassClass::findStringInWordList error{}: !(startIndexToPerformFind < wordList->size())" << endl;
		exit(EXIT_ERROR);
	}
	*/
			
	return indexOfWordFound;
}
int LRPpreprocessorWordClassClass::findStringInWordListReverse(vector<LRPpreprocessorPlainTextWord*>* wordList, const string stringToFind, int startIndexToPerformFind)
{
	bool result  = false;
	int indexOfWordFound = CPP_STRING_FIND_RESULT_FAIL_VALUE;
	
	if(startIndexToPerformFind < wordList->size())
	{
		for(int i=startIndexToPerformFind; i>=0; i--)
		{
			if(!result)
			{//return first instance
				string currentWord = (*wordList)[i]->tagName;
				if(currentWord == stringToFind)
				{
					result = true;
					indexOfWordFound = i;
				}
			}
		}
	}
			
	return indexOfWordFound;
}

bool LRPpreprocessorWordClassClass::findSubstringAtStartOfWordInWordList(vector<LRPpreprocessorPlainTextWord*>* wordList, const string substringToFind)
{
	int startIndexToPerformFind = 0;
	return findSubstringAtStartOfWordInWordList(wordList, substringToFind, startIndexToPerformFind);
}
bool LRPpreprocessorWordClassClass::findSubstringAtStartOfWordInWordList(vector<LRPpreprocessorPlainTextWord*>* wordList, const string substringToFind, int startIndexToPerformFind)
{
	bool result  = false;
	//int indexOfWordFound = CPP_STRING_FIND_RESULT_FAIL_VALUE;
	
	if(startIndexToPerformFind < wordList->size())
	{
		for(int i=startIndexToPerformFind; i<wordList->size(); i++)
		{
			if(!result)
			{//return first instance
				string currentWord = (*wordList)[i]->tagName;
				if(currentWord.find(substringToFind) == 0)
				{
					result = true;
					//indexOfWordFound = i;
				}
			}
		}
	}
	/*
	else
	{
		cerr << "LRPpreprocessorWordClassClass::findSubstringAtStartOfWordInWordList error{}: !(startIndexToPerformFind < wordList->size())" << endl;
		exit(EXIT_ERROR);
	}
	*/
			
	return result;
}



vector<LRPpreprocessorPlainTextWord*> LRPpreprocessorWordClassClass::extractSubWordListInWordList(vector<LRPpreprocessorPlainTextWord*>* wordList, int startIndexToExtract)
{
	int numberOfWordsToExtract = wordList->size() - startIndexToExtract;
	return extractSubWordListInWordList(wordList, startIndexToExtract, numberOfWordsToExtract);
}
vector<LRPpreprocessorPlainTextWord*> LRPpreprocessorWordClassClass::extractSubWordListInWordList(vector<LRPpreprocessorPlainTextWord*>* wordList, int startIndexToExtract, int numberOfWordsToExtract)
{
	bool result = false;
	vector<LRPpreprocessorPlainTextWord*> wordListSub;
	
	if(startIndexToExtract+numberOfWordsToExtract <= wordList->size())
	{
		result = true;
		for(int i=startIndexToExtract; i<startIndexToExtract+numberOfWordsToExtract; i++)
		{
			wordListSub.push_back((*wordList)[i]);
		}
	}
	/*
	else
	{
		cerr << "LRPpreprocessorWordClassClass::extractSubWordListInWordList error{}: !(startIndexToExtract+numberOfWordsToExtract <= wordList->size())" << endl;
		exit(EXIT_ERROR);
	}
	*/
			
	return wordListSub;
}

bool LRPpreprocessorWordClassClass::insertWordListIntoWordList(vector<LRPpreprocessorPlainTextWord*>* wordList, vector<LRPpreprocessorPlainTextWord*>* wordListToInsert, int indexToInsert)
{
	bool result = false;
		
	if(indexToInsert <= wordList->size())
	{
		result = true;
		for(int i=0; i<wordListToInsert->size(); i++)
		{
			vector<LRPpreprocessorPlainTextWord*>::iterator iter = wordList->begin() + indexToInsert + i;
			wordList->insert(iter, (*wordListToInsert)[i]);				
		}
	}
	else
	{
		cerr << "LRPpreprocessorWordClassClass::insertWordIntoWordList error{}: !(indexToInsert <= wordList->size())" << endl;
		exit(EXIT_ERROR);
	}
		
	return result;
}
bool LRPpreprocessorWordClassClass::insertStringIntoWordList(vector<LRPpreprocessorPlainTextWord*>* wordList, const string stringToInsert, int indexToInsert)
{
	bool result = true;
	LRPpreprocessorPlainTextWord* wordToInsert = new LRPpreprocessorMultiwordReductionPlainTextWord(stringToInsert);
	if(!insertWordIntoWordList(wordList, wordToInsert, indexToInsert))
	{
		result = false;
	}
		
	return result;
}
//why can't this be declared const?; LRPpreprocessorPlainTextWord* wordToInsert
bool LRPpreprocessorWordClassClass::insertWordIntoWordList(vector<LRPpreprocessorPlainTextWord*>* wordList, LRPpreprocessorPlainTextWord* wordToInsert, int indexToInsert)
{
	bool result = false;
	
	if(indexToInsert <= wordList->size())
	{
		result = true;
		vector<LRPpreprocessorPlainTextWord*>::iterator iter = wordList->begin() + indexToInsert;
		wordList->insert(iter, wordToInsert);	
	}
	/*
	else
	{
		cerr << "LRPpreprocessorWordClassClass::insertWordIntoWordList error{}: !(indexToInsert <= wordList->size())" << endl;
		exit(EXIT_ERROR);
	}
	*/
		
	return result;
}

bool LRPpreprocessorWordClassClass::wordListFindAndRemoveAllOccurancesSimpleSubstringInWordList(vector<LRPpreprocessorPlainTextWord*>* wordList, const string stringToFind)
{
	bool result = false;
	
	bool stillFinding = true;
	for(int i=0; i<wordList->size(); i++)
	{
		string currentWord = (*wordList)[i]->tagName;
		if(currentWord == stringToFind)
		{
			removeWordFromWordList(wordList, i);
			i = i - 1;
		}
	}
	return result;
}
bool LRPpreprocessorWordClassClass::removeWordFromWordList(vector<LRPpreprocessorPlainTextWord*>* wordList, int indexToRemove)
{
	return removeWordsFromWordList(wordList, indexToRemove, 1);
}
bool LRPpreprocessorWordClassClass::removeWordsFromWordList(vector<LRPpreprocessorPlainTextWord*>* wordList, int indexToRemove, int numberElementsToRemove)
{
	bool result = false;

	if(indexToRemove+numberElementsToRemove <= wordList->size())
	{
		result = true;
		wordList->erase((wordList->begin() + indexToRemove), (wordList->begin() + indexToRemove+numberElementsToRemove));
	}
	/*
	else
	{
		cerr << "LRPpreprocessorWordClassClass::removeWordsFromWordList error{}: !(indexToRemove+numberElementsToRemove <= wordList->size())" << endl;
		exit(EXIT_ERROR);
	}
	*/
		
	return result;
}


bool LRPpreprocessorWordClassClass::replaceWordListAtIndexWithSimpleSubstring(vector<LRPpreprocessorPlainTextWord*>* wordList, int indexToPerformReplacement, string stringSimpleReplacement)
{
	bool result = true;
	
	vector<LRPpreprocessorPlainTextWord*> wordListReplacement;
	generateSentenceWordListFromStringSimple(&wordListReplacement, &stringSimpleReplacement);
	
	if(!replaceWordListAtIndexWithSimpleSubstring(wordList, indexToPerformReplacement, &wordListReplacement))
	{
		result = false;
	}
	
	return result;
}

bool LRPpreprocessorWordClassClass::replaceWordListAtIndexWithSimpleSubstring(vector<LRPpreprocessorPlainTextWord*>* wordList, int indexToPerformReplacement, vector<LRPpreprocessorPlainTextWord*>* wordListReplacement)
{
	bool result = false;
	
	if(indexToPerformReplacement+wordListReplacement->size() <= wordList->size())
	{
		result = true;
		for(int i=0; i<wordListReplacement->size(); i++)
		{
			(*wordList)[indexToPerformReplacement+i]->tagName = (*wordListReplacement)[i]->tagName;
		}
	}
	/*
	else
	{
		cerr << "LRPpreprocessorWordClassClass::replaceWordListAtIndexWithSimpleSubstring error{}: !(indexToPerformReplacement+wordListReplacement.size() <= wordList.size())" << endl;
		exit(EXIT_ERROR);
	}
	*/
		
	return result;
}

bool LRPpreprocessorWordClassClass::copyWordListAndReplicateWordObjects(const vector<LRPpreprocessorPlainTextWord*>* wordList1, vector<LRPpreprocessorPlainTextWord*>* wordList2)
{
	clearWordListAndDeleteWordObjects(wordList2);
	
	for(int w=0; w<wordList1->size(); w++)
	{
		LRPpreprocessorMultiwordReductionPlainTextWord* currentWord = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(wordList1->at(w));
		LRPpreprocessorMultiwordReductionPlainTextWord* replicaWord = new LRPpreprocessorMultiwordReductionPlainTextWord(*currentWord);
		wordList2->push_back(replicaWord);
	}
}

bool LRPpreprocessorWordClassClass::clearWordListAndDeleteWordObjects(vector<LRPpreprocessorPlainTextWord*>* wordList)
{
	for(int w=0; w<wordList->size(); w++)
	{	
		LRPpreprocessorMultiwordReductionPlainTextWord* currentWord = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(wordList->at(w));
		delete currentWord;
	}
	wordList->clear();
}

#endif			


bool LRPpreprocessorWordClassClass::isWordInWordlist(vector<LRPpreprocessorPlainTextWord*>* wordList, LRPpreprocessorPlainTextWord* word)
{
	bool result  = false;
	if(find(wordList->begin(), wordList->end(), word) != wordList->end())
	{
		result = true;
	}
	return result;
}

bool LRPpreprocessorWordClassClass::printWordList(const vector<LRPpreprocessorPlainTextWord*>* wordList)
{
	cerr << printWordListString(wordList) << endl;
}
string LRPpreprocessorWordClassClass::printWordListString(const vector<LRPpreprocessorPlainTextWord*>* wordList)
{
	string output = "";
	for(int i=0; i<wordList->size(); i++)
	{
		output = output + ((*wordList)[i])->tagName + STRING_SPACE;
	}
	return output;
}


void LRPpreprocessorWordClassClass::preprocessorFillCurrentWord(LRPpreprocessorMultiwordReductionPlainTextWord** currentWordInSentence, string* currentWord, int* entityIndex, const int lastCharacterIndexOfWordInSentence)
{
	(*currentWordInSentence)->tagName = *currentWord;
	(*currentWordInSentence)->entityIndex = *entityIndex;
	#ifdef LRP_PREPROCESSOR_RECORD_REFERENCES
	(*currentWordInSentence)->characterIndexInSentenceContentsOriginalText = lastCharacterIndexOfWordInSentence - currentWord->size() - 1;
	#endif
	(*currentWordInSentence)->nextTag = new LRPpreprocessorMultiwordReductionPlainTextWord();
	(*currentWordInSentence) = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>((*currentWordInSentence)->nextTag);
	(*entityIndex) = (*entityIndex) + 1;
	*currentWord = "";
}

int LRPpreprocessorWordClassClass::getPOStypeFromName(const string wordPOStypeName)
{	
	int wordPOStype = LRP_PREPROCESSOR_POS_TYPE_UNDEFINED;
	if(!SHAREDvars.textInTextArray(wordPOStypeName, LRPpreprocessorPOStypeNameArray, LRP_PREPROCESSOR_POS_TYPE_ARRAY_NUMBER_OF_TYPES, &wordPOStype))
	{
		cerr << "LRPpreprocessorWordClassClass::getPOStypeFromName{} error: GIA LRP wordPOStypeName not found in LRPpreprocessorPOStypeNameArray, wordPOStypeName = " << wordPOStypeName << endl;
		exit(EXIT_ERROR);
	}
	return wordPOStype;
}

//based on NLCpreprocessorSentenceClassClass::isStringNumberPreprocessorMath(string phrase)
bool LRPpreprocessorWordClassClass::isStringNumber(const string phrase)
{
	bool stringIsNumber = false;
	if(phrase == STRING_FULLSTOP)
	{
		stringIsNumber = false;
	}
	else
	{
		stringIsNumber = true;
		for(int i=0; i<phrase.length(); i++)
		{
			char c = phrase[i];
			bool numberFound = SHAREDvars.charInCharArray(c, translatorEnglishNumbersNumericalFractionalArray, GIA_TRANSLATOR_ENGLISH_NUMBERS_NUMERICAL_FRACTIONAL_NUMBER_OF_TYPES);
			if(!numberFound)
			{
				stringIsNumber = false;
			}
		}
	}
	
	return stringIsNumber;
}

#ifdef GIA_POS_REL_TRANSLATOR_RULES_TREAT_UNKNOWN_POSTYPES_MID_SENTENCE_CAPITALISED_WORDS_AS_PROPERNOUNS
bool LRPpreprocessorWordClassClass::isMidSentenceUppercaseWordLikelyProperNoun(constEffective LRPpreprocessorPlainTextWord* contextWord)
{
	string wordText = contextWord->tagName;
	int entityIndex = (static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(contextWord))->entityIndex;

	bool foundWordInLists = false;
	if(entityIndex != LRP_NLP_START_ENTITY_INDEX)
	{
		if(wordIsUpperCase(wordText))
		{
			foundWordInLists = true;
		}
	}
	return foundWordInLists;
}
#endif

bool LRPpreprocessorWordClassClass::wordIsUpperCase(string wordText)
{
	bool result = false;
	if(wordText.size() > 0)
	{
		char firstCharacterOfWord = (wordText)[0];
		if(isupper(firstCharacterOfWord))
		{
			result = true;
		}
	}
	return result;
}

int LRPpreprocessorWordClassClass::convertSentenceContentsIndexToEntityIndex(const int sentenceContentsIndex)
{
	int entityIndex = sentenceContentsIndex+LRP_NLP_START_ENTITY_INDEX;
	return entityIndex;
}
int LRPpreprocessorWordClassClass::convertEntityIndexToSentenceContentsIndex(const int entityIndex)
{
	int sentenceContentsIndex = entityIndex-LRP_NLP_START_ENTITY_INDEX;
	return sentenceContentsIndex;
}


