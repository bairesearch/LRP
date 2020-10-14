import keras
from keras.models import Sequential
from keras.layers import Dense
from keras.models import model_from_json
#from keras.models import model_from_yaml
import numpy
import os

#global variables
numEpochs = 5
numBatches = 1203
useMinibatches = True
numMinibatches = 32
experienceDataFolderName = "/home/systemusername/source/LRPPOStaggerDatabase/"
XtrainBatchFileNamePrepend = "XtrainBatch" 
YtrainBatchFileNamePrepend = "YtrainBatch"
XpredictBatchFileNamePrepend = "XpredictBatch"
YpredictBatchFileNamePrepend = "YpredictBatch"
XtrainBatchFileExtension = ".dat" 
YtrainBatchFileExtension = ".dat"
experienceDataFileDelimiter = " "
batchStringMaxNumCharacters = 4	#eg XtrainBatch0000.txt
batchStringMaxNumCharactersString = str(batchStringMaxNumCharacters)
#batchStringFormat = "%0" + batchStringMaxNumCharactersString + "d"
os.chdir(experienceDataFolderName)
