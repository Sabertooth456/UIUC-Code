"""
Part 1: Simple baseline that only uses word statistics to predict tags
"""

def match(data, ratio, word):
    if (word not in data):
        best_match = ""
        most_used = 0
        for tag in ratio.keys():
            if (ratio[tag] > most_used):
                best_match = tag
                most_used = ratio[tag]
        return best_match
    tags = data[word]
    best_match = ""
    most_used = 0
    for tag in tags.keys():
        if (tags[tag] > most_used):
            best_match = tag
            most_used = tags[tag]
    return best_match

def baseline(train, test):
    """
    input:
    training data (list of sentences, with tags on the words). E.g.,  [[(word1, tag1), (word2, tag2)], [(word3, tag3), (word4, tag4)]]
    test data (list of sentences, no tags on the words). E.g.,  [[word1, word2], [word3, word4]]
    output:
    list of sentences, each sentence is a list of (word,tag) pairs.
    E.g., [[(word1, tag1), (word2, tag2)], [(word3, tag3), (word4, tag4)]]
    """

    data = dict()
    tags = dict()
    for sentence in train:
        for word, tag in sentence:
            if (tag not in tags):
                tags[tag] = 0
            if (word not in data):
                data[word] = dict()
            if (tag not in data[word]):
                data[word][tag] = 0
            data[word][tag] += 1
            tags[tag] += 1
    
    sentences = []
    for sentence in test:
        tagged_sentence = []
        for word in sentence:
            tagged_sentence.append((word, match(data, tags, word)))
        sentences.append(tagged_sentence)
    # print(sentences)
    return sentences