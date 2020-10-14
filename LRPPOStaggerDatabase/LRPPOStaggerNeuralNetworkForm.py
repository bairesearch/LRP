import keras
from keras.models import Sequential
from keras.layers import Dense
from keras.models import model_from_json
#from keras.models import model_from_yaml
import numpy
import os
from keras.utils import plot_model

from LRPPOStaggerNeuralNetworkInitialise import *
from LRPPOStaggerNeuralNetworkSave import saveModel
from LRPPOStaggerNeuralNetworkCompile import compileModel

#formation
model = Sequential()
model.add(Dense(units=440, activation='relu', input_dim=120))
model.add(Dense(units=220, activation='relu'))
model.add(Dense(units=110, activation='relu'))
model.add(Dense(units=8, activation='softmax'))

compileModel(model)

plot_model(model, to_file='model.png')

saveModel(model)

