import sys
import unittest

import shuffle


class RiffleTests(unittest.TestCase):
	def assertRifflePreservesDeck(self, deck):
		shuffled = shuffle.riffle(deck)

		self.assertIsInstance(shuffled, list)
		self.assertIsNot(shuffled, deck)
		self.assertEqual(len(shuffled), len(deck))
		self.assertCountEqual(shuffled, deck)

	def test_riffle_handles_empty_list(self):
		self.assertRifflePreservesDeck([])

	def test_riffle_handles_single_item_list(self):
		self.assertRifflePreservesDeck([1])

	def test_riffle_handles_two_item_list(self):
		self.assertRifflePreservesDeck([1, 2])

	def test_riffle_handles_three_item_list(self):
		self.assertRifflePreservesDeck([1, 2, 3])

	def test_riffle_handles_normal_list(self):
		self.assertRifflePreservesDeck(list(range(10)))


class PileTests(unittest.TestCase):
	def test_pile_rejects_zero_piles(self):
		with self.assertRaises(ValueError):
			shuffle.pile([1, 2, 3], 0)

	def test_pile_rejects_negative_piles(self):
		with self.assertRaises(ValueError):
			shuffle.pile([1, 2, 3], -1)

	def test_pile_handles_valid_pile_count(self):
		deck = list(range(10))
		shuffled = shuffle.pile(deck, 3)

		self.assertIsInstance(shuffled, list)
		self.assertIsNot(shuffled, deck)
		self.assertEqual(len(shuffled), len(deck))
		self.assertCountEqual(shuffled, deck)


class RefCountTests(unittest.TestCase):
	def assertShuffleDoesNotLeakItemRefs(self, shuffle_func):
		marker = object()
		deck = [marker] + [object() for _ in range(9)]
		before = sys.getrefcount(marker)

		shuffled = shuffle_func(deck)
		self.assertEqual(sys.getrefcount(marker), before + 1)

		del shuffled
		self.assertEqual(sys.getrefcount(marker), before)

	def test_riffle_does_not_leak_item_refs(self):
		self.assertShuffleDoesNotLeakItemRefs(lambda deck: shuffle.riffle(deck, 3))

	def test_overhand_does_not_leak_item_refs(self):
		self.assertShuffleDoesNotLeakItemRefs(lambda deck: shuffle.overhand(deck, 3))

	def test_mongean_does_not_leak_item_refs(self):
		self.assertShuffleDoesNotLeakItemRefs(lambda deck: shuffle.mongean(deck, 3))

	def test_pile_does_not_leak_item_refs(self):
		self.assertShuffleDoesNotLeakItemRefs(lambda deck: shuffle.pile(deck, 3, 3))


if __name__ == '__main__':
	unittest.main()
