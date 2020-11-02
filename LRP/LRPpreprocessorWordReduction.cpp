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
 * File Name: LRPpreprocessorWordReduction.cpp
 * Author: Richard Bruce Baxter - Copyright (c) 2005-2020 Baxter AI (baxterai.com)
 * Project: Language Reduction Preprocessor
 * Project Version: 3n4a 31-October-2020
 * Requirements: requires plain text file
 * Description: Preprocessor Word Reduction
 * /
 *******************************************************************************/


#include "LRPpreprocessorWordReduction.hpp"
#ifdef GIA_POS_REL_TRANSLATOR_HYBRID_PREFERENCE_NLP_PRELIM_POS_TAGS_OVER_LRP_WORD_TYPE_LISTS
#include "GIAsynRelTranslatorDefs.hpp"
#endif


#ifdef LRP_PREPROCESSOR_WORD_MULTIWORD_REDUCTION

LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo* textLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo;
LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo* queryLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo;
LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo* activeLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo;

multimap<string, LRPpreprocessorMultiwordReductionPhrasalVerbSentence*> phrasalVerbListGlobal;
unordered_map<string, multimap<string, LRPpreprocessorMultiwordReductionBasicSentence*>*> multiwordListsGlobal;	//string: preprocessorPOStype

/*
multimap<string, LRPpreprocessorMultiwordReductionPhrasalVerbSentence*>* LRPpreprocessorWordReductionClass::getPhrasalVerbListGlobal()
{
	return phrasalVerbListGlobal;
}
unordered_map<string, multimap<string, LRPpreprocessorMultiwordReductionBasicSentence*>*>* LRPpreprocessorWordReductionClass::getMultiwordListsGlobal()
{
	return multiwordListsGlobal;
}
*/

bool LRPpreprocessorWordReductionClass::initialiseLRPreduction(unordered_map<string, LRPpreprocessorMultiwordReductionIrregularVerbSentence*>* irregularVerbList)
{
	bool result = true;
	
	if(!loadPhrasalVerbDataAndGenerateAllTenseVariants(LRP_PREPROCESSOR_WORD_PHRASALVERB_DATABASE_FILE_NAME, &phrasalVerbListGlobal, irregularVerbList))
	{
		result = false;
	}	

	int numberOfMultiwordLists;
	vector<string> multiwordListFileNames;
	if(!SHAREDvars.getLinesFromFile(LRP_PREPROCESSOR_WORD_MULTIWORDLISTS_DATABASE_FILE_NAME, &multiwordListFileNames, &numberOfMultiwordLists))
	{
		result = false;
	}
	for(int i=0; i<numberOfMultiwordLists; i++)
	{
		if(multiwordListFileNames[i] != "")
		{				
			int indexOfSpace = multiwordListFileNames[i].find(CHAR_SPACE);
			if(indexOfSpace != CPP_STRING_FIND_RESULT_FAIL_VALUE)
			{
				string preprocessorPOStypeName = multiwordListFileNames[i].substr(0, indexOfSpace);
				string multiwordListFileName = multiwordListFileNames[i].substr(indexOfSpace+1);
				//cout << "preprocessorPOStypeName = " << preprocessorPOStypeName << endl;
				//cout << "multiwordListFileName = " << multiwordListFileName << endl;

				multimap<string, LRPpreprocessorMultiwordReductionBasicSentence*>* multiwordList = new multimap<string, LRPpreprocessorMultiwordReductionBasicSentence*>;
				if(!loadMultiwordWordList(multiwordListFileName, multiwordList))
				{
					result = false;
				}
				multiwordListsGlobal.insert(pair<string, multimap<string, LRPpreprocessorMultiwordReductionBasicSentence*>*>(preprocessorPOStypeName, multiwordList));	
			}
			else
			{
				cerr << "LRPpreprocessorWordReductionClass::initialiseLRP{} error: cannot find space character in multiwordListFileNames[i], i = " << i << endl;
				exit(EXIT_ERROR);
			}
		}
		else
		{
			//skip blank lines
		}
	}
	
	return result;
}

/*
---
LRP rules;
add alternate phrasal verb based on 'or'
add alternate tag based upon '/'
take (at least) first word in phrasal verb definition string as base
generate all tenses variations of the verb based upon a) rules and b) irregular verbs
*/

//NB current implementation cannot take into account 3 alternate tags (ie x/y/z)
bool LRPpreprocessorWordReductionClass::loadPhrasalVerbDataAndGenerateAllTenseVariants(const string phrasalVerbDatabaseFileName, multimap<string, LRPpreprocessorMultiwordReductionPhrasalVerbSentence*>* phrasalVerbList, unordered_map<string, LRPpreprocessorMultiwordReductionIrregularVerbSentence*>* irregularVerbList)
{
	bool result = true;

	LRPpreprocessorMultiwordReductionPhrasalVerbSentence* currentTagInPhrasalVerbList = new LRPpreprocessorMultiwordReductionPhrasalVerbSentence();
	LRPpreprocessorMultiwordReductionPhrasalVerbSentence* recordOfNonAlternateTagInPhrasalVerbList = currentTagInPhrasalVerbList;

	LRPpreprocessorMultiwordReductionPhrasalVerbWord* firstTagInPhrasalVerb = currentTagInPhrasalVerbList->firstTagInSentence;
	LRPpreprocessorMultiwordReductionPhrasalVerbWord* currentTagInPhrasalVerb = firstTagInPhrasalVerb;
	LRPpreprocessorMultiwordReductionPhrasalVerbWord* recordOfNonAlternateTagInPhrasalVerb = currentTagInPhrasalVerb;
	
	ifstream parseFileObject(phrasalVerbDatabaseFileName.c_str());
	if(!parseFileObject.rdbuf()->is_open())
	{
		//txt file does not exist in current directory.
		cout << "Error: Phrasal Verb Database File does not exist in current directory: " << phrasalVerbDatabaseFileName << endl;
		result = false;
	}
	else
	{
		int charCount = 0;
		char currentToken;
		bool parsingVerbBase = true;
		string currentWord = "";
		bool currentWordOptional = false;
		bool currentWordAlternate = false;
		bool currentPhrasalVerbAlternate = false;
		string phrasalVerbBaseNameRecord = "";
		while(parseFileObject.get(currentToken))
		{
			if((currentToken == CHAR_NEWLINE) || (currentToken == CHAR_SPACE))
			{
				if(currentWord == LRP_PREPROCESSOR_WORD_PHRASALVERB_DATABASE_TAG_OR)
				{
					currentTagInPhrasalVerbList->alternateSentence = new LRPpreprocessorMultiwordReductionPhrasalVerbSentence();
					currentTagInPhrasalVerbList = currentTagInPhrasalVerbList->alternateSentence;
					currentTagInPhrasalVerb = currentTagInPhrasalVerbList->firstTagInSentence;
					currentPhrasalVerbAlternate = true;
				}
				else
				{
					//COPY1
					//add LRP tag to LRP string (LRP tag list)
					currentTagInPhrasalVerb->tagName = currentWord;
					if(currentWordOptional)
					{
						currentTagInPhrasalVerb->optional = true;
					}
					if(parsingVerbBase)
					{
						currentTagInPhrasalVerb->base = true;
					}
					else
					{
						if(currentWord == phrasalVerbBaseNameRecord)
						{
							currentTagInPhrasalVerb->base = true;
						}
					}
					bool foundTagArbitraryName = false;
					int i;
					if(SHAREDvars.textInTextArray(currentWord, lrpPhrasalVerbDatabaseTagArbitraryNameArray, LRP_PREPROCESSOR_WORD_PHRASALVERB_DATABASE_TAG_ARBITRARYNAME_SOMEBODY_NUMBER_OF_TYPES, &i))
					{
						foundTagArbitraryName = true;
						currentTagInPhrasalVerb->tagSpecialArbitraryName = true;
						currentTagInPhrasalVerb->tagSpecialArbitraryNameType = i;
					}
					if(currentTagInPhrasalVerb->base)
					{
						bool irregularVerbFound = generateIrregularTenseVariantsOfVerbBase(currentTagInPhrasalVerb, irregularVerbList, LRP_PREPROCESSOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY_VALUE);
						generateStandardTenseVariantsOfVerbBase(currentTagInPhrasalVerb, irregularVerbFound, LRP_PREPROCESSOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY_VALUE);
					}


					currentTagInPhrasalVerb->nextTag = new LRPpreprocessorMultiwordReductionPhrasalVerbWord();
					currentTagInPhrasalVerb = static_cast<LRPpreprocessorMultiwordReductionPhrasalVerbWord*>(currentTagInPhrasalVerb->nextTag);
					if(currentWordAlternate)
					{
						//revert to non-alternate tag...
						currentTagInPhrasalVerb = recordOfNonAlternateTagInPhrasalVerb;
						currentTagInPhrasalVerb->nextTag = new LRPpreprocessorMultiwordReductionPhrasalVerbWord();
						currentTagInPhrasalVerb = static_cast<LRPpreprocessorMultiwordReductionPhrasalVerbWord*>(currentTagInPhrasalVerb->nextTag);
					}

					if(currentToken == CHAR_NEWLINE)
					{
						if(currentPhrasalVerbAlternate)
						{
							//revert to non-alternate phrasal verb...
							currentTagInPhrasalVerbList = recordOfNonAlternateTagInPhrasalVerbList;
							currentPhrasalVerbAlternate = false;
						}
						
						phrasalVerbList->insert(pair<string, LRPpreprocessorMultiwordReductionPhrasalVerbSentence*>(firstTagInPhrasalVerb->tagName, currentTagInPhrasalVerbList));
						if(firstTagInPhrasalVerb->base)
						{	
							//for each variant of the firstTagInPhrasalVerb, insert the sentence into the list keyed by the variant name
							for(int i=0; i<LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_MAX_NUM_TENSE_FORMS; i++)
							{
								for(int j=0; j<firstTagInPhrasalVerb->grammaticalTenseFormsArray[i].size(); j++)
								{
									string verbVariant = (firstTagInPhrasalVerb->grammaticalTenseFormsArray[i])[j];
									phrasalVerbList->insert(pair<string, LRPpreprocessorMultiwordReductionPhrasalVerbSentence*>(verbVariant, currentTagInPhrasalVerbList));
								}
							}
						}
						
						currentTagInPhrasalVerbList = new LRPpreprocessorMultiwordReductionPhrasalVerbSentence();
						firstTagInPhrasalVerb = currentTagInPhrasalVerbList->firstTagInSentence;
						currentTagInPhrasalVerb = firstTagInPhrasalVerb; 
						recordOfNonAlternateTagInPhrasalVerbList = currentTagInPhrasalVerbList;
						recordOfNonAlternateTagInPhrasalVerb = currentTagInPhrasalVerb;
					}

					if(!currentWordAlternate)
					{
						recordOfNonAlternateTagInPhrasalVerb = currentTagInPhrasalVerb;
					}
					if(parsingVerbBase)
					{
						phrasalVerbBaseNameRecord = currentWord;
					}
				}

				parsingVerbBase = false;
				//currentWordOptional = false;	//removed 1p1aTEMP5b
				currentWordAlternate = false;
				currentWord = "";
				if((currentToken == CHAR_NEWLINE) || (currentWord == LRP_PREPROCESSOR_WORD_PHRASALVERB_DATABASE_TAG_OR))
				{
					parsingVerbBase = true;
					phrasalVerbBaseNameRecord = "";
				}
				//start parsing new

			}
			else if(currentToken == LRP_PREPROCESSOR_WORD_PHRASALVERB_DATABASE_TAG_ALTERNATE)
			{

				//COPY2

				currentTagInPhrasalVerb->tagName = currentWord;		//NB current implementation can take into account 2 or more alternate tags (eg x/y/z)...
				if(currentWordOptional)
				{
					currentTagInPhrasalVerb->optional = true;
				}
				if(parsingVerbBase)
				{
					currentTagInPhrasalVerb->base = true;
				}
				else
				{
					if(currentWord == phrasalVerbBaseNameRecord)
					{
						currentTagInPhrasalVerb->base = true;
					}
				}
				bool foundTagArbitraryName = false;
				int i;
				if(SHAREDvars.textInTextArray(currentWord, lrpPhrasalVerbDatabaseTagArbitraryNameArray, LRP_PREPROCESSOR_WORD_PHRASALVERB_DATABASE_TAG_ARBITRARYNAME_SOMEBODY_NUMBER_OF_TYPES, &i))
				{
					foundTagArbitraryName = true;
					currentTagInPhrasalVerb->tagSpecialArbitraryName = true;
					currentTagInPhrasalVerb->tagSpecialArbitraryNameType = i;
				}
				if(currentTagInPhrasalVerb->base)
				{
					bool irregularVerbFound = generateIrregularTenseVariantsOfVerbBase(currentTagInPhrasalVerb, irregularVerbList, LRP_PREPROCESSOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY_VALUE);
					generateStandardTenseVariantsOfVerbBase(currentTagInPhrasalVerb, irregularVerbFound, LRP_PREPROCESSOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY_VALUE);								
				}
				if(!currentWordAlternate)
				{//added 1p1aTEMP6d
					recordOfNonAlternateTagInPhrasalVerb = currentTagInPhrasalVerb;
				}

				currentTagInPhrasalVerb->alternateTag = new LRPpreprocessorMultiwordReductionPhrasalVerbWord();
				currentTagInPhrasalVerb = currentTagInPhrasalVerb->alternateTag;

				//wordIndex does not change...
				//parsingVerbBase = false;		//should not be parsing verb base here, as it should never have alternate cases eg base/base
				currentWordAlternate = true;
				//currentWordOptional = false;	//removed 1p1aTEMP5b
				currentWord = "";
			}
			else if(currentToken == LRP_PREPROCESSOR_WORD_PHRASALVERB_DATABASE_TAG_OPEN_OPTIONAL)
			{
				currentWordOptional = true;
			}
			else if(currentToken == LRP_PREPROCESSOR_WORD_PHRASALVERB_DATABASE_TAG_CLOSE_OPTIONAL)
			{//moved 1p1aTEMP5b
				currentWordOptional = false;
			}
			#ifdef LRP_PREPROCESSOR_WORD_NLP_PARSABLE_PHRASE_SUPPORT_APOSTROPHES_POSSESSION_AND_OMISSION
			else if(currentToken == CHAR_APOSTROPHE)
			{
				currentTagInPhrasalVerb->tagName = currentWord;
				currentTagInPhrasalVerb->nextTag = new LRPpreprocessorMultiwordReductionPhrasalVerbWord();
				currentTagInPhrasalVerb = static_cast<LRPpreprocessorMultiwordReductionPhrasalVerbWord*>(currentTagInPhrasalVerb->nextTag);
				currentWord = "";
				currentWord = currentWord + currentToken;
			}
			#endif
			else
			{
				currentWord = currentWord + currentToken;
			}

			charCount++;
		}
		parseFileObject.close();
	}




	return result;
}








LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo* LRPpreprocessorWordReductionClass::getActiveLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo()
{
	return activeLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo;
}
void LRPpreprocessorWordReductionClass::setActiveLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo(const bool isQuery)
{
	if(isQuery)
	{
		activeLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo = queryLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo;
	}
	else
	{
		activeLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo = textLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo;
	}
}
void LRPpreprocessorWordReductionClass::initialiseActiveLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo(const bool isQuery)
{
	if(isQuery)
	{
		queryLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo = new LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo();
	}
	else
	{
		textLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo = new LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo();
	}
}
void LRPpreprocessorWordReductionClass::deinitialiseActiveLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo(const bool isQuery)
{
	if(isQuery)
	{
		delete queryLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo;
	}
	else
	{
		delete textLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo;
	}
}


bool LRPpreprocessorWordReductionClass::parseTextFileAndReduceLanguage(LRPpreprocessorSentence* firstLRPpreprocessorSentenceInList, const string plainTextLRPoutputFileName, const string plainTextLRPforNLPoutputFileName)
{
	bool result = true;

	string currentFolder = SHAREDvars.getCurrentDirectory();

	#ifdef LRP_PREPROCESSOR_WORD_COLLAPSE_NUMERICAL_WORDS_TO_NUMBERS
	if(!replaceAllNumericalWordsWithNumbers(firstLRPpreprocessorSentenceInList))
	{
		result = false;
	}
	#endif

	//cout << "searchAndReplacePhrasalVerbs" << endl;
	LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo* firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo = getActiveLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo();
	if(!searchAndReplacePhrasalVerbs(firstLRPpreprocessorSentenceInList, &phrasalVerbListGlobal, firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo))
	{
		result = false;
	}

	#ifdef LRP_PREPROCESSOR_WORD_ORIGINAL
	for(unordered_map<string, multimap<string, LRPpreprocessorMultiwordReductionBasicSentence*>*>::iterator multiwordListsIter = multiwordListsGlobal.begin(); multiwordListsIter != multiwordListsGlobal.end(); multiwordListsIter++)
	{
		string multiwordListName = multiwordListsIter->first;
		multimap<string, LRPpreprocessorMultiwordReductionBasicSentence*>* multiwordList = multiwordListsIter->second;
		int multiwordListType = LRPpreprocessorWordClassObject.getPOStypeFromName(multiwordListName);
		if(!searchAndReplaceMultiwordWordList(firstLRPpreprocessorSentenceInList, multiwordList, firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo, multiwordListType))
		{
			result = false;
		}
	}
	#endif		

	#ifdef LRP_PREPROCESSOR_WORD_DYNAMIC
	for(unordered_map<string, unordered_map<string, LRPpreprocessorMultiwordReductionWord*>*>::iterator wordListsIter = wordListsGlobal.begin(); wordListsIter != wordListsGlobal.end(); wordListsIter++)
	{
		string wordListName = wordListsIter->first;
		unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* wordList = wordListsIter->second;
		int wordListType = LRPpreprocessorWordClassObject.getPOStypeFromName(wordListName);
		if(!searchAndReplaceMultiwordWordListDynamic(firstLRPpreprocessorSentenceInList, wordList, firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo, wordListType))
		{
			result = false;
		}			
	}
	#endif
	
	SHAREDvars.setCurrentDirectory(outputFolder);
	if(!writeTagListToFile(firstLRPpreprocessorSentenceInList, plainTextLRPoutputFileName, plainTextLRPforNLPoutputFileName, true, true))
	{
		result = false;
	}
	SHAREDvars.setCurrentDirectory(currentFolder);
	
	return result;
}





	

//NB the collapsed phrasal verb contains precisely 2 entities: phrasalVerbCollapsed, entity2: thing/place/body (eg belong_to + sb/Tom) - thing/place/bodies are not currently being differentiated by the LRP as this information is only first generated at NLP/GIA parse stage
bool LRPpreprocessorWordReductionClass::searchAndReplacePhrasalVerbs(LRPpreprocessorSentence* firstLRPpreprocessorSentenceInList, multimap<string, LRPpreprocessorMultiwordReductionPhrasalVerbSentence*>* phrasalVerbList, LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo* firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo)
{
	bool result = true;

	LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo* currentCorrespondenceInfo = firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo;	//new correspondence info for each found phrasal verb
	
	//not required because searchAndReplacePhrasalVerbs is called first by parseTextFileAndReduceLanguage:
	while(currentCorrespondenceInfo->next != NULL)
	{
		currentCorrespondenceInfo = currentCorrespondenceInfo->next;	//added 3f1a
	}
	
	LRPpreprocessorSentence* currentLRPpreprocessorSentenceInList = firstLRPpreprocessorSentenceInList;
	while(currentLRPpreprocessorSentenceInList->next != NULL)
	{
		LRPpreprocessorMultiwordReductionPlainTextWord* firstTagInPlainTextSentence = NULL;
		//cout << "LRPpreprocessorSentenceClassObject.getSentenceContents(currentLRPpreprocessorSentenceInList)->size() = " << LRPpreprocessorSentenceClassObject.getSentenceContents(currentLRPpreprocessorSentenceInList)->size() << endl;
		LRPpreprocessorWordClassObject.generateFlatSentenceWordList(LRPpreprocessorSentenceClassObject.getSentenceContents(currentLRPpreprocessorSentenceInList), &firstTagInPlainTextSentence);
		LRPpreprocessorMultiwordReductionPlainTextWord* currentTagInPlainTextSentence = firstTagInPlainTextSentence;
		LRPpreprocessorMultiwordReductionPlainTextWord* previousTagInPlainTextSentence = NULL;
		while(currentTagInPlainTextSentence->nextTag != NULL)
		{
			bool foundAtLeastOnePhrasalVerbInSentenceAndCollapsed = false;
			int numberWordsInMultiwordMatched = 0;
			
			pair<multimap<string, LRPpreprocessorMultiwordReductionPhrasalVerbSentence*>::iterator, multimap<string, LRPpreprocessorMultiwordReductionPhrasalVerbSentence*>::iterator> range;
			range = phrasalVerbList->equal_range(currentTagInPlainTextSentence->tagName);
			for(multimap<string, LRPpreprocessorMultiwordReductionPhrasalVerbSentence*>::iterator currentTagInPhrasalVerbListNormOrAlternateIterator = range.first; currentTagInPhrasalVerbListNormOrAlternateIterator != range.second; ++currentTagInPhrasalVerbListNormOrAlternateIterator)
			{	
				LRPpreprocessorMultiwordReductionPhrasalVerbSentence* currentTagInPhrasalVerbListNormOrAlternate = currentTagInPhrasalVerbListNormOrAlternateIterator->second;
				
				bool alternatePhrasalVerb = false;
				while(!alternatePhrasalVerb || (currentTagInPhrasalVerbListNormOrAlternate->alternateSentence != NULL))
				{
					/*
					if(tolower((currentTagInPhrasalVerbList->tagName)[0]) == tolower((currentTagInPlainTextSentence->tagName)[0]))
					{//optimisation; only deal with phrasal verbs that start with the same character...
					*/
					LRPpreprocessorMultiwordReductionPhrasalVerbWord* firstTagInPhrasalVerb = currentTagInPhrasalVerbListNormOrAlternate->firstTagInSentence;
								
					LRPpreprocessorMultiwordReductionPhrasalVerbWord* currentTagInPhrasalVerb = firstTagInPhrasalVerb;
					bool foundPhrasalVerbInSentenceAndCollapsed = false;
					bool stillFoundVerbMatchOfArbitraryTense = true;
					int numberWordsInMultiword = 0;
					bool foundAtLeastOneMatch = false;
					LRPpreprocessorMultiwordReductionPlainTextWord* currentTagInPlainTextSentenceTemp = currentTagInPlainTextSentence;
					LRPpreprocessorMultiwordReductionPlainTextWord* lastTagInPlainTextSentenceTemp = NULL;	//pointer used to change original plain text to collapsed phrasal verb text
					LRPpreprocessorMultiwordReductionPlainTextWord* firstTagInCollapsedPhrasalVerb = new LRPpreprocessorMultiwordReductionPlainTextWord();
					LRPpreprocessorMultiwordReductionPlainTextWord* currentTagInCollapsedPhrasalVerb = firstTagInCollapsedPhrasalVerb;
					bool currentlyParsingTagSpecial = false;
					int tagInPhrasalVerbSpecialAndNotFoundCount = 0;
					int numberTagSpecialTagsFound = 0;
					int numberTagConsecutiveOptionalTagsFound = 0;
					bool phrasalVerbHasTagSpecial = false;
					while((currentTagInPhrasalVerb->nextTag != NULL) && (currentTagInPlainTextSentenceTemp->nextTag != NULL) && (stillFoundVerbMatchOfArbitraryTense))
					{
						bool currentTagInPhrasalVerbOptionalAndNotFound = false;
						bool currentTagInPhrasalVerbSpecialAndNotFound = false;
						
						if(currentTagInPhrasalVerb->tagSpecialArbitraryName)
						{
							phrasalVerbHasTagSpecial = true;
							//NB the collapsed phrasal verb contains precisely 2 entities: phrasalVerbCollapsed, entity2: thing/place/body (eg belong_to + sb/Tom) - thing/place/bodies are not currently being differentiated by the LRP as this information is only first generated at NLP/GIA parse stage
							currentTagInCollapsedPhrasalVerb->nextTag = new LRPpreprocessorMultiwordReductionPlainTextWord();
							LRPpreprocessorMultiwordReductionPlainTextWord* currentTagInCollapsedPhrasalVerbSpecialArbitrarySection = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(currentTagInCollapsedPhrasalVerb->nextTag);
							currentTagInCollapsedPhrasalVerbSpecialArbitrarySection->tagName = currentTagInPlainTextSentenceTemp->tagName;		//arbitrary thing/place/body name
							currentTagInCollapsedPhrasalVerbSpecialArbitrarySection->collapsedPhrasalVerbExactDefinedSection = false;
							//currentTagInCollapsedPhrasalVerbSpecialArbitrarySection->collapsedPhrasalVerbExactDefinedSectionTemp = false;
							/*
							//removed 7 Sept 2012b - now interpret: ask sb over/round -> ask_over sb
							currentTagInCollapsedPhrasalVerb = currentTagInCollapsedPhrasalVerb->nextTag;
							*/
				
							currentlyParsingTagSpecial = true;
							numberTagSpecialTagsFound++;
						}
						else
						{
							bool foundVerbMatchOfArbitraryTenseTemp = false;
							LRPpreprocessorMultiwordReductionPhrasalVerbWord* currentTagInPhrasalVerbNormOrAlternate = currentTagInPhrasalVerb;
							bool normalTag = true;
							bool alternateTag = false;
							string generatedTagNameLemma = "";
							while(normalTag || alternateTag)
							{
								if(currentTagInPhrasalVerb->base)
								{
									for(int i=0; i<LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_MAX_NUM_TENSE_FORMS; i++)
									{
										for(int j=0; j<currentTagInPhrasalVerbNormOrAlternate->grammaticalTenseFormsArray[i].size(); j++)
										{
											if((currentTagInPhrasalVerbNormOrAlternate->grammaticalTenseFormsArray[i])[j] == currentTagInPlainTextSentenceTemp->tagName)		//USED TO BE currentTagInCollapsedPhrasalVerb before 6 Sept 2012
											{
												currentTagInCollapsedPhrasalVerb->grammaticalTenseFormDetected = i;								//USED TO BE AND STILL IS currentTagInCollapsedPhrasalVerb before 6 Sept 2012
												generatedTagNameLemma = currentTagInPhrasalVerbNormOrAlternate->grammaticalTenseFormsArray[LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_INFINITIVE][0];
												foundVerbMatchOfArbitraryTenseTemp = true;
											}
										}
									}
								}
								else
								{
									if(currentTagInPhrasalVerbNormOrAlternate->tagName == currentTagInPlainTextSentenceTemp->tagName)
									{
										foundVerbMatchOfArbitraryTenseTemp = true;
									}
								}
								if(currentTagInPhrasalVerbNormOrAlternate->alternateTag != NULL)
								{
									currentTagInPhrasalVerbNormOrAlternate = currentTagInPhrasalVerbNormOrAlternate->alternateTag;
									alternateTag = true;
								}
								else
								{
									alternateTag = false;
								}
								normalTag = false;
							}


							if(foundVerbMatchOfArbitraryTenseTemp)
							{
								if(currentlyParsingTagSpecial)
								{
									/*
									//removed 7 Sept 2012b - now interpret: ask sb over/round -> ask_over sb
									currentTagInCollapsedPhrasalVerb->nextTag = new LRPpreprocessorMultiwordReductionWord();
									currentTagInCollapsedPhrasalVerb = currentTagInCollapsedPhrasalVerb->nextTag;
									*/
									currentlyParsingTagSpecial = false;
								}

								#ifdef GIA_DEBUG_PREPROCESSOR
								cout << "numberWordsInMultiword++" << endl;
								cout << "currentTagInPhrasalVerb = " << currentTagInPhrasalVerb->tagName << endl;
								cout << "currentTagInPlainTextSentenceTemp = " << currentTagInPlainTextSentenceTemp->tagName << endl;
								cout << "generateTextFromPreprocessorSentenceWordList(firstTagInPhrasalVerb) = " << LRPpreprocessorWordClassObject.generateTextFromPreprocessorSentenceWordList(firstTagInPhrasalVerb) << endl;
								#endif
								
								foundAtLeastOneMatch = true;
								numberWordsInMultiword++;
								currentTagInCollapsedPhrasalVerb->collapsedPhrasalVerbExactDefinedSection = true;
								currentTagInCollapsedPhrasalVerb->collapsedMultiwordWordType = LRP_PREPROCESSOR_POS_TYPE_VERB;	//added 3f1a
								//currentTagInCollapsedPhrasalVerb->collapsedPhrasalVerbExactDefinedSectionTemp = true;
								currentTagInCollapsedPhrasalVerb->tagName = currentTagInCollapsedPhrasalVerb->tagName + currentTagInPlainTextSentenceTemp->tagName + LRP_TRANSLATOR_UNIQUE_CONCATENATION_TYPES_MULTIWORD_WORD_DELIMITER;		//this is part of the defined/main section of the phrasal verb
								if(currentTagInPhrasalVerb->base)
								{
									currentTagInCollapsedPhrasalVerb->tagNameLemma = currentTagInCollapsedPhrasalVerb->tagNameLemma + generatedTagNameLemma + LRP_TRANSLATOR_UNIQUE_CONCATENATION_TYPES_MULTIWORD_WORD_DELIMITER;
								}
								else
								{
									currentTagInCollapsedPhrasalVerb->tagNameLemma = currentTagInCollapsedPhrasalVerb->tagNameLemma + currentTagInPlainTextSentenceTemp->tagName + LRP_TRANSLATOR_UNIQUE_CONCATENATION_TYPES_MULTIWORD_WORD_DELIMITER;
								}
							}
							else
							{
								if(currentTagInPhrasalVerb->optional)
								{
									currentTagInPhrasalVerbOptionalAndNotFound = true;
								}
								else if(currentlyParsingTagSpecial && (tagInPhrasalVerbSpecialAndNotFoundCount <= LRP_PREPROCESSOR_WORD_PHRASALVERB_DATABASE_TAG_ARBITRARYNAME_MAX_NUMBER_WORDS))
								{
									//changed 2b2e - 24 May 2017
									LRPpreprocessorMultiwordReductionPlainTextWord* currentTagInCollapsedPhrasalVerbSpecialArbitrarySection = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(currentTagInCollapsedPhrasalVerb->nextTag);
									while(currentTagInCollapsedPhrasalVerbSpecialArbitrarySection->nextTag != NULL)
									{
										currentTagInCollapsedPhrasalVerbSpecialArbitrarySection = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(currentTagInCollapsedPhrasalVerbSpecialArbitrarySection->nextTag);
									}
									currentTagInCollapsedPhrasalVerbSpecialArbitrarySection->nextTag = new LRPpreprocessorMultiwordReductionPlainTextWord();
									currentTagInCollapsedPhrasalVerbSpecialArbitrarySection = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(currentTagInCollapsedPhrasalVerbSpecialArbitrarySection->nextTag);
									currentTagInCollapsedPhrasalVerbSpecialArbitrarySection->tagName = currentTagInPlainTextSentenceTemp->tagName;		//arbitrary thing/place/body name
									currentTagInCollapsedPhrasalVerbSpecialArbitrarySection->collapsedPhrasalVerbExactDefinedSection = false;
									//currentTagInCollapsedPhrasalVerbSpecialArbitrarySection->collapsedPhrasalVerbExactDefinedSectionTemp = false;
									/*
									//removed 7 Sept 2012b - now interpret: ask sb over/round -> ask_over sb
									currentTagInCollapsedPhrasalVerb->tagName = currentTagInCollapsedPhrasalVerb->tagName + currentTagInPlainTextSentenceTemp->tagName;
									*/
									currentTagInPhrasalVerbSpecialAndNotFound = true;
									tagInPhrasalVerbSpecialAndNotFoundCount++;
								}
								else
								{
									stillFoundVerbMatchOfArbitraryTense = false;
								}						
							}
						}

						if(currentTagInPhrasalVerb->tagSpecialArbitraryName)
						{
							if(currentTagInPhrasalVerb->optional)
							{
								//special+optional tag found - just increment the phrasal verb for now [for this round]...
								currentTagInPhrasalVerb = static_cast<LRPpreprocessorMultiwordReductionPhrasalVerbWord*>(currentTagInPhrasalVerb->nextTag);
							}
							else
							{
								currentTagInPhrasalVerb = static_cast<LRPpreprocessorMultiwordReductionPhrasalVerbWord*>(currentTagInPhrasalVerb->nextTag);
								currentTagInPlainTextSentenceTemp = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(currentTagInPlainTextSentenceTemp->nextTag);
							}
						}
						else if(currentTagInPhrasalVerbOptionalAndNotFound)
						{
							//optional phrasal verb tag not found - just increment the phrasal verb tag list...
							currentTagInPhrasalVerb = static_cast<LRPpreprocessorMultiwordReductionPhrasalVerbWord*>(currentTagInPhrasalVerb->nextTag);
						}
						else if(currentTagInPhrasalVerbSpecialAndNotFound)
						{
							//next phrasal verb tag not found yet - just increment the sentence tag list...
							currentTagInPlainTextSentenceTemp = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(currentTagInPlainTextSentenceTemp->nextTag);
						}
						else
						{
							currentTagInPhrasalVerb = static_cast<LRPpreprocessorMultiwordReductionPhrasalVerbWord*>(currentTagInPhrasalVerb->nextTag);
							currentTagInPlainTextSentenceTemp = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(currentTagInPlainTextSentenceTemp->nextTag);
						}
					}

					if(stillFoundVerbMatchOfArbitraryTense && foundAtLeastOneMatch)
					{
						if(currentTagInPhrasalVerb->nextTag == NULL)
						{//make sure the entire multiword phrasal verb is matched (in case currentTagInPlainTextSentenceTemp reaches end of sentence without matching the multiword phrasal verb in its entirety)

							if(numberTagSpecialTagsFound <= 1)
							{//do not preprocess phrasal verbs with more than one special tag (ie sth/sb/swh) - as this generally involves more than a verb [verb sth preposition sth1] - added 1p1aTEMP5
								
								//reduce all entities
								
								numberWordsInMultiwordMatched = numberWordsInMultiword;
								
								#ifdef LRP_PREPROCESSOR_RECORD_REFERENCES
								//updated 3b4b
								//assume firstTagInCollapsedMultiwordWord->preprocessorUpperLevelWordReference already set
								LRPpreprocessorMultiwordReductionPlainTextWord* currentTagInPlainTextSentenceTemp2 = currentTagInPlainTextSentence;
								for(int i=0; i<numberWordsInMultiwordMatched; i++)
								{
									if(currentTagInPlainTextSentenceTemp2->preprocessorUpperLevelWordReference != NULL)	//added 3d8a
									{
										currentTagInPlainTextSentenceTemp2->preprocessorUpperLevelWordReference->preprocessorLowerLevelWordReference = firstTagInCollapsedPhrasalVerb;
									}
									currentTagInPlainTextSentenceTemp2 = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(currentTagInPlainTextSentenceTemp2->nextTag);	
								}
								firstTagInCollapsedPhrasalVerb->preprocessorUpperLevelWordReference = currentTagInPlainTextSentence->preprocessorUpperLevelWordReference;
								firstTagInCollapsedPhrasalVerb->preprocessorUpperLevelWordReferenceSize = numberWordsInMultiwordMatched;
								#endif							
								
								if(phrasalVerbHasTagSpecial)
								{
									currentTagInCollapsedPhrasalVerb->nextTag->nextTag = currentTagInPlainTextSentenceTemp;		//NB currentTagInCollapsedPhrasalVerb->nextTag is the collapsed phrasal verb thing/place/body entity
								}
								else
								{
									currentTagInCollapsedPhrasalVerb->nextTag = currentTagInPlainTextSentenceTemp;
								}
								if(previousTagInPlainTextSentence != NULL)
								{
									previousTagInPlainTextSentence->nextTag = firstTagInCollapsedPhrasalVerb;
								}
								else
								{
									firstTagInPlainTextSentence = firstTagInCollapsedPhrasalVerb;								
								}
								
								foundAtLeastOnePhrasalVerbInSentenceAndCollapsed = true;
								foundPhrasalVerbInSentenceAndCollapsed = true;
							}
						}
					}
					if(!foundPhrasalVerbInSentenceAndCollapsed)
					{
						delete firstTagInCollapsedPhrasalVerb;
					}
					/*
					}
					*/
					if(currentTagInPhrasalVerbListNormOrAlternate->alternateSentence != NULL)
					{
						currentTagInPhrasalVerbListNormOrAlternate = currentTagInPhrasalVerbListNormOrAlternate->alternateSentence;
					}
					alternatePhrasalVerb = true;
				}
				/*
				if(currentTagInPhrasalVerbList->alternateSentence != NULL)
				{
					currentTagInPhrasalVerbList = currentTagInPhrasalVerbList->alternateSentence;
				}
				*/
			}
			if(foundAtLeastOnePhrasalVerbInSentenceAndCollapsed)
			{
				#ifdef GIA_DEBUG_PREPROCESSOR
				cout << "foundAtLeastOnePhrasalVerbInSentenceAndCollapsed; currentTagInPlainTextSentence = " << currentTagInPlainTextSentence->tagName << endl;
				#endif
				
				//renumberEntityIndiciesInCorrespondenceInfo(firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo, currentLRPpreprocessorSentenceInList->sentenceIndexOriginal, numberWordsInMultiwordMatched); - this is not required because searchAndReplaceMultiwordWordList is executed after searchAndReplacePhrasalVerbs
				LRPpreprocessorMultiwordReductionPlainTextWord* currentTagInPlainTextSentenceTemp2 = firstTagInPlainTextSentence;
				int newEntityIndex = LRP_NLP_START_ENTITY_INDEX;
				while(currentTagInPlainTextSentenceTemp2->nextTag != NULL)
				{
					//if(currentTagInPlainTextSentenceTemp2->collapsedPhrasalVerbExactDefinedSectionTemp)
					if(currentTagInPlainTextSentenceTemp2->collapsedPhrasalVerbExactDefinedSection)
					{//create a new correspondenceInfo
											
						//currentTagInPlainTextSentenceTemp2->collapsedPhrasalVerbExactDefinedSectionTemp = false;
						string tagNameLemma = currentTagInPlainTextSentenceTemp2->tagNameLemma;
						string tagName = currentTagInPlainTextSentenceTemp2->tagName;
						string tagNameLemmaWithLastLetterDropped = tagNameLemma.substr(0, tagNameLemma.length()-1);
						string tagNameWithLastLetterDropped = tagName.substr(0, tagName.length()-1);
						currentTagInPlainTextSentenceTemp2->tagNameLemma = tagNameLemmaWithLastLetterDropped;
						currentTagInPlainTextSentenceTemp2->tagName = tagNameWithLastLetterDropped;
						currentCorrespondenceInfo->sentenceIndex = currentLRPpreprocessorSentenceInList->sentenceIndexOriginal;
						currentCorrespondenceInfo->entityIndex = newEntityIndex;
						currentCorrespondenceInfo->lemmaWithLRP = currentTagInPlainTextSentenceTemp2->tagNameLemma;
						currentCorrespondenceInfo->wordWithLRP = currentTagInPlainTextSentenceTemp2->tagName;
						currentCorrespondenceInfo->wordWithLRPforNLPonly = generateWordWithLRPforNLPonly(currentTagInPlainTextSentenceTemp2);
						currentCorrespondenceInfo->next = new LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo();
						currentCorrespondenceInfo = currentCorrespondenceInfo->next;
						
						currentTagInPlainTextSentence = currentTagInPlainTextSentenceTemp2;	//added 3d8a
					}
					currentTagInPlainTextSentenceTemp2->entityIndex = newEntityIndex;
					currentTagInPlainTextSentenceTemp2 = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(currentTagInPlainTextSentenceTemp2->nextTag);
					newEntityIndex++;
				}
			}
			
			previousTagInPlainTextSentence = currentTagInPlainTextSentence;
			currentTagInPlainTextSentence = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(currentTagInPlainTextSentence->nextTag);
		}

		currentTagInPlainTextSentence = firstTagInPlainTextSentence;
		
		#ifdef LRP_PREPROCESSOR_DEBUG
		cout << "\t";
		while(currentTagInPlainTextSentence->nextTag != NULL)
		{
			cout << currentTagInPlainTextSentence->tagName << " ";
			currentTagInPlainTextSentence = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(currentTagInPlainTextSentence->nextTag);
		}
		cout << endl;
		#endif
		
		LRPpreprocessorWordClassObject.generateSentenceWordList(firstTagInPlainTextSentence, LRPpreprocessorSentenceClassObject.getSentenceContents(currentLRPpreprocessorSentenceInList));

		currentLRPpreprocessorSentenceInList = currentLRPpreprocessorSentenceInList->next;
	}

	return result;
}

	
bool LRPpreprocessorWordReductionClass::loadMultiwordWordList(const string multiwordWordListFileName, multimap<string, LRPpreprocessorMultiwordReductionBasicSentence*>* multiwordWordList)
{
	bool result = true;

	LRPpreprocessorMultiwordReductionWord* firstTagInMultiwordWord = new LRPpreprocessorMultiwordReductionWord();
	LRPpreprocessorMultiwordReductionWord* currentTagInMultiwordWord = firstTagInMultiwordWord;

	ifstream parseFileObject(multiwordWordListFileName.c_str());
	if(!parseFileObject.rdbuf()->is_open())
	{
		//txt file does not exist in current directory.
		cout << "Error: Multiword word Database File does not exist in current directory: " << multiwordWordListFileName << endl;
		result = false;
	}
	else
	{
		int charCount = 0;
		char currentToken;
		string currentMultiwordWord = "";	//not used [multiword words are matched word by word; like phrasal verbs]
		string currentWord = "";
		while(parseFileObject.get(currentToken))
		{
			if((currentToken == CHAR_NEWLINE) || (currentToken == CHAR_SPACE))
			{
				currentTagInMultiwordWord->tagName = currentWord;
				currentTagInMultiwordWord->nextTag = new LRPpreprocessorMultiwordReductionWord();
				currentTagInMultiwordWord = static_cast<LRPpreprocessorMultiwordReductionWord*>(currentTagInMultiwordWord->nextTag);
				if(currentToken == CHAR_NEWLINE)
				{
					LRPpreprocessorMultiwordReductionBasicSentence* sentence = new LRPpreprocessorMultiwordReductionBasicSentence();
					sentence->firstTagInSentence = firstTagInMultiwordWord;
					multiwordWordList->insert(pair<string, LRPpreprocessorMultiwordReductionBasicSentence*>(firstTagInMultiwordWord->tagName, sentence));
					firstTagInMultiwordWord = new LRPpreprocessorMultiwordReductionWord();
					currentTagInMultiwordWord = firstTagInMultiwordWord;
					currentMultiwordWord = "";
				}
				else if(currentToken == CHAR_SPACE)
				{
					currentMultiwordWord = currentMultiwordWord + currentToken;
				}
				currentWord = "";
			}
			else
			{
				currentWord = currentWord + currentToken;
				currentMultiwordWord = currentMultiwordWord + currentToken;
			}

			charCount++;
		}
		parseFileObject.close();
	}
	return result;
}

bool LRPpreprocessorWordReductionClass::searchAndReplaceMultiwordWordList(LRPpreprocessorSentence* firstLRPpreprocessorSentenceInList, multimap<string, LRPpreprocessorMultiwordReductionBasicSentence*>* multiwordWordList, LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo* firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo, const int wordListType)
{
	bool result = true;
	
	LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo* currentCorrespondenceInfo = firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo;	//new correspondence info for each found multiword word
	while(currentCorrespondenceInfo->next != NULL)
	{
		currentCorrespondenceInfo = currentCorrespondenceInfo->next;	//added 2j6d (add to end of list)
	}
		
	int sentenceIndexTemp = 0;
	
	LRPpreprocessorSentence* currentLRPpreprocessorSentenceInList = firstLRPpreprocessorSentenceInList;
	while(currentLRPpreprocessorSentenceInList->next != NULL)
	{	
		//cout << "sentenceIndexTemp = " << sentenceIndexTemp << endl;
	
		LRPpreprocessorMultiwordReductionPlainTextWord* firstTagInPlainTextSentence = NULL;
		LRPpreprocessorWordClassObject.generateFlatSentenceWordList(LRPpreprocessorSentenceClassObject.getSentenceContents(currentLRPpreprocessorSentenceInList), &firstTagInPlainTextSentence);
		LRPpreprocessorMultiwordReductionPlainTextWord* currentTagInPlainTextSentence = firstTagInPlainTextSentence;
		LRPpreprocessorMultiwordReductionPlainTextWord* previousTagInPlainTextSentence = NULL;

		int entityIndex = LRP_NLP_START_ENTITY_INDEX;
		while(currentTagInPlainTextSentence->nextTag != NULL)
		{						
			bool foundAtLeastOneMultiwordWordInSentenceAndCollapsed = false;
			int numberWordsInMultiwordMatched = 0;
			
			pair<multimap<string, LRPpreprocessorMultiwordReductionBasicSentence*>::iterator, multimap<string, LRPpreprocessorMultiwordReductionBasicSentence*>::iterator> range;
			range = multiwordWordList->equal_range(currentTagInPlainTextSentence->tagName);
			for(multimap<string, LRPpreprocessorMultiwordReductionBasicSentence*>::iterator currentTagInMultiwordWordListIterator = range.first; currentTagInMultiwordWordListIterator != range.second; ++currentTagInMultiwordWordListIterator)
			{	
				LRPpreprocessorMultiwordReductionBasicSentence* currentTagInMultiwordWordList = currentTagInMultiwordWordListIterator->second;
				//cout << "currentTagInMultiwordWordList->firstTagInSentence->tagName = " << currentTagInMultiwordWordList->firstTagInSentence->tagName << endl;
		
				bool foundMultiwordWordInSentenceAndCollapsed = false;
				bool stillFoundWordMatch = true;
				int numberWordsInMultiword = 0;
				bool foundAtLeastOneMatch = false;
				LRPpreprocessorMultiwordReductionPlainTextWord* currentTagInPlainTextSentenceTemp = currentTagInPlainTextSentence;
				LRPpreprocessorMultiwordReductionPlainTextWord* firstTagInCollapsedMultiwordWord = new LRPpreprocessorMultiwordReductionPlainTextWord();
				const LRPpreprocessorMultiwordReductionWord* firstTagInMultiwordWord = currentTagInMultiwordWordList->firstTagInSentence;
				const LRPpreprocessorMultiwordReductionWord* currentTagInMultiwordWord = firstTagInMultiwordWord;
				while((currentTagInMultiwordWord->nextTag != NULL) && (currentTagInPlainTextSentenceTemp->nextTag != NULL) && (stillFoundWordMatch))
				{
					if(currentTagInMultiwordWord->tagName != currentTagInPlainTextSentenceTemp->tagName)
					{
						stillFoundWordMatch = false;
					}
					else
					{
						firstTagInCollapsedMultiwordWord->collapsedMultiwordWord = true;
						firstTagInCollapsedMultiwordWord->collapsedMultiwordWordTemp = true;	//this is reset everytime searchAndReplaceMultiwordWordList is executed to prevent collapsedMultiwordWord from being redetected
						firstTagInCollapsedMultiwordWord->collapsedMultiwordWordType = wordListType;
						firstTagInCollapsedMultiwordWord->tagName = firstTagInCollapsedMultiwordWord->tagName + currentTagInPlainTextSentenceTemp->tagName + LRP_TRANSLATOR_UNIQUE_CONCATENATION_TYPES_MULTIWORD_WORD_DELIMITER;
						foundAtLeastOneMatch = true;
						numberWordsInMultiword++;
					}
					currentTagInMultiwordWord = static_cast<LRPpreprocessorMultiwordReductionWord*>(currentTagInMultiwordWord->nextTag);
					currentTagInPlainTextSentenceTemp = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(currentTagInPlainTextSentenceTemp->nextTag);
				}

				if(stillFoundWordMatch && foundAtLeastOneMatch)
				{
					if(currentTagInMultiwordWord->nextTag == NULL)
					{//make sure the entire multiword word is matched (in case currentTagInPlainTextSentenceTemp reaches end of sentence without matching the multiword word in its entirety)
						
						//reduce all entities
						
						numberWordsInMultiwordMatched = numberWordsInMultiword;
						#ifdef LRP_PREPROCESSOR_RECORD_REFERENCES
						//updated 3b4b
						//assume firstTagInCollapsedMultiwordWord->preprocessorUpperLevelWordReference already set
						LRPpreprocessorMultiwordReductionPlainTextWord* currentTagInPlainTextSentenceTemp2 = currentTagInPlainTextSentence;
						for(int i=0; i<numberWordsInMultiwordMatched; i++)
						{
							if(currentTagInPlainTextSentenceTemp2->preprocessorUpperLevelWordReference != NULL)	//added 3d8a
							{
								currentTagInPlainTextSentenceTemp2->preprocessorUpperLevelWordReference->preprocessorLowerLevelWordReference = firstTagInCollapsedMultiwordWord;
							}
							currentTagInPlainTextSentenceTemp2 = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(currentTagInPlainTextSentenceTemp2->nextTag);	
						}
						firstTagInCollapsedMultiwordWord->preprocessorUpperLevelWordReference = currentTagInPlainTextSentence->preprocessorUpperLevelWordReference;
						firstTagInCollapsedMultiwordWord->preprocessorUpperLevelWordReferenceSize = numberWordsInMultiwordMatched;
						#endif
						
						firstTagInCollapsedMultiwordWord->nextTag = currentTagInPlainTextSentenceTemp;
						if(previousTagInPlainTextSentence != NULL)
						{
							previousTagInPlainTextSentence->nextTag = firstTagInCollapsedMultiwordWord;
						}
						else
						{
							firstTagInPlainTextSentence = firstTagInCollapsedMultiwordWord;
						}
						foundAtLeastOneMultiwordWordInSentenceAndCollapsed = true;
						foundMultiwordWordInSentenceAndCollapsed = true;
						
					}
				}
				if(!foundMultiwordWordInSentenceAndCollapsed)
				{
					delete firstTagInCollapsedMultiwordWord;
				}
			}
			if(foundAtLeastOneMultiwordWordInSentenceAndCollapsed)
			{
				createNewCorrespondenceInfo(firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo, &currentCorrespondenceInfo, currentLRPpreprocessorSentenceInList, firstTagInPlainTextSentence, &currentTagInPlainTextSentence, entityIndex, numberWordsInMultiwordMatched);
			}

			entityIndex++;
			previousTagInPlainTextSentence = currentTagInPlainTextSentence;
			currentTagInPlainTextSentence = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(currentTagInPlainTextSentence->nextTag);
		}
		
		LRPpreprocessorWordClassObject.generateSentenceWordList(firstTagInPlainTextSentence, LRPpreprocessorSentenceClassObject.getSentenceContents(currentLRPpreprocessorSentenceInList));

		sentenceIndexTemp++;
		currentLRPpreprocessorSentenceInList = currentLRPpreprocessorSentenceInList->next;
	}
	
	return result;
}

#ifdef LRP_PREPROCESSOR_WORD_DYNAMIC
bool LRPpreprocessorWordReductionClass::searchAndReplaceMultiwordWordListDynamic(LRPpreprocessorSentence* firstLRPpreprocessorSentenceInList, unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* wordList, LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo* firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo, const int wordListType)
{
	bool result = true;
	
	LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo* currentCorrespondenceInfo = firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo;	//new correspondence info for each found multiword word
	while(currentCorrespondenceInfo->next != NULL)
	{
		currentCorrespondenceInfo = currentCorrespondenceInfo->next;
	}
		
	int sentenceIndexTemp = 0;
	
	LRPpreprocessorSentence* currentLRPpreprocessorSentenceInList = firstLRPpreprocessorSentenceInList;
	while(currentLRPpreprocessorSentenceInList->next != NULL)
	{	
		//cout << "sentenceIndexTemp = " << sentenceIndexTemp << endl;
	
		LRPpreprocessorMultiwordReductionPlainTextWord* firstTagInPlainTextSentence = NULL;
		LRPpreprocessorWordClassObject.generateFlatSentenceWordList(LRPpreprocessorSentenceClassObject.getSentenceContents(currentLRPpreprocessorSentenceInList), &firstTagInPlainTextSentence);
		LRPpreprocessorMultiwordReductionPlainTextWord* currentTagInPlainTextSentence = firstTagInPlainTextSentence;
		LRPpreprocessorMultiwordReductionPlainTextWord* previousTagInPlainTextSentence = NULL;

		int entityIndex = LRP_NLP_START_ENTITY_INDEX;
		while(currentTagInPlainTextSentence->nextTag != NULL)
		{			
			if(currentTagInPlainTextSentence->nextTag->nextTag != NULL)
			{			
				bool foundConsecutiveWordsOfSameListType = false;
				int numberWordsInMultiwordMatched = 0;

				unordered_map<string, LRPpreprocessorMultiwordReductionWord*>::iterator wordListIter = wordList->find(currentTagInPlainTextSentence->tagName);
				if(wordListIter != wordList->end())
				{
					unordered_map<string, LRPpreprocessorMultiwordReductionWord*>::iterator wordListIter2 = wordList->find(currentTagInPlainTextSentence->nextTag->tagName);
					if(wordListIter2 != wordList->end())
					{
						foundConsecutiveWordsOfSameListType = true;
						numberWordsInMultiwordMatched = 2;
					}
				}
	
				if(foundConsecutiveWordsOfSameListType)
				{							
					LRPpreprocessorMultiwordReductionPlainTextWord* firstTagInCollapsedMultiwordWord = currentTagInPlainTextSentence;
										
					firstTagInCollapsedMultiwordWord->collapsedMultiwordWord = true;
					firstTagInCollapsedMultiwordWord->collapsedMultiwordWordTemp = false;
					firstTagInCollapsedMultiwordWord->collapsedMultiwordWordType = wordListType;
					firstTagInCollapsedMultiwordWord->tagName = firstTagInCollapsedMultiwordWord->tagName + LRP_TRANSLATOR_UNIQUE_CONCATENATION_TYPES_MULTIWORD_WORD_DELIMITER + currentTagInPlainTextSentence->nextTag->tagName + LRP_TRANSLATOR_UNIQUE_CONCATENATION_TYPES_MULTIWORD_WORD_DELIMITER;

					#ifdef LRP_PREPROCESSOR_RECORD_REFERENCES
					firstTagInCollapsedMultiwordWord->preprocessorUpperLevelWordReferenceSize = numberWordsInMultiwordMatched;
					#endif
					
					firstTagInCollapsedMultiwordWord->nextTag =  currentTagInPlainTextSentence->nextTag->nextTag;	//remove currentTagInPlainTextSentence->nextTag from list
					
					createNewCorrespondenceInfo(firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo, &currentCorrespondenceInfo, currentLRPpreprocessorSentenceInList, firstTagInPlainTextSentence, &currentTagInPlainTextSentence, entityIndex, numberWordsInMultiwordMatched);
				}
			}
			
			entityIndex++;
			previousTagInPlainTextSentence = currentTagInPlainTextSentence;
			currentTagInPlainTextSentence = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(currentTagInPlainTextSentence->nextTag);
		}
		
		LRPpreprocessorWordClassObject.generateSentenceWordList(firstTagInPlainTextSentence, LRPpreprocessorSentenceClassObject.getSentenceContents(currentLRPpreprocessorSentenceInList));

		sentenceIndexTemp++;
		currentLRPpreprocessorSentenceInList = currentLRPpreprocessorSentenceInList->next;
	}

	return result;
}
#endif



void LRPpreprocessorWordReductionClass::createNewCorrespondenceInfo(LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo* firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo, LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo** currentCorrespondenceInfo, LRPpreprocessorSentence* currentLRPpreprocessorSentenceInList, LRPpreprocessorMultiwordReductionPlainTextWord* firstTagInPlainTextSentence, LRPpreprocessorMultiwordReductionPlainTextWord** currentTagInPlainTextSentence, int entityIndex, int numberWordsInMultiwordMatched)
{
	renumberEntityIndiciesInCorrespondenceInfo(firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo, currentLRPpreprocessorSentenceInList->sentenceIndexOriginal, entityIndex, numberWordsInMultiwordMatched);	//this is required for revertNLPtagNameToOfficialLRPtagName	//this is required because searchAndReplaceMultiwordWordList (zero or more times)/searchAndReplacePhrasalVerbs is executed before searchAndReplaceMultiwordWordList 
	LRPpreprocessorMultiwordReductionPlainTextWord* currentTagInPlainTextSentenceTemp2 = firstTagInPlainTextSentence;
	int newEntityIndex = LRP_NLP_START_ENTITY_INDEX;
	//int collapsedMultiwordWordIndex = 0;

	while(currentTagInPlainTextSentenceTemp2->nextTag != NULL)
	{
		if(currentTagInPlainTextSentenceTemp2->collapsedMultiwordWordTemp)
		{//create a new correspondenceInfo
			if(newEntityIndex != entityIndex)
			{
				cerr << "newEntityIndex = " << newEntityIndex << endl;
				cerr << "entityIndex = " << entityIndex << endl;
				cerr << "currentTagInPlainTextSentenceTemp2->tagName = " << currentTagInPlainTextSentenceTemp2->tagName << endl;
				cerr << "LRPpreprocessorWordReductionClass::searchAndReplaceMultiwordWordList error: (newEntityIndex != entityIndex)" << endl;
				cerr << "generateTextFromPreprocessorSentenceWordList(firstTagInPlainTextSentence) = " << LRPpreprocessorWordClassObject.generateTextFromPreprocessorSentenceWordList(firstTagInPlainTextSentence) << endl;
				exit(EXIT_ERROR);
			}
			currentTagInPlainTextSentenceTemp2->collapsedMultiwordWordTemp = false;
			string tagName = currentTagInPlainTextSentenceTemp2->tagName;
			string tagNameWithLastLetterDropped = tagName.substr(0, tagName.length()-1);
			currentTagInPlainTextSentenceTemp2->tagName = tagNameWithLastLetterDropped;	//remove last LRP_TRANSLATOR_UNIQUE_CONCATENATION_TYPES_MULTIWORD_WORD_DELIMITER
			(*currentCorrespondenceInfo)->sentenceIndex = currentLRPpreprocessorSentenceInList->sentenceIndexOriginal;
			(*currentCorrespondenceInfo)->entityIndex = newEntityIndex;	//this is not currently used for LRP collapsed multiword word
			//#ifdef GIA_SEM_REL_TRANSLATOR
			(*currentCorrespondenceInfo)->lemmaWithLRP = currentTagInPlainTextSentenceTemp2->tagName;	//added 2j6c	//required for GIA2 only?
			//#endif
			(*currentCorrespondenceInfo)->wordWithLRP = currentTagInPlainTextSentenceTemp2->tagName;
			(*currentCorrespondenceInfo)->wordWithLRPforNLPonly = generateWordWithLRPforNLPonly(currentTagInPlainTextSentenceTemp2); //lrpDummyCollapsedMultiwordWordLemmaNameForNLPArray[collapsedMultiwordWordIndex];
			//collapsedMultiwordWordIndex++;

			(*currentCorrespondenceInfo)->next = new LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo();
			(*currentCorrespondenceInfo) = (*currentCorrespondenceInfo)->next;

			*currentTagInPlainTextSentence = currentTagInPlainTextSentenceTemp2;	//added 3d8a
		}
		currentTagInPlainTextSentenceTemp2->entityIndex = newEntityIndex;
		currentTagInPlainTextSentenceTemp2 = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(currentTagInPlainTextSentenceTemp2->nextTag);
		newEntityIndex++;
	}
}				
				
void LRPpreprocessorWordReductionClass::renumberEntityIndiciesInCorrespondenceInfo(LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo* firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo, int sentenceIndex, int entityIndex, int numberWordsInMultiwordMatched)
{
	LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo* currentCorrespondenceInfo = firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo;
	while(currentCorrespondenceInfo->next != NULL)
	{
		if(currentCorrespondenceInfo->sentenceIndex == sentenceIndex)
		{
			if(currentCorrespondenceInfo->entityIndex > entityIndex)
			{
				currentCorrespondenceInfo->entityIndex = currentCorrespondenceInfo->entityIndex - (numberWordsInMultiwordMatched-1);
			}
		}
		currentCorrespondenceInfo = currentCorrespondenceInfo->next;
	}
}

bool LRPpreprocessorWordReductionClass::writeTagListToFile(LRPpreprocessorSentence* firstLRPpreprocessorSentenceInList, const string plainTextLRPoutputFileName, const string plainTextLRPforNLPoutputFileName, const bool performLRPoutput, const bool performLRPforNLPoutput)
{
	bool result = true;
	
	ofstream* plainTextLRPOutput;
	ofstream* plainTextLRPforNLPOutput;
	if(performLRPoutput)
	{
		plainTextLRPOutput = new ofstream(plainTextLRPoutputFileName.c_str());
	}
	if(performLRPforNLPoutput)
	{
		plainTextLRPforNLPOutput = new ofstream(plainTextLRPforNLPoutputFileName.c_str());
	}
	
	bool firstCharacterInFile = true;

	LRPpreprocessorSentence* currentLRPpreprocessorSentenceInList = firstLRPpreprocessorSentenceInList;
	while(currentLRPpreprocessorSentenceInList->next != NULL)
	{
		string sentenceContentsLRP = "";
		string sentenceContentsLRPforNLP = "";
		bool firstCharacterInSentence = true;
		LRPpreprocessorMultiwordReductionPlainTextWord* firstTagInPlainTextSentence = NULL;
		LRPpreprocessorWordClassObject.generateFlatSentenceWordList(LRPpreprocessorSentenceClassObject.getSentenceContents(currentLRPpreprocessorSentenceInList), &firstTagInPlainTextSentence);

		LRPpreprocessorMultiwordReductionPlainTextWord* currentTagInPlainTextSentence = firstTagInPlainTextSentence;
		while(currentTagInPlainTextSentence->nextTag != NULL)
		{
			string plainTextLRPOutputTag = "";
			if(performLRPoutput)
			{
				plainTextLRPOutputTag = currentTagInPlainTextSentence->tagName;
			}
			string plainTextLRPforNLPOutputTag = "";
			if(performLRPforNLPoutput)
			{
				//cout << "currentTagInPlainTextSentence->collapsedMultiwordWord = " << currentTagInPlainTextSentence->collapsedMultiwordWord << endl;
				plainTextLRPforNLPOutputTag = generateWordWithLRPforNLPonly(currentTagInPlainTextSentence);
				/*
				if((currentTagInPlainTextSentence->collapsedPhrasalVerbExactDefinedSection) || (currentTagInPlainTextSentence->collapsedMultiwordWord))
				{
					plainTextLRPforNLPOutputTag = generateWordWithLRPforNLPonly(currentTagInPlainTextSentence);
				}
				else
				{
					plainTextLRPforNLPOutputTag = currentTagInPlainTextSentence->tagName;
				}
				*/
				
				if(plainTextLRPforNLPOutputTag != plainTextLRPOutputTag)
				{
					currentTagInPlainTextSentence->tagNameLRPforNLP = plainTextLRPforNLPOutputTag;
				}
				/*
				//Alternate method;
				if(!performLRPoutput)
				{
					if(currentTagInPlainTextSentence->tagNameLRPforNLP != "")
					{
						plainTextLRPforNLPOutputTag = currentTagInPlainTextSentence->tagNameLRPforNLP;
					}				
				}
				*/
			}
			
			bool punctuationMarkFound = false;
			for(int i=0; i<LRP_NLP_NUMBER_OF_PUNCTUATION_MARK_CHARACTERS; i++)
			{
				string nlpPunctionMarkCharacterString = "";
				nlpPunctionMarkCharacterString = nlpPunctionMarkCharacterString + nlpPunctionMarkCharacterArray[i];
				if(plainTextLRPOutputTag == nlpPunctionMarkCharacterString)
				{
					punctuationMarkFound = true;
				}
			}

			#ifdef LRP_PREPROCESSOR_WORD_REINSERT_NEWLINE_CHARACTERS_AFTER_EVERY_SENTENCE
			if(!punctuationMarkFound && !firstCharacterInFile && !firstCharacterInSentence)
			#else
			if(!punctuationMarkFound && !firstCharacterInFile)			
			#endif
			{
				if(performLRPoutput)
				{
					plainTextLRPOutputTag = string(STRING_SPACE) + plainTextLRPOutputTag;
				}
				if(performLRPforNLPoutput)
				{
					plainTextLRPforNLPOutputTag = string(STRING_SPACE) + plainTextLRPforNLPOutputTag;
				}
			}
			if(performLRPoutput)
			{
				sentenceContentsLRP = sentenceContentsLRP + plainTextLRPOutputTag;
			}
			if(performLRPforNLPoutput)
			{
				sentenceContentsLRPforNLP = sentenceContentsLRPforNLP + plainTextLRPforNLPOutputTag;
			}
			
			firstCharacterInFile = false;
			firstCharacterInSentence = false;

			currentTagInPlainTextSentence = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(currentTagInPlainTextSentence->nextTag);
		}
		
		if(performLRPoutput)
		{
			#ifdef LRP_PREPROCESSOR_WORD_REINSERT_NEWLINE_CHARACTERS_AFTER_EVERY_SENTENCE
			sentenceContentsLRP = sentenceContentsLRP + CHAR_NEWLINE;
			#endif
			plainTextLRPOutput->write(sentenceContentsLRP.c_str(), sentenceContentsLRP.length());
		}
		if(performLRPforNLPoutput)
		{
			#ifdef LRP_PREPROCESSOR_WORD_REINSERT_NEWLINE_CHARACTERS_AFTER_EVERY_SENTENCE
			sentenceContentsLRPforNLP = sentenceContentsLRPforNLP + CHAR_NEWLINE;
			#endif
			plainTextLRPforNLPOutput->write(sentenceContentsLRPforNLP.c_str(), sentenceContentsLRPforNLP.length());
		}
				
		currentLRPpreprocessorSentenceInList = currentLRPpreprocessorSentenceInList->next;
	}

	if(performLRPoutput)
	{
		plainTextLRPOutput->close();
	}
	if(performLRPforNLPoutput)
	{
		plainTextLRPforNLPOutput->close();
	}
	
	return result;
}

string LRPpreprocessorWordReductionClass::generateWordWithLRPforNLPonly(const LRPpreprocessorMultiwordReductionPlainTextWord* currentTagInPlainTextSentence)
{
	string wordWithLRPforNLPonly = currentTagInPlainTextSentence->tagName;
	if(currentTagInPlainTextSentence->collapsedPhrasalVerbExactDefinedSection)
	{
		wordWithLRPforNLPonly = lrpDummyCollapsedPhrasalVerbNameForNLPgrammaticalTenseFormsArray[currentTagInPlainTextSentence->grammaticalTenseFormDetected];
	}
	else if(currentTagInPlainTextSentence->collapsedMultiwordWord)
	{
		wordWithLRPforNLPonly = LRPpreprocessorPOStypeMultiwordReplacementArray[currentTagInPlainTextSentence->collapsedMultiwordWordType];
	}
	/*
	else
	{
		cout << "LRPpreprocessorWordReductionClass::generateWordWithLRPforNLPonly{} warning: !(currentTagInPlainTextSentence->collapsedPhrasalVerbExactDefinedSection) && !(currentTagInPlainTextSentence->collapsedMultiwordWord)" << endl;
	}
	*/
	return wordWithLRPforNLPonly;
}
			
			
#ifdef LRP_PREPROCESSOR_WORD_REPLACE_OUTPUT_FOR_NLP_TEMPORARILY

//NB preposition reversion routine will not work for RelEx as RelEx defines dependency relations based on lemmas not words...
void LRPpreprocessorWordReductionClass::revertNLPtagNameToOfficialLRPtagName(GIAfeature* feature, const GIAsentence* currentSentenceInList, const GIArelation* currentRelationInListForPrepositionsOnly, const bool isPreposition, bool* foundOfficialLRPreplacementString)
{
	int entityIndexForNonPrepositionsOnly = feature->entityIndex;
	
	//save original values for NLP only (required during a multiword preposition replacement with an adjacent multiword verb
	feature->wordWithLRPforNLPonly = feature->word;		
						
	string word = feature->word;
	//string lemma = feature->lemma;	//only used for prepositions (dependency relation) calculations, where lemma has already been calculated via revertNLPtagNameToOfficialLRPtagName()

	LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo* firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo = getActiveLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo();

	int sentenceIndex = currentSentenceInList->sentenceIndex;

	bool foundCorrespondingLRPtag = false;
	LRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo* currentLRPtoLRPforNLPonlyTagNameAndLocationCorrespondenceInfo = firstLRPpreprocessorMultiwordReductiontagCorrespondenceInfo;
	while(currentLRPtoLRPforNLPonlyTagNameAndLocationCorrespondenceInfo->next != NULL)
	{
		if(currentLRPtoLRPforNLPonlyTagNameAndLocationCorrespondenceInfo->sentenceIndex == sentenceIndex)
		{	
			if(word == currentLRPtoLRPforNLPonlyTagNameAndLocationCorrespondenceInfo->wordWithLRPforNLPonly)
			{
				if(isPreposition)
				{
					//now search entire sentence->feature list and find entity/word that has same name, and has the governor/dependent closest to it...
					string relationGovernor = currentRelationInListForPrepositionsOnly->relationGovernor;
					string relationDependent = currentRelationInListForPrepositionsOnly->relationDependent;
					const GIAfeature* firstFeatureInList = currentSentenceInList->firstFeatureInList;
					const GIAfeature* currentFeatureInList = firstFeatureInList;
					int indexOfPrepositionWithMinimumProximityOfGovernorDependentWords = GIA_ENTITY_INDEX_UNDEFINED;
					int minimumProximityOfGovernorDependentWords = MAXIMUM_NUMBER_WORDS_PER_SENTENCE;
					int indexOfLastInstanceOfPreposition = GIA_ENTITY_INDEX_UNDEFINED;
					int indexOfLastInstanceOfGovernor = GIA_ENTITY_INDEX_UNDEFINED;
					int indexOfLastInstanceOfDependent = GIA_ENTITY_INDEX_UNDEFINED;
					bool foundPrepositionGovernorAndDependentInFeatureList = false;

					while(currentFeatureInList->next != NULL)
					{
						if(currentFeatureInList->wordWithLRPforNLPonly == relationGovernor)
						{
							indexOfLastInstanceOfGovernor = currentFeatureInList->entityIndex;
						}
						if(currentFeatureInList->word == currentLRPtoLRPforNLPonlyTagNameAndLocationCorrespondenceInfo->wordWithLRP)	//NB currentLRPtoLRPforNLPonlyTagNameAndLocationCorrespondenceInfo->wordWithLRP has already been assigned to feature lemma via previous non-preposition execution of revertNLPtagNameToOfficialLRPtagName
						{
							indexOfLastInstanceOfPreposition = currentFeatureInList->entityIndex;
						}
						if(currentFeatureInList->wordWithLRPforNLPonly == relationDependent)
						{		
							indexOfLastInstanceOfDependent = currentFeatureInList->entityIndex;
							if((indexOfLastInstanceOfPreposition != GIA_ENTITY_INDEX_UNDEFINED) && (indexOfLastInstanceOfGovernor != GIA_ENTITY_INDEX_UNDEFINED))
							{

								int proximityOfPrepositionToGovernor = indexOfLastInstanceOfPreposition - indexOfLastInstanceOfGovernor;
								int proximityOfPrepositionToDependent = indexOfLastInstanceOfDependent - indexOfLastInstanceOfPreposition;
								int totalProximityOfPrepositionToGovernorAndDependent = proximityOfPrepositionToGovernor + proximityOfPrepositionToDependent;
								if(totalProximityOfPrepositionToGovernorAndDependent < minimumProximityOfGovernorDependentWords)
								{
									minimumProximityOfGovernorDependentWords = totalProximityOfPrepositionToGovernorAndDependent;
									indexOfPrepositionWithMinimumProximityOfGovernorDependentWords = indexOfLastInstanceOfPreposition;
									foundPrepositionGovernorAndDependentInFeatureList = true;
								}
							}
						}

						currentFeatureInList = currentFeatureInList->next;
					}
					if(foundPrepositionGovernorAndDependentInFeatureList)
					{
						if(currentLRPtoLRPforNLPonlyTagNameAndLocationCorrespondenceInfo->entityIndex == indexOfPrepositionWithMinimumProximityOfGovernorDependentWords)
						{
							feature->word = currentLRPtoLRPforNLPonlyTagNameAndLocationCorrespondenceInfo->wordWithLRP;
							//feature->lemma = currentLRPtoLRPforNLPonlyTagNameAndLocationCorrespondenceInfo->wordWithLRP;			//lemma is not defined for prepositions
							feature->featureRevertedToOfficialLRPTemp = true;
							foundCorrespondingLRPtag = true;
						}
					}
					else
					{
						cerr << "error: !foundPrepositionGovernorAndDependentInFeatureList;" << endl;
						cerr << word << "(" << relationGovernor << ", " << relationDependent << ")" << endl;
						exit(EXIT_ERROR);
					}
				}
				else
				{
					if(currentLRPtoLRPforNLPonlyTagNameAndLocationCorrespondenceInfo->entityIndex == entityIndexForNonPrepositionsOnly)
					{
						/*
						cout << "currentLRPtoLRPforNLPonlyTagNameAndLocationCorrespondenceInfo->wordWithLRPforNLPonly = " << currentLRPtoLRPforNLPonlyTagNameAndLocationCorrespondenceInfo->wordWithLRPforNLPonly << endl;
						cout << "currentLRPtoLRPforNLPonlyTagNameAndLocationCorrespondenceInfo->entityIndex = " << currentLRPtoLRPforNLPonlyTagNameAndLocationCorrespondenceInfo->entityIndex << endl;
						cout << "sentenceIndex = " << sentenceIndex << endl;
						cout << "entityIndexForNonPrepositionsOnly = " << entityIndexForNonPrepositionsOnly << endl;
						cout << "currentLRPtoLRPforNLPonlyTagNameAndLocationCorrespondenceInfo->wordWithLRP = " << currentLRPtoLRPforNLPonlyTagNameAndLocationCorrespondenceInfo->wordWithLRP << endl;						
						*/

						feature->word = currentLRPtoLRPforNLPonlyTagNameAndLocationCorrespondenceInfo->wordWithLRP;
						feature->lemma = currentLRPtoLRPforNLPonlyTagNameAndLocationCorrespondenceInfo->lemmaWithLRP;
						feature->featureRevertedToOfficialLRPTemp = true;

						foundCorrespondingLRPtag = true;
					}
				}
			}
		}
		currentLRPtoLRPforNLPonlyTagNameAndLocationCorrespondenceInfo = currentLRPtoLRPforNLPonlyTagNameAndLocationCorrespondenceInfo->next;
	}
	
	if(foundCorrespondingLRPtag)
	{
		*foundOfficialLRPreplacementString = true;
	}
}
#endif
#endif


bool LRPpreprocessorWordReductionClass::generateIrregularTenseVariantsOfVerbBase(LRPpreprocessorMultiwordReductionWord* baseTag, unordered_map<string, LRPpreprocessorMultiwordReductionIrregularVerbSentence*>* irregularVerbList, bool grammaticallyStrict)
{
	bool irregularVerbFound = false;
	
	string base = SHAREDvars.convertStringToLowerCase(&(baseTag->tagName));

	LRPpreprocessorMultiwordReductionIrregularVerbSentence* sentenceFound = NULL;
	if(findSentenceInSentenceListIrregularVerb(irregularVerbList, base, &sentenceFound))
	{
		irregularVerbFound = true;
		
		//cout << "irregularVerbFound" << endl;
		//cout << "base = " << base << endl;
					
		LRPpreprocessorMultiwordReductionIrregularVerbWord* firstTagInIrregularVerb = sentenceFound->firstTagInSentence;
		LRPpreprocessorMultiwordReductionIrregularVerbWord* currentTagInIrregularVerb = static_cast<LRPpreprocessorMultiwordReductionIrregularVerbWord*>(firstTagInIrregularVerb);
		int irregularVerbTagIndex = 0;
		while(currentTagInIrregularVerb->nextTag != NULL)
		{
			//second index is past, third is past participle
			int verbTenseForm = LRPpreprocessorMultiwordReductionIrregularVerbColumnCrossReferenceVerbDatabaseBaseTenseForm[irregularVerbTagIndex];

			#ifdef LRP_PREPROCESSOR_WORD_VERB_VARIANTS_PRORITISE_IRREGULAR_VERB_INFO
			if(verbTenseForm == LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_INFINITIVE)
			{
				LRPpreprocessorMultiwordReductionIrregularVerbWord* currentTagInAlternateList = currentTagInIrregularVerb;
				while(currentTagInAlternateList->alternateTag != NULL)
				{
					//cout << "1 currentTagInIrregularVerb->tagName = " << currentTagInIrregularVerb->tagName << endl;
					baseTag->grammaticalTenseFormsArray[LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_INFINITIVE].push_back(currentTagInIrregularVerb->tagName);
					currentTagInAlternateList = currentTagInAlternateList->alternateTag;
				}	
			}
			else if(verbTenseForm == LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_CONTINUOUS)
			{
				LRPpreprocessorMultiwordReductionIrregularVerbWord* currentTagInAlternateList = currentTagInIrregularVerb;
				while(currentTagInAlternateList->alternateTag != NULL)
				{
					//cout << "2 currentTagInIrregularVerb->tagName = " << currentTagInIrregularVerb->tagName << endl;
					baseTag->grammaticalTenseFormsArray[LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_CONTINUOUS].push_back(currentTagInIrregularVerb->tagName);
					currentTagInAlternateList = currentTagInAlternateList->alternateTag;
				}	
			} else
			#endif
			if(verbTenseForm == LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PAST)
			{
				LRPpreprocessorMultiwordReductionIrregularVerbWord* currentTagInAlternateList = currentTagInIrregularVerb;
				while(currentTagInAlternateList->alternateTag != NULL)
				{
					//cout << "3 currentTagInIrregularVerb->tagName = " << currentTagInIrregularVerb->tagName << endl;
					baseTag->grammaticalTenseFormsArray[LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PAST].push_back(currentTagInIrregularVerb->tagName);
					currentTagInAlternateList = currentTagInAlternateList->alternateTag;
				}	
			}
			else if(verbTenseForm == LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PASTPARTICIPLE)
			{
				LRPpreprocessorMultiwordReductionIrregularVerbWord* currentTagInAlternateList = currentTagInIrregularVerb;
				while(currentTagInAlternateList->alternateTag != NULL)
				{
					//cout << "4 currentTagInIrregularVerb->tagName = " << currentTagInIrregularVerb->tagName << endl;
					baseTag->grammaticalTenseFormsArray[LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PASTPARTICIPLE].push_back(currentTagInIrregularVerb->tagName);
					currentTagInAlternateList = currentTagInAlternateList->alternateTag;
				}
			}

			irregularVerbTagIndex++;
			currentTagInIrregularVerb = static_cast<LRPpreprocessorMultiwordReductionIrregularVerbWord*>(currentTagInIrregularVerb->nextTag);
		}
	}
	
	return irregularVerbFound;
}

bool LRPpreprocessorWordReductionClass::generateStandardTenseVariantsOfVerbBase(LRPpreprocessorMultiwordReductionWord* baseTag, bool irregularVerbFound, bool grammaticallyStrict)
{
	bool result = true;

	//NB grammaticallyStrict: do not interpret special cases (e.g. continuous) as verbs; as these are stricly nouns 
	
	string base = SHAREDvars.convertStringToLowerCase(&(baseTag->tagName));

	//2. generate verb tenses based upon rules (now wouldn't it be simpler if universities just published their data?) - http://a4esl.org/q/h/9807/km-doubles.html
	int baseStringLength = base.length();
	int indexOfLastCharacterInBase = baseStringLength-1;
	int indexOfSecondLastCharacterInBase = baseStringLength-2;
	int indexOfThirdLastCharacterInBase = baseStringLength-3;
	char lastCharacterInBase = base[indexOfLastCharacterInBase];
	char secondLastCharacterInBase = base[indexOfSecondLastCharacterInBase];
	char thirdLastCharacterInBase = base[indexOfThirdLastCharacterInBase];
	bool lastCharacterIsVowel = false;
	bool secondLastCharacterIsVowel = false;
	bool thirdLastCharacterIsVowel = false;
	//bool verbDoubleConsonantRule1LastLetterException = false;
	if(SHAREDvars.charInCharArray(lastCharacterInBase, englishVowelArray, LRP_PREPROCESSOR_WORD_NUMBER_OF_VOWELS))
	{
		lastCharacterIsVowel = true;
	}
	if(SHAREDvars.charInCharArray(secondLastCharacterInBase, englishVowelArray, LRP_PREPROCESSOR_WORD_NUMBER_OF_VOWELS))
	{
		secondLastCharacterIsVowel = true;
	}
	if(SHAREDvars.charInCharArray(thirdLastCharacterInBase, englishVowelArray, LRP_PREPROCESSOR_WORD_NUMBER_OF_VOWELS))
	{
		thirdLastCharacterIsVowel = true;
	}
	/*
	for(int i = 0; i<LRP_PREPROCESSOR_WORD_VERB_DOUBLE_CONSONANT_RULE1_LAST_LETTER_EXCEPTIONS_NUMBER_OF_TYPES; i++)
	{
		if(base[indexOfLastCharacterInBase] == lrpVerbDoubleConsonantRuleOneLastLetterExceptions[i])
		{
			verbDoubleConsonantRule1LastLetterException = true;
		}
	}
	*/

	bool rule1 = false;
	bool rule1b = false;
	bool rule2 = false;
	bool rule3a = false;
	bool rule3b = false;
	bool rule4 = false;

	if(!lastCharacterIsVowel && secondLastCharacterIsVowel && !thirdLastCharacterIsVowel)
	{
		// && !verbDoubleConsonantRule1LastLetterException - this requirement cannot be asserted as it is apparently only enforced for one syllable words
		rule1 = true;
		rule1b = true; //unknown [therefore have to assume both cases: alternate+ non-alternate)	//Two-syllable words: ED = If the stress is on the first syllable, the word only gets one consonant
	}

	if(lastCharacterInBase == 'e')
	{
		rule2 = true;
	}

	if(lastCharacterInBase == 'y')
	{
		if(secondLastCharacterIsVowel)
		{
			rule3b = true;
		}
		else
		{
			rule3a = true;
		}
	}

	if((!lastCharacterIsVowel && secondLastCharacterIsVowel && thirdLastCharacterIsVowel) || (!lastCharacterIsVowel && !secondLastCharacterIsVowel))
	{
		rule4 = true;
	}

	string baseWithLastLetterDropped = base.substr(0, baseStringLength-1);

	#ifdef LRP_PREPROCESSOR_WORD_VERB_VARIANTS_PRORITISE_IRREGULAR_VERB_INFO
	if(!irregularVerbFound)
	{
	#endif
		//generate (ie record) infinitive tense form [for consistency; even though this is the same as tagName...]
		baseTag->grammaticalTenseFormsArray[LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_INFINITIVE].push_back(base);
	#ifdef LRP_PREPROCESSOR_WORD_VERB_VARIANTS_PRORITISE_IRREGULAR_VERB_INFO
	}
	#endif
		
	//Rule 1: Words ending with a Consonant-Vowel-Consonant Pattern
	if(rule1)
	{

		//a. generate present tense form
		baseTag->grammaticalTenseFormsArray[LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PRESENT].push_back(base + LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PRESENT_APPEND);
		if(rule1b)
		{
			baseTag->grammaticalTenseFormsArray[LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PRESENT].push_back(base + LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PRESENT_APPEND);
		}

		#ifdef LRP_PREPROCESSOR_WORD_VERB_VARIANTS_PRORITISE_IRREGULAR_VERB_INFO
		if(!irregularVerbFound)
		{
		#endif
			#ifndef LRP_PREPROCESSOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY_BUT_TREAT_CONTINUOUS_AS_VERB
			if(!grammaticallyStrict)
			{
			#endif
				//b. generate continuous tense form
				//happening/entering
				baseTag->grammaticalTenseFormsArray[LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_CONTINUOUS].push_back(base + LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_CONTINUOUS_APPEND);
				if(rule1b)
				{
					//hopping/sitting
					//beginning/permitting
					baseTag->grammaticalTenseFormsArray[LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_CONTINUOUS].push_back(base + lastCharacterInBase + LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_CONTINUOUS_APPEND);	//double consonant
				}
			#ifndef LRP_PREPROCESSOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY_BUT_TREAT_CONTINUOUS_AS_VERB
			}
			#endif
		#ifdef LRP_PREPROCESSOR_WORD_VERB_VARIANTS_PRORITISE_IRREGULAR_VERB_INFO
		}
		#endif
		if(!irregularVerbFound)
		{
			//c. generate past tense form
			//visited/opened
			baseTag->grammaticalTenseFormsArray[LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PAST].push_back(base + LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PAST_APPEND);
			if(rule1b)
			{
				//rubbed/stopped
				//referred/admitted
				baseTag->grammaticalTenseFormsArray[LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PAST].push_back(base + lastCharacterInBase + LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PAST_APPEND);	//double consonant
			}

			//d. generate past participle form (assume same as past form)
			baseTag->grammaticalTenseFormsArray[LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PASTPARTICIPLE].push_back(base + LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PAST_APPEND);
			if(rule1b)
			{
				baseTag->grammaticalTenseFormsArray[LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PASTPARTICIPLE].push_back(base + lastCharacterInBase + LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PAST_APPEND);	//double consonant
			}
		}
	}
	//Rule 2: Words ending in E
	else if(rule2)
	{

		//a. generate present tense form
		baseTag->grammaticalTenseFormsArray[LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PRESENT].push_back(base + LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PRESENT_APPEND);

		#ifdef LRP_PREPROCESSOR_WORD_VERB_VARIANTS_PRORITISE_IRREGULAR_VERB_INFO
		if(!irregularVerbFound)
		{
		#endif
			#ifndef LRP_PREPROCESSOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY_BUT_TREAT_CONTINUOUS_AS_VERB
			if(!grammaticallyStrict)
			{
			#endif
				//b. generate continuous tense form
				//dancing/skating
				baseTag->grammaticalTenseFormsArray[LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_CONTINUOUS].push_back(baseWithLastLetterDropped + LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_CONTINUOUS_APPEND);
			#ifndef LRP_PREPROCESSOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY_BUT_TREAT_CONTINUOUS_AS_VERB
			}
			#endif
		#ifdef LRP_PREPROCESSOR_WORD_VERB_VARIANTS_PRORITISE_IRREGULAR_VERB_INFO
		}
		#endif
		if(!irregularVerbFound)
		{
			//c. generate past tense form
			//smiled/fined
			baseTag->grammaticalTenseFormsArray[LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PAST].push_back(baseWithLastLetterDropped + LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PAST_APPEND);

			//d. generate past participle form (assume same as past form)
			baseTag->grammaticalTenseFormsArray[LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PASTPARTICIPLE].push_back(baseWithLastLetterDropped + LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PAST_APPEND);
		}
	}
	//Rule 3: Words ending in Y
	else if(rule3a)
	{

		//a. generate present tense form
		baseTag->grammaticalTenseFormsArray[LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PRESENT].push_back(baseWithLastLetterDropped + LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PRESENT_APPEND_CASE3);

		#ifdef LRP_PREPROCESSOR_WORD_VERB_VARIANTS_PRORITISE_IRREGULAR_VERB_INFO
		if(!irregularVerbFound)
		{
		#endif
			#ifndef LRP_PREPROCESSOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY_BUT_TREAT_CONTINUOUS_AS_VERB
			if(!grammaticallyStrict)
			{
			#endif
				//b. generate continuous tense form
				//carrying/replying
				baseTag->grammaticalTenseFormsArray[LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_CONTINUOUS].push_back(base + LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_CONTINUOUS_APPEND);
			#ifndef LRP_PREPROCESSOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY_BUT_TREAT_CONTINUOUS_AS_VERB
			}
			#endif
		#ifdef LRP_PREPROCESSOR_WORD_VERB_VARIANTS_PRORITISE_IRREGULAR_VERB_INFO
		}
		#endif
		if(!irregularVerbFound)
		{
			//c. generate past tense form
			//studied/married
			baseTag->grammaticalTenseFormsArray[LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PAST].push_back(baseWithLastLetterDropped + LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PAST_APPEND_CASE3);

			//d. generate past participle form (assume same as past form)
			baseTag->grammaticalTenseFormsArray[LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PASTPARTICIPLE].push_back(baseWithLastLetterDropped + LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PAST_APPEND_CASE3);
		}
	}
	//Rule 4: Other words...
	else if(rule3b || rule4)
	{

		//a. generate present tense form
		baseTag->grammaticalTenseFormsArray[LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PRESENT].push_back(base + LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PRESENT_APPEND);

		#ifdef LRP_PREPROCESSOR_WORD_VERB_VARIANTS_PRORITISE_IRREGULAR_VERB_INFO
		if(!irregularVerbFound)
		{
		#endif
			#ifndef LRP_PREPROCESSOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY_BUT_TREAT_CONTINUOUS_AS_VERB
			if(!grammaticallyStrict)
			{
			#endif
				//b. generate continuous tense form
				//enjoying/straying
				//needing/beeping
				//needing/laughing
				baseTag->grammaticalTenseFormsArray[LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_CONTINUOUS].push_back(base + LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_CONTINUOUS_APPEND);
			#ifndef LRP_PREPROCESSOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY_BUT_TREAT_CONTINUOUS_AS_VERB
			}
			#endif
		#ifdef LRP_PREPROCESSOR_WORD_VERB_VARIANTS_PRORITISE_IRREGULAR_VERB_INFO
		}
		#endif
		if(!irregularVerbFound)
		{
			//c. generate past tense form
			//played/stayed
			//dreamed/rained
			//parked/earned
			baseTag->grammaticalTenseFormsArray[LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PAST].push_back(base + LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PAST_APPEND);

			//d. generate past participle form (assume same as past form)
			baseTag->grammaticalTenseFormsArray[LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PASTPARTICIPLE].push_back(base + LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PAST_APPEND);
		}
	}

	return result;
}

bool LRPpreprocessorWordReductionClass::findSentenceInSentenceListIrregularVerb(unordered_map<string, LRPpreprocessorMultiwordReductionIrregularVerbSentence*>* sentenceList, const string word, LRPpreprocessorMultiwordReductionIrregularVerbSentence** sentenceFound)
{	
	bool result = false;
	
	unordered_map<string, LRPpreprocessorMultiwordReductionIrregularVerbSentence*>::iterator it;
	it = sentenceList->find(word);
	if(it != sentenceList->end())
	{
		result = true;
		*sentenceFound = it->second;
	}
	
	return result;
}


#ifdef LRP_PREPROCESSOR_WORD_COLLAPSE_NUMERICAL_WORDS_TO_NUMBERS

bool LRPpreprocessorWordReductionClass::replaceAllNumericalWordsWithNumbers(LRPpreprocessorSentence* firstLRPpreprocessorSentenceInList)
{
	bool result = true;
	
	LRPpreprocessorSentence* currentLRPpreprocessorSentenceInList = firstLRPpreprocessorSentenceInList;
	while(currentLRPpreprocessorSentenceInList->next != NULL)
	{	
		LRPpreprocessorMultiwordReductionPlainTextWord* firstTagInPlainTextSentence = NULL;
		LRPpreprocessorWordClassObject.generateFlatSentenceWordList(LRPpreprocessorSentenceClassObject.getSentenceContents(currentLRPpreprocessorSentenceInList), &firstTagInPlainTextSentence);
		LRPpreprocessorPlainTextWord* currentTag = firstTagInPlainTextSentence;
		LRPpreprocessorPlainTextWord* firstTagInNumericalWord = NULL;
		LRPpreprocessorPlainTextWord* previousTag = NULL;
		bool parsingNumericalWord = false;
		while(currentTag->nextTag != NULL)
		{
			bool foundDelimiter = false;
			
			string currentWordText = SHAREDvars.convertStringToLowerCase(&(currentTag->tagName));
			if(SHAREDvars.textInTextArray(currentWordText, translatorEnglishNumbersArray, GIA_TRANSLATOR_ENGLISH_NUMBERS_NUMBER_OF_TYPES))
			{
				//cout << "currentWordText = " << currentWordText << endl;
				if(parsingNumericalWord)
				{
					//continue parsing
				}
				else
				{
					firstTagInNumericalWord = currentTag;
					parsingNumericalWord = true;
				}
			}
			else
			{
				if(parsingNumericalWord)
				{
					if(currentWordText == GIA_TRANSLATOR_ENGLISH_NUMBERS_DELIMITER)
					{
						//continue parsing
						foundDelimiter = true;
					}
					else
					{
						parsingNumericalWord = false;
						LRPpreprocessorPlainTextWord* tagSucceedingNumericalWordTagList = NULL;
						if(previousTag->nextTag->tagName == GIA_TRANSLATOR_ENGLISH_NUMBERS_DELIMITER)
						{
							tagSucceedingNumericalWordTagList = static_cast<LRPpreprocessorPlainTextWord*>(previousTag->nextTag);
						}
						else
						{
							tagSucceedingNumericalWordTagList = currentTag;
						} 
						//cout << "tagSucceedingNumericalWordTagList = " << tagSucceedingNumericalWordTagList->tagName << endl;
						//cout << "previousTag = " << previousTag->tagName << endl;
						previousTag->nextTag = new LRPpreprocessorPlainTextWord();	//isolate the numerical word tag list
						vector<LRPpreprocessorPlainTextWord*> text;
						LRPpreprocessorWordClassObject.generateSentenceWordList(firstTagInNumericalWord, &text);
						
						//convert numerical word list to lower case and remove all references to 'and':
						for(vector<LRPpreprocessorPlainTextWord*>::iterator iter = text.begin(); iter != text.end(); )
						{
							string wordText = (*iter)->tagName;
							//cout << "wordText = " << wordText << endl;
							(*iter)->tagName = SHAREDvars.convertStringToLowerCase(&wordText);
							
							if(wordText == GIA_TRANSLATOR_ENGLISH_NUMBERS_DELIMITER)
							{
								iter = text.erase(iter);
							}
							else
							{
								iter++;
							}
						}

						
						int64_t number = convertWordToNumber(&text);
						//cout << "number = " << number << endl;
						firstTagInNumericalWord->tagName = SHAREDvars.convertLongToString(number);
						firstTagInNumericalWord->nextTag = tagSucceedingNumericalWordTagList;
					}
				}
			}
			if(!foundDelimiter)
			{
				previousTag = currentTag;	//don't include any final "and" words within the numerical word list (e.g "the number of dogs was three and the number of chickens was five")
			}
			
			currentTag = static_cast<LRPpreprocessorPlainTextWord*>(currentTag->nextTag);
		}
			
		LRPpreprocessorWordClassObject.generateSentenceWordList(firstTagInPlainTextSentence, LRPpreprocessorSentenceClassObject.getSentenceContents(currentLRPpreprocessorSentenceInList));
	
		currentLRPpreprocessorSentenceInList = currentLRPpreprocessorSentenceInList->next;
	}
		
	return result;		
}

//based on https://www.roseindia.net/answers/viewqa/Java-Interview-Questions/13787-Java-program---convert-words-into-numbers.html

int64_t LRPpreprocessorWordReductionClass::convertWordToNumber(vector<LRPpreprocessorPlainTextWord*>* numericalWordList)
{
		
	int64_t totalValue = 0;
	bool processed = false;
	for(int n = 0; n < GIA_TRANSLATOR_ENGLISH_NUMBERS_MAGITUDES_NUMBER_OF_TYPES; n++) 
	{
		int index = LRPpreprocessorWordClassObject.findStringInWordList(numericalWordList, translatorEnglishNumbersMagnitudesTextArray[n]);
		if(index != CPP_STRING_FIND_RESULT_FAIL_VALUE)
		{
			vector<LRPpreprocessorPlainTextWord*>::const_iterator first1 = numericalWordList->begin();
			vector<LRPpreprocessorPlainTextWord*>::const_iterator last1 = numericalWordList->begin() + index;
			vector<LRPpreprocessorPlainTextWord*> text1(first1, last1);

			vector<LRPpreprocessorPlainTextWord*>::const_iterator first2 = numericalWordList->begin() + index + translatorEnglishNumbersMagnitudesTextArray[n].length();
			vector<LRPpreprocessorPlainTextWord*>::const_iterator last2 = numericalWordList->end();
			vector<LRPpreprocessorPlainTextWord*> text2(first2, last2);

			if(text1.empty())
			{	
				LRPpreprocessorMultiwordReductionPlainTextWord* newWord = new LRPpreprocessorMultiwordReductionPlainTextWord();
				newWord->tagName = "one";
				text1.push_back(newWord);
			}
			if(text2.empty())
			{
				LRPpreprocessorMultiwordReductionPlainTextWord* newWord = new LRPpreprocessorMultiwordReductionPlainTextWord();
				newWord->tagName = "zero";
				text2.push_back(newWord);
			}
			
			totalValue = parseNumerals(&text1) * translatorEnglishNumbersMagnitudesNumberArray[n] + convertWordToNumber(&text2);
			processed = true;
			break;
		}
	}

	if(processed)
	{
		return totalValue;
	}
	else
	{
		return parseNumerals(numericalWordList);
	}
}

int64_t LRPpreprocessorWordReductionClass::parseNumerals(vector<LRPpreprocessorPlainTextWord*>* numericalWordListSubset)
{
	int64_t value = 0;
		
	for(int i=0; i<numericalWordListSubset->size(); i++) 
	{
		string wordText = (*numericalWordListSubset)[i]->tagName;
		
		if(!SHAREDvars.textInTextArray(wordText, translatorEnglishNumbersNumeralsTextArray, GIA_TRANSLATOR_ENGLISH_NUMBERS_NUMBER_OF_TYPES))
		{
			cerr << "LRPpreprocessorWordIdentificationClass::parseNumerals{} error: unknown numerical word: " << wordText << endl;
			exit(EXIT_ERROR);
		}

		int64_t subval = getValueOf(wordText);
		if(subval == 100) 
		{
			if(value == 0)
			{
				value = 100;
			}
			else
			{
				value *= 100;
			}
		} 
		else
		{
			value += subval;
		}
	}

	return value;
}

int64_t LRPpreprocessorWordReductionClass::getValueOf(const string wordText) 
{
	int index = 0;
	if(!SHAREDvars.textInTextArray(wordText, translatorEnglishNumbersNumeralsTextArray, GIA_TRANSLATOR_ENGLISH_NUMBERS_NUMERALS_NUMBER_OF_TYPES, &index))
	{
		cerr << "LRPpreprocessorWordIdentificationClass::getValueOf{} error: wordText not in translatorEnglishNumbersNumeralsTextArray: " << wordText << endl;
		exit(EXIT_ERROR);
	}
	int64_t number = translatorEnglishNumbersNumeralsNumberArray[index];
	return number;
}
			
#endif





