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


if __name__ == '__main__':
	unittest.main()
