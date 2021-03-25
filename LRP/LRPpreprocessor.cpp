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
 * File Name: LRPpreprocessor.cpp
 * Author: Richard Bruce Baxter - Copyright (c) 2005-2021 Baxter AI (baxterai.com)
 * Project: Language Reduction Preprocessor
 * Project Version: 3p4a 24-March-2021
 * Requirements: requires plain text file
 * Description: Preprocessor
 * /
 *******************************************************************************/


#include "LRPpreprocessor.hpp"
#ifdef USE_GIA
#include "GIAsynRelTranslatorDefs.hpp"
#endif
#include "XMLrulesClass.hpp"

#ifdef LRP_PREPROCESSOR

	
	
bool LRPpreprocessorClass::preprocessTextForGIAwrapper(const bool useLRP, string* inputTextPlainTXTfileName, const string outputLRPTextPlainTXTFileName, bool isQuery, LRPtranslatorVariablesClass* translatorVariables, bool* useInputTextPlainTXTFile, const string inputTextNLPfeatureXMLfileName)
{
	bool result = true;
	
	if(translatorVariables->firstLRPpreprocessorSentenceInList != NULL)
	{
		*useInputTextPlainTXTFile = true;
	}
	
	if(useLRP)
	{
		if(!preprocessTextForGIA(inputTextPlainTXTfileName, outputLRPTextPlainTXTFileName, isQuery, translatorVariables, inputTextNLPfeatureXMLfileName))
		{
			result = false;
		}
	}
	else
	{
		if(translatorVariables->firstLRPpreprocessorSentenceInList != NULL)
		{
			if(!regenerateFileFromPreprocessedTextWithoutLRP(inputTextPlainTXTfileName, outputLRPTextPlainTXTFileName, translatorVariables))
			{
				result = false;
			}
		}

		#ifdef LRP_PREPROCESSOR_RECORD_REFERENCES
		if(!createPreprocessSentencesForGIA(*inputTextPlainTXTfileName, translatorVariables))
		{
			result = false;
		}
		#endif	
	}
	
	return result;
}

bool LRPpreprocessorClass::preprocessTextForGIA(string* inputTextPlainTXTfileName, const string outputLRPTextPlainTXTFileName, bool isQuery, LRPtranslatorVariablesClass* translatorVariables, const string inputTextNLPfeatureXMLfileName)
{
	bool result = true;
	
	
	string outputLRPTextForNLPonlyPlainTXTFileNameBase = outputLRPTextPlainTXTFileName + LRP_PREPROCESSOR_OUTPUT_FOR_NLP_ONLY_FILE_EXTENSION;	
	
	#ifdef GIA_POS_REL_TRANSLATOR
	#ifdef LRP_PREPROCESSOR_WORD_MULTIWORD_REDUCTION
	string outputLRPTextPlainTXTFileNameIntermediaryMultiword = outputLRPTextPlainTXTFileName + LRP_PREPROCESSOR_INTERMEDIARY_MULTIWORD_FILE_EXTENSION;
	string outputLRPTextForNLPonlyPlainTXTFileNameIntermediaryMultiword = outputLRPTextForNLPonlyPlainTXTFileNameBase + LRP_PREPROCESSOR_INTERMEDIARY_MULTIWORD_FILE_EXTENSION;
	#else
	string outputLRPTextPlainTXTFileNameIntermediaryMultiword = *inputTextPlainTXTfileName;
	#endif
	string outputLRPTextPlainTXTFileNameIntermediarySentence = outputLRPTextPlainTXTFileName + LRP_PREPROCESSOR_INTERMEDIARY_SENTENCE_FILE_EXTENSION;
	string outputLRPTextForNLPonlyPlainTXTFileNameIntermediarySentence = outputLRPTextForNLPonlyPlainTXTFileNameBase + LRP_PREPROCESSOR_INTERMEDIARY_SENTENCE_FILE_EXTENSION;
	string outputLRPTextForNLPonlyPlainTXTFileName = outputLRPTextForNLPonlyPlainTXTFileNameIntermediarySentence;
	#else
	#ifdef LRP_PREPROCESSOR_WORD_MULTIWORD_REDUCTION
	string outputLRPTextPlainTXTFileNameIntermediaryMultiword = outputLRPTextPlainTXTFileName;	
	string outputLRPTextForNLPonlyPlainTXTFileNameIntermediaryMultiword = outputLRPTextForNLPonlyPlainTXTFileNameBase + LRP_PREPROCESSOR_INTERMEDIARY_MULTIWORD_FILE_EXTENSION;
	string outputLRPTextForNLPonlyPlainTXTFileName = outputLRPTextForNLPonlyPlainTXTFileNameIntermediaryMultiword;
	#else
	string outputLRPTextForNLPonlyPlainTXTFileName = outputLRPTextForNLPonlyPlainTXTFileNameBase;
	#endif
	#endif

	if(!createPreprocessSentencesForGIA(*inputTextPlainTXTfileName, translatorVariables))
	{
		result = false;
	}
	
	#ifdef LRP_PREPROCESSOR_DEBUG
	LRPpreprocessorSentence* currentLRPpreprocessorSentenceInList = translatorVariables->firstLRPpreprocessorSentenceInList;
	while(currentLRPpreprocessorSentenceInList->next != NULL)
	{
		cout << "currentLRPpreprocessorSentenceInList->sentenceContentsLRPfirstWord = " << LRPpreprocessorWordClassObject.generateTextFromVectorWordList(LRPpreprocessorSentenceClassObject.getSentenceContents(currentPreprocessorSentenceInList), false) << endl;
		currentLRPpreprocessorSentenceInList = currentLRPpreprocessorSentenceInList->next;
	}
	#endif
	
	#ifdef LRP_PREPROCESSOR_WORD_MULTIWORD_REDUCTION
	LRPpreprocessorWordReduction.initialiseActiveLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo(isQuery);
	LRPpreprocessorWordReduction.setActiveLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo(isQuery);	//required for local variable access
	if(!LRPpreprocessorWordReduction.parseTextFileAndReduceLanguage(translatorVariables->firstLRPpreprocessorSentenceInList, outputLRPTextPlainTXTFileNameIntermediaryMultiword, outputLRPTextForNLPonlyPlainTXTFileNameIntermediaryMultiword))
	{
		result = false;
	}
	#endif
	
	#ifdef GIA_POS_REL_TRANSLATOR_HYBRID
	if(!preprocessSentencesForGIAwrapper(translatorVariables, outputLRPTextPlainTXTFileName, inputTextNLPfeatureXMLfileName, outputLRPTextPlainTXTFileNameIntermediarySentence, outputLRPTextForNLPonlyPlainTXTFileNameIntermediarySentence, isQuery))
	{
		result = false;
	}
	#endif
	
	*inputTextPlainTXTfileName = outputLRPTextForNLPonlyPlainTXTFileName;	//now perform NLP using NLP specific (dummy) version of LRP output	
	SHAREDvars.setCurrentDirectory(outputFolder);	//this is required for NLP to parse preprocessed text from outputfolder	//3c2b

	#ifdef GIA_DEBUG_PREPROCESSOR_SENTENCE_PREMATURE_QUIT
	cerr << "Premature quit for debug" << endl;
	exit(EXIT_ERROR);
	#endif
	
	
	return result;
}

#ifdef GIA_POS_REL_TRANSLATOR_HYBRID
bool LRPpreprocessorClass::preprocessSentencesForGIAwrapper(LRPtranslatorVariablesClass* translatorVariables, const string outputLRPTextPlainTXTFileName, const string inputTextNLPfeatureXMLfileName, string outputFileName, string outputFileNameLRPforNLP, bool isQuery)
{
	bool result = true;
	
#ifdef GIA_POS_REL_TRANSLATOR_HYBRID_DEPRECIATED
	#ifdef GIA_POS_REL_TRANSLATOR_HYBRID_EXECUTE_PRELIM_POS_TAGGER
	if(!GIAposRelTranslatorHybrid.executePrelimFeatureProcessingOnSentences(outputLRPTextPlainTXTFileName, inputTextNLPfeatureXMLfileName, translatorVariables))
	{
		result = true;
	}	
	#endif
	if(!GIAposRelTranslatorHybrid.executeTxtRelTranslatorDepreciated(translatorVariables->firstLRPpreprocessorSentenceInList, outputFileName, outputFileNameLRPforNLP))
	{
		result = false;
	}
	#ifdef LRP_PREPROCESSOR_WORD_MULTIWORD_REDUCTION
	if(!GIAposRelTranslatorHybrid.updateLRPpreprocessorMultiwordReductionTagTextCorrespondenceInfo(translatorVariables->firstLRPpreprocessorSentenceInList, isQuery))
	{
		result = false;
	}			
	#endif
#else
	SANIGroup* firstTxtRelTranslatorRulesGroupInSentence = new SANIGroup();
	if(!GIAposRelTranslator.executePosRelTranslator(translatorVariables, &firstTxtRelTranslatorRulesGroupInSentence))
	{
		result = false;
	}
		
	//NOT YET CODED:
	if(!GIAposRelTranslatorHybrid.convertTxtRelationsToLogicReferencesAndReferenceSets(translatorVariables, firstTxtRelTranslatorRulesGroupInSentence))
	{
		result = false;
	}	
	
#endif
	
	return result;
}
#endif
	
			
bool LRPpreprocessorClass::regenerateFileFromPreprocessedTextWithoutLRP(string* inputTextPlainTXTfileName, const string outputLRPTextPlainTXTFileName, const LRPtranslatorVariablesClass* translatorVariables)
{
	bool result = true;
	
	//write text file for nlp input based on firstLRPpreprocessorSentenceInList data

	string outputLRPTextForNLPonlyPlainTXTFileContents = "";
	const LRPpreprocessorSentence* currentLRPpreprocessorSentenceInList = translatorVariables->firstLRPpreprocessorSentenceInList;
	while(currentLRPpreprocessorSentenceInList->next != NULL)
	{
		outputLRPTextForNLPonlyPlainTXTFileContents = outputLRPTextForNLPonlyPlainTXTFileContents + LRPpreprocessorWordClassObject.generateTextFromVectorWordList(&(currentLRPpreprocessorSentenceInList->sentenceContentsOriginal)) + CHAR_NEWLINE;
		currentLRPpreprocessorSentenceInList = currentLRPpreprocessorSentenceInList->next;
	}

	//sync generated filename with preprocessTextForGIA;
	string outputLRPTextForNLPonlyPlainTXTFileNameBase = outputLRPTextPlainTXTFileName + LRP_PREPROCESSOR_OUTPUT_FOR_NLP_ONLY_FILE_EXTENSION;
	string outputLRPTextForNLPonlyPlainTXTFileName = outputLRPTextForNLPonlyPlainTXTFileNameBase;
	
	SHAREDvars.setCurrentDirectory(outputFolder);
	SHAREDvars.writeStringToFile(outputLRPTextForNLPonlyPlainTXTFileName, &outputLRPTextForNLPonlyPlainTXTFileContents);
	SHAREDvars.setCurrentDirectory(inputFolder);
	
	*inputTextPlainTXTfileName = outputLRPTextForNLPonlyPlainTXTFileName;	
	
	return result;
}

bool LRPpreprocessorClass::createPreprocessSentencesForGIA(const string inputFileName, LRPtranslatorVariablesClass* translatorVariables)
{
	bool result = true;

	if(translatorVariables->firstLRPpreprocessorSentenceInList == NULL)
	{
		translatorVariables->firstLRPpreprocessorSentenceInList = new LRPpreprocessorSentence();	//the GIA translator semantic parser needs to use this object to reconstruct the semantic relations from the segregated NLP parsed reference sets generated by the GIA preprocessor and subsequently processed by NLP(/GIA direct semantic relation parser)	
		string fileContents = SHAREDvars.getFileContents(inputFileName);
		bool interpretNewLinesAsNewSentences = false;
		bool splitMultisentenceLines = true;
		if(!createPreprocessSentences(fileContents, translatorVariables->firstLRPpreprocessorSentenceInList, interpretNewLinesAsNewSentences, splitMultisentenceLines))
		{
			result = false;
		}
	}

	//copy sentenceContentsOriginalCurrentWord to sentenceContentsLRPCurrentWord (required by multiword preprocessor)
	LRPpreprocessorSentence* currentLRPpreprocessorSentenceInList = translatorVariables->firstLRPpreprocessorSentenceInList;
	while(currentLRPpreprocessorSentenceInList->next != NULL)
	{
		currentLRPpreprocessorSentenceInList->sentenceContentsLRP = currentLRPpreprocessorSentenceInList->sentenceContentsOriginal;
		
		#ifdef LRP_PREPROCESSOR_RECORD_REFERENCES
		//added 3b4b
		for(int w=0; w<currentLRPpreprocessorSentenceInList->sentenceContentsLRP.size(); w++)
		{		
			LRPpreprocessorMultiwordReductionPlainTextWord* sentenceContentsLRPcurrentWord = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>((currentLRPpreprocessorSentenceInList->sentenceContentsLRP)[w]);
			LRPpreprocessorMultiwordReductionPlainTextWord* sentenceContentsOriginalcurrentWord = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>((currentLRPpreprocessorSentenceInList->sentenceContentsLRP)[w]);
			sentenceContentsLRPcurrentWord->preprocessorUpperLevelWordReference = sentenceContentsOriginalcurrentWord;
			sentenceContentsLRPcurrentWord->preprocessorUpperLevelWordReferenceSize = 1;
		}
		#endif
	
		currentLRPpreprocessorSentenceInList = currentLRPpreprocessorSentenceInList->next;
	}
		
	return result;
}



bool LRPpreprocessorClass::createPreprocessSentences(string fileContents, LRPpreprocessorSentence* firstLRPpreprocessorSentenceInList, const bool interpretNewLinesAsNewSentences, const bool splitMultisentenceLines)
{
	bool result = true;
	
	int charCount = 0;
	char currentToken;
	bool readingQuotation = false;
	string currentWord = "";
	LRPpreprocessorSentence* currentLRPpreprocessorSentenceInList = firstLRPpreprocessorSentenceInList;
	LRPpreprocessorMultiwordReductionPlainTextWord* firstWordInSentence = new LRPpreprocessorMultiwordReductionPlainTextWord();
	LRPpreprocessorMultiwordReductionPlainTextWord* currentWordInSentence = firstWordInSentence;
	string sentenceContentsOriginalText = "";
	int entityIndex = LRP_NLP_START_ENTITY_INDEX;	//only assigned after collapse?
	int sentenceIndex = LRP_NLP_START_SENTENCE_INDEX;
	bool endOfSentencePunctuationMarkFoundPreviously = false;	//NB if interpretNewLinesAsNewSentences && splitMultisentenceLines, GIA preprocessor expects a new line character immediately after a punctuation mark character for it to be interpreted as the delimiter of a single sentence. i.e ". \n" will be interpreted as 2 sentences.
	bool previousCharacterIsMathGrouped = false;
	while(charCount < fileContents.length())
	{		
		currentToken = fileContents[charCount];
		sentenceContentsOriginalText = sentenceContentsOriginalText + currentToken;
		
		bool punctuationMarkFound = false;
		if(SHAREDvars.charInCharArray(currentToken, nlpPunctionMarkCharacterArray, LRP_NLP_NUMBER_OF_PUNCTUATION_MARK_CHARACTERS))
		{
			#ifdef LRP_PREPROCESSOR_WORD_NLP_PARSABLE_PHRASE_SUPPORT_INTRAWORD_PUNCTUATION_MARK
			if(!LRPpreprocessorWordClassObject.isIntrawordPunctuationMark(charCount, &fileContents))
			{
			#endif
				punctuationMarkFound = true;
			#ifdef LRP_PREPROCESSOR_WORD_NLP_PARSABLE_PHRASE_SUPPORT_INTRAWORD_PUNCTUATION_MARK
			}
			#endif
		}
		
		bool whiteSpaceFound = false;
		if(SHAREDvars.charInCharArray(currentToken, nlpWhitespaceCharacterArray, LRP_NLP_NUMBER_OF_WHITESPACE_CHARACTERS))	//NB this includes CHAR_NEWLINE
		{
			whiteSpaceFound = true;
		}
		
		bool newlineFound = false;
		if(currentToken == CHAR_NEWLINE)
		{
			newlineFound = true;
		}
		
		bool apostrophePossessionOrOmissionFound = false;
		#ifdef LRP_PREPROCESSOR_WORD_NLP_PARSABLE_PHRASE_SUPPORT_APOSTROPHES_POSSESSION_AND_OMISSION
		if(LRPpreprocessorWordClassObject.isApostrophePossessionOrOmission(charCount, &fileContents))
		{
			apostrophePossessionOrOmissionFound = true;
			punctuationMarkFound = false;
		}
		#endif
		
		bool dollarFound = false;
		#ifdef LRP_PREPROCESSOR_WORD_NLP_PARSABLE_PHRASE_SUPPORT_DOLLARS
		if(currentToken == CHAR_DOLLAR)
		{
			dollarFound = true;
		}
		#endif
		
		bool quotationMarkFound = false;
		#ifdef LRP_PREPROCESSOR_WORD_NLP_PARSABLE_PHRASE_SUPPORT_APOSTROPHES_POSSESSION_AND_OMISSION
		if(!apostrophePossessionOrOmissionFound)
		{
		#endif
			if(SHAREDvars.charInCharArray(currentToken, nlpQuotationMarkCharacterArray, LRP_NLP_NUMBER_OF_QUOTATIONMARK_CHARACTERS))
			{
				quotationMarkFound = true;
			}
		#ifdef LRP_PREPROCESSOR_WORD_NLP_PARSABLE_PHRASE_SUPPORT_APOSTROPHES_POSSESSION_AND_OMISSION	
		}
		#endif
		
		bool mathFound = false;
		#ifdef LRP_PREPROCESSOR_WORD_NLP_PARSABLE_PHRASE_SUPPORT_MATH
		#ifdef LRP_PREPROCESSOR_WORD_NLP_PARSABLE_PHRASE_SUPPORT_MATH_GROUPING
		if(SHAREDvars.charInCharArray(currentToken, nlpMathCharacterGroupedArray, LRP_NLP_NUMBER_OF_MATH_CHARACTERS_GROUPED))
		{
			//group consecutive math characters into single word; e.g. && / || / == / ~~ / <= / >= etc
			if(charCount+1 < fileContents.length())
			{
				if(!SHAREDvars.charInCharArray(fileContents[charCount+1], nlpMathCharacterGroupedArray, LRP_NLP_NUMBER_OF_MATH_CHARACTERS_GROUPED))
				{
					mathFound = true;
				}
				else
				{
					previousCharacterIsMathGrouped = true;
				}
			}
			else
			{
				mathFound = true;
			}
		}
		if(SHAREDvars.charInCharArray(currentToken, nlpMathCharacterUngroupedArray, LRP_NLP_NUMBER_OF_MATH_CHARACTERS_UNGROUPED))
		{
			mathFound = true;
		}
		#else
		if(SHAREDvars.charInCharArray(currentToken, nlpMathCharacterArray, LRP_NLP_NUMBER_OF_MATH_CHARACTERS))
		{
			mathFound = true;
		}
		#endif
		#endif
		
		#ifdef LRP_PREPROCESSOR_WORD_REDUCE_QUOTES_TO_SINGLE_WORDS
		if(quotationMarkFound)
		{//NB imbedded/recursive quotation marks not currently supported eg "'hello'"
			if(readingQuotation)
			{
				readingQuotation = false;
				currentWord = currentWord + currentToken;	//add quotation mark to word
				LRPpreprocessorWordClassObject.preprocessorFillCurrentWord(&currentWordInSentence, &currentWord, &entityIndex, charCount);
			}
			else
			{
				readingQuotation = true;
				if(currentWord != "")
				{
					cerr << "LRPpreprocessorClass::createPreprocessSentences{}: LRP_PREPROCESSOR_WORD_REDUCE_QUOTES_TO_SINGLE_WORDS error: quotationMarkFound && !readingQuotation && (currentWord != "")" << endl;
					exit(EXIT_ERROR);
				}
				currentWord = currentWord + currentToken;	//add quotation mark to word
			}
		}
		else
		{
		#endif
			bool endOfSentencePunctuationMarkFound = false;
			
			if((whiteSpaceFound || newlineFound || punctuationMarkFound || apostrophePossessionOrOmissionFound || dollarFound || quotationMarkFound || mathFound) && !readingQuotation)
			{
				bool lastWordBlank = true;
				
				int lastCharacterIndexOfLastWordBeingFilled = charCount-1; 
				
				#ifdef GIA_STANFORD_PARSER_AND_CORENLP_FEATURE_PARSER_ANOMALY_INTERPRET_CANNOT_AS_CAN_NOT
				if(currentWord == GIA_STANFORD_PARSER_AND_CORENLP_FEATURE_PARSER_ANOMALY_INTERPRET_CANNOT_AS_CAN_NOT_ORIGINAL_WORD)
				{
					//note this will break characterIndexInSentenceContentsOriginalText (required implementation: need to preprepreprocess words like "cannot" that require splitting by NLP - ie preprocess the text before executing the GIA prepreprocessor createPreprocessSentences)
					int lastCharacterIndexOfWord1BeingFilled = lastCharacterIndexOfLastWordBeingFilled - (string(GIA_STANFORD_PARSER_AND_CORENLP_FEATURE_PARSER_ANOMALY_INTERPRET_CANNOT_AS_CAN_NOT_NEW_WORD_2)).length();
					currentWord = GIA_STANFORD_PARSER_AND_CORENLP_FEATURE_PARSER_ANOMALY_INTERPRET_CANNOT_AS_CAN_NOT_NEW_WORD_1;
					LRPpreprocessorWordClassObject.preprocessorFillCurrentWord(&currentWordInSentence, &currentWord, &entityIndex, lastCharacterIndexOfLastWordBeingFilled);
					currentWord = GIA_STANFORD_PARSER_AND_CORENLP_FEATURE_PARSER_ANOMALY_INTERPRET_CANNOT_AS_CAN_NOT_NEW_WORD_2;
				}
				#endif

				#ifdef LRP_PREPROCESSOR_WORD_NLP_PARSABLE_PHRASE_SUPPORT_MATH_GROUPING
				if(mathFound && previousCharacterIsMathGrouped)
				{
					currentWord = currentWord + currentToken;
					lastCharacterIndexOfLastWordBeingFilled = charCount;
				}
				#endif
				 
				if(currentWord != "")
				{//do not add empty tag after closing quotation marks	//e.g. LRP_PREPROCESSOR_WORD_REDUCE_QUOTES_TO_SINGLE_WORDS or (newlineFound && interpretNewLinesAsNewSentences && previousCharacter==whitespace)
					lastWordBlank = false;
					LRPpreprocessorWordClassObject.preprocessorFillCurrentWord(&currentWordInSentence, &currentWord, &entityIndex, lastCharacterIndexOfLastWordBeingFilled);
				}

				if(quotationMarkFound)
				{
					string quotationMark = ""; 
					quotationMark = quotationMark + currentToken;
					LRPpreprocessorWordClassObject.preprocessorFillCurrentWord(&currentWordInSentence, &quotationMark, &entityIndex, charCount);
				}
				#ifdef LRP_PREPROCESSOR_WORD_NLP_PARSABLE_PHRASE_SUPPORT_MATH_GROUPING
				else if(mathFound && !previousCharacterIsMathGrouped)
				{
					string mathMark = ""; 
					mathMark = mathMark + currentToken;
					LRPpreprocessorWordClassObject.preprocessorFillCurrentWord(&currentWordInSentence, &mathMark, &entityIndex, charCount);
				}
				#endif
				else if(punctuationMarkFound)
				{
					string punctuationMark = ""; 
					punctuationMark = punctuationMark + currentToken;
					LRPpreprocessorWordClassObject.preprocessorFillCurrentWord(&currentWordInSentence, &punctuationMark, &entityIndex, charCount);

					if(SHAREDvars.charInCharArray(currentToken, nlpPunctionMarkCharacterEndOfSentenceArray, LRP_NLP_NUMBER_OF_PUNCTUATION_MARK_CHARACTERS_END_OF_SENTENCE))
					{
						#ifdef LRP_PREPROCESSOR_WORD_NLP_PARSABLE_PHRASE_SUPPORT_INTRAWORD_PUNCTUATION_MARK
						if(!LRPpreprocessorWordClassObject.isIntrawordPunctuationMark(charCount, &fileContents))
						{
						#endif
							endOfSentencePunctuationMarkFound = true;
						#ifdef LRP_PREPROCESSOR_WORD_NLP_PARSABLE_PHRASE_SUPPORT_INTRAWORD_PUNCTUATION_MARK
						}
						#endif
					}
					
					bool lastCharacterInFile = false;
					#ifndef GIA_EXPECT_NEWLINE_AT_END_OF_INPUT_TEXT_FILE
					if(charCount == fileContents.length()-1)
					{
						lastCharacterInFile = true;
					}
					#endif
					if(endOfSentencePunctuationMarkFound && (!interpretNewLinesAsNewSentences || lastCharacterInFile || splitMultisentenceLines))
					{
						LRPpreprocessorWordClassObject.generateSentenceWordList(firstWordInSentence, &(currentLRPpreprocessorSentenceInList->sentenceContentsOriginal));
						currentLRPpreprocessorSentenceInList->sentenceIndexOriginal = sentenceIndex;
						currentLRPpreprocessorSentenceInList->sentenceContentsOriginalText = sentenceContentsOriginalText;
						currentLRPpreprocessorSentenceInList->next = new LRPpreprocessorSentence();
						currentLRPpreprocessorSentenceInList = currentLRPpreprocessorSentenceInList->next;
						firstWordInSentence = new LRPpreprocessorMultiwordReductionPlainTextWord();
						currentWordInSentence = firstWordInSentence;
						sentenceContentsOriginalText = "";
						entityIndex = LRP_NLP_START_ENTITY_INDEX;
						sentenceIndex++;
					}
				}
				else if(newlineFound && interpretNewLinesAsNewSentences)
				{	
					if(!(splitMultisentenceLines && endOfSentencePunctuationMarkFoundPreviously))
					{				
						LRPpreprocessorWordClassObject.generateSentenceWordList(firstWordInSentence, &(currentLRPpreprocessorSentenceInList->sentenceContentsOriginal));
						currentLRPpreprocessorSentenceInList->sentenceIndexOriginal = sentenceIndex;
						currentLRPpreprocessorSentenceInList->sentenceContentsOriginalText = sentenceContentsOriginalText;
						currentLRPpreprocessorSentenceInList->next = new LRPpreprocessorSentence();
						currentLRPpreprocessorSentenceInList = currentLRPpreprocessorSentenceInList->next;
						firstWordInSentence = new LRPpreprocessorMultiwordReductionPlainTextWord();
						currentWordInSentence = firstWordInSentence;
						sentenceContentsOriginalText = "";
						entityIndex = LRP_NLP_START_ENTITY_INDEX;
						sentenceIndex++;
					}

					#ifdef LRP_PREPROCESSOR_WORD_EXTRACT_INDENTATION
					string indentationContents = "";
					int currentIndentation = 0;
					extractIndentationFromCurrentLine(&fileContents, &charCount, &(currentLRPpreprocessorSentenceInList->indentation), &indentationContents);
					//cout << "LRP_PREPROCESSOR_WORD_EXTRACT_INDENTATION: indentationContents = " << indentationContents << endl;
					sentenceContentsOriginalText = sentenceContentsOriginalText + indentationContents;
					#endif
				}
				#ifdef LRP_PREPROCESSOR_WORD_NLP_PARSABLE_PHRASE_SUPPORT_DOLLARS
				else if(dollarFound)
				{
					if(lastWordBlank)
					{
						string dollarMark = ""; 
						dollarMark = dollarMark + currentToken;
						LRPpreprocessorWordClassObject.preprocessorFillCurrentWord(&currentWordInSentence, &dollarMark, &entityIndex, charCount);
					}
					else
					{
						cerr << "LRPpreprocessorClass::createPreprocessSentences{}: error: dollarFound && !lastWordBlank" << endl;	//text cannot be followed by $ sign; e.g. 10$
						exit(EXIT_ERROR);
					}
				}
				#endif
				#ifdef LRP_PREPROCESSOR_WORD_NLP_PARSABLE_PHRASE_SUPPORT_APOSTROPHES_POSSESSION_AND_OMISSION
				else if(apostrophePossessionOrOmissionFound)
				{
					currentWord = currentWord + currentToken;
				}
				#endif
				
				#ifdef LRP_PREPROCESSOR_WORD_NLP_PARSABLE_PHRASE_SUPPORT_MATH_GROUPING
				if(mathFound && previousCharacterIsMathGrouped)
				{
					previousCharacterIsMathGrouped = false;
				}
				#endif
				
			}
			else
			{
				currentWord = currentWord + currentToken;
			}
			
			endOfSentencePunctuationMarkFoundPreviously = endOfSentencePunctuationMarkFound;

		#ifdef LRP_PREPROCESSOR_WORD_REDUCE_QUOTES_TO_SINGLE_WORDS
		}
		#endif
		charCount++;
	}
		
	return result;
}

#ifdef LRP_PREPROCESSOR_WORD_EXTRACT_INDENTATION
bool LRPpreprocessorClass::extractIndentationFromCurrentLine(string* lineContents, int* indexOfCurrentToken, int* currentIndentation, string* indentationContents)
{
	bool result = false;
	if(*indexOfCurrentToken+1 < lineContents->length())
	{
		int i = *indexOfCurrentToken+1;
		while((i < lineContents->length()) && ((*lineContents)[i] == LRP_PREPROCESSOR_INDENTATION_CHAR))
		{
			*currentIndentation = *currentIndentation + 1;
			*indentationContents = *indentationContents + LRP_PREPROCESSOR_INDENTATION_CHAR;
			i++;
		}
		if(i > *indexOfCurrentToken+1)
		{
			*indexOfCurrentToken = *indexOfCurrentToken + *currentIndentation;
			result = true;
		}
	}
	return result;
}

string LRPpreprocessorClass::generateIndentationContents(const int currentIndentation)
{
	string indentationContents = "";
	for(int i=0; i<currentIndentation; i++)
	{
		indentationContents = indentationContents + LRP_PREPROCESSOR_INDENTATION_CHAR;
	}
	return indentationContents;
}
#endif



			
void LRPpreprocessorClass::copySentenceContentsPreprocessor(LRPpreprocessorMultiwordReductionPlainTextWord* sentenceContents1FirstWord, LRPpreprocessorMultiwordReductionPlainTextWord* sentenceContents2firstWord)
{
	LRPpreprocessorMultiwordReductionPlainTextWord* sentenceContents1currentWord = sentenceContents1FirstWord;
	LRPpreprocessorMultiwordReductionPlainTextWord* sentenceContents2currentWord = sentenceContents2firstWord;
		
	while(sentenceContents1currentWord->nextTag != NULL)
	{		
		sentenceContents2currentWord->tagName = sentenceContents1currentWord->tagName;
		#ifdef LRP_PREPROCESSOR_RECORD_REFERENCES
		sentenceContents2currentWord->preprocessorUpperLevelWordReference = sentenceContents1currentWord->preprocessorUpperLevelWordReference;
		sentenceContents2currentWord->preprocessorUpperLevelWordReferenceSize = sentenceContents1currentWord->preprocessorUpperLevelWordReferenceSize;
		//sentenceContents2currentWord->preprocessorLowerLevelWordReference = sentenceContents1currentWord->preprocessorLowerLevelWordReference;	//CHECKTHIS (never added)
		//sentenceContents2currentWord->characterIndexInSentenceContentsOriginalText = sentenceContents1currentWord->characterIndexInSentenceContentsOriginalText;	//CHECKTHIS (never added)
		#endif
		sentenceContents2currentWord->entityIndex = sentenceContents1currentWord->entityIndex;
		sentenceContents2currentWord->nextTag = new LRPpreprocessorMultiwordReductionPlainTextWord();
		sentenceContents2currentWord = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(sentenceContents2currentWord->nextTag);

		sentenceContents1currentWord = static_cast<LRPpreprocessorMultiwordReductionPlainTextWord*>(sentenceContents1currentWord->nextTag);
	}
}


#endif





