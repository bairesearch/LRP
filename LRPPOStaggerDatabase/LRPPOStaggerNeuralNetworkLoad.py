import keras
from keras.models import Sequential
from keras.layers import Dense
from keras.models import model_from_json
#from keras.models import model_from_yaml
import numpy
import os

from LRPPOStaggerNeuralNetworkCompile import compileModel

def loadModel():

	#load
	# load json and create model
	json_file = open('LRPPOStaggerNeuralNetwork.json', 'r')
	loaded_model_json = json_file.read()
	json_file.close()
	loaded_model = model_from_json(loaded_model_json)	#model_from_yaml
	# load weights into new model
	loaded_model.load_weights("LRPPOStaggerNeuralNetwork.h5")
	print("Loaded model from disk")
	
	compileModel(loaded_model)

	return loaded_model
