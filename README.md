# Shuffle

A simple python module that is meant to simulate the action of shuffling
a python List as if it were a deck of cards.

## Types
  http://en.wikipedia.org/wiki/Shuffling

### Riffle
  Predictable behaviours that make a human riffle shuffle imperfect

  - Once a deck of cards is divided to each hand, the amount of cards in each hand is not (likely) equal.
  - As each thumb releases from it's half of the deck, sometimes more than one card drops from a single hand at a time.
  - As the deck in each hand gets more and more thin, the probability of larger chucks of cards dropping from each hand increases.

```
shuffle.riffle(list[, int])
```

## Installation

```
git clone git://github.com/jondavidjohn/Card-Shuffle-for-Python.git shuffle
cd ./shuffle
python setup.py install
```

## Usage

```python
import shuffle

shuffled_list = shuffle.riffle(original_list, 3)  # shuffles the list 3 times
