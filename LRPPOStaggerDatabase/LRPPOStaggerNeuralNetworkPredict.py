import keras
from keras.models import Sequential
from keras.layers import Dense
from keras.models import model_from_json
#from keras.models import model_from_yaml
import numpy
import os

from LRPPOStaggerNeuralNetworkInitialise import *
from LRPPOStaggerNeuralNetworkLoad import loadModel

model = loadModel()

#generate predictions
XpredictBatchFileName = XpredictBatchFileNamePrepend + XtrainBatchFileExtension
YpredictBatchFileName = YpredictBatchFileNamePrepend + YtrainBatchFileExtension
XpredictBatch = numpy.genfromtxt(XpredictBatchFileName, delimiter=experienceDataFileDelimiter)
if (XpredictBatch.ndim == 1):
	XpredictBatch = numpy.array([XpredictBatch])	#required in case XpredictBatchFileName only contains a single line, then XpredictBatch will be a 1 dimensional array
	#print(XpredictBatch)
classes = model.predict_classes(XpredictBatch) 
#classes = model.predict(XpredictBatch, batch_size = 1, verbose = 1)
#print(classes)
numpy.savetxt(YpredictBatchFileName, classes, delimiter=experienceDataFileDelimiter)
