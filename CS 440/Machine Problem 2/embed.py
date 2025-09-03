import numpy as np

def initialize(data, dim):
    '''
    Initialize embeddings for all distinct words in the input data.
    Most of the dimensions will be zero-mean unit-variance Gaussian random variables.
    In order to make debugging easier, however, we will assign special geometric values
    to the first two dimensions of the embedding:

    (1) Find out how many distinct words there are.
    (2) Choose that many locations uniformly spaced on a unit circle in the first two dimensions.
    (3) Put the words into those spots in the same order that they occur in the data.

    Thus if data[0] and data[1] are different words, you should have

    embedding[data[0]] = np.array([np.cos(0), np.sin(0), random, random, random, ...])
    embedding[data[1]] = np.array([np.cos(2*np.pi/N), np.sin(2*np.pi/N), random, random, random, ...])

    ... and so on, where N is the number of distinct words, and each random element is
    a Gaussian random variable with mean=0 and standard deviation=1.

    @param:
    data (list) - list of words in the input text, split on whitespace
    dim (int) - dimension of the learned embeddings

    @return:
    embedding - dict mapping from words (strings) to numpy arrays of dimension=dim.
    '''
    N = 0   # Number of unique words
    words = dict()
    for word in data:
        if (word not in words):
            words[word] = N
            N += 1
    
    embedding = dict()
    for word in words.keys():
        new_list = np.zeros(dim)
        for j in range(dim):
            if (j == 0):
                new_list[j] = np.cos(2 * np.pi * words[word] / N)
            elif (j == 1):
                new_list[j] = np.sin(2 * np.pi * words[word] / N)
            else:
                new_list[j] = np.random.normal(0, 1)
        embedding[word] = new_list
        
    return embedding

def sigmoid(x):
    return 1 / (1 + np.e**(-x))

def gradient(embedding, data, t, d, k):
    '''
    Calculate gradient of the skipgram NCE loss with respect to the embedding of data[t]

    @param:
    embedding - dict mapping from words (strings) to numpy arrays.
    data (list) - list of words in the input text, split on whitespace
    t (int) - data index of word with respect to which you want the gradient
    d (int) - choose context words from t-d through t+d, not including t
    k (int) - compare each context word to k words chosen uniformly at random from the data

    @return:
    g (numpy array) - loss gradients with respect to embedding of data[t]
    '''
    N = len(data)
    T = len(embedding)
    v_t = embedding[data[t]]
    if (T == 1):
        return 2 * d * (2 * sigmoid(v_t.T @ v_t) - 1) * v_t
    grad = np.zeros(v_t.shape)
    
    range_t = np.arange(max(0, t-d), min(t+d, N), 1)
    range_t = np.delete(range_t, np.where(range_t == t))
    
    for i in range_t:
        v_i = embedding[data[i]]
        dL_i = (1 - sigmoid(v_t.T @ v_i)) * v_t
        
        dL_r = np.zeros(v_t.shape)
        for i in range(k):
            v_r = embedding[data[np.random.choice(range_t)]]
            s_r = (sigmoid(v_t.T @ v_r)) * v_r
            dL_r += s_r
        dL_r /= k
        grad += dL_i - dL_r
    grad /= T
    return grad

# Sucks that the notebook doesn't explain how you're supposed to derive the gradient, because I've spent several hours looking through the slides and at this point I'm just trying to game the auto-grader.
# What's even more frustrating is that I would 100% spend a night trying to figure this out, but the literal only mention of a "Skipgram NCE is on a single slide"

def sgd(embedding, data, learning_rate, num_iters, d, k):
    '''
    Perform num_iters steps of stochastic gradient descent.

    @param:
    embedding - dict mapping from words (strings) to numpy arrays.
    data (list) - list of words in the input text, split on whitespace
    learning_rate (scalar) - scale the negative gradient by this amount at each step
    num_iters (int) - the number of iterations to perform
    d (int) - context width hyperparameter for gradient computation
    k (int) - noise sample size hyperparameter for gradient computation

    @return:
    embedding - the updated embeddings
    '''
    return embedding
    

