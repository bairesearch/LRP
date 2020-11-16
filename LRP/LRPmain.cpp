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
 * File Name: LRPmain.cpp
 * Author: Richard Bruce Baxter - Copyright (c) 2005-2020 Baxter AI (baxterai.com)
 * Project: Language Reduction Preprocessor
 * Project Version: 3o3b 16-November-2020
 * Requirements: requires plain text file
 * Description: Main
 * /
 *******************************************************************************/


#include "LRPmain.hpp"

#ifdef LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE
static char errmessage[] = "Usage:  LRPgeneratePOStaggerDatabase.exe [options]\n\n\twhere options are any of the following\n"
"\n\t-dbpostaggerfolder [string]   : pos tagger database base folder path (def: /home/systemusername/source/LRPPOStaggerDatabase)"
"\n\t-lrp                          : language reduction preprocessor"
"\n\t-lrpfolder [string]           : folder of LRP data files (list of multiword verbs, multiword prepositions etc) (def: same as exe)"
"\n\t-wikiDumpFolder               : wiki dump folder (def: /home/systemusername/soft/wiki/output)"
"\n\t-wikiDumpFileBatchIndex       : batch index"
#ifdef GIA_NEURAL_NETWORK
"\n\t-oannxml [string]    : neural network definition .xml output filename (def: neuralNet.xml)"
"\n\t-oannldr [string]    : neural network display .ldr output filename (def: neuralNet.ldr)"
"\n\t-oannsvg [string]    : neural network display .svg output filename (def: neuralNet.svg)"
"\n\t-oannppm [string]    : neural network display .ppm opengl output filename (def: neuralNet.ppm)"
"\n\t-oannppm2 [string]   : neural network display .ppm raytraced output filename (def: neuralNetRaytraced.ppm)"
"\n\t-oannall [string]    : neural network display .xml/.svg/.ldr/.ppm default generic output filename (def: neuralNet)"
#endif
"\n\n\tThis program generates the POS tagger database for a wiki dump batch.\n\n";

int main(const int argc, const char** argv)
{
	bool result = true;
	
	string currentFolder = SHAREDvarsClass().getCurrentDirectory();
	string inputFolderLocal = currentFolder;
	string outputFolderLocal = currentFolder;

	bool useLRP = false;
	string lrpDataFolderName = "";
	string POStaggerDatabaseFolderName = LRP_DATABASE_FILESYSTEM_DEFAULT_SERVER_OR_MOUNT_NAME_BASE + LRP_PREPROCESSOR_POS_TAGGER_DATABASE_FILESYSTEM_DEFAULT_DATABASE_NAME;
	string wikiDumpFolderName = LRP_PREPROCESSOR_POS_TAGGER_GENERATE_DATABASE_DOC_XML_OUTPUT_FOLDER;
	int wikiDumpFileBatchIndex = 0;
	
	#ifdef GIA_NEURAL_NETWORK
	bool ANNuseInputXMLFile = false;
	string ANNinputXMLFileName = string(NEURAL_NETWORK_VISUALISATION_BASE_FILE_NAME) + NEURAL_NETWORK_VISUALISATION_XML_FILE_EXTENSION;
	bool ANNuseOutputXMLFile = false;
	string ANNoutputXMLFileName = string(NEURAL_NETWORK_VISUALISATION_BASE_FILE_NAME) + NEURAL_NETWORK_VISUALISATION_XML_FILE_EXTENSION;
	bool ANNuseOutputLDRFile = false;
	string ANNoutputLDRFileName = string(NEURAL_NETWORK_VISUALISATION_BASE_FILE_NAME) + NEURAL_NETWORK_VISUALISATION_LDR_FILE_EXTENSION;
	bool ANNuseOutputSVGFile = false;
	string ANNoutputSVGFileName = string(NEURAL_NETWORK_VISUALISATION_BASE_FILE_NAME) + NEURAL_NETWORK_VISUALISATION_SVG_FILE_EXTENSION;
	bool ANNuseOutputPPMFile = false;
	string ANNoutputPPMFileName = string(NEURAL_NETWORK_VISUALISATION_BASE_FILE_NAME) + NEURAL_NETWORK_VISUALISATION_PPM_FILE_EXTENSION;
	bool ANNuseOutputPPMFileRaytraced = false;
	string ANNoutputPPMFileNameRaytraced = string(NEURAL_NETWORK_VISUALISATION_BASE_FILE_NAME) + NEURAL_NETWORK_VISUALISATION_PPM_RAYTRACED_FILE_EXTENSION;
	string ANNoutputTALFileName = string(NEURAL_NETWORK_VISUALISATION_BASE_FILE_NAME) + NEURAL_NETWORK_VISUALISATION_TAL_FILE_EXTENSION;
	bool ANNuseOutputAllFile = false;
	string ANNoutputAllFileName = NEURAL_NETWORK_VISUALISATION_BASE_FILE_NAME;
	bool ANNuseSprites = true;
	#endif
	
	int rasterImageWidth = 640;
	int rasterImageHeight = 480;
			
	if(SHAREDvarsClass().argumentExists(argc, argv, "-lrpfolder"))
	{
		lrpDataFolderName = SHAREDvarsClass().getStringArgument(argc, argv, "-lrpfolder");
	}
	else
	{
		lrpDataFolderName = currentFolder;
	}	
	if(SHAREDvarsClass().argumentExists(argc, argv, "-lrp"))
	{
		useLRP = true;
	}
	if(SHAREDvarsClass().argumentExists(argc, argv, "-dbpostaggerfolder"))
	{
		POStaggerDatabaseFolderName = SHAREDvarsClass().getStringArgument(argc, argv, "-dbpostaggerfolder");
	}
	if(SHAREDvarsClass().argumentExists(argc, argv, "-wikiDumpFolder"))
	{
		wikiDumpFolderName = SHAREDvarsClass().getStringArgument(argc, argv, "-wikiDumpFolder");
	}
	if(SHAREDvarsClass().argumentExists(argc, argv, "-wikiDumpFileBatchIndex"))
	{
		wikiDumpFileBatchIndex = int(SHAREDvarsClass().getFloatArgument(argc, argv, "-wikiDumpFileBatchIndex"));
	}
	
	#ifdef GIA_NEURAL_NETWORK
	if(SHAREDvarsClass().argumentExists(argc, argv, "-oannxml"))
	{
		ANNoutputXMLFileName = SHAREDvarsClass().getStringArgument(argc, argv, "-oannxml");
		ANNuseOutputXMLFile = true;
	}
	if(SHAREDvarsClass().argumentExists(argc, argv, "-oannldr"))
	{
		ANNoutputLDRFileName = SHAREDvarsClass().getStringArgument(argc, argv, "-oannldr");
		ANNuseOutputLDRFile = true;
	}
	if(SHAREDvarsClass().argumentExists(argc, argv, "-oannsvg"))
	{
		ANNoutputSVGFileName = SHAREDvarsClass().getStringArgument(argc, argv, "-oannsvg");
		ANNuseOutputSVGFile = true;
	}
	if(SHAREDvarsClass().argumentExists(argc, argv, "-oannppm"))
	{
		ANNoutputPPMFileName = SHAREDvarsClass().getStringArgument(argc, argv, "-oannppm");
		ANNuseOutputPPMFile = true;
		ANNuseOutputLDRFile = true;	//required for OpenGL image generation
	}
	if(SHAREDvarsClass().argumentExists(argc, argv, "-oannppm2"))
	{
		ANNoutputPPMFileNameRaytraced = SHAREDvarsClass().getStringArgument(argc, argv, "-oannppm2");
		ANNuseOutputPPMFileRaytraced = true;
		ANNuseOutputLDRFile = true;	//required for raytrace image generation
	}
	if(SHAREDvarsClass().argumentExists(argc, argv, "-oannall"))
	{
		ANNoutputAllFileName = SHAREDvarsClass().getStringArgument(argc, argv, "-oannall");
		ANNuseOutputAllFile = true;
	}
	#ifndef ANN_DISPLAY_DISABLE_SPRITES
	if(SHAREDvarsClass().argumentExists(argc, argv, "-annsprites"))
	{
		ANNuseSprites = true;
	}
	#endif
	#endif
	

	#ifdef GIA_NEURAL_NETWORK
	bool ANNdisplayInOpenGL = false;
	bool ANNdrawOutput = false;
	if(ANNuseOutputPPMFile)
	{
		ANNdisplayInOpenGL = true;
	}
	if(ANNuseOutputLDRFile)
	{
		ANNdrawOutput = true;
	}
	if(ANNuseOutputSVGFile)
	{
		ANNdrawOutput = true;
	}
	if(ANNuseOutputPPMFile)
	{
		ANNdrawOutput = true;
	}
	if(ANNuseOutputPPMFileRaytraced)
	{
		ANNdrawOutput = true;
	}
	if(ANNuseOutputAllFile)
	{
		ANNdrawOutput = true;
	}	
	if(ANNdrawOutput)
	{
		if(ANNuseOutputAllFile)
		{
			if(!ANNuseOutputLDRFile)
			{
				ANNuseOutputLDRFile = true;
				ANNoutputLDRFileName = ANNoutputAllFileName + NEURAL_NETWORK_VISUALISATION_LDR_FILE_EXTENSION;
			}

			if(!ANNuseOutputXMLFile)
			{
				ANNuseOutputXMLFile = true;
				ANNoutputXMLFileName = ANNoutputAllFileName + NEURAL_NETWORK_VISUALISATION_XML_FILE_EXTENSION;
			}

			if(!ANNuseOutputSVGFile)
			{
				ANNuseOutputSVGFile = true;	//SVG output is not always required when printing/drawing neural network
				ANNoutputSVGFileName = ANNoutputAllFileName + NEURAL_NETWORK_VISUALISATION_SVG_FILE_EXTENSION;
			}
			if(!ANNuseOutputPPMFile)
			{
				ANNuseOutputPPMFile = true;
				ANNoutputPPMFileName = ANNoutputAllFileName + NEURAL_NETWORK_VISUALISATION_PPM_FILE_EXTENSION;
			}
			/* disable raytrace output by default
			if(!ANNuseOutputPPMFileRaytraced)
			{
				ANNuseOutputPPMFileRaytraced = true;
				ANNoutputPPMFileNameRaytraced = ANNoutputAllFileName + NEURAL_NETWORK_VISUALISATION_PPM_RAYTRACED_FILE_EXTENSION;
			}
			*/
		}
	}
	#endif
		
	POStaggerDatabaseFolderName = POStaggerDatabaseFolderName + CHAR_FOLDER_DELIMITER;
	lrpDataFolderName = lrpDataFolderName + CHAR_FOLDER_DELIMITER;	
	wikiDumpFolderName = wikiDumpFolderName + CHAR_FOLDER_DELIMITER;	
	
	inputFolder = inputFolderLocal;
	outputFolder = outputFolderLocal;
	SHAREDvarsClass().setCurrentDirectory(inputFolder);

	cout << "executing GIAposTaggerGenerateDatabase.exe with the following parameters;" << endl;
	cout << "lrpDataFolderName = " << lrpDataFolderName << endl;
	cout << "useLRP = " << useLRP << endl;
	cout << "POStaggerDatabaseFolderName = " << POStaggerDatabaseFolderName << endl;
	cout << "wikiDumpFolderName = " << wikiDumpFolderName << endl;
	cout << "wikiDumpFileBatchIndex = " << wikiDumpFileBatchIndex << endl;

	#ifdef SANI_NEURAL_NETWORK
	ANNtranslatorVariablesClass* ANNtranslatorVariables = new ANNtranslatorVariablesClass();
	#endif
	
	LRPpreprocessorPOStaggerDatabaseClass().initialisePOStaggerDatabase(POStaggerDatabaseFolderName);
	
	#ifdef LRP_PREPROCESSOR
	if(!LRPpreprocessorWordIdentificationClass().initialiseLRP(lrpDataFolderName, useLRP))
	{
		result = false;
	}
	#endif

	#ifdef SANI_SEQUENCE_GRAMMAR		
	vector<SANIGroupType*>* SANIGroupTypes = new vector<SANIGroupType*>;
	vector<XMLparserTag*>* SANIrulesTokenLayers = new vector<XMLparserTag*>;
	if(!SANIrules.extractSANIrules(SANIGroupTypes, SANIrulesTokenLayers))
	{
		result = false;
	}
	if(!SANIformationClass().createSANI(SANIrulesTokenLayers, SANIGroupTypes))
	{
		result = false;
	}
	#endif	
	
	#ifdef LRP_PREPROCESSOR_POS_TAGGER_DATABASE_NEURAL_NETWORK_EXTERNAL_TRAIN_SINGLE_BATCH_ONLY
	if(!LRPpreprocessorPOStaggerClass().generatePOStaggerDatabaseFromWikiDumpText(wikiDumpFolderName, wikiDumpFileBatchIndex, useLRP))
	#else
	if(!LRPpreprocessorPOStaggerClass().generatePOStaggerDatabaseFromWikiDumpText(wikiDumpFolderName, useLRP))	
	#endif
	{
		result = false;
	}	
	
	#ifdef SANI_SEQUENCE_GRAMMAR
	#ifdef SANI_ANN_DELAY_ANN_CONNECTIVITY_TILL_END
	SANIformationClass().createANNconnectivity(SANIGroupTypes);
	#endif
	#endif

	#ifdef SANI_NEURAL_NETWORK
	
	#ifdef SANI_ANN
	SANIneuralNetworkOperationsClass().generateNeuralNetFromSANInet(ANNtranslatorVariables);	//generate GIA NLP neural network
	#endif
	
	if(ANNdrawOutput)
	{
		string ANNoutputTALFileName = string(NEURAL_NETWORK_VISUALISATION_BASE_FILE_NAME) + NEURAL_NETWORK_VISUALISATION_TAL_FILE_EXTENSION;
		ANNdisplayClass().outputNeuralNetworkToVectorGraphicsAndRaytrace(translatorVariables->firstInputNeuronInNetwork, ANNuseSprites, ANNuseOutputPPMFileRaytraced, ANNdisplayInOpenGL, ANNuseOutputLDRFile, ANNuseOutputSVGFile, ANNuseOutputPPMFile, ANNoutputLDRFileName, ANNoutputSVGFileName, ANNoutputPPMFileName, ANNoutputPPMFileNameRaytraced, ANNoutputTALFileName, rasterImageWidth, rasterImageHeight);
	}
	if(ANNuseOutputXMLFile)
	{
		if(!GIAneuralNetworkOperationsClass().writeNeuralNetXMLfile(ANNoutputXMLFileName, translatorVariables->firstInputNeuronInNetwork))
		{
			result = false;
		}
	}		
	#endif
}
#endif

