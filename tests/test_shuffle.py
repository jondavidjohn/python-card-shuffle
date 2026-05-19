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


if __name__ == '__main__':
	unittest.main()
