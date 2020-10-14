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

#testing
#FUTURE: need to use an independent set of test data (not a subset of train data) for testing
batchStringTest = "0000"
XtestBatchFileName = XtrainBatchFileNamePrepend + batchStringTest + XtrainBatchFileExtension
YtestBatchFileName = XtrainBatchFileNamePrepend + batchStringTest + YtrainBatchFileExtension
XtestBatch = numpy.genfromtxt(XtestBatchFileName, delimiter=experienceDataFileDelimiter)
YtestBatch = numpy.genfromtxt(YtestBatchFileName, delimiter=experienceDataFileDelimiter)
loss_and_metrics = model.evaluate(x_test, y_test, batch_size=128)	

