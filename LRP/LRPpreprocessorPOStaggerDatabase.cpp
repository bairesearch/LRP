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
 * File Name: LRPpreprocessorPOStaggerDatabase.cpp
 * Author: Richard Bruce Baxter - Copyright (c) 2005-2021 Baxter AI (baxterai.com)
 * Project: Language Reduction Preprocessor
 * Project Version: 3p6b 07-August-2021
 * Requirements: requires plain text file
 * Description: Preprocessor POS tagger database
 * /
 *******************************************************************************/


#include "LRPpreprocessorPOStaggerDatabase.hpp"
#ifdef USE_GIA
#include "GIAsynRelTranslatorDefs.hpp"
#endif



static string LRPposTaggerDatabaseFolderName;
#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_INTERNAL
ANNneuron* firstInputNeuronInNetworkGIAposTagger;
ANNneuron* firstOutputNeuronInNetworkGIAposTagger;
int64_t numberOfInputNeuronsGIAposTagger;
int64_t numberOfOutputNeuronsGIAposTagger;
int64_t numberOfLayersGIAposTagger;	//static variable not currently utilised
#endif
//#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_PERSISTENT
void LRPpreprocessorPOStaggerDatabaseClass::initialisePOStaggerDatabase(const string newLRPposTaggerDatabaseFolderName)
{
	LRPposTaggerDatabaseFolderName = newLRPposTaggerDatabaseFolderName;
	if(!SHAREDvars.directoryExists(&LRPposTaggerDatabaseFolderName))
	{
		cerr << "LRPpreprocessorPOStaggerDatabaseClass::initialisePOStaggerDatabase{} error: LRPposTaggerDatabaseFolderName does not exist: " << LRPposTaggerDatabaseFolderName << endl;
		exit(EXIT_ERROR);	
	}
		
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_INTERNAL
	//read existing neural network
	firstInputNeuronInNetworkGIAposTagger = new ANNneuron();
	//string neuralNetworkXmlFileName = LRPposTaggerDatabaseFolderName + LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_DEFAULT_XML_FILE_NAME;	//alternate method
	string neuralNetworkXmlFileName = LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_DEFAULT_XML_FILE_NAME;
	SHAREDvars.setCurrentDirectory(&LRPposTaggerDatabaseFolderName);
	if(SHAREDvars.fileExists(neuralNetworkXmlFileName))
	{
		firstOutputNeuronInNetworkGIAposTagger = ANNxmlConversion.readNetXMLfileAndRecordFormationVariables(neuralNetworkXmlFileName, firstInputNeuronInNetworkGIAposTagger, &numberOfInputNeuronsGIAposTagger, &numberOfOutputNeuronsGIAposTagger, &numberOfLayersGIAposTagger);
		cout << "numberOfInputNeuronsGIAposTagger = " << numberOfInputNeuronsGIAposTagger << endl;
		cout << "numberOfOutputNeuronsGIAposTagger = " << numberOfOutputNeuronsGIAposTagger << endl;
	}
	else
	{
		numberOfInputNeuronsGIAposTagger = LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_NUMBER_OF_INPUT_NEURONS;
		numberOfOutputNeuronsGIAposTagger = LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_NUMBER_OF_OUTPUT_NEURONS;
		numberOfLayersGIAposTagger = LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_NUMBER_OF_LAYERS;
		firstOutputNeuronInNetworkGIAposTagger = ANNformation.formNeuralNetWithOptimisedProperties(firstInputNeuronInNetworkGIAposTagger, numberOfInputNeuronsGIAposTagger, numberOfOutputNeuronsGIAposTagger, numberOfLayersGIAposTagger);
		ANNalgorithmBackpropagationTraining.resetNeuralNetworkWithRandomBiasAndWeights(firstInputNeuronInNetworkGIAposTagger);
	}
	#endif
}
//#endif


#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_MAP
multimap<string, pair<uint64_t, int>> POStaggerMap;		//each key is ~10 64bit ints int64_t: word context POS (ambiguity info) permutation, and the value is 1 64 bit int int64_t: POS (ambiguity info) for central word
	//as it currently stands POStaggerMap will be roughly the same size as the original wiki dump text (ie 12GB; too large)
bool LRPpreprocessorPOStaggerDatabaseClass::findInstancePOStaggerMap(const vector<uint64_t>* POSambiguityInfoPermutation, const uint64_t centreWordPOSambiguityInfo, int* numberOfInstances, const bool incrementIfFound)
{
	bool result = false;

	string POSambiguityInfoPermutationIndexString = convertPOSambiguityInfoPermutationToPOSambiguityInfoPermutationIndexString(POSambiguityInfoPermutation);	
	pair<multimap<string, pair<uint64_t, int>>::iterator, multimap<string, pair<uint64_t, int>>::iterator> rangeFound = POStaggerMap.equal_range(POSambiguityInfoPermutationIndexString);
	for(multimap<string, pair<uint64_t, int>>::iterator it = rangeFound.first; it != rangeFound.second; it++)
	{
		uint64_t centreWordPOSambiguityInfoCurrent = (it->second).first;
		if(centreWordPOSambiguityInfoCurrent == centreWordPOSambiguityInfo)
		{
			result = true;
			*numberOfInstances = (it->second).second;
			if(incrementIfFound)
			{
				(it->second).second = (it->second).second + 1;
			}
		}
	}
	
	return result;
}	
void LRPpreprocessorPOStaggerDatabaseClass::insertInstanceInPOStaggerMap(const vector<uint64_t>* POSambiguityInfoPermutation, const uint64_t centreWordPOSambiguityInfo, const int numberOfInstances)
{
	string POSambiguityInfoPermutationIndexString = convertPOSambiguityInfoPermutationToPOSambiguityInfoPermutationIndexString(POSambiguityInfoPermutation);
	pair<uint64_t, int> value = make_pair(centreWordPOSambiguityInfo, numberOfInstances);
	POStaggerMap.insert(pair<string, pair<uint64_t, int>>(POSambiguityInfoPermutationIndexString, value));
}	
multimap<string,pair<uint64_t,int>>* LRPpreprocessorPOStaggerDatabaseClass::getPOStaggerMap()
{
	return &POStaggerMap;
}
#endif
string LRPpreprocessorPOStaggerDatabaseClass::convertPOSambiguityInfoPermutationToPOSambiguityInfoPermutationIndexString(const vector<uint64_t>* POSambiguityInfoPermutation)
{
	string POSambiguityInfoPermutationIndexString = "";
	for(int i=0; i<POSambiguityInfoPermutation->size(); i++)
	{
		uchar POSpermutationIndexByte = convertPOSambiguityInfoToIndex(POSambiguityInfoPermutation->at(i));
		#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_AND_MAP_USE_6BIT_INDICES
		char character = DBconvertByteToBase64(POSpermutationIndexByte);		
		string str = "";
		str = str + character;	
		#else
		string str = DBconvertByteToHex(POSpermutationIndexByte);
		#endif		
		POSambiguityInfoPermutationIndexString = POSambiguityInfoPermutationIndexString + str;
	}
	return POSambiguityInfoPermutationIndexString;
}

#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK
#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_INTERNAL
#ifdef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE
void LRPpreprocessorPOStaggerDatabaseClass::feedNeuralNetworkWithExperience(ANNexperience* currentExperience)
{
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_INTERNAL_CLASSIFICATION_NET
	ANNalgorithmClassificationNetworkTraining.feedNeuralNetworkWithExperienceClassification(firstInputNeuronInNetworkGIAposTagger, numberOfInputNeuronsGIAposTagger, currentExperience);	
	#else
	ANNalgorithmBackpropagationTraining.feedNeuralNetworkWithExperienceBackpropagation(firstInputNeuronInNetworkGIAposTagger, firstOutputNeuronInNetworkGIAposTagger, numberOfInputNeuronsGIAposTagger, numberOfOutputNeuronsGIAposTagger, currentExperience);
	#endif
}
bool LRPpreprocessorPOStaggerDatabaseClass::writeDatabaseNeuralNetwork()
{
	bool result = true;
	
	SHAREDvars.setCurrentDirectory(LRPposTaggerDatabaseFolderName);
	string neuralNetworkXmlFileName = LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_DEFAULT_XML_FILE_NAME;
	if(!ANNxmlConversion.writeNetXMLfile(neuralNetworkXmlFileName, firstInputNeuronInNetworkGIAposTagger))
	{
		result = false;
	}
	
	return result;
}
#endif
bool LRPpreprocessorPOStaggerDatabaseClass::calculateIdealClassTargetOfInputExperience(const ANNexperience* experience, const int* idealClassTarget, const double* experienceBackPropagationPassError)
{
	bool result = false;
	
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_INTERNAL_CLASSIFICATION_NET
	if(ANNalgorithmClassificationNetworkTraining.calculateIdealClassTargetOfInputExperience(firstInputNeuronInNetworkGIAposTagger, firstOutputNeuronInNetworkGIAposTagger, numberOfInputNeuronsGIAposTagger, numberOfOutputNeuronsGIAposTagger, experience, idealClassTarget, experienceBackPropagationPassError))
	{
		result = true;
	}	
	if(*idealClassTarget >= LRP_PREPROCESSOR_POS_TYPE_ARRAY_NUMBER_OF_TYPES)
	{
		cerr << "LRPpreprocessorPOStaggerDatabaseClass::calculateIdealClassTargetOfInputExperience{} error: (*idealClassTarget >= LRP_PREPROCESSOR_POS_TYPE_ARRAY_NUMBER_OF_TYPES)" << endl;
		cerr << "classification network has identified the ambiguous word as a punctuation character" << endl;
		result = false;
	}
	#else
	if(ANNalgorithmBackpropagationTraining.calculateIdealClassTargetOfInputExperience(firstInputNeuronInNetworkGIAposTagger, firstOutputNeuronInNetworkGIAposTagger, numberOfInputNeuronsGIAposTagger, numberOfOutputNeuronsGIAposTagger, experience, idealClassTarget, experienceBackPropagationPassError))
	{
		result = true;
	}
	#endif
	
	return result;
}
#endif
#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL

#ifdef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE
bool LRPpreprocessorPOStaggerDatabaseClass::externalANNgenerateBatchTrainData(const ANNexperience* firstExperienceInList, const int batchIndex)
{
	string XtrainBatchFileName = externalANNgenerateBatchFileName(LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_X_TRAIN_BATCH_FILE_NAME_PARTA, batchIndex);
	string YtrainBatchFileName = externalANNgenerateBatchFileName(LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_Y_TRAIN_BATCH_FILE_NAME_PARTA, batchIndex);
	
	//generate train batch file
	vector<string> batchTrainDataInput;
	vector<string> batchTrainDataOutput;
	externalANNgenerateBatchDataExperiences(firstExperienceInList, &batchTrainDataInput, &batchTrainDataOutput);
	SHAREDvars.writeStringListToFile(XtrainBatchFileName, &batchTrainDataInput);
	SHAREDvars.writeStringListToFile(YtrainBatchFileName, &batchTrainDataOutput);
	
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_TRAIN_EXECUTE_FEED_SINGLE_BATCH_ONLY
	externalANNgenerateBatchTrainDataExecuteFeed();
	#endif
}

string LRPpreprocessorPOStaggerDatabaseClass::externalANNgenerateBatchFileName(const string fileNamePrepend, const int batchIndex)
{
	SHAREDvars.setCurrentDirectory(LRPposTaggerDatabaseFolderName);	//save output files to output folder

	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_TRAIN_SINGLE_BATCH_ONLY
	string XtrainBatchFileName = fileNamePrepend + LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_BATCH_FILE_EXTENSION;
	#else
	string formatString = generateIntFormatString(LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE_DOC_XML_OUTPUT_NAME_PART_B_INDEX_NUMBER_OF_CHARACTERS);
	string XtrainBatchFileName = fileNamePrepend + SHAREDvars.convertIntToString(batchIndex, formatString) + LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_BATCH_FILE_EXTENSION;
	#endif
	
	return XtrainBatchFileName;
}

#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_TRAIN_EXECUTE_FEED
bool LRPpreprocessorPOStaggerDatabaseClass::externalANNgenerateBatchTrainDataExecuteFeed()
{
	//immediately train the batch
	externalANNtrainEpochBatch();
}	
bool LRPpreprocessorPOStaggerDatabaseClass::externalANNform()
{
	externalANNexecuteScript(LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_SCRIPT_NAME_FORM);
}
bool LRPpreprocessorPOStaggerDatabaseClass::externalANNtrain()
{
	externalANNexecuteScript(LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_SCRIPT_NAME_TRAIN);
}
bool LRPpreprocessorPOStaggerDatabaseClass::externalANNtrainEpoch()
{
	externalANNexecuteScript(LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_SCRIPT_NAME_TRAIN_EPOCH);
}
bool LRPpreprocessorPOStaggerDatabaseClass::externalANNtrainEpochBatch()
{
	externalANNexecuteScript(LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_SCRIPT_NAME_TRAIN_EPOCH_BATCH);
}
bool LRPpreprocessorPOStaggerDatabaseClass::externalANNtest()
{
	externalANNexecuteScript(LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_SCRIPT_NAME_TEST);
}
#endif

#endif

bool LRPpreprocessorPOStaggerDatabaseClass::externalANNpredict(ANNexperience* firstExperienceInList)
{
	bool result = true;
	
	string batchExperiencesString = "";
	
	SHAREDvars.setCurrentDirectory(LRPposTaggerDatabaseFolderName);	//save output files to output folder

	string XpredictBatchFileName = string(LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_X_PREDICT_BATCH_FILE_NAME_PREPEND) + (LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_BATCH_FILE_EXTENSION);
	string YpredictBatchFileName = string(LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_Y_PREDICT_BATCH_FILE_NAME_PREPEND) + (LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_BATCH_FILE_EXTENSION);

	//generate predictions batch file
	vector<string> batchPredictionsDataInput;
	vector<string> batchPredictionsDataOutputNOTUSED;
	externalANNgenerateBatchDataExperiences(firstExperienceInList, &batchPredictionsDataInput, &batchPredictionsDataOutputNOTUSED);
	SHAREDvars.writeStringListToFile(XpredictBatchFileName, &batchPredictionsDataInput);
	
	//generate predictions
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_PREDICT_RETURN_ERRORS
	string externalANNscriptName = LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_SCRIPT_NAME_PREDICT_RETURN_ERRORS;	//will become mandatory in the future
	#else
	string externalANNscriptName = LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_SCRIPT_NAME_PREDICT;	//to be depreciated
	#endif
	if(!externalANNexecuteScript(externalANNscriptName))
	{
		result = false;
	}
	
	//read predictions batch file
	vector<string> YpredictBatchFileContents;
	int numberOfLinesInFile = 0;	//verify int is sufficient (else require int64_t)
	if(!SHAREDvars.getLinesFromFile(YpredictBatchFileName, &YpredictBatchFileContents, &numberOfLinesInFile))
	{
		result = false;
	}
	ANNexperience* currentExperienceInList = firstExperienceInList;
	for(int i=0; i<YpredictBatchFileContents.size(); i++)
	{
		#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_PREDICT_RETURN_ERRORS
		int positionOfDelimiter = YpredictBatchFileContents[i].find(LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_BATCH_FILE_DELIMITER);
		string classTargetValueString = "";
		string predictionErrorString = "";
		if(positionOfDelimiter != CPP_STRING_FIND_RESULT_FAIL_VALUE)
		{
			classTargetValueString = YpredictBatchFileContents[i].substr(0, positionOfDelimiter);
			predictionErrorString = YpredictBatchFileContents[i].substr(positionOfDelimiter+1);
		}
		else
		{
			cerr << "LRPpreprocessorPOStaggerDatabaseClass::externalANNpredict{} error: (positionOfDelimiter == CPP_STRING_FIND_RESULT_FAIL_VALUE)" << endl;
			exit(EXIT_ERROR);		
		}
		#else
		string classTargetValueString = YpredictBatchFileContents[i];
		#endif
		
		#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_Y_PREDICT_HOT_ENCODED
		cerr << "LRPpreprocessorPOStaggerDatabaseClass::externalANNpredict{} error: LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_Y_PREDICT_HOT_ENCODED not coded" << endl;
		exit(EXIT_ERROR);
		#else
		int64_t classTargetValue = int64_t(SHAREDvars.convertStringToDouble(classTargetValueString));	//assume that YpredictBatchFileContents is not hot encoded
		cout << "LRPpreprocessorPOStaggerDatabaseClass::externalANNgeneratePredictions{}: classTargetValue = " << classTargetValue << endl;
		#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_PREDICT_RETURN_ERRORS
		double predictionError = SHAREDvars.convertStringToDouble(predictionErrorString);
		cout << "LRPpreprocessorPOStaggerDatabaseClass::externalANNgeneratePredictions{}: predictionError = " << predictionError << endl;
		#endif
		#endif
		
		currentExperienceInList->classTargetValue = classTargetValue;
		#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_PREDICT_RETURN_ERRORS
		currentExperienceInList->predictionError = predictionError;
		#endif
		currentExperienceInList = currentExperienceInList->next;
	}

	return result;
}
bool LRPpreprocessorPOStaggerDatabaseClass::externalANNgenerateBatchDataExperiences(const ANNexperience* firstExperienceInList, vector<string>* batchDataInput, vector<string>* batchDataOutput)
{
	bool result = true;
	
	//generate predictions batch file
	const ANNexperience* currentExperienceInList = firstExperienceInList;
	while(currentExperienceInList->next != NULL)
	{
		string experienceInputString = externalANNgenerateBatchDataExperienceInput(currentExperienceInList);
		batchDataInput->push_back(experienceInputString);
		string experienceOutputString = externalANNgenerateBatchDataExperienceInput(currentExperienceInList);
		batchDataOutput->push_back(experienceOutputString);
		
		currentExperienceInList = currentExperienceInList->next;
	}
}

string LRPpreprocessorPOStaggerDatabaseClass::externalANNgenerateBatchDataExperienceInput(const ANNexperience* currentExperienceInList)
{	
	return externalANNgenerateExperienceInputStandalone(currentExperienceInList);
}

string LRPpreprocessorPOStaggerDatabaseClass::externalANNgenerateBatchDataExperienceOutput(const ANNexperience* currentExperienceInList)
{	
	string experienceOutputString = "";
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_Y_TRAIN_HOT_ENCODED
	int numberOfOutputNeurons = LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_NUMBER_OF_OUTPUT_NEURONS;
	for(int64_t i = 0; i < numberOfOutputNeurons; i++)
	{		
		string format = "%g";	//"%0.0f";
		if(i == currentExperienceInList->classTargetValue)
		{
			experienceOutputString = experienceOutputString + SHAREDvars.convertDoubleToString(1.0, format);
		}
		else
		{
			experienceOutputString = experienceOutputString + SHAREDvars.convertDoubleToString(0.0, format);
		}
		experienceOutputString = experienceOutputString + LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_BATCH_FILE_DELIMITER;
	}
	#else
	experienceOutputString = SHAREDvars.convertLongToString(currentExperienceInList->classTargetValue);
	#endif
	return experienceOutputString;
}

bool LRPpreprocessorPOStaggerDatabaseClass::externalANNexecuteScript(string scriptName)
{
	SHAREDvars.setCurrentDirectory(LRPposTaggerDatabaseFolderName);
	scriptName = string("python ") + scriptName + string(LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_SCRIPT_EXTENSION);
	//scriptName = string("./") + scriptName + string(LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_SCRIPT_EXTENSION);	//requires "#!/usr/bin/env python" to be added to top of each py file
	cout << "scriptName = " << scriptName << endl;
	system(scriptName.c_str());
}
#endif
#endif


#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM
string LRPpreprocessorPOStaggerDatabaseClass::DBgenerateFileName(const vector<uint64_t>* POSambiguityInfoPermutation)
{
	//eg network/server/LRPPOStaggerDatabase/ffff/ffff/ffff/ffff/ffff/POSpermutationffffffffffffffffffffff.pos
	//string serverName = GIAdatabase.DBgenerateServerDatabaseName(&(GIAconnectionistNetworkPOStypeNameAbbreviationArray[firstFeatureInList->GIAsemRelTranslatorPOStype]), fileType, LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_DEFAULT_DATABASE_NAME, LRPposTaggerDatabaseFolderName);
	string serverName = LRPposTaggerDatabaseFolderName;
	string fileName = serverName;

	SHAREDvars.setCurrentDirectory(&fileName);

	int numberOfPOSpermutationLevels = LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_SUBDIRECTORY_INDEX_NUMBER_OF_LEVELS;
	string POSpermutationFileName = "";
	for(int level=0; level<numberOfPOSpermutationLevels; level++)
	{
		//cout << "level = " << level << endl;
		string folderName = DBgenerateSubFolderName(POSambiguityInfoPermutation, level, LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_SUBDIRECTORY_INDEX_NUMBER_OF_WORDS_PER_LEVEL);
		//cout << "folderName = " << folderName << endl;
		POSpermutationFileName = POSpermutationFileName + folderName;
		fileName = fileName + folderName + "/";
		SHAREDvars.checkIfFolderExistsAndIfNotMakeAndSetAsCurrent(&folderName);
	}
	
	int numberOfRemainingWordsInPOSpermutation = (POSambiguityInfoPermutation->size()-LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_SUBDIRECTORY_INDEX_NUMBER_OF_LEVELS*LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_SUBDIRECTORY_INDEX_NUMBER_OF_WORDS_PER_LEVEL);
	if(numberOfRemainingWordsInPOSpermutation > 0)
	{
		POSpermutationFileName = POSpermutationFileName + DBgenerateSubFolderName(POSambiguityInfoPermutation, LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_SUBDIRECTORY_INDEX_NUMBER_OF_LEVELS, numberOfRemainingWordsInPOSpermutation);
	}
	
	POSpermutationFileName = string(LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_POS_PERMUTATION_ENTRY_FILE_NAME_PARTA) + POSpermutationFileName + LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_POS_PERMUTATION_ENTRY_FILE_NAME_EXTENSION;
	fileName = fileName + POSpermutationFileName;
	//cout << "fileName = " << fileName << endl;

	return fileName;
}

string LRPpreprocessorPOStaggerDatabaseClass::DBgenerateSubFolderName(const vector<uint64_t>* POSambiguityInfoPermutation, const int level, const int numberOfWordsPerLevel)
{
	string folderName = "";
	for(int i=level*numberOfWordsPerLevel; i<(level*numberOfWordsPerLevel)+numberOfWordsPerLevel; i++)
	{
		uchar POSpermutationIndexByte = convertPOSambiguityInfoToIndex(POSambiguityInfoPermutation->at(i));
		#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_AND_MAP_USE_6BIT_INDICES
		folderName = folderName + DBconvertByteToBase64(POSpermutationIndexByte);
		#else
		folderName = folderName + DBconvertByteToHex(POSpermutationIndexByte);
		#endif
	}
	return folderName;
}














bool LRPpreprocessorPOStaggerDatabaseClass::DBreadPOSpermutationEstimates(const vector<uint64_t>* POSambiguityInfoPermutation, vector<string>* centreWordPOSambiguityInfoList)
{
	bool POSpermutationEntryExistent = false;
	
	/*
	Format:
	Z 0000000000 [1 6 bit char (central POStag), space, then 1 32 bit numReferences]
	Z 0000000000
	Z 0000000000
	Z 0000000000
	...
	*/

	string referencesFileName = DBgenerateFileName(POSambiguityInfoPermutation);
	//now read file

	int numberOfLinesInFile = 0;
	if(SHAREDvars.getLinesFromFile(referencesFileName, centreWordPOSambiguityInfoList, &numberOfLinesInFile))
	{
		if(numberOfLinesInFile != 0)
		{
			POSpermutationEntryExistent = true;
		}
		else
		{
			cout << "LRPpreprocessorPOStaggerDatabaseClass::DBreadPOSpermutationEstimates{} error: numberOfLinesInFile == 0; referencesFileName, " << referencesFileName << endl;		
		}
	}
	else
	{
		//cout << "LRPpreprocessorPOStaggerDatabaseClass::DBreadPOSpermutationEstimates{} error: !SHAREDvars.getLinesFromFile; referencesFileName, " << referencesFileName << endl;		
	}
	
	return POSpermutationEntryExistent;
}

#ifdef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE
bool LRPpreprocessorPOStaggerDatabaseClass::DBwritePOSpermutationEstimate(const vector<uint64_t>* POSambiguityInfoPermutation, const uint64_t centreWordPOSambiguityInfo)
{
	bool result = true;
	
	//cout << "DBwritePOSpermutationEstimate: convertPOSambiguityInfoPermutationToPOSambiguityInfoPermutationIndexString: " << convertPOSambiguityInfoPermutationToPOSambiguityInfoPermutationIndexString(POSambiguityInfoPermutation) << endl;
	
	uchar centreWordPOSpermutationIndexByte = convertPOSambiguityInfoToIndex(centreWordPOSambiguityInfo);
	
	vector<string> centreWordPOSambiguityInfoList;
	bool POSpermutationEntryExistent = DBreadPOSpermutationEstimates(POSambiguityInfoPermutation, &centreWordPOSambiguityInfoList);
	
	bool foundMatchingCentreWordPOSambiguityInfo = false;
	if(POSpermutationEntryExistent)
	{
		for(int i=0; i<centreWordPOSambiguityInfoList.size(); i++)
		{
			string centreWordPOSambiguityReferenceString = centreWordPOSambiguityInfoList[i];
			string centreWordPOSambiguityInfoCurrentByteBaseXstring = centreWordPOSambiguityReferenceString.substr(LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_POS_PERMUTATION_ENTRY_CENTRE_WORD_POS_AMBIGUITY_BYTE_CODED_START_POS, LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_POS_PERMUTATION_ENTRY_CENTRE_WORD_POS_AMBIGUITY_BYTE_CODED_LENGTH);
			#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_AND_MAP_USE_6BIT_INDICES
			char centreWordPOSambiguityInfoCurrentByteBaseXchar = centreWordPOSambiguityInfoCurrentByteBaseXstring[0];
			uchar centreWordPOSambiguityInfoCurrentByte = DBconvertBase64ToByte(centreWordPOSambiguityInfoCurrentByteBaseXchar);
			#else
			uchar centreWordPOSambiguityInfoCurrentByte = DBconvertHexToByte(centreWordPOSambiguityInfoCurrentByteBaseXstring);			
			#endif
			if(centreWordPOSambiguityInfoCurrentByte == centreWordPOSpermutationIndexByte)
			{				
				foundMatchingCentreWordPOSambiguityInfo = true;
				string centreWordPOSambiguityInfoNumberInstancesCurrentIntString = centreWordPOSambiguityReferenceString.substr(LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_POS_PERMUTATION_ENTRY_CENTRE_WORD_POS_AMBIGUITY_NUMBER_OF_INSTANCES_INT_START_POS, LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_POS_PERMUTATION_ENTRY_CENTRE_WORD_POS_AMBIGUITY_NUMBER_OF_INSTANCES_INT_LENGTH);
				int centreWordPOSambiguityInfoNumberInstancesCurrentInt = SHAREDvars.convertStringToInt(centreWordPOSambiguityInfoNumberInstancesCurrentIntString);
				
				#ifdef LRP_PREPROCESSOR_POS_TAGGER_POS_DEBUG
				cout << "foundMatchingCentreWordPOSambiguityInfo (incrementing)" << endl;
				cout << "int(centreWordPOSambiguityInfoCurrentByte) = " << int(centreWordPOSambiguityInfoCurrentByte) << endl;
				cout << "centreWordPOSambiguityInfoNumberInstancesCurrentInt = " << centreWordPOSambiguityInfoNumberInstancesCurrentInt << endl;
				#endif
				
				#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_EFFICIENT_OVERWRITE_EXISTENT_REFERENCES_WITH_NEW_NUMBER_OF_INSTANCES
					//FUTURE: efficiently overwrite the number of instances of the references entry
				cerr << "LRPpreprocessorPOStaggerDatabaseClass::DBwritePOSpermutationEstimate error{}: LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_EFFICIENT_OVERWRITE_EXISTENT_REFERENCES_WITH_NEW_NUMBER_OF_INSTANCES has not been coded" << endl;
				exit(EXIT_ERROR);
				#else
				centreWordPOSambiguityInfoNumberInstancesCurrentInt = centreWordPOSambiguityInfoNumberInstancesCurrentInt + 1;
				string formatString = generateIntFormatString(LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_POS_PERMUTATION_ENTRY_CENTRE_WORD_POS_AMBIGUITY_NUMBER_OF_INSTANCES_INT_LENGTH);
				centreWordPOSambiguityInfoNumberInstancesCurrentIntString = SHAREDvars.convertIntToString(centreWordPOSambiguityInfoNumberInstancesCurrentInt, formatString);	//rewrite number of instances
				centreWordPOSambiguityReferenceString = centreWordPOSambiguityReferenceString.replace(LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_POS_PERMUTATION_ENTRY_CENTRE_WORD_POS_AMBIGUITY_NUMBER_OF_INSTANCES_INT_START_POS, LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_POS_PERMUTATION_ENTRY_CENTRE_WORD_POS_AMBIGUITY_NUMBER_OF_INSTANCES_INT_LENGTH, centreWordPOSambiguityInfoNumberInstancesCurrentIntString);
				centreWordPOSambiguityInfoList[i] = centreWordPOSambiguityReferenceString;
				#endif
			}
		}
	}
	
	if(!foundMatchingCentreWordPOSambiguityInfo)
	{		
		//create a new entry in reference file
		int numberOfInstances = 1;
		//cout << "create a new entry in reference file" << endl;
		#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_AND_MAP_USE_6BIT_INDICES
		char centreWordPOSambiguityInfoCurrentBaseXchar = DBconvertByteToBase64(centreWordPOSpermutationIndexByte);
		string centreWordPOSambiguityInfoCurrentBaseXstring = "";
		centreWordPOSambiguityInfoCurrentBaseXstring = centreWordPOSambiguityInfoCurrentBaseXstring + centreWordPOSambiguityInfoCurrentBaseXchar;
		#else
		string centreWordPOSambiguityInfoCurrentBaseXstring = DBconvertByteToHex(centreWordPOSpermutationIndexByte);
		#endif
		string formatString = generateIntFormatString(LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_POS_PERMUTATION_ENTRY_CENTRE_WORD_POS_AMBIGUITY_NUMBER_OF_INSTANCES_INT_LENGTH);
		string centreWordPOSambiguityInfoNumberInstancesCurrentIntString = SHAREDvars.convertIntToString(numberOfInstances, formatString);

		string centreWordPOSambiguityReferenceString = centreWordPOSambiguityInfoCurrentBaseXstring + CHAR_SPACE + centreWordPOSambiguityInfoNumberInstancesCurrentIntString;
		centreWordPOSambiguityInfoList.push_back(centreWordPOSambiguityReferenceString);
		
		#ifdef LRP_PREPROCESSOR_POS_TAGGER_POS_DEBUG
		cout << "!foundMatchingCentreWordPOSambiguityInfo (create new entry)" << endl;
		cout << "centreWordPOSambiguityReferenceString = " << centreWordPOSambiguityReferenceString << endl;
		#endif
		
		#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_EFFICIENT_OVERWRITE_EXISTENT_REFERENCES_WITH_NEW_NUMBER_OF_INSTANCES
		string referencesFileName = DBgenerateFileName(POSambiguityInfoPermutation);
		SHAREDvars.writeStringListToFile(referencesFileName, &centreWordPOSambiguityInfoList);
		#endif
	}
	
	#ifndef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_EFFICIENT_OVERWRITE_EXISTENT_REFERENCES_WITH_NEW_NUMBER_OF_INSTANCES
	string referencesFileName = DBgenerateFileName(POSambiguityInfoPermutation);
	SHAREDvars.writeStringListToFile(referencesFileName, &centreWordPOSambiguityInfoList);		//rewrite entire file
	#endif
	
	return result;
}
#endif


#endif

string LRPpreprocessorPOStaggerDatabaseClass::generateIntFormatString(const int numberOfCharacters)
{
	string formatString = string("%0") + SHAREDvars.convertIntToString(numberOfCharacters) + "d";
	return formatString;
}


string LRPpreprocessorPOStaggerDatabaseClass::DBconvertByteToBinaryString(const uint64_t integer)
{
	string binaryString = ""; 
	for(int i=0; i<64; i++)
	{
		bool val = SHAREDvars.getBitValue(integer, i);
		binaryString = binaryString + SHAREDvars.convertIntToString(int(val));
	}
	return binaryString;
}

#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_PREDICTION_VERIFICATION
bool LRPpreprocessorPOStaggerDatabaseClass::verifyPOStaggerDatabasePredictionAgainstPOSambiguityInfo(const uchar centreWordPOSindexPrediction, const uint32_t centreWordPOSambiguityInfo, uchar* centreWordPOSvalueFirstAmbiguousPrediction)
{
	bool predictionMatchesPOSambiguityInfo = false;
	if(centreWordPOSambiguityInfo == 0)
	{
		predictionMatchesPOSambiguityInfo = true;
	}
	else
	{
		for(int i=0; i<LRP_PREPROCESSOR_POS_TYPE_ARRAY_NUMBER_OF_TYPES; i++)
		{
			if(SHAREDvars.getBitValue(centreWordPOSambiguityInfo, i))
			{
				*centreWordPOSvalueFirstAmbiguousPrediction = i;

				if(centreWordPOSindexPrediction == i)
				{
					predictionMatchesPOSambiguityInfo = true;
				}
			}
		}
	}
	return predictionMatchesPOSambiguityInfo;
}
#endif




#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_AND_MAP_USE_6BIT_INDICES
char LRPpreprocessorPOStaggerDatabaseClass::DBconvertByteToBase64(const uchar byte)
{
	char base64char;
	if((int(byte) >= LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_POS_PERMUTATION_ENTRY_CENTRE_WORD_POS_AMBIGUITY_BYTE_CODED_BASE_INDEX_0) && (int(byte) <= LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_POS_PERMUTATION_ENTRY_CENTRE_WORD_POS_AMBIGUITY_BYTE_CODED_BASE_INDEX_9))
	{
		base64char = int(byte) + int('0') - LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_POS_PERMUTATION_ENTRY_CENTRE_WORD_POS_AMBIGUITY_BYTE_CODED_BASE_INDEX_0;
	}
	else if((int(byte) >= LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_POS_PERMUTATION_ENTRY_CENTRE_WORD_POS_AMBIGUITY_BYTE_CODED_BASE_INDEX_A) && (int(byte) <= LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_POS_PERMUTATION_ENTRY_CENTRE_WORD_POS_AMBIGUITY_BYTE_CODED_BASE_INDEX_Z))
	{
		base64char = int(byte) + int('A') - LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_POS_PERMUTATION_ENTRY_CENTRE_WORD_POS_AMBIGUITY_BYTE_CODED_BASE_INDEX_A;
	}
	else if((int(byte) >= LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_POS_PERMUTATION_ENTRY_CENTRE_WORD_POS_AMBIGUITY_BYTE_CODED_BASE_INDEX_a) && (int(byte) <= LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_POS_PERMUTATION_ENTRY_CENTRE_WORD_POS_AMBIGUITY_BYTE_CODED_BASE_INDEX_z))
	{
		base64char = int(byte) + int('a') - LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_POS_PERMUTATION_ENTRY_CENTRE_WORD_POS_AMBIGUITY_BYTE_CODED_BASE_INDEX_a;
	}	
	else if(int(byte) == LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_POS_PERMUTATION_ENTRY_CENTRE_WORD_POS_AMBIGUITY_BYTE_CODED_BASE_INDEX_DASH)
	{
		base64char = int('-');
	}
	else if(int(byte) == LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_POS_PERMUTATION_ENTRY_CENTRE_WORD_POS_AMBIGUITY_BYTE_CODED_BASE_INDEX_UNDERSCORE)
	{
		base64char = int('_');
	}	
	else
	{
		cerr << "LRPpreprocessorPOStaggerDatabaseClass::DBconvertByteToBase64{} error - illegal base64char detected" << base64char << endl;
		exit(EXIT_ERROR);
	}
	return base64char;
}
uchar LRPpreprocessorPOStaggerDatabaseClass::DBconvertBase64ToByte(const char base64char)
{
	uchar byte;
	if((int(base64char) >= int('0')) && (int(base64char) <= int('9')))
	{
		byte = base64char - int('0') + LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_POS_PERMUTATION_ENTRY_CENTRE_WORD_POS_AMBIGUITY_BYTE_CODED_BASE_INDEX_0;
	}
	else if((int(base64char) >= int('A')) && (int(base64char) <= int('Z')))
	{
		byte = base64char - int('A') + LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_POS_PERMUTATION_ENTRY_CENTRE_WORD_POS_AMBIGUITY_BYTE_CODED_BASE_INDEX_A;
	}
	else if((int(base64char) >= int('a')) && (int(base64char) <= int('z')))
	{
		byte = base64char - int('a') + LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_POS_PERMUTATION_ENTRY_CENTRE_WORD_POS_AMBIGUITY_BYTE_CODED_BASE_INDEX_a;
	}
	else if(int(base64char) == '-')
	{
		byte = 62;
	}
	else if(int(base64char) == '_')
	{
		byte = 63;
	}	
	else
	{
		cerr << "LRPpreprocessorPOStaggerDatabaseClass::DBconvertBase64ToByte{} error - illegal base64char detected " << base64char << ", int(base64char) = " << int(base64char) << endl;
		exit(EXIT_ERROR);
	}
	return byte;
}
#else
string LRPpreprocessorPOStaggerDatabaseClass::DBconvertByteToHex(const uchar byte)
{
	/*
	stringstream ss;
    	ss << hex << (int)byte;
	string hexString = ss.str();
	*/
	//cerr << "int(byte) = " << int(byte) << endl;
	string formatString = "%02x";
	string hexString = SHAREDvars.convertIntToString(int(byte), formatString);
	/*
	cout << "LRPpreprocessorPOStaggerDatabaseClass::DBconvertByteToHex:" << endl;
	cout << "byte = " << int(byte) << endl;
	cout << "hexString = " << hexString << endl;
	*/
	return hexString;
}
uchar LRPpreprocessorPOStaggerDatabaseClass::DBconvertHexToByte(const string hexString)
{
	uchar byte = (uchar)strtol(hexString.c_str(), NULL, 16);
	/*
	cout << "LRPpreprocessorPOStaggerDatabaseClass::DBconvertHexToByte:" << endl;
	cout << "byte = " << int(byte) << endl;
	cout << "hexString = " << hexString << endl;
	*/
	return byte;
}
#endif



uchar LRPpreprocessorPOStaggerDatabaseClass::convertPOSambiguityInfoToIndex(const uint64_t POSambiguityInfo)
{
	uchar POSambiguityInfoIndex = LRP_PREPROCESSOR_POS_TYPE_UNDEFINED;
	if(!determinePOSambiguityInfoIsAmbiguous(POSambiguityInfo, &POSambiguityInfoIndex, true))
	{
		//cout << "unambiguousPOSvalue = " << int(unambiguousPOSvalue) << endl;
		//cout << "unambiguousPOSvalue = " << int((uchar)(char(int(unambiguousPOSvalue)))) << endl;
	}
	else
	{
		cerr << "LRPpreprocessorPOStaggerDatabaseClass::convertPOSambiguityInfoToIndex error: determinePOSambiguityInfoIsAmbiguous; verify that LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FEED_ALL_PERMUTATIONS_INDIVIDUALLY_ONLY_TRAIN_UNAMBIGUOUS_PERMUTATIONS is true" << endl;
		exit(EXIT_ERROR);
	}
	
	return POSambiguityInfoIndex;
}

bool LRPpreprocessorPOStaggerDatabaseClass::determinePOSambiguityInfoIsAmbiguous(const uint64_t POSambiguityInfo, uchar* unambiguousPOSinfoIndex, const bool treatWordAsAmbiguousIfNullPOSvalue)
{
	bool ambiguous = false;
	
	//cout << "POSambiguityInfo = " << POSambiguityInfo << endl;
	//cout << "LRP_PREPROCESSOR_POS_TAGGER_DATABASE_POS_NUMBER_OF_TYPES = " << LRP_PREPROCESSOR_POS_TAGGER_DATABASE_POS_NUMBER_OF_TYPES << endl;
	
	int numberOfPOStypesRecorded = 0;
	for(int POStype = 0; POStype<LRP_PREPROCESSOR_POS_TAGGER_DATABASE_POS_NUMBER_OF_TYPES; POStype++)	//LRP_PREPROCESSOR_POS_TYPE_ARRAY_NUMBER_OF_TYPES
	{
		bool bitValue = SHAREDvars.getBitValue(POSambiguityInfo, POStype);
		if(bitValue)
		{
			*unambiguousPOSinfoIndex = POStype;
			numberOfPOStypesRecorded++;
		}
	}
	if(numberOfPOStypesRecorded > 1)
	{
		ambiguous = true;
	}
	if(POSambiguityInfo == 0)
	{
		if(treatWordAsAmbiguousIfNullPOSvalue)
		{
			ambiguous = true;
		}
		else
		{
			*unambiguousPOSinfoIndex = LRP_PREPROCESSOR_POS_TYPE_UNDEFINED;
		}
	}

	/*
	if(!ambiguous)
	{
		//this is required when determinePOSambiguityInfoIsAmbiguous is being used to calculate unambiguousPOSvalue for special characters also
		for(int POStype = 0; POStype<LRP_PREPROCESSOR_POS_TAGGER_DATABASE_POS_NUMBER_OF_TYPES; POStype++)
		{
			bool bitValue = SHAREDvars.getBitValue(POSambiguityInfo, POStype);
			if(bitValue)
			{
				*unambiguousPOSinfoIndex = POStype;
			}
		}
	}
	*/
	
	//cout << "POSambiguityInfoIndex = " << int(*unambiguousPOSinfoIndex) << endl;
				
	return ambiguous;
}

#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL
string LRPpreprocessorPOStaggerDatabaseClass::externalANNgenerateExperienceInputStandalone(const ANNexperience* currentExperienceInList)
{	
	string experienceInputString = "";
	const ANNexperienceInput* currentExperienceInput = currentExperienceInList->firstExperienceInput;
	while(currentExperienceInput->next != NULL)
	{
		string format = "%g";	//"%0.0f";
		experienceInputString = experienceInputString + SHAREDvars.convertDoubleToString(currentExperienceInput->inputValue, format) + LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_BATCH_FILE_DELIMITER;
		currentExperienceInput = currentExperienceInput->next;
	}
	return experienceInputString;
}
#endif
	

