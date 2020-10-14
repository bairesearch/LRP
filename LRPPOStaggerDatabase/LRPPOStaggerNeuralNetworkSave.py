import keras
from keras.models import Sequential
from keras.layers import Dense
from keras.models import model_from_json
#from keras.models import model_from_yaml
import numpy
import os

def saveModel(model):

	#save
	# serialize model to JSON
	model_json = model.to_json()
	with open("LRPPOStaggerNeuralNetwork.json", "w") as json_file:
		json_file.write(model_json)
	# serialize weights to HDF5
	model.save_weights("LRPPOStaggerNeuralNetwork.h5")
	print("Saved model to disk")

