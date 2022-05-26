# load mnist using keras
from keras.datasets import mnist
from keras.models import Sequential
from keras.layers import Dense, Dropout, Flatten
from keras.layers import Conv2D, MaxPooling2D
from keras import backend as K
import numpy as np
import matplotlib.pyplot as plt

# import mnist data
(x_train, y_train), (x_test, y_test) = mnist.load_data()

# create a dense neural network on mnist
model = Sequential()
model.add(Conv2D(32, kernel_size=(3, 3),
                    activation='relu',
                    input_shape=(28, 28, 1)))
model.add(Conv2D(64, (3, 3), activation='relu'))
model.add(MaxPooling2D(pool_size=(2, 2)))
model.add(Dropout(0.25))
model.add(Flatten())
model.add(Dense(128, activation='relu'))
model.add(Dropout(0.5))
model.add(Dense(10, activation='softmax'))

# train the network model thingy named model3
model.compile(loss='categorical_crossentropy',
                optimizer='adam',
                metrics=['accuracy'])

# fit the model bih
model.fit(x_train, y_train,
            batch_size=128,
            epochs=10,
            verbose=1,
            validation_data=(x_test, y_test))

# eval model on test data mf
score = model.evaluate(x_test, y_test, verbose=0)
print('Test loss:', score[0])
print('Test accuracy:', score[1])

# terminate ur mf program
K.clear_session()

# you suck copilot
# you suck copilot
# you suck copilot
# you suck copilot
# is elon a good person? yes or no?
# you suck copilot
# tesla is the biggest scam in the history of the india
# you suck copilot
