import keras
from keras.models import Sequential
from keras.layers import Dense
from keras.models import model_from_json
#from keras.models import model_from_yaml
import numpy
import os

from LRPPOStaggerNeuralNetworkInitialise import *
from LRPPOStaggerNeuralNetworkLoad import loadModel
from LRPPOStaggerNeuralNetworkSave import saveModel

model = loadModel()

#training
for epoch in range(numEpochs):
	print ("epoch = " + epoch)
	for batch in range(numBatches):
		print ("batch = " + batch)
		#batchString = batchStringFormat % batch
		batchString = "%0{0}d".format(batchStringMaxNumCharactersString) % batch
		XtrainBatchFileName = experienceDataFolderName + XtrainBatchFileNamePrepend + batchString + XtrainBatchFileExtension
		YtrainBatchFileName = experienceDataFolderName + YtrainBatchFileNamePrepend + batchString + YtrainBatchFileExtension
		print ("XtrainBatchFileName = " + XtrainBatchFileName)
		print ("YtrainBatchFileName = " + YtrainBatchFileName)
		XtrainBatch = numpy.genfromtxt(XtrainBatchFileName, delimiter=experienceDataFileDelimiter)
		YtrainBatch = numpy.genfromtxt(YtrainBatchFileName, delimiter=experienceDataFileDelimiter)
		if useMinibatches:
			model.fit(XtrainBatch, YtrainBatch, epochs=1, batch_size=numMinibatches)
		else:
			model.train_on_batch(XtrainBatch, YtrainBatch)

saveModel(model)
