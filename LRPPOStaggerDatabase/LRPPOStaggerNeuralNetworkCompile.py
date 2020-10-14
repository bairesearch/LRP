import keras
from keras.models import Sequential
from keras.layers import Dense
from keras.models import model_from_json
#from keras.models import model_from_yaml
import numpy
import os

def compileModel(model):

	model.compile(loss='categorical_crossentropy', optimizer='sgd', metrics=['accuracy'])
	model.compile(loss=keras.losses.categorical_crossentropy, optimizer=keras.optimizers.SGD(lr=0.00001, momentum=0.9, nesterov=True))

