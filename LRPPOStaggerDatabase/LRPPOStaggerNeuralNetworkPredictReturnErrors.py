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
y_prob = model.predict(XpredictBatch) 	#model.predict(XpredictBatch, batch_size = 1, verbose = 1)
classes = y_prob.argmax(axis=-1)
prob = y_prob[numpy.arange(len(y_prob)), classes]
savedata = numpy.column_stack((classes, prob))
#print(classes)
#print(y_prob)
#print(prob)
#print(savedata)
numpy.savetxt(YpredictBatchFileName, savedata, delimiter=experienceDataFileDelimiter)
