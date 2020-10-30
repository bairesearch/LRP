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
 * File Name: LRPpreprocessorWordIdentification.cpp
 * Author: Richard Bruce Baxter - Copyright (c) 2005-2020 Baxter AI (baxterai.com)
 * Project: Language Reduction Preprocessor
 * Project Version: 3n3a 29-October-2020
 * Requirements: requires plain text file
 * Description: Preprocessor Word Identification
 * /
 *******************************************************************************/


#include "LRPpreprocessorWordIdentification.hpp"
#ifdef GIA_POS_REL_TRANSLATOR_HYBRID_PREFERENCE_NLP_PRELIM_POS_TAGS_OVER_LRP_WORD_TYPE_LISTS
#include "GIAsynRelTranslatorDefs.hpp"
#endif


bool lrpInitialised = false;
static string lrpDataFolderName;
static bool useLRP;

#ifdef LRP_PREPROCESSOR_WORD_LOAD_INVERSE_PREPOSITIONS_LIST
unordered_map<string, LRPpreprocessorMultiwordReductionBasicSentence*> prepositionInverseListGlobal;
bool prepositionInverseListLoaded;
#endif

#ifdef LRP_PREPROCESSOR_WORD_LOAD_WORD_LISTS
bool wordListsLoaded;
unordered_map<string, unordered_map<string, LRPpreprocessorMultiwordReductionWord*>*> wordListsGlobal;	//string: preprocessorPOStype
unordered_map<string, LRPpreprocessorMultiwordReductionWord*> verbCaseStandardListGlobal;
unordered_map<string, LRPpreprocessorMultiwordReductionWord*> verbCaseAdditionalListGlobal;
unordered_map<string, LRPpreprocessorMultiwordReductionIrregularVerbSentence*> irregularVerbListGlobal;
unordered_map<string, LRPpreprocessorMultiwordReductionBasicSentence*> irregularNounListGlobal;
bool irregularVerbListLoaded;
bool irregularNounListLoaded;
#ifdef LRP_PREPROCESSOR_DERIVE_NOUN_VARIANTS
unordered_map<string, LRPpreprocessorMultiwordReductionWord*> nounPluralVariantsListGlobal;
#endif
#ifdef LRP_PREPROCESSOR_WORD_COLLAPSE_AUXILIARY_LISTS_TO_VERB_LISTS
unordered_map<string, LRPpreprocessorMultiwordReductionWord*> wordListRecordAuxiliaryBeing;
unordered_map<string, LRPpreprocessorMultiwordReductionWord*> wordListRecordAuxiliaryHaving;
unordered_map<string, LRPpreprocessorMultiwordReductionWord*> wordListRecordAuxiliaryDoing;
#endif
#ifdef LRP_PREPROCESSOR_WORD_COLLAPSE_PREDETERMINER_LIST_TO_DETERMINER_LIST
unordered_map<string, LRPpreprocessorMultiwordReductionWord*> wordListRecordAuxiliaryPredeterminer;
#endif
#ifdef LRP_PREPROCESSOR_INITIALISE_WORD_INDEX_LIST_FROM_LRP_FILES
unordered_map<string, LRPpreprocessorMultiwordReductionWord*> verbListWithVariantsGlobal;
unordered_map<string, LRPpreprocessorMultiwordReductionWord*> nounListWithVariantsGlobal;
#endif
#endif


unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* LRPpreprocessorWordIdentificationClass::getWordList(int GIAposType)
{
	bool result = false;
	unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* wordList = getWordListIfExistent(GIAposType, &result);
	if(!result)
	{
		if(GIAposType < LRP_PREPROCESSOR_POS_TYPE_ARRAY_NUMBER_OF_TYPES)
		{
			string GIAposTypeName = LRPpreprocessorPOStypeNameArray[GIAposType];
			cerr << "LRPpreprocessorWordIdentificationClass::getWordList: cannot locate word list in wordListsGlobal, GIAposTypeName = " << GIAposTypeName << endl;
		}
		else
		{
			cerr << "LRPpreprocessorWordIdentificationClass::getWordList: cannot locate word list in wordListsGlobal, GIAposType = " << GIAposType << endl;		
		}
		exit(EXIT_ERROR);		
	}
	return wordList;
}
unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* LRPpreprocessorWordIdentificationClass::getWordListIfExistent(int GIAposType, bool* result)
{
	unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* wordList;
	string GIAposTypeName = LRPpreprocessorPOStypeNameArray[GIAposType];
	unordered_map<string, unordered_map<string, LRPpreprocessorMultiwordReductionWord*>*>::iterator wordListsIter = wordListsGlobal.find(GIAposTypeName); 
	if(wordListsIter != wordListsGlobal.end())
	{
		wordList = wordListsIter->second;
		*result = true;
	}
	else
	{
		*result = false;		
	}
	return wordList;
}
bool LRPpreprocessorWordIdentificationClass::wordListExistent(int GIAposType)
{
	bool result = false;
	unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* wordList = getWordListIfExistent(GIAposType, &result);
	return result;
}
bool LRPpreprocessorWordIdentificationClass::transferWordList(int GIAposType1, int GIAposType2, unordered_map<string, LRPpreprocessorMultiwordReductionWord*>** wordList1record)
{
	bool result = true;
	unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* wordList1 = getWordList(GIAposType1);
	unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* wordList2 = getWordList(GIAposType2);
	wordList2->insert(wordList1->begin(), wordList1->end());
	
	string GIAposTypeName = LRPpreprocessorPOStypeNameArray[GIAposType1];
	unordered_map<string, unordered_map<string, LRPpreprocessorMultiwordReductionWord*>*>::iterator wordListsIter = wordListsGlobal.find(GIAposTypeName); 
	wordListsGlobal.erase(wordListsIter);
	
	*wordList1record = wordList1;
	
	return result;
}





#ifdef LRP_PREPROCESSOR_INITIALISE_WORD_INDEX_LIST_FROM_LRP_FILES
bool wordListAllTypesWithPOSambiguityInfoLoaded;
unordered_map<string, pair<LRPpreprocessorMultiwordReductionWord*, uint64_t>> wordListAllTypesWithPOSambiguityInfo;		//NB the int corresponds to the POS type ambiguity of the word (binary 11000000 implies that the word may either be a verb or a preposition)	
unordered_map<string, pair<LRPpreprocessorMultiwordReductionWord*, uint64_t>>* LRPpreprocessorWordIdentificationClass::getWordListAllTypesWithPOSambiguityInfo()
{
	return &wordListAllTypesWithPOSambiguityInfo;
}
#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FEED_ALL_PERMUTATIONS_INDIVIDUALLY_ASSIGN_WEIGHTS_TO_TRAINED_POS_TYPES
vector<double> LRPpreprocessorPOStypeWeights;
double LRPpreprocessorWordIdentificationClass::getLRPpreprocessorPOStypeWeight(int GIAposType)
{
	return LRPpreprocessorPOStypeWeights[GIAposType];
}
#endif
/*
bool LRPpreprocessorWordIdentificationClass::findInstanceInMapWordListAllTypesWithPOSambiguityInfo(unordered_map<string, pair<LRPpreprocessorMultiwordReductionWord*, uint64_t>>* mapWordListAllTypesWithPOSambiguityInfo, const string wordIndex, LRPpreprocessorMultiwordReductionWord* word, uint64_t* POSambiguityInfo)
{
	bool result = false;
	
	unordered_map<string, pair<LRPpreprocessorMultiwordReductionWord*, uint64_t>>::iterator it;
	it = mapWordListAllTypesWithPOSambiguityInfo->find(wordIndex);
	if(it != mapWordListAllTypesWithPOSambiguityInfo->end())
	{
		result = true;
		word = (it->second).first;
		*POSambiguityInfo = (it->second).second;
	}
	
	return result;
}
*/
void LRPpreprocessorWordIdentificationClass::insertInstanceInMapWordListAllTypesWithPOSambiguityInfo(unordered_map<string, pair<LRPpreprocessorMultiwordReductionWord*, uint64_t>>* mapWordListAllTypesWithPOSambiguityInfo, const string wordIndex, LRPpreprocessorMultiwordReductionWord* word, const uint64_t POSambiguityInfo)
{
	pair<LRPpreprocessorMultiwordReductionWord*, uint64_t> value = make_pair(word, POSambiguityInfo);
	mapWordListAllTypesWithPOSambiguityInfo->insert(pair<string, pair<LRPpreprocessorMultiwordReductionWord*, uint64_t>>(wordIndex, value));
}
#endif










bool LRPpreprocessorWordIdentificationClass::initialiseLRP(const string newLRPDataFolderName, const bool newUseLRP)
{
	bool result = true;
	
	useLRP = newUseLRP;
	lrpDataFolderName = newLRPDataFolderName;
	
	if(!lrpInitialised)
	{
		string currentFolder = SHAREDvarsClass().getCurrentDirectory(); 
		SHAREDvarsClass().setCurrentDirectory(lrpDataFolderName); 
		
		#ifdef LRP_PREPROCESSOR_WORD_LOAD_INVERSE_PREPOSITIONS_LIST
		prepositionInverseListLoaded = false;
		if(!loadStructuredDataList(LRP_PREPROCESSOR_WORD_INVERSEPREPOSITIONS_DATABASE_FILE_NAME, &prepositionInverseListGlobal))
		{
			cout << "!loadStructuredDataList (GIA with LRP_PREPROCESSOR_WORD_NORMALISE_PREPOSITIONS requires -lrpfolder to be set): prepositionsInverseListFileName = " << LRP_PREPROCESSOR_WORD_INVERSEPREPOSITIONS_DATABASE_FILE_NAME << endl;
			result = false;
		}
		else
		{
			prepositionInverseListLoaded = true;
		}
		#endif

		#ifdef LRP_PREPROCESSOR_WORD_LOAD_WORD_LISTS
		wordListsLoaded = false;
		
		int numberOfWordLists;
		vector<string> wordListFileNames;
		if(!SHAREDvars.getLinesFromFile(LRP_PREPROCESSOR_WORD_WORDLISTS_DATABASE_FILE_NAME, &wordListFileNames, &numberOfWordLists))
		{
			result = false;
		}
		for(int i=0; i<numberOfWordLists; i++)
		{
			if(wordListFileNames[i] != "")
			{
				int indexOfSpace = wordListFileNames[i].find(CHAR_SPACE);
				if(indexOfSpace != CPP_STRING_FIND_RESULT_FAIL_VALUE)
				{
					string preprocessorPOStypeName = wordListFileNames[i].substr(0, indexOfSpace);
					string wordListFileName = wordListFileNames[i].substr(indexOfSpace+1);
					//cout << "preprocessorPOStypeName = " << preprocessorPOStypeName << endl;
					//cout << "wordListFileName = " << wordListFileName << endl;

					string dummyWordForWordListPosType = "";
					bool wordListLoaded = false;	//NOTUSED
					unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* wordList = new unordered_map<string, LRPpreprocessorMultiwordReductionWord*>;
					if(!loadWordListWrapper(&wordListLoaded, wordListFileName, wordList))
					{
						result = false;
					}
					wordListsGlobal.insert(pair<string, unordered_map<string, LRPpreprocessorMultiwordReductionWord*>*>(preprocessorPOStypeName, wordList));
					
					wordListsLoaded = true;
				}
				else
				{
					cerr << "LRPpreprocessorWordIdentificationClass::initialiseLRP{} error: cannot find space character in wordListFileNames[i], i = " << i << endl;
					exit(EXIT_ERROR);
				}
			}
			else
			{
				//skip blank lines
			}
		}
		
	
		#ifdef LRP_PREPROCESSOR_WORD_COLLAPSE_AUXILIARY_LISTS_TO_VERB_LISTS
		transferWordList(LRP_PREPROCESSOR_POS_TYPE_AUXILIARY_BEING, LRP_PREPROCESSOR_POS_TYPE_AUXILIARY_VERB, &wordListRecordAuxiliaryBeing);
		transferWordList(LRP_PREPROCESSOR_POS_TYPE_AUXILIARY_HAVING, LRP_PREPROCESSOR_POS_TYPE_AUXILIARY_VERB, &wordListRecordAuxiliaryHaving);
		transferWordList(LRP_PREPROCESSOR_POS_TYPE_AUXILIARY_DOING, LRP_PREPROCESSOR_POS_TYPE_AUXILIARY_VERB, &wordListRecordAuxiliaryDoing);
		#endif
		#ifdef LRP_PREPROCESSOR_WORD_COLLAPSE_PREDETERMINER_LIST_TO_DETERMINER_LIST
		transferWordList(LRP_PREPROCESSOR_POS_TYPE_PREDETERMINER, LRP_PREPROCESSOR_POS_TYPE_DETERMINER, &wordListRecordPredeterminer);
		#endif
		
		irregularVerbListLoaded = false;
		if(!loadStructuredDataListIrregularVerb(LRP_PREPROCESSOR_WORD_IRREGULARVERB_DATABASE_FILE_NAME, &irregularVerbListGlobal))
		{
			cout << "!loadStructuredDataListIrregularVerb: irregularVerbListFileName = " << LRP_PREPROCESSOR_WORD_IRREGULARVERB_DATABASE_FILE_NAME << endl;
			//(GIA with GIA_TRANSLATOR_CORRECT_IRREGULAR_VERB_LEMMAS_CONSERVATIVE requires -lrpfolder to be set)
			result = false;
		}
		else
		{
			irregularVerbListLoaded = true;
		}

		irregularNounListLoaded = false;
		if(!loadStructuredDataList(LRP_PREPROCESSOR_WORD_IRREGULARNOUN_DATABASE_FILE_NAME, &irregularNounListGlobal))
		{
			cout << "!loadIrregularNounList: irregularNounListFileName = " << LRP_PREPROCESSOR_WORD_IRREGULARNOUN_DATABASE_FILE_NAME << endl;
			result = false;
		}
		else
		{
			irregularNounListLoaded = true;
		}

		if(!generateVerbCaseStandardAndAdditionalList(LRP_PREPROCESSOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY_VALUE))	//this is required to make verbList usable
		{
			result = false;
		}
		#ifdef LRP_PREPROCESSOR_DERIVE_NOUN_VARIANTS
		if(!generateNounPluralVariantsList(&irregularNounListGlobal))
		{
			result = false;
		}
		#endif
		#endif
		
		#ifdef LRP_PREPROCESSOR_WORD_MULTIWORD_REDUCTION
 		if(!LRPpreprocessorWordReduction.initialiseLRPreduction(&irregularVerbListGlobal))
		{
			result = false;
		}
		#endif

		if(!result)
		{
			cout << "lrpDataFolderName = " << lrpDataFolderName << endl;
		}
		
		lrpInitialised = true;
		
		SHAREDvarsClass().setCurrentDirectory(currentFolder); 
	}
	
	return result;
}
bool LRPpreprocessorWordIdentificationClass::getUseLRP()
{
	return useLRP;
}

bool LRPpreprocessorWordIdentificationClass::loadWordListWrapper(bool* wordListLoaded, const string wordListFileName, unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* wordList)
{
	bool result = true;
	
	*wordListLoaded = false;
	if(!loadWordList(wordListFileName, wordList))
	{
		cout << "!loadWordList (GIA with wordListFileName requires -lrpfolder to be set): wordListFileName = " << wordListFileName << endl;
		result = false;
	}
	else
	{
		*wordListLoaded = true;
	}
	return result;
}

bool LRPpreprocessorWordIdentificationClass::loadWordList(const string wordListFileName, unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* wordList)
{
	bool result = true;

	ifstream parseFileObject(wordListFileName.c_str());
	if(!parseFileObject.rdbuf()->is_open())
	{
		//txt file does not exist in current directory.
		cout << "Error: Word list File does not exist in current directory: " << wordListFileName << endl;
		result = false;
	}
	else
	{
		int charCount = 0;
		char currentToken;
		int wordIndex = 0;
		string currentWord = "";
		bool currentWordAlternate = false;
		while(parseFileObject.get(currentToken))
		{
			if(currentToken == CHAR_NEWLINE)
			{
				LRPpreprocessorMultiwordReductionWord* currentTagInWordList = new LRPpreprocessorMultiwordReductionWord();
				//cout << "currentWord = " << currentWord << endl;
				#ifndef LRP_PREPROCESSOR_INITIALISE_WORD_INDEX_LIST_FROM_LRP_FILES_SUPPORT_UPPERCASE_PROPERNOUN_WORD_LISTS
				currentWord = SHAREDvars.convertStringToLowerCase(&(currentWord));
				#endif
				currentTagInWordList->tagName = currentWord;
				wordList->insert(pair<string, LRPpreprocessorMultiwordReductionWord*>(currentWord, currentTagInWordList));
				//*dummyWordForWordListPosType = currentWord;	//OLD: set dummyWordForWordListPosType to last word in list (e.g. z..)
				currentWord = "";
			}
			else
			{
				string c = "";
				c = c + currentToken;
				
				#ifdef LRP_PREPROCESSOR_WORD_MULTIWORD_REDUCTION
				if(currentToken == CHAR_SPACE)
				{
					c = LRP_TRANSLATOR_UNIQUE_CONCATENATION_TYPES_MULTIWORD_WORD_DELIMITER;	//assuming that multiword preprocessor has been executed
				}
				#endif
				
				currentWord = currentWord + c;
			}

			charCount++;
		}
		parseFileObject.close();
	}

	return result;
}

//note this function is a replica of loadStructuredDataList;
bool LRPpreprocessorWordIdentificationClass::loadStructuredDataListIrregularVerb(const string irregularVerbListFileName, unordered_map<string, LRPpreprocessorMultiwordReductionIrregularVerbSentence*>* irregularVerbList)
{
	bool result = true;

	//irregular verb list row format: infinitive past[/alt/alt/..] pastParticiple[/alt/alt/..] continuous
	
	LRPpreprocessorMultiwordReductionIrregularVerbWord* firstTagInIrregularVerb = new LRPpreprocessorMultiwordReductionIrregularVerbWord();
	LRPpreprocessorMultiwordReductionIrregularVerbWord* currentTagInIrregularVerb = firstTagInIrregularVerb;
	LRPpreprocessorMultiwordReductionIrregularVerbWord* currentTagInAlternateList = currentTagInIrregularVerb;
				
	ifstream parseFileObject(irregularVerbListFileName.c_str());
	if(!parseFileObject.rdbuf()->is_open())
	{
		//txt file does not exist in current directory.
		cout << "Error: Irregular Verb Database File does not exist in current directory: " << irregularVerbListFileName << endl;
		result = false;
	}
	else
	{
		int charCount = 0;
		char currentToken;
		int wordIndex = 0;
		int alternateWordIndex = 0;
		string currentWord = "";
		while(parseFileObject.get(currentToken))
		{
			if((currentToken == CHAR_NEWLINE) || (currentToken == CHAR_SPACE))
			{
				//create "LRP tag" to store phrasal verb base or past/past participle tense variant
				currentTagInAlternateList->tagName = currentWord;
				currentTagInAlternateList->alternateTag = new LRPpreprocessorMultiwordReductionIrregularVerbWord();
				//cout << "currentWord = " << currentWord << endl;
					
				currentTagInIrregularVerb->nextTag = new LRPpreprocessorMultiwordReductionIrregularVerbWord();
				currentTagInIrregularVerb = static_cast<LRPpreprocessorMultiwordReductionIrregularVerbWord*>(currentTagInIrregularVerb->nextTag);
				currentTagInAlternateList = currentTagInIrregularVerb;

				if(currentToken == CHAR_NEWLINE)
				{
					LRPpreprocessorMultiwordReductionIrregularVerbSentence* sentence = new LRPpreprocessorMultiwordReductionIrregularVerbSentence();
					sentence->firstTagInSentence = firstTagInIrregularVerb;
					irregularVerbList->insert(pair<string, LRPpreprocessorMultiwordReductionIrregularVerbSentence*>(firstTagInIrregularVerb->tagName, sentence));
					firstTagInIrregularVerb = new LRPpreprocessorMultiwordReductionIrregularVerbWord();
					currentTagInIrregularVerb = firstTagInIrregularVerb;
					currentTagInAlternateList = currentTagInIrregularVerb;
					wordIndex = 0;
				}
				else
				{
					wordIndex++;
				}
				
				alternateWordIndex = 0;
				currentWord = "";

				//start parsing new line
			}
			else if(currentToken == LRP_PREPROCESSOR_WORD_PHRASALVERB_DATABASE_TAG_ALTERNATE)
			{
				currentTagInAlternateList->tagName = currentWord;
				currentTagInAlternateList->alternateTag = new LRPpreprocessorMultiwordReductionIrregularVerbWord();
				currentTagInAlternateList = currentTagInAlternateList->alternateTag;
				//cout << "currentWord = " << currentWord << endl;
				
				//wordIndex does not change...
				currentWord = "";
				alternateWordIndex++;
			}
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









#ifdef LRP_PREPROCESSOR_DERIVE_NOUN_VARIANTS
bool LRPpreprocessorWordIdentificationClass::generateNounPluralVariantsList(unordered_map<string, LRPpreprocessorMultiwordReductionBasicSentence*>* irregularNounList)
{
	//aka parseVerbDataGenerateAllTenseVariants
	
	bool result = true;
	
	if(!wordListsLoaded)
	{
		cerr << "LRPpreprocessorWordIdentificationClass::generateNounPluralVariantsList: (!nounListLoaded)" << endl;
		exit(EXIT_ERROR);
	}
	else
	{		
		unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* nounList = getWordList(LRP_PREPROCESSOR_POS_TYPE_NOUN);
		unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* nounPluralVariantsList = &nounPluralVariantsListGlobal;
		for(unordered_map<string, LRPpreprocessorMultiwordReductionWord*>::iterator currentTagInNounListIterator = nounList->begin(); currentTagInNounListIterator != nounList->end(); ++currentTagInNounListIterator)
		{	
			LRPpreprocessorMultiwordReductionWord* currentTagInNounList = currentTagInNounListIterator->second;	
			generateNounPluralVariants(currentTagInNounList, nounPluralVariantsList, irregularNounList);
		}
	}
	
	return result;
}

bool LRPpreprocessorWordIdentificationClass::generateNounPluralVariants(LRPpreprocessorMultiwordReductionWord* wordTag, unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* nounPluralVariantsList, unordered_map<string, LRPpreprocessorMultiwordReductionBasicSentence*>* irregularNounList)
{
	bool result = true;
	
	//vector<string>* nounPluralVariants = wordTag->nounPluralVariants;
	string word = wordTag->tagName;
	
	LRPpreprocessorMultiwordReductionBasicSentence* sentenceFound = NULL;
	if(findSentenceInSentenceListBasic(irregularNounList, word, &sentenceFound))
	{
		//irregular noun found
		LRPpreprocessorMultiwordReductionWord* firstTagInIrregularNoun = sentenceFound->firstTagInSentence;
		LRPpreprocessorMultiwordReductionWord* currentTagInIrregularNoun = firstTagInIrregularNoun;
		for(int i=1; i<=LRP_PREPROCESSOR_WORD_IRREGULARNOUN_DATABASE_NUMBER_OF_TAGS; i++)
		{
			if(i == LRP_PREPROCESSOR_WORD_IRREGULARNOUN_DATABASE_TAG_NOUN_PLURAL_FORM)
			{
				LRPpreprocessorMultiwordReductionWord* firstTagInNounPluralVariants = currentTagInIrregularNoun;
				LRPpreprocessorMultiwordReductionWord* currentTagInNounPluralVariants = firstTagInNounPluralVariants;
				while(currentTagInNounPluralVariants->alternateTagBasic != NULL)
				{
					string nounPluralVariant = currentTagInNounPluralVariants->tagName;
					//cout << "irregular nounPluralVariant = " << nounPluralVariant << endl;
					nounPluralVariantsList->insert(pair<string, LRPpreprocessorMultiwordReductionWord*>(nounPluralVariant, wordTag));
					
					currentTagInNounPluralVariants = static_cast<LRPpreprocessorMultiwordReductionWord*>(currentTagInNounPluralVariants->alternateTagBasic);
				}
			}
			currentTagInIrregularNoun = static_cast<LRPpreprocessorMultiwordReductionWord*>(currentTagInIrregularNoun->nextTag);
		}	
	}
	else
	{
		for(int i=0; i<GIA_TRANSLATOR_ENGLISH_NOUN_PLURAL_MODIFIER_ADDITION_DEFAULT_NUMBER_OF_TYPES; i++)
		{
			string stringToAddOrReplace = translatorEnglishNounPluralModifierAdditionDefaultArray[i];
			string nounPluralVariant = word + stringToAddOrReplace;
			//nounPluralVariants->push_back(nounPluralVariant);
			nounPluralVariantsList->insert(pair<string, LRPpreprocessorMultiwordReductionWord*>(nounPluralVariant, wordTag));
			//cout << "nounPluralVariant = " << nounPluralVariant << endl;
		}

		for(int i=0; i<GIA_TRANSLATOR_ENGLISH_NOUN_PLURAL_MODIFIER_ADDITION_NUMBER_OF_TYPES; i++)
		{
			string stringToFind = translatorEnglishNounPluralModifierAdditionArray[i][0];
			//find string at end of word
			if(word.find(stringToFind, word.length()-stringToFind.length()) != CPP_STRING_FIND_RESULT_FAIL_VALUE)
			{
				string stringToAddOrReplace = translatorEnglishNounPluralModifierAdditionArray[i][1];
				string nounPluralVariant = word + stringToAddOrReplace;
				//nounPluralVariants->push_back(nounPluralVariant);
				nounPluralVariantsList->insert(pair<string, LRPpreprocessorMultiwordReductionWord*>(nounPluralVariant, wordTag));
				//cout << "nounPluralVariant = " << nounPluralVariant << endl;
			}
		}

		for(int i=0; i<GIA_TRANSLATOR_ENGLISH_NOUN_PLURAL_MODIFIER_REPLACEMENT_SUCCEEDING_CONSONANT_NUMBER_OF_TYPES; i++)
		{
			string stringToFind = translatorEnglishNounPluralModifierReplacementSucceedingConsonantArray[i][0];
			if(word.length()-stringToFind.length() >= 1)
			{
				char hypotheticalConsonantChar = word[word.length()-stringToFind.length()-1];
				if(SHAREDvars.charInCharArray(hypotheticalConsonantChar, translatorEnglishConsonants, GIA_TRANSLATOR_ENGLISH_CONSONANTS_NUMBER_OF_TYPES))
				{
					//find string at end of word
					stringToFind[0] = hypotheticalConsonantChar;	//modify stringToFind; replace template consonant C with actual consonant
					if(word.find(stringToFind, word.length()-stringToFind.length()) != CPP_STRING_FIND_RESULT_FAIL_VALUE)
					{
						string stringToAddOrReplace = translatorEnglishNounPluralModifierReplacementSucceedingConsonantArray[i][1];
						stringToAddOrReplace[0] = hypotheticalConsonantChar;	//modify stringToAddOrReplace; replace template consonant C with actual consonant
						
						string nounPluralVariant = word.substr(0, word.length()-stringToFind.length());
						nounPluralVariant = nounPluralVariant + stringToAddOrReplace;
						//nounPluralVariants->push_back(nounPluralVariant);
						nounPluralVariantsList->insert(pair<string, LRPpreprocessorMultiwordReductionWord*>(nounPluralVariant, wordTag));
						//cout << "nounPluralVariant = " << nounPluralVariant << endl;
						
						//cout << "word" << word << endl;
						//cout << "stringToFind = " << stringToFind << endl;
						//cout << "nounPluralVariant = " << nounPluralVariant << endl;
					}
				}
			}
		}

		for(int i=0; i<GIA_TRANSLATOR_ENGLISH_NOUN_PLURAL_MODIFIER_REPLACEMENT_NUMBER_OF_TYPES; i++)
		{
			string stringToFind = translatorEnglishNounPluralModifierReplacementArray[i][0];
			//find string at end of word
			if(word.find(stringToFind, word.length()-stringToFind.length()) != CPP_STRING_FIND_RESULT_FAIL_VALUE)
			{
				string stringToAddOrReplace = translatorEnglishNounPluralModifierReplacementArray[i][1];
				string nounPluralVariant = word.substr(0, word.length()-stringToFind.length());
				nounPluralVariant = nounPluralVariant + stringToAddOrReplace;
				//nounPluralVariant->push_back(nounPluralVariant);
				nounPluralVariantsList->insert(pair<string, LRPpreprocessorMultiwordReductionWord*>(nounPluralVariant, wordTag));
				//cout << "nounPluralVariant = " << nounPluralVariant << endl;
			}
		}
	}
	
	return result;
}

#endif

#ifdef LRP_PREPROCESSOR_WORD_LOAD_WORD_LISTS
//grammaticallyStrict is by default set to false: this function was developed for special verb cases e.g. continuous (these verb cases are formally nouns; but are interpreted semantically as actions)
bool LRPpreprocessorWordIdentificationClass::generateVerbCaseStandardAndAdditionalList(bool grammaticallyStrict)
{
	//aka parseVerbDataGenerateAllTenseVariants
	
	bool result = true;
	
	if(!wordListsLoaded || !irregularVerbListLoaded)
	{
		cout << "LRPpreprocessorWordIdentificationClass::generateVerbCaseStandardAndAdditionalList: (!wordListsLoaded || !irregularVerbListLoaded)" << endl;
		exit(EXIT_ERROR);
	}
	else
	{		
		//get global variables
		unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* verbList = getWordList(LRP_PREPROCESSOR_POS_TYPE_VERB);
		unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* verbCaseStandardList = &verbCaseStandardListGlobal;
		unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* verbCaseAdditionalList = &verbCaseAdditionalListGlobal;
		unordered_map<string, LRPpreprocessorMultiwordReductionIrregularVerbSentence*>* irregularVerbList = &irregularVerbListGlobal;
		
		for(unordered_map<string, LRPpreprocessorMultiwordReductionWord*>::iterator currentTagInVerbListIterator = verbList->begin(); currentTagInVerbListIterator != verbList->end(); ++currentTagInVerbListIterator)
		{	
			LRPpreprocessorMultiwordReductionWord* currentTagInVerbList = currentTagInVerbListIterator->second;	

			/*
			currentTagInVerbList->grammaticalTenseForm = LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_INFINITIVE;	//added 3d8a
			verbCaseStandardList->insert(pair<string, LRPpreprocessorMultiwordReductionWord*>(currentTagInVerbList->tagName, currentTagInVerbList));
			*/
			
			bool irregularVerbFound = LRPpreprocessorWordReduction.generateIrregularTenseVariantsOfVerbBase(currentTagInVerbList, irregularVerbList, grammaticallyStrict);

			//for each variant of the firstTagInPhrasalVerb, insert the sentence into the list keyed by the variant name
			LRPpreprocessorWordReduction.generateStandardTenseVariantsOfVerbBase(currentTagInVerbList, irregularVerbFound, grammaticallyStrict);

			for(int i=0; i<LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_MAX_NUM_TENSE_FORMS; i++)
			{
				for(int j=0; j<currentTagInVerbList->grammaticalTenseFormsArray[i].size(); j++)
				{

					//added 3d8a;
					LRPpreprocessorMultiwordReductionWord* newWord = new LRPpreprocessorMultiwordReductionWord();
					newWord->grammaticalTenseForm = i;
					newWord->baseName = currentTagInVerbList->tagName;
	
					string generatedTagNameLemma = (currentTagInVerbList->grammaticalTenseFormsArray[i])[j];
					
					verbCaseStandardList->insert(pair<string, LRPpreprocessorMultiwordReductionWord*>(generatedTagNameLemma, newWord));
				}
			}
			
			generateAdditionalTenseVariantsOfVerbBase(verbCaseAdditionalList, currentTagInVerbList, irregularVerbFound, grammaticallyStrict);
		}
	}
	
	return result;
}

#endif	


#ifdef LRP_PREPROCESSOR_WORD_LOAD_WORD_LISTS

//grammaticallyStrict is by default set to false: this function was developed for special verb cases e.g. continuous (these verb cases are formally nouns; but are interpreted semantically as actions)
bool LRPpreprocessorWordIdentificationClass::generateAdditionalTenseVariantsOfVerbBase(unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* verbCaseAdditionalList, LRPpreprocessorMultiwordReductionWord* baseTag, bool irregularVerbFound, bool grammaticallyStrict)
{
	bool result = true;
		
	/*
	detectContinuousVerbBasic Algorithm:

	Note a simple check for "ing" is not posssible as;
		some nouns end in ing also eg "thing"
	Note a simple "ing" appendition check against a verb infinitive list is not possible because there are some grammatical variants;
		Case 1: thinking
		Case 2: running - "run" + "n" [run n] + "ing"
		Case 3: changing - "chang" [change e] + "ing"

		ongoing?
		outstanding?
		being?
		becoming?

	This code uses a subset of code from generateStandardTenseVariantsOfVerbBase (note generateStandardTenseVariantsOfVerbBase is not used itself as this code is designed to be liberal/robust and detect all possible verbs without being subject to theoretical grammar rules)
	*/

	LRPpreprocessorMultiwordReductionWord* currentTagInVerbList = baseTag;

	string base = SHAREDvars.convertStringToLowerCase(&(currentTagInVerbList->tagName));	//OLD: currentTagInVerbList->tagName

	#ifdef LRP_PREPROCESSOR_WORD_VERB_VARIANTS_PRORITISE_IRREGULAR_VERB_INFO
	if(!irregularVerbFound)
	{
	#endif
		addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, base, "", LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_INFINITIVE);
	#ifdef LRP_PREPROCESSOR_WORD_VERB_VARIANTS_PRORITISE_IRREGULAR_VERB_INFO
	}
	#endif

	if(base.length() >= 3)	//match to baseWithLast3LettersDropped
	{
		int baseStringLength = base.length();
		int indexOfLastCharacterInBase = baseStringLength-1;
		char lastCharacterInBase = base[indexOfLastCharacterInBase];
		string baseWithLast1LettersDropped = base.substr(0, baseStringLength-1);
		string baseWithLast2LettersDropped = base.substr(0, baseStringLength-2);
		string baseWithLast3LettersDropped = base.substr(0, baseStringLength-3);


		#ifndef LRP_PREPROCESSOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY_BUT_TREAT_CONTINUOUS_AS_VERB
		if(!grammaticallyStrict)
		{
		#endif
			#ifdef LRP_PREPROCESSOR_WORD_VERB_VARIANTS_PRORITISE_IRREGULAR_VERB_INFO
			if(!irregularVerbFound)
			{
			#endif
				//LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_CONTINUOUS_APPEND/LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_CONTINUOUS _ing:
				//continuous rule 1a/3b/4: thinking/happening/entering
				addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, base, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_CONTINUOUS_APPEND, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_CONTINUOUS);
				//continuous rule 1b: running - "run" + "n" [run n] + "ing"
				addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, base+lastCharacterInBase, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_CONTINUOUS_APPEND, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_CONTINUOUS);
				//continuous rule 2: changing - "chang" [change e] + "ing"
				addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, baseWithLast1LettersDropped, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_CONTINUOUS_APPEND, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_CONTINUOUS);
				/*
				//continuous rule 3a: N/A !marriing (use marrying)
				addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, baseWithLast1LettersDropped, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_CONTINUOUS_APPEND, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_CONTINUOUS);
				*/
			#ifdef LRP_PREPROCESSOR_WORD_VERB_VARIANTS_PRORITISE_IRREGULAR_VERB_INFO
			}
			#endif
		#ifndef LRP_PREPROCESSOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY_BUT_TREAT_CONTINUOUS_AS_VERB
		}
		#endif
		if(!grammaticallyStrict)
		{
			#ifdef GIA_FEATURE_POS_TAG_VERB_POTENTIAL
			//added 2h2a
			//LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL_APPEND/LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL _able:
			//potential rule 1a/3b/4: thinkable/changeable
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, base, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL_APPEND, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL);
			//potential rule 1b: running - "run" + "n" [run n] + "able"
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, base+lastCharacterInBase, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL_APPEND, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL);
			/*
			//potential rule 2: N/A !changable (use changeable)
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, baseWithLast1LettersDropped, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL_APPEND, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL);
			*/
			//potential rule 3a: running - "marr" + "i" + "able"
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, base+baseWithLast1LettersDropped, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL_APPEND_CASE3, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL);
			#ifdef GIA_FEATURE_POS_TAG_VERB_POTENTIAL_INVERSE
			//added 2h2c
			//LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL_INVERSE_APPEND/LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL_INVERSE _ive:
			//potential rule 1ai: -> ive eg resistive/adaptive
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, base, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL_INVERSE_APPEND, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL_INVERSE);
			//potential rule 1aii: -> itive eg additive
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, base, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL_INVERSE_APPEND_CASE1II, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL_INVERSE);
			//potential rule 1aiii: -> ative eg affirmative
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, base, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL_INVERSE_APPEND_CASE1III, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL_INVERSE);
			/*
			//potential rule 1b: running - "run" + "n" [run n] + "itive"
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, base+lastCharacterInBase, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL_INVERSE_APPEND, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL_INVERSE);
			*/
			//potential rule 2i: e -> itive eg competitive/definitive/accomodative
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, baseWithLast1LettersDropped, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL_INVERSE_APPEND_CASE2, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL_INVERSE);
			//potential rule 2ii: e -> ment + ive eg judgementive
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, baseWithLast1LettersDropped, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL_INVERSE_APPEND_CASE2II, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL_INVERSE);
			//potential rule 3a: y -> iment + ive eg supplimentive
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, baseWithLast1LettersDropped, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL_INVERSE_APPEND_CASE3, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL_INVERSE);
			#endif
			#endif
		}

		#ifdef LRP_PREPROCESSOR_WORD_VERB_VARIANTS_PRORITISE_IRREGULAR_VERB_INFO
		if(!irregularVerbFound)
		{
		#endif
			#ifdef GIA_FEATURE_POS_TAG_VERB_STATE
			//added 2h2a
			//LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PAST_APPEND/LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PAST _ed:
			//possible state rule 1a/3b/4: visited/opened
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, base, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PAST_APPEND, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PAST);
			//possible state rule 1b: rubbed/stopped/referred/admitted - "rub" + "b" + "ed"
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, base+lastCharacterInBase, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PAST_APPEND, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PAST);
			//possible state rule 2: smiled/fined - "smil" [change e] + "ed"
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, baseWithLast1LettersDropped, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PAST_APPEND, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PAST);
			//possible state rule 3a: studied/married - "marr" + "i" + "ed"
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, baseWithLast1LettersDropped, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PAST_APPEND_CASE3, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PAST);
			#endif
		#ifdef LRP_PREPROCESSOR_WORD_VERB_VARIANTS_PRORITISE_IRREGULAR_VERB_INFO
		}
		#endif

		if(!grammaticallyStrict)
		{
			#ifdef GIA_FEATURE_POS_TAG_VERB_DESCRIPTION
			//added 2h2d
			//LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION_APPEND/LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION _ment:
			//potential rule 1ai: -> ment eg movement/government/derailment/detainment/enjoyment
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, base, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION_APPEND, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION);
			//potential rule 1aii: -> ament eg disarmament
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, base, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION_APPEND_CASE1II, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION);
			//potential rule 1aiii: -> lment eg enrollment/installment/fulfillment
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, base, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION_APPEND_CASE1III, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION);
			/*
			//potential rule 1b: running - "run" + "n" [run n] + "itive"
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, base+lastCharacterInBase, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION_INVERSE_APPEND, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION);
			*/
			//potential rule 2i: e -> ment eg judgement/dislodgment
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, baseWithLast1LettersDropped, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION_APPEND_CASE2, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION);
			/*
			//potential rule 2ii: e -> ment + ive eg judgementive
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, baseWithLast1LettersDropped, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL_INVERSE_APPEND_CASE2II, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION);
			*/
			//potential rule 3a: y -> iment eg worriment/suppliment/embodiment
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, baseWithLast1LettersDropped, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION_APPEND_CASE3, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION);
			//potential rule 5: pt -> pment eg entrapment/equipment
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, baseWithLast2LettersDropped, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION_APPEND_CASE5, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION);

			//LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION_APPEND/LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION _ion:
			//potential rule 1ai: -> ion eg absorption/abstraction/adaptation
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, base, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION2_APPEND, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION);
			//potential rule 1aii: -> ition eg addition
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, base, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION2_APPEND_CASE1II, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION);
			//potential rule 1aiii: -> ation eg acceptation/affirmation
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, base, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION2_APPEND_CASE1III, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION);
			/*
			//potential rule 1b: running - "run" + "n" [run n] + "itive"
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, base+lastCharacterInBase, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION2_INVERSE_APPEND, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION);
			*/
			//potential rule 2i: e -> ion eg relation/acclimatisation/accommodation/activation/accretion
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, baseWithLast1LettersDropped, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION2_APPEND_CASE2, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION);
			//potential rule 2ii: e -> ition + ive eg competition/composition/definition
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, baseWithLast1LettersDropped, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION2_APPEND_CASE2II, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION);
			//potential rule 2iii: e -> ation + ive eg admiration/organisation
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, baseWithLast1LettersDropped, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION2_APPEND_CASE2III, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION);
			//potential rule 3a: ify -> ification eg subjectification/amplification/ammonification/identification/beautification
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, baseWithLast1LettersDropped, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION2_APPEND_CASE3, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION);
			//potential rule 6i: aim -> amation eg acclamation {acclimation?}
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, baseWithLast3LettersDropped, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION2_APPEND_CASE6I, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION);
			//potential rule 6i: ide -> ision eg division
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, baseWithLast3LettersDropped, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION2_APPEND_CASE6II, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION);
			//potential rule 6iii: ish -> ition eg abolition/demolition
			addVerbCaseAdditional(currentTagInVerbList, verbCaseAdditionalList, baseWithLast3LettersDropped, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION2_APPEND_CASE6III, LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION);
			#endif
		}
	}
	
	return result;
}

void LRPpreprocessorWordIdentificationClass::addVerbCaseAdditional(LRPpreprocessorMultiwordReductionWord* currentTagInVerbList, unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* verbCaseAdditionalList, const string baseTenseFormStart, const string baseTenseFormAppend, const int grammaticalTenseFormNew)
{
	string hypotheticalVerbVariantCase = baseTenseFormStart + baseTenseFormAppend;

	LRPpreprocessorMultiwordReductionWord* newWord = new LRPpreprocessorMultiwordReductionWord();
	newWord->grammaticalTenseForm = grammaticalTenseFormNew;
	newWord->baseName = currentTagInVerbList->tagName;

	verbCaseAdditionalList->insert(pair<string, LRPpreprocessorMultiwordReductionWord*>(hypotheticalVerbVariantCase, newWord));
}
#endif

#ifdef GIA_TRANSLATOR_CORRECT_IRREGULAR_VERB_LEMMAS
#ifdef GIA_TRANSLATOR_CORRECT_IRREGULAR_VERB_LEMMAS_LIBERAL
//warning: this function is only currently developed for infinitive and continuous case
bool LRPpreprocessorWordIdentificationClass::determineVerbCaseAdditionalWrapper(const string word, string* baseNameFound, int* grammaticalBaseTenseForm)
{
	bool result = true;
	bool foundVerbCase = false;
	if(!wordListsLoaded)
	{
		cout << "!wordListsLoaded (GIA with GIA_TRANSLATOR_CORRECT_IRREGULAR_VERB_LEMMAS_LIBERAL requires -lrpfolder to be set)" << endl;
		result = false;
	}
	else
	{
		if(determineVerbCaseAdditional(word, baseNameFound, grammaticalBaseTenseForm))
		{
			foundVerbCase = true;
		}
	}
	return foundVerbCase;
}

#endif


#ifdef GIA_TRANSLATOR_CORRECT_IRREGULAR_VERB_LEMMAS_CONSERVATIVE
//NB determineIfWordIsIrregularVerbContinuousCaseWrapper() can be used instead of determineVerbCaseAdditionalWrapper(), as Stanford only has a problem identifying verbs (pos tag "VBG") when they are irregular varbs
bool LRPpreprocessorWordIdentificationClass::determineIfWordIsIrregularVerbContinuousCaseWrapper(const string word, string* baseNameFound)
{
	bool result = true;
	bool foundIrregularVerbContinuousCase = false;
	string irregularVerbListFileName = lrpDataFolderName + LRP_PREPROCESSOR_WORD_IRREGULARVERB_DATABASE_FILE_NAME;
	if(!irregularVerbListLoaded)
	{
		cout << "!irregularVerbListLoaded (GIA with GIA_TRANSLATOR_CORRECT_IRREGULAR_VERB_LEMMAS_CONSERVATIVE requires -lrpfolder to be set): irregularVerbListFileName = " << irregularVerbListFileName << endl;
		result = false;
	}
	else
	{
		foundIrregularVerbContinuousCase = determineIfWordIsIrregularVerbContinuousCase(word, firstTagInIrregularVerbListGlobal, baseNameFound);
	}
	return foundIrregularVerbContinuousCase;
}


//determineIfWordIsIrregularVerbContinuousCase requires updating for new vector list implementation:
bool LRPpreprocessorWordIdentificationClass::determineIfWordIsIrregularVerbContinuousCase(const string word, unordered_map<string, LRPpreprocessorMultiwordReductionIrregularVerbSentence*>* irregularVerbList, string* baseNameFound)
{
	bool foundIrregularVerbContinuousCase = false;

	LRPpreprocessorMultiwordReductionWord* currentTagInIrregularVerbList = firstTagInIrregularVerbList;
	while(currentTagInIrregularVerbList->nextSentence != NULL)
	{
		int irregularVerbTagIndex = 0;
		LRPpreprocessorMultiwordReductionWord* currentTagInIrregularVerb = currentTagInIrregularVerbList;
		while(currentTagInIrregularVerb->nextTag != NULL)
		{
			string wordLowerCase = SHAREDvars.convertStringToLowerCase(&word);

			if(irregularVerbTagIndex == 3)
			{
				if(wordLowerCase == currentTagInIrregularVerb->tagName)
				{
					foundIrregularVerbContinuousCase = true;
					*baseNameFound = currentTagInIrregularVerbList->tagName;
				}
			}
			currentTagInIrregularVerb = currentTagInIrregularVerb->nextTag;
			irregularVerbTagIndex++;
		}


		/*OLD (before RBB addition of continuous cases to wikipedia english irregular verb list - ie final/4th column of WikipediaIrregularVerbs.txt):

		string irregularVerbBaseForm = currentTagInIrregularVerbList->tagName;
		int irregularVerbBaseFormLength = irregularVerbBaseForm.length();

		string baseWithLastLetterDropped = irregularVerbBaseForm.substr(0, irregularVerbBaseFormLength-1);
		#ifdef LRP_PREPROCESSOR_WORD_DEBUG
		//cout << "baseWithLastLetterDropped = " << baseWithLastLetterDropped << endl;
		#endif

		string irregularVerbContinuousForm1 =  irregularVerbBaseForm + LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_CONTINUOUS_APPEND;
		string irregularVerbContinuousForm2 = baseWithLastLetterDropped + LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_CONTINUOUS_APPEND;

		if((word == irregularVerbContinuousForm1) || (word == irregularVerbContinuousForm2))
		{
			cout << "foundIrregularVerbContinuousCase" << endl;
			cout << "irregularVerbBaseForm = " << irregularVerbBaseForm << endl;
			foundIrregularVerbContinuousCase = true;
			*baseNameFound = irregularVerbBaseForm;
		}
		*/

		currentTagInIrregularVerbList = currentTagInIrregularVerbList->nextSentence;
	}

	return foundIrregularVerbContinuousCase;
}
#endif
#endif


#ifdef LRP_PREPROCESSOR_WORD_LOAD_INVERSE_PREPOSITIONS_LIST
bool LRPpreprocessorWordIdentificationClass::loadStructuredDataList(const string structuredDataListFileName, unordered_map<string, LRPpreprocessorMultiwordReductionBasicSentence*>* structuredDataList)
{
	bool result = true;

	LRPpreprocessorMultiwordReductionWord* firstTagInRow = new LRPpreprocessorMultiwordReductionWord();
	LRPpreprocessorMultiwordReductionWord* currentTagInRow = firstTagInRow;
	LRPpreprocessorMultiwordReductionWord* currentTagInAlternateList = currentTagInRow;
	
	ifstream parseFileObject(structuredDataListFileName.c_str());
	if(!parseFileObject.rdbuf()->is_open())
	{
		//txt file does not exist in current directory.
		cout << "Error: Structured data list does not exist in current directory: " << structuredDataListFileName << endl;
		result = false;
	}
	else
	{
		int lineIndex = 0;
		int charCount = 0;
		int wordIndex = 0;
		int alternateWordIndex = 0;
		char currentToken;
		string currentWord = "";
		while(parseFileObject.get(currentToken))
		{
			if((currentToken == CHAR_NEWLINE) || (currentToken == LRP_PREPROCESSOR_WORD_STRUCTURED_DATABASE_TAG_NEXTCOLUMN))
			{
				//create "LRP tag" to store phrasal verb base or past/past participle tense variant
				currentTagInAlternateList->tagName = currentWord;
				currentTagInAlternateList->alternateTagBasic = new LRPpreprocessorMultiwordReductionWord();
				//cout << "currentWord = " << currentWord << endl;
					
				currentTagInRow->nextTag = new LRPpreprocessorMultiwordReductionWord();
				currentTagInRow = static_cast<LRPpreprocessorMultiwordReductionWord*>(currentTagInRow->nextTag);
				currentTagInAlternateList = currentTagInRow;

				if(currentToken == CHAR_NEWLINE)
				{
					LRPpreprocessorMultiwordReductionBasicSentence* sentence = new LRPpreprocessorMultiwordReductionBasicSentence();
					sentence->firstTagInSentence = firstTagInRow;
					structuredDataList->insert(pair<string, LRPpreprocessorMultiwordReductionBasicSentence*>(firstTagInRow->tagName, sentence));
					firstTagInRow = new LRPpreprocessorMultiwordReductionWord();
					currentTagInRow = firstTagInRow;
					currentTagInAlternateList = currentTagInRow;
					wordIndex = 0;
					lineIndex++;
				}
				else
				{
					wordIndex++;
				}
				
				alternateWordIndex = 0;
				currentWord = "";

				//start parsing new line
			}
			else if(currentToken == LRP_PREPROCESSOR_WORD_STRUCTURED_DATABASE_TAG_ALTERNATE)
			{
				currentTagInAlternateList->tagName = currentWord;
				currentTagInAlternateList->alternateTagBasic = new LRPpreprocessorMultiwordReductionWord();
				currentTagInAlternateList = currentTagInAlternateList->alternateTagBasic;
				//cout << "currentWord = " << currentWord << endl;
				
				//wordIndex does not change...
				currentWord = "";
				alternateWordIndex++;
			}
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
#endif

#ifdef LRP_PREPROCESSOR_WORD_NORMALISE_PREPOSITIONS
void LRPpreprocessorWordIdentificationClass::detectIfInverseOrTwoWayConditionRequired(const string conditionName, bool* inverseConditionRequired, bool* twoWayConditionRequired, string* inverseConditionName)
{
	unordered_map<string, LRPpreprocessorMultiwordReductionBasicSentence*>* prepositionInverseList = &prepositionInverseListGlobal;

	*inverseConditionRequired = false;
	*twoWayConditionRequired = false;

	LRPpreprocessorMultiwordReductionBasicSentence* sentenceFound = NULL;
	if(findSentenceInSentenceListBasic(prepositionInverseList, conditionName, &sentenceFound))
	{
		//invert condition if necessary
		LRPpreprocessorMultiwordReductionWord* firstTagInPrepositionsInverseListSentence = sentenceFound->firstTagInSentence;
		LRPpreprocessorMultiwordReductionWord* currentTagInPrepositionsInverseListSentence = firstTagInPrepositionsInverseListSentence;
		bool foundConditionToInvert = false;
		string conditionNameInverted = "";	//= currentTagInPrepositionsInverseList->tagName;
		for(int i=1; i<=LRP_PREPROCESSOR_WORD_INVERSEPREPOSITIONS_DATABASE_NUMBER_OF_TAGS; i++)
		{
			if(i == LRP_PREPROCESSOR_WORD_INVERSEPREPOSITIONS_DATABASE_TAG_CONDITION)
			{
				if(currentTagInPrepositionsInverseListSentence->tagName != conditionName)
				{
					cerr << "LRPpreprocessorWordIdentificationClass::detectIfInverseOrTwoWayConditionRequired error{}: (currentTagInPrepositionsInverseListSentence->tagName != conditionName)" << endl;
					exit(EXIT_ERROR);
				}
			}
			else if(i == LRP_PREPROCESSOR_WORD_INVERSEPREPOSITIONS_DATABASE_TAG_REVERSE_CONDITION)
			{
				conditionNameInverted = currentTagInPrepositionsInverseListSentence->tagName;
				if(currentTagInPrepositionsInverseListSentence->tagName == conditionName)
				{
					*twoWayConditionRequired = true;
				}
				else
				{
					foundConditionToInvert = true;
				}
			}
			else if(i == LRP_PREPROCESSOR_WORD_INVERSEPREPOSITIONS_DATABASE_TAG_INVERT_REVERSE_CONDITION_VALID)
			{
				if(foundConditionToInvert)
				{
					if(currentTagInPrepositionsInverseListSentence->tagName == LRP_PREPROCESSOR_WORD_INVERSEPREPOSITIONS_DATABASE_TAG_INVERT_REVERSE_CONDITION_VALID_VALUE_TRUE)
					{
						*inverseConditionRequired = true;
						*inverseConditionName = conditionNameInverted;
					}
				}
			}
			currentTagInPrepositionsInverseListSentence = static_cast<LRPpreprocessorMultiwordReductionWord*>(currentTagInPrepositionsInverseListSentence->nextTag);
		}
	}
}

#ifdef LRP_PREPROCESSOR_WORD_DETECT_PREPOSITION_TYPE
bool LRPpreprocessorWordIdentificationClass::identifyConditionType(GIAentityNode* conditionRelationshipEntity)
{
	bool conditionTypeIdentified = false;

	unordered_map<string, LRPpreprocessorMultiwordReductionBasicSentence*>* prepositionInverseList = &prepositionInverseListGlobal;

	//identify condition type
	LRPpreprocessorMultiwordReductionBasicSentence* sentenceFound = NULL;
	if(findSentenceInSentenceListBasic(prepositionInverseList, conditionRelationshipEntity->entityName, &sentenceFound))
	{
		//the first tag in the sentence object corresponds to the preposition name
		
		LRPpreprocessorMultiwordReductionWord* firstTagInPrepositionsInverseListSentence = sentenceFound->firstTagInSentence;
		LRPpreprocessorMultiwordReductionWord* currentTagInPrepositionsInverseListSentence = firstTagInPrepositionsInverseListSentence;
		for(int i=1; i<=LRP_PREPROCESSOR_WORD_INVERSEPREPOSITIONS_DATABASE_NUMBER_OF_TAGS; i++)
		{
			if(i == LRP_PREPROCESSOR_WORD_INVERSEPREPOSITIONS_DATABASE_TAG_TYPE)
			{
				conditionRelationshipEntity->conditionType2 = currentTagInPrepositionsInverseListSentence->tagName;
				conditionTypeIdentified = true;
			}
			currentTagInPrepositionsInverseListSentence = static_cast<LRPpreprocessorMultiwordReductionWord*>(currentTagInPrepositionsInverseListSentence->nextTag);
		}
	}

	return conditionTypeIdentified;
}
#endif

#endif

#ifdef GIA_POS_REL_TRANSLATOR_HYBRID_PREFERENCE_NLP_PRELIM_POS_TAGS_OVER_LRP_WORD_TYPE_LISTS
bool LRPpreprocessorWordIdentificationClass::checkGrammaticalWordTypeFeaturePrelim(LRPpreprocessorPlainTextWord* wordTag, const int GIAposType)
{
	bool result = false;
	
	//use feature POS type (instead of LRPpreprocessor word type/wordNet lists) if available
	if(wordTag->featureReferencePrelim != NULL)
	{
		/*
		cout << "wordTag->tagName = " << wordTag->tagName << endl;
		cout << "GIAposType = " << GIAposType << endl;
		cout << "wordTag->featureReferencePrelim->GIAposType = " << wordTag->featureReferencePrelim->GIAposType << endl;
		*/
		if(wordTag->featureReferencePrelim->GIAposType == GIAposType)
		{
			result = true;
		}
	}
	
	return result;
}
#endif

bool LRPpreprocessorWordIdentificationClass::determineIsWordType(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim, const bool grammaticallyStrict, const int GIAposType)
{
	string baseNameFound = "";
	int grammaticalBaseTenseForm = INT_DEFAULT_VALUE;
	return determineIsWordType(wordTag, usePOSprelim, grammaticallyStrict, GIAposType, &baseNameFound, &grammaticalBaseTenseForm);
}
//preconditions: if usePOSprelim, then grammaticallyStrict is assumed true
bool LRPpreprocessorWordIdentificationClass::determineIsWordType(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim, const bool grammaticallyStrict, const int GIAposType, string* baseNameFound, int* grammaticalBaseTenseForm)
{
	bool wordTypeDetected = false;
	
	string wordLowerCase = SHAREDvars.convertStringToLowerCase(&(wordTag->tagName));
	
	#ifdef GIA_POS_REL_TRANSLATOR_HYBRID_PREFERENCE_NLP_PRELIM_POS_TAGS_OVER_LRP_WORD_TYPE_LISTS
	if(usePOSprelim)
	{
		if(checkGrammaticalWordTypeFeaturePrelim(wordTag, GIAposType))		
		{
			wordTypeDetected = true;		//usePOSprelim assumes grammaticallyStrict so no check is required
			if(GIAposType == LRP_PREPROCESSOR_POS_TYPE_VERB)
			{
				if(!determineVerbCaseStandardWithAdditional(wordLowerCase, baseNameFound, grammaticalBaseTenseForm))	//added 3e8a
				{
					LRPpreprocessorMultiwordReductionPlainTextWord* currentTagInPlainTextSentenceTemp = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(wordTag);
					if(!(currentTagInPlainTextSentenceTemp->collapsedMultiwordWord || currentTagInPlainTextSentenceTemp->collapsedPhrasalVerbExactDefinedSection))
					{
						cout << "LRPpreprocessorWordIdentificationClass::determineIsWordType{} warning: GIA_POS_REL_TRANSLATOR_HYBRID_PREFERENCE_NLP_PRELIM_POS_TAGS_OVER_LRP_WORD_TYPE_LISTS: usePOSprelim && checkGrammaticalWordTypeFeaturePrelim && (GIAposType == LRP_PREPROCESSOR_POS_TYPE_VERB) && !determineVerbCaseStandardWithAdditional" << endl;		
					}
					
					*grammaticalBaseTenseForm = LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_INFINITIVE;
				}
			}
		}
	}
	else 
	{
	#endif
		if(GIAposType == LRP_PREPROCESSOR_POS_TYPE_VERB)
		{
			if(determineVerbCaseStandardWithAdditional(wordLowerCase, baseNameFound, grammaticalBaseTenseForm))
			{
				if(!grammaticallyStrict || verbCaseDetectGrammaticallyStrictVariant(*grammaticalBaseTenseForm))
				{
					wordTypeDetected = true;
				}
			}
		}
		else
		{
			unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* wordTypeList = getWordList(GIAposType);	
			if(determineIsWordType(wordLowerCase, wordTypeList))
			{
				wordTypeDetected = true;
			}
		}
	#ifdef GIA_POS_REL_TRANSLATOR_HYBRID_PREFERENCE_NLP_PRELIM_POS_TAGS_OVER_LRP_WORD_TYPE_LISTS
	}
	#endif
	
	if(GIAposType == LRP_PREPROCESSOR_POS_TYPE_NOUN)
	{
		if(wordTypeDetected)
		{
			*baseNameFound = wordLowerCase;
			*grammaticalBaseTenseForm = LRP_PREPROCESSOR_WORD_NOUN_DATABASE_TAG_BASE_TENSE_FORM_SINGULAR;	
		}

		LRPpreprocessorMultiwordReductionWord* nounBaseFound = NULL;	
		int nounGrammaticalBaseTenseForm = LRP_PREPROCESSOR_WORD_NOUN_DATABASE_TAG_BASE_TENSE_FORM_UNKNOWN;
		if(determineNounPluralVariant(wordLowerCase, &nounBaseFound, &nounGrammaticalBaseTenseForm))
		{
			wordTypeDetected = true;
			*baseNameFound = nounBaseFound->tagName;
			#ifdef LRP_PREPROCESSOR_WORD_NOUN_VARIANTS_PRIORITISE_IRREGULAR_PLURAL_FORM
			*grammaticalBaseTenseForm = LRP_PREPROCESSOR_WORD_NOUN_DATABASE_TAG_BASE_TENSE_FORM_PLURAL;
			#else
			*grammaticalBaseTenseForm = nounGrammaticalBaseTenseForm;	//could be GRAMMATICAL_NUMBER_PLURAL or LRP_PREPROCESSOR_WORD_NOUN_DATABASE_TAG_BASE_TENSE_FORM_SINGULAR_OR_PLURAL
			#endif
		}
	}
	
	if(GIAposType == LRP_PREPROCESSOR_POS_TYPE_VERB)
	{
		LRPpreprocessorMultiwordReductionPlainTextWord* wordTagPlaintext = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(wordTag);
		if(wordTagPlaintext->collapsedPhrasalVerbExactDefinedSection)
		{
			*grammaticalBaseTenseForm = wordTagPlaintext->grammaticalTenseFormDetected;	//already determined by LRPpreprocessorWordIdentificationClass::searchAndReplacePhrasalVerbs
			//cout << "*grammaticalBaseTenseForm = " << *grammaticalBaseTenseForm << endl;
			wordTypeDetected = true;
		}	
	}
	
	LRPpreprocessorMultiwordReductionPlainTextWord* wordTagPlaintext = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(wordTag);
	if(wordTagPlaintext->collapsedMultiwordWord && (wordTagPlaintext->collapsedMultiwordWordType == GIAposType))
	{
		if(GIAposType == LRP_PREPROCESSOR_POS_TYPE_VERB)
		{
			cout << "LRPpreprocessorWordIdentificationClass::determineIsWordType warning: !(wordTagPlaintext->collapsedPhrasalVerbExactDefinedSection) && (wordTagPlaintext->collapsedMultiwordWord && (wordTagPlaintext->collapsedMultiwordWordType == LRP_PREPROCESSOR_WORD_DUMMY_COLLAPSED_MULTIWORD_VERB_TYPE))" << endl;
		}
		wordTypeDetected = true;
	}

	return wordTypeDetected;
}
bool LRPpreprocessorWordIdentificationClass::determineIsWordType(const string word, unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* wordTypeList)
{
	return findWordInWordList(wordTypeList, word);
}



bool LRPpreprocessorWordIdentificationClass::determineIsVerb(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim, const bool grammaticallyStrict)
{
	string baseNameFound = "";
	int grammaticalBaseTenseForm = INT_DEFAULT_VALUE;
	return determineIsVerb(wordTag, usePOSprelim, grammaticallyStrict, &baseNameFound, &grammaticalBaseTenseForm);
}
bool LRPpreprocessorWordIdentificationClass::determineIsVerb(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim, const bool grammaticallyStrict, string* baseNameFound, int* grammaticalBaseTenseForm)
{
	bool wordTypeDetected = determineIsWordType(wordTag, usePOSprelim, grammaticallyStrict, LRP_PREPROCESSOR_POS_TYPE_VERB, baseNameFound, grammaticalBaseTenseForm);
	return wordTypeDetected;	
}
bool LRPpreprocessorWordIdentificationClass::determineIsVerbString(const string word)
{
	bool wordTypeDetected = false;
	int grammaticalBaseTenseForm = INT_DEFAULT_VALUE;
	string baseNameFoundNOTUSED = "";
	if(determineVerbCaseStandardWithAdditional(word, &baseNameFoundNOTUSED, &grammaticalBaseTenseForm))
	{
		wordTypeDetected = true;
	}
	return wordTypeDetected;
}

bool LRPpreprocessorWordIdentificationClass::determineVerbCaseStandardWithAdditional(const string word, string* baseNameFound, int* grammaticalBaseTenseForm)
{
	bool foundVerbCaseStandardOrAdditional = false;
	
	bool foundVerbCaseStandard = determineVerbCaseStandard(word, baseNameFound, grammaticalBaseTenseForm);
	if(foundVerbCaseStandard)
	{
		//cout << "determineVerbCaseStandard, word = " << word << endl;
		//cout << "determineVerbCaseStandard, baseNameFound = " << *baseNameFound << endl;
		foundVerbCaseStandardOrAdditional = true;
	}
	
	bool foundVerbCaseAdditional = determineVerbCaseAdditional(word, baseNameFound, grammaticalBaseTenseForm);
	if(foundVerbCaseAdditional)
	{
		//cout << "determineVerbCaseAdditional, word = " << word << endl;
		//cout << "determineVerbCaseAdditional, baseNameFound = " << *baseNameFound << endl;
		foundVerbCaseStandardOrAdditional = true;
	}
	
	return foundVerbCaseStandardOrAdditional;
}
/*
bool LRPpreprocessorWordIdentificationClass::convertVerbCaseGrammaticalTenseFormToTenseModifier(const int grammaticalTenseForm, int* grammaticalTenseModifier)
{
	bool result = false;
	
	*grammaticalTenseModifier = GRAMMATICAL_TENSE_MODIFIER_INFINITIVE_OR_IMPERATIVE_OR_PRESENT_NOT_THIRD_PERSON_SINGULAR_OR_STATE_TEMP;
	
	if(grammaticalTenseForm == LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_CONTINUOUS)
	{
		*grammaticalTenseModifier = GRAMMATICAL_TENSE_MODIFIER_PROGRESSIVE_TEMP;	//corrected @GIA3f1a //OLD: GRAMMATICAL_TENSE_MODIFIER_PROGRESSIVE
		result = true;
	}
	else if(grammaticalTenseForm == LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_PAST)
	{
		*grammaticalTenseModifier = GRAMMATICAL_TENSE_MODIFIER_PAST_TENSE_OR_PAST_PARTICIPLE_OR_STATE_TEMP;
		result = true;
	}
	#ifdef LRP_PREPROCESSOR_DETERMINE_VERB_CASE_ADDITIONAL_EXTENDED
	else if(grammaticalTenseForm == LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_DESCRIPTION)
	{
		*grammaticalTenseModifier = GRAMMATICAL_TENSE_MODIFIER_DESCRIPTION_TEMP;
		result = true;
	}
	else if(grammaticalTenseForm == LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL)
	{
		*grammaticalTenseModifier = GRAMMATICAL_TENSE_MODIFIER_POTENTIAL_TEMP;
		result = true;
	}
	else if(grammaticalTenseForm == LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_POTENTIAL_INVERSE)
	{
		*grammaticalTenseModifier = GRAMMATICAL_TENSE_MODIFIER_POTENTIAL_INVERSE_TEMP;
		result = true;
	}
	#endif
	
	return result;
}
*/
bool LRPpreprocessorWordIdentificationClass::verbCaseDetectGrammaticallyStrictVariant(const int grammaticalTenseForm)
{
	bool result = false;
	if(LRPpreprocessorMultiwordReductionVerbDatabaseTagBaseTenseFormGrammaticallyStrictArray[grammaticalTenseForm])
	{
		result = true;
	}
	return result;
}
bool LRPpreprocessorWordIdentificationClass::determineVerbCaseStandard(const string word, string* baseNameFound, int* grammaticalBaseTenseForm)
{
	bool foundVerbCase = false;
	
	LRPpreprocessorMultiwordReductionWord* wordFound = NULL;
	unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* verbCaseStandardList = &verbCaseStandardListGlobal;
	if(findWordInWordList(verbCaseStandardList, word, &wordFound))
	{
		foundVerbCase = true;
		*grammaticalBaseTenseForm = wordFound->grammaticalTenseForm;
		*baseNameFound = wordFound->baseName;
	}
	
	return foundVerbCase;	
}
bool LRPpreprocessorWordIdentificationClass::determineVerbCaseAdditional(const string word, string* baseNameFound, int* grammaticalBaseTenseForm)
{
	bool foundVerbCase = false;
	
	LRPpreprocessorMultiwordReductionWord* wordFound = NULL;
	unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* verbCaseAdditionalList = &verbCaseAdditionalListGlobal;
	if(findWordInWordList(verbCaseAdditionalList, word, &wordFound))
	{
		foundVerbCase = true;
		*grammaticalBaseTenseForm = wordFound->grammaticalTenseForm;
		*baseNameFound = wordFound->baseName;
	}
	
	return foundVerbCase;	
}


bool LRPpreprocessorWordIdentificationClass::determineIsPreposition(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim)
{
	bool wordTypeDetected = determineIsWordType(wordTag, usePOSprelim, LRP_PREPROCESSOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY_VALUE_IRRELEVANT, LRP_PREPROCESSOR_POS_TYPE_PREPOSITION);
	return wordTypeDetected;
}
bool LRPpreprocessorWordIdentificationClass::determineIsPrepositionString(const string word)
{
	unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* prepositionsList = getWordList(LRP_PREPROCESSOR_POS_TYPE_PREPOSITION);
	bool wordTypeDetected = findWordInWordList(prepositionsList, word);
	return wordTypeDetected;
}


bool LRPpreprocessorWordIdentificationClass::determineIsAdverb(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim)
{
	bool wordTypeDetected = determineIsWordType(wordTag, usePOSprelim, LRP_PREPROCESSOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY_VALUE_IRRELEVANT, LRP_PREPROCESSOR_POS_TYPE_ADVERB);
	return wordTypeDetected;
}
bool LRPpreprocessorWordIdentificationClass::determineIsAdverbString(const string word)
{
	unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* adverbList = getWordList(LRP_PREPROCESSOR_POS_TYPE_ADVERB);
	bool wordTypeDetected = findWordInWordList(adverbList, word);
	return wordTypeDetected;
}


bool LRPpreprocessorWordIdentificationClass::determineIsAdjective(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim)
{
	bool wordTypeDetected = determineIsWordType(wordTag, usePOSprelim, LRP_PREPROCESSOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY_VALUE_IRRELEVANT, LRP_PREPROCESSOR_POS_TYPE_ADJECTIVE);
	return wordTypeDetected;
}
bool LRPpreprocessorWordIdentificationClass::determineIsAdjectiveString(const string word)
{
	unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* adjectiveList = getWordList(LRP_PREPROCESSOR_POS_TYPE_ADJECTIVE);
	bool wordTypeDetected = findWordInWordList(adjectiveList, word);
	return wordTypeDetected;
}


bool LRPpreprocessorWordIdentificationClass::determineIsNoun(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim)
{
	string baseNameFound = "";
	int grammaticalBaseForm = INT_DEFAULT_VALUE;
	return determineIsNoun(wordTag, usePOSprelim, &baseNameFound, &grammaticalBaseForm);
}
bool LRPpreprocessorWordIdentificationClass::determineIsNoun(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim, string* baseNameFound, int* grammaticalBaseForm)
{
	bool wordTypeDetected = determineIsWordType(wordTag, usePOSprelim, LRP_PREPROCESSOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY_VALUE_IRRELEVANT, LRP_PREPROCESSOR_POS_TYPE_NOUN, baseNameFound, grammaticalBaseForm);
	return wordTypeDetected;
}
bool LRPpreprocessorWordIdentificationClass::determineIsNounString(const string word)
{
	unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* nounList = getWordList(LRP_PREPROCESSOR_POS_TYPE_NOUN);
	bool wordTypeDetected = findWordInWordList(nounList, word);
	
	LRPpreprocessorMultiwordReductionWord* nounBaseFound = NULL;	
	int nounGrammaticalBaseTenseForm = LRP_PREPROCESSOR_WORD_NOUN_DATABASE_TAG_BASE_TENSE_FORM_UNKNOWN;
	if(determineNounPluralVariant(word, &nounBaseFound, &nounGrammaticalBaseTenseForm))
	{
		wordTypeDetected = true;
	}
	
	return wordTypeDetected;
}
bool LRPpreprocessorWordIdentificationClass::determineNounPluralVariant(const string word, LRPpreprocessorMultiwordReductionWord** nounBaseFound, int* grammaticalBaseTenseForm)
{
	bool foundNounPluralVariant = false;
	
	LRPpreprocessorMultiwordReductionWord* wordFound = NULL;
	unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* nounPluralVariantsList = &nounPluralVariantsListGlobal;
	if(findWordInWordList(nounPluralVariantsList, word, &wordFound))
	{
		foundNounPluralVariant = true;
		*nounBaseFound = wordFound;
		*grammaticalBaseTenseForm = LRP_PREPROCESSOR_WORD_NOUN_DATABASE_TAG_BASE_TENSE_FORM_PLURAL;
	
		//compensate for irregular nouns (GIA3f5a);
		string nounSingularForm = wordFound->tagName;
		string nounPluralForm = word;
		if(nounSingularForm == nounPluralForm)
		{
			*grammaticalBaseTenseForm = LRP_PREPROCESSOR_WORD_NOUN_DATABASE_TAG_BASE_TENSE_FORM_SINGULAR_OR_PLURAL;
		}
	}
	
	
	return foundNounPluralVariant;	
}

bool LRPpreprocessorWordIdentificationClass::determineIsConjunction(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim)
{
	bool wordTypeDetected = determineIsWordType(wordTag, usePOSprelim, LRP_PREPROCESSOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY_VALUE_IRRELEVANT, LRP_PREPROCESSOR_POS_TYPE_CONJUNCTION_COORDINATING);	//OLD: LRP_PREPROCESSOR_POS_TYPE_CONJUNCTION
	return wordTypeDetected;
}
/*
bool LRPpreprocessorWordIdentificationClass::determineIsConjunctionString(const string word)
{
	unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* conjunctionsList = getWordList(LRP_PREPROCESSOR_POS_TYPE_CONJUNCTION_COORDINATING);	//OLD: LRP_PREPROCESSOR_POS_TYPE_CONJUNCTION
	return findWordInWordList(conjunctionsList, word);
}
*/


bool LRPpreprocessorWordIdentificationClass::determineIsDeterminer(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim)
{
	bool wordTypeDetected = determineIsWordType(wordTag, usePOSprelim, LRP_PREPROCESSOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY_VALUE_IRRELEVANT, LRP_PREPROCESSOR_POS_TYPE_DETERMINER);
	return wordTypeDetected;
}
/*
bool LRPpreprocessorWordIdentificationClass::determineIsDeterminerString(const string word)
{
	unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* determinersList = getWordList(LRP_PREPROCESSOR_POS_TYPE_DETERMINER);
	return findWordInWordList(determinersList, word);
}
*/


bool LRPpreprocessorWordIdentificationClass::determineIsAuxiliaryBeing(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim)
{
	bool wordTypeDetected = false;
	#ifndef LRP_PREPROCESSOR_WORD_COLLAPSE_AUXILIARY_LISTS_TO_VERB_LISTS
	wordTypeDetected = determineIsWordType(wordTag, usePOSprelim, LRP_PREPROCESSOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY_VALUE_IRRELEVANT, LRP_PREPROCESSOR_POS_TYPE_AUXILIARY_BEING);
	#else
	wordTypeDetected = determineIsWordType(wordTag->tagName, wordListRecordAuxiliaryBeing);
	/*
	if(SHAREDvars.textInTextArray(*relationshipName, entityAuxiliaryBeingArray, ENTITY_AUXILIARY_BEING_ARRAY_NUMBER_OF_TYPES))		
	{
		wordTypeDetected = true;
	}
	*/
	#endif
	return wordTypeDetected;
}
bool LRPpreprocessorWordIdentificationClass::determineIsAuxiliaryHaving(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim)
{
	bool wordTypeDetected = false;
	#ifndef LRP_PREPROCESSOR_WORD_COLLAPSE_AUXILIARY_LISTS_TO_VERB_LISTS
	wordTypeDetected = determineIsWordType(wordTag, usePOSprelim, LRP_PREPROCESSOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY_VALUE_IRRELEVANT, LRP_PREPROCESSOR_POS_TYPE_AUXILIARY_HAVING);
	#else
	wordTypeDetected = determineIsWordType(wordTag->tagName, wordListRecordAuxiliaryHaving);
	/*
	if(SHAREDvars.textInTextArray(*relationshipName, entityAuxiliaryHavingArray, ENTITY_AUXILIARY_HAVING_ARRAY_NUMBER_OF_TYPES))
	{
		wordTypeDetected = true;
	}
	*/
	#endif
	return wordTypeDetected;
}
bool LRPpreprocessorWordIdentificationClass::determineIsAuxiliaryDoing(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim)
{
	bool wordTypeDetected = false;
	#ifndef LRP_PREPROCESSOR_WORD_COLLAPSE_AUXILIARY_LISTS_TO_VERB_LISTS
	wordTypeDetected = determineIsWordType(wordTag, usePOSprelim, LRP_PREPROCESSOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY_VALUE_IRRELEVANT, LRP_PREPROCESSOR_POS_TYPE_AUXILIARY_DOING);
	#else
	wordTypeDetected = determineIsWordType(wordTag->tagName, wordListRecordAuxiliaryDoing);
	/*
	if(SHAREDvars.textInTextArray(*relationshipName, entityAuxiliaryDoingArray, ENTITY_AUXILIARY_DOING_ARRAY_NUMBER_OF_TYPES))
	{
		wordTypeDetected = true;
	}
	*/
	#endif
	return wordTypeDetected;
}

bool LRPpreprocessorWordIdentificationClass::detectAuxiliary(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim)
{
	bool wordTypeDetected = false;
	#ifndef LRP_PREPROCESSOR_WORD_COLLAPSE_AUXILIARY_LISTS_TO_VERB_LISTS
	wordTypeDetected = determineIsWordType(wordTag, usePOSprelim, LRP_PREPROCESSOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY_VALUE_IRRELEVANT, LRP_PREPROCESSOR_POS_TYPE_AUXILIARY);
	#else
	if(determineIsWordType(wordTag->tagName, wordListRecordAuxiliaryBeing))
	{
		wordTypeDetected = true;
	}
	if(determineIsWordType(wordTag->tagName, wordListRecordAuxiliaryHaving))
	{
		wordTypeDetected = true;
	}
	if(determineIsWordType(wordTag->tagName, wordListRecordAuxiliaryDoing))
	{
		wordTypeDetected = true;
	}	
	/*
	if(SHAREDvars.textInTextArray(currentWord, entityAuxiliaryBeingArray, ENTITY_AUXILIARY_BEING_ARRAY_NUMBER_OF_TYPES))
	{
		wordTypeDetected = true;
	}
	if(SHAREDvars.textInTextArray(currentWord, entityAuxiliaryHavingArray, ENTITY_AUXILIARY_HAVING_ARRAY_NUMBER_OF_TYPES))
	{
		wordTypeDetected = true;
	}
	if(SHAREDvars.textInTextArray(currentWord, entityAuxiliaryDoingArray, ENTITY_AUXILIARY_DOING_ARRAY_NUMBER_OF_TYPES))
	{
		wordTypeDetected = true;
	}
	*/
	#endif
	return wordTypeDetected;
}

bool LRPpreprocessorWordIdentificationClass::detectModalAuxiliary(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim)
{
	bool wordTypeDetected = false;
	wordTypeDetected = determineIsWordType(wordTag, usePOSprelim, LRP_PREPROCESSOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY_VALUE_IRRELEVANT, LRP_PREPROCESSOR_POS_TYPE_MODALAUXILIARY);
	/*
	if(SHAREDvars.textInTextArray(currentWord, entityModalAuxiliaryArray, ENTITY_MODALAUXILIARY_NUMBER_OF_TYPES))
	{
		wordTypeDetected = true;
	}
	*/
	return wordTypeDetected;
}

bool LRPpreprocessorWordIdentificationClass::detectRcmodSameReferenceSetDelimiter(LRPpreprocessorPlainTextWord* wordTag, const bool usePOSprelim)
{
	bool wordTypeDetected = false;
	wordTypeDetected = determineIsWordType(wordTag, usePOSprelim, LRP_PREPROCESSOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY_VALUE_IRRELEVANT, LRP_PREPROCESSOR_POS_TYPE_RCMOD);
	/*
	if(SHAREDvars.textInTextArray(currentWord, entityRcmodSameReferenceSetDelimiter, ENTITY_REFERENCE_SET_RCMOD_SAME_REFERENCE_SET_DELIMITER_NUMBER_OF_TYPES))
	{
		rcmodSameReferenceSetDelimiterFound = true;
	}
	*/
	return wordTypeDetected;
}

#ifdef SANI_PARSE_SIMULTANEOUS_SET_WORD_POSTYPE_INFERRED_DYNAMIC_OLD
bool LRPpreprocessorWordIdentificationClass::determineIsWordTypeStringBasic(const string word, const int GIAposType)
{
	unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* wordTypeList = getWordList(GIAposType);
	bool wordTypeDetected = findWordInWordList(wordTypeList, word);
	return wordTypeDetected;
}
#endif


bool LRPpreprocessorWordIdentificationClass::findWordInWordList(unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* wordList, const string word)
{	
	bool result = false;
	
	LRPpreprocessorMultiwordReductionWord* wordFound = NULL;
	if(findWordInWordList(wordList, word, &wordFound))
	{
		result = true;
	}
	
	return result;
}	
bool LRPpreprocessorWordIdentificationClass::findWordInWordList(unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* wordList, const string word, LRPpreprocessorMultiwordReductionWord** wordFound)
{	
	bool result = false;
		
	unordered_map<string, LRPpreprocessorMultiwordReductionWord*>::iterator it;
	it = wordList->find(word);
	if(it != wordList->end())
	{
		result = true;
		*wordFound = it->second;
	}
	
	return result;
}	


bool LRPpreprocessorWordIdentificationClass::findSentenceInSentenceListBasic(unordered_map<string, LRPpreprocessorMultiwordReductionBasicSentence*>* sentenceList, const string word, LRPpreprocessorMultiwordReductionBasicSentence** sentenceFound)
{	
	bool result = false;
	
	unordered_map<string, LRPpreprocessorMultiwordReductionBasicSentence*>::iterator it;
	it = sentenceList->find(word);
	if(it != sentenceList->end())
	{
		result = true;
		*sentenceFound = it->second;
	}
	
	return result;
}





/*
//FUTURE GIA;
#ifdef GIA_NLG
#ifdef GIA_NLG_MORPHOLOGY_GENERATOR
bool LRPpreprocessorWordIdentificationClass::generateVerbCase(const string baseName, string* wordGrammatised, int grammaticalTenseForm, int version)
{
	bool foundVerbCase = false;
	
	LRPpreprocessorMultiwordReductionWord* verbList = verbListGlobal;
	LRPpreprocessorMultiwordReductionWord* currentTagInVerbList = verbList;
	while(currentTagInVerbList->nextSentence != NULL)
	{
		if(currentTagInVerbList->tagName == baseName)
		{
			foundVerbCase = true;
			*wordGrammatised = >grammaticalTenseFormsArray[grammaticalTenseForm][version];
		}
		
		currentTagInVerbList = currentTagInVerbList->nextSentence;
	}

	return foundVerbCase;	
}
#endif
#endif
*/


#ifdef LRP_PREPROCESSOR_INITIALISE_WORD_INDEX_LIST_FROM_LRP_FILES
bool LRPpreprocessorWordIdentificationClass::createWordIndexListFromLRPfiles()
{
	bool result = true;
	
	if(!wordListAllTypesWithPOSambiguityInfoLoaded)
	{
		//LRP_PREPROCESSOR_WORD_LOAD_WORD_LISTS: assume that all word lists have been loaded by LRPpreprocessorWordIdentificationClass::initialiseLRP
		if(wordListsLoaded)
		{		
			if(verbListWithVariantsGlobal.size() == 0)
			{	
				//cout << "createWordIndexListFromLRPfiles" << endl;
							
				//generate verbListWithVariantsGlobal and nounListWithVariantsGlobal;
				
				//verbListWithVariantsGlobal = *(getWordList(LRP_PREPROCESSOR_POS_TYPE_VERB));
				//nounListWithVariantsGlobal = *(getWordList(LRP_PREPROCESSOR_POS_TYPE_NOUN));
				unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* verbListGlobal = getWordList(LRP_PREPROCESSOR_POS_TYPE_VERB); 
				unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* nounListGlobal = getWordList(LRP_PREPROCESSOR_POS_TYPE_NOUN); 

				verbListWithVariantsGlobal.insert(verbListGlobal->begin(), verbListGlobal->end());
				#ifdef LRP_PREPROCESSOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY
				verbListWithVariantsGlobal.insert(verbCaseStandardListGlobal.begin(), verbCaseStandardListGlobal.end());
				verbListWithVariantsGlobal.insert(verbCaseAdditionalListGlobal.begin(), verbCaseAdditionalListGlobal.end());
				#else
				//LRP_PREPROCESSOR_POS_TAGGER requires grammatically strict verb variants (regardless of whether grammatically strict verb variants are used by GIA preprocessor for higher level semantic processing of actions; by default they are not)
				
				for(unordered_map<string, LRPpreprocessorMultiwordReductionWord*>::iterator iter = verbCaseStandardListGlobal.begin(); iter != verbCaseStandardListGlobal.end(); iter++)
				{
					string index = iter->first;
					LRPpreprocessorMultiwordReductionWord* word = iter->second;
					#ifdef GIA_POS_REL_TRANSLATOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY
					if(verbCaseDetectGrammaticallyStrictVariant(word->grammaticalTenseForm))	//ensure that the word is a grammatically strict verb
					{
					#endif
						verbListWithVariantsGlobal.insert(pair<string, LRPpreprocessorMultiwordReductionWord*>(index, word));
					#ifdef GIA_POS_REL_TRANSLATOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY
					}
					#endif
				}
				for(unordered_map<string, LRPpreprocessorMultiwordReductionWord*>::iterator iter = verbCaseAdditionalListGlobal.begin(); iter != verbCaseAdditionalListGlobal.end(); iter++)
				{
					string index = iter->first;
					LRPpreprocessorMultiwordReductionWord* word = iter->second;
					#ifdef GIA_POS_REL_TRANSLATOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY
					if(verbCaseDetectGrammaticallyStrictVariant(word->grammaticalTenseForm))	//ensure that the word is a grammatically strict verb
					{
					#endif
						verbListWithVariantsGlobal.insert(pair<string, LRPpreprocessorMultiwordReductionWord*>(index, word));
					#ifdef GIA_POS_REL_TRANSLATOR_GRAMMATICALLY_STRICT_VERB_VARIANTS_ONLY
					}
					#endif
				}
				#endif
				nounListWithVariantsGlobal.insert(nounListGlobal->begin(), nounListGlobal->end());
				nounListWithVariantsGlobal.insert(nounPluralVariantsListGlobal.begin(), nounPluralVariantsListGlobal.end());
				
				#ifdef LRP_PREPROCESSOR_REPLACE_VERB_AND_NOUN_LISTS_WITH_VERB_AND_NOUN_LISTS_WITH_VARIANTS
				verbListGlobal->clear();
				nounListGlobal->clear();
				verbListGlobal->insert(verbListWithVariantsGlobal.begin(), verbListWithVariantsGlobal.end());
				nounListGlobal->insert(nounListWithVariantsGlobal.begin(), nounListWithVariantsGlobal.end());
				#endif
				
				#ifdef LRP_PREPROCESSOR_REMOVE_VERB_VARIANT_CONTINUOUS_FROM_NOUN_LISTS
				for(unordered_map<string, LRPpreprocessorMultiwordReductionWord*>::iterator iter = verbListWithVariantsGlobal.begin(); iter != verbListWithVariantsGlobal.end(); iter++)
				{
					string index = iter->first;
					LRPpreprocessorMultiwordReductionWord* word = iter->second;
					if(word->grammaticalTenseForm == LRP_PREPROCESSOR_WORD_VERB_DATABASE_TAG_BASE_TENSE_FORM_CONTINUOUS)
					{
						unordered_map<string, LRPpreprocessorMultiwordReductionWord*>::iterator it=nounListGlobal.find(index);
						if(it != nounListGlobal.end())
						{
							nounListGlobal.delete(it);
						}
					}
				}
				#endif
			}
	
	
			//see LRP_PREPROCESSOR_WORD_MULTIWORD_REDUCTION:loadWordList - expect multiwords within the word lists to be concatenated with delimiter _ (because all tags in input text with multiwords have had their space delimiters replaced with _ delimiters)
			unordered_map<string, LRPpreprocessorMultiwordReductionWord*> wordListAllTypes;
			for(unordered_map<string, unordered_map<string, LRPpreprocessorMultiwordReductionWord*>*>::iterator iter = wordListsGlobal.begin(); iter != wordListsGlobal.end(); iter++)
			{
				unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* wordList = iter->second;
				wordListAllTypes.insert(wordList->begin(), wordList->end());
			}
		
			for(unordered_map<string, LRPpreprocessorMultiwordReductionWord*>::iterator iter = wordListAllTypes.begin(); iter != wordListAllTypes.end(); iter++)
			{
				string wordIndex = iter->first;
				LRPpreprocessorMultiwordReductionWord* word = iter->second;
				uint64_t POStypeAmbiguity = 0;
				for(int GIAposType=0; GIAposType<LRP_PREPROCESSOR_POS_TYPE_ARRAY_NUMBER_OF_TYPES; GIAposType++)
				{		
					string POStypeName = LRPpreprocessorPOStypeNameArray[GIAposType];
					bool foundWordList = false;
					unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* wordList = getWordListIfExistent(GIAposType, &foundWordList);
					if(foundWordList)
					{
						unordered_map<string, LRPpreprocessorMultiwordReductionWord*>::iterator it;
						it = wordList->find(wordIndex);
						if(it != wordList->end())
						{
							POStypeAmbiguity = (SHAREDvars.setBitValue(POStypeAmbiguity, GIAposType, true));
							//cout << "POStypeAmbiguity = " << POStypeAmbiguity << endl;
							//cout << "(1 << GIAposType) = " << (1 << GIAposType) << endl;
						}
					}
					else
					{
						//ignore posTypes with missing wordLists
						//cerr << "LRPpreprocessorWordIdentificationClass::createWordIndexListFromLRPfiles{} warning: cannot find wordlist, POStypeName = " << POStypeName << endl;
					}
				}
				
				insertInstanceInMapWordListAllTypesWithPOSambiguityInfo(&wordListAllTypesWithPOSambiguityInfo, wordIndex, word, POStypeAmbiguity);
				//cout << "insertInstanceInMapWordListAllTypesWithPOSambiguityInfo: wordIndex = " << wordIndex << ", word->tagName = " << word->tagName << ", POStypeAmbiguity = " << POStypeAmbiguity << endl;
			}
			
			#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FEED_ALL_PERMUTATIONS_INDIVIDUALLY_ASSIGN_WEIGHTS_TO_TRAINED_POS_TYPES
			LRPpreprocessorPOStypeWeights.resize(LRP_PREPROCESSOR_POS_TYPE_ARRAY_NUMBER_OF_TYPES);
			for(int GIAposType=0; GIAposType<LRP_PREPROCESSOR_POS_TYPE_ARRAY_NUMBER_OF_TYPES; GIAposType++)
			{
				string POStypeName = LRPpreprocessorPOStypeNameArray[i];
				unordered_map<string, LRPpreprocessorMultiwordReductionWord*>* wordList = getWordListIfExistent(GIAposType, &foundWordList); 
				if(foundWordList)
				{
					int wordListSize = wordList->size();
					double wordListWeight = LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FEED_ALL_PERMUTATIONS_INDIVIDUALLY_ASSIGN_WEIGHTS_TO_TRAINED_POS_TYPES_WEIGHT_STANDARD;
					if(wordListSize < 100)
					{
						wordListWeight = LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FEED_ALL_PERMUTATIONS_INDIVIDUALLY_ASSIGN_WEIGHTS_TO_TRAINED_POS_TYPES_WEIGHT_HIGH;
					}
					LRPpreprocessorPOStypeWeights[GIAposType] = wordListWeight;
				}
				else
				{
					//ignore posTypes with missing wordLists
					//cerr << "LRPpreprocessorWordIdentificationClass::createWordIndexListFromLRPfiles{} warning: cannot find wordlist, POStypeName = " << POStypeName << endl;
				}
			}
			#endif	
					
			wordListAllTypesWithPOSambiguityInfoLoaded = true;
		}
		else
		{
			cerr << "LRPpreprocessorPOStaggerClass::createWordIndexListFromLRPfiles{} error: !(wordListsLoaded)" << endl;
			exit(EXIT_ERROR);
		}
	}
		
	return result;
}
#endif







