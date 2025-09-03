# naive_bayes.py
# ---------------
# Licensing Information:  You are free to use or extend this projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to the University of Illinois at Urbana-Champaign
#
# Created by Justin Lizama (jlizama2@illinois.edu) on 09/28/2018
# Last Modified 8/23/2023


"""
This is the main code for this MP.
You only need (and should) modify code within this file.
Original staff versions of all other files will be used by the autograder
so be careful to not modify anything else.
"""


import reader
import math
from tqdm import tqdm
from collections import Counter

import nltk
import numpy as np


'''
util for printing values
'''
def print_values(laplace, pos_prior):
    print(f"Unigram Laplace: {laplace}")
    print(f"Positive prior: {pos_prior}")

"""
load_data loads the input data by calling the provided utility.
You can adjust default values for stemming and lowercase, when we haven't passed in specific values,
to potentially improve performance.
"""
def load_data(trainingdir, testdir, stemming=False, lowercase=True, silently=False):
    print(f"Stemming: {stemming}")
    print(f"Lowercase: {lowercase}")
    train_set, train_labels, dev_set, dev_labels = reader.load_dataset(trainingdir,testdir,stemming,lowercase,silently)
    return train_set, train_labels, dev_set, dev_labels

def process_data(data):
    stats = dict()
    word_count = 0
    for word in data:
        word_count += 1
        l_word = word.lower()
        if (not (l_word in stats)):
            stats[l_word] = 0
        stats[l_word] += 1
    return stats, word_count

def process_all(train_labels, train_data):
    all_stats = {}
    total_words = [0, 0]
    for i in range(len(train_labels)):
        stats, word_count = process_data(train_data[i])
        label = train_labels[i]
        total_words[label] += word_count
        for word in stats:
            l_word = word.lower()
            if (not (l_word in all_stats)):
                all_stats[l_word] = [0, 0]
            all_stats[l_word][label] += stats[l_word]
    return all_stats, total_words

def bayes_probability(statistics, total_words, laplace, pos_prior, data):
    pos_chance = math.log(pos_prior)
    pos_words = total_words[1] + laplace
    
    neg_chance = math.log(1 - pos_prior)
    neg_words = total_words[0] + laplace
    
    stats_data, word_count = process_data(data)
    for word in stats_data:
        l_word = word.lower()
        if (l_word in statistics):
            pos_chance += math.log(((statistics[l_word][1] + laplace) / pos_words)) * stats_data[l_word]
            neg_chance += math.log(((statistics[l_word][0] + laplace) / neg_words)) * stats_data[l_word]
        else:
            pos_chance += math.log(pos_prior) * stats_data[l_word]
            neg_chance += math.log(1 - pos_prior) * stats_data[l_word]
    return pos_chance, neg_chance

"""
Main function for training and predicting with naive bayes.
    You can modify the default values for the Laplace smoothing parameter and the prior for the positive label.
    Notice that we may pass in specific values for these parameters during our testing.
"""
def naive_bayes(train_labels, train_data, dev_data, laplace=1.0, pos_prior=0.5, silently=False):
    print_values(laplace,pos_prior)
    
    statistics, total_words = process_all(train_labels, train_data)
    
    yhats = []
    for doc in tqdm(dev_data, disable=silently):
        pos_chance, neg_chance = bayes_probability(statistics, total_words, laplace, pos_prior, doc)
        yhats.append(pos_chance > neg_chance)
        
    return yhats
