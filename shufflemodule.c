#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PyObject *do_pile(PyObject *origList, int num_piles, int num_shuffles)
{

	if (num_shuffles == 0) { return origList; }

	srand((int)time(NULL));
	double length;
	PyObject * shuffledList;

	length = PyList_Size(origList);
	shuffledList = PyList_New((int)length);

	printf("Num Piles => %d\n", num_piles);

	int i;
	for(i = 0; i < num_piles; i++)
	{
		printf("Pile => %d\n", i);
		int card_index = i;

		while ( (card_index + num_piles) < length )
		{
			printf("Card Index => %d\n", card_index);
			PyObject *temp = PyList_GetItem(origList, card_index);
			if (temp == NULL) {
				Py_DECREF(shuffledList);
				return NULL;
			}
			PyList_SET_ITEM(shuffledList, card_index + num_piles , temp);
			Py_INCREF(temp);

			card_index += num_piles;
		}

		printf("End Pile ----------------------\n");
	}

	printf("End Shuffle ======== \n");

	// recursively shuffle the desired amount
	printf("NUM_SHUFFLES => %d\n", num_shuffles);
	printf("NUM_PILES => %d\n", num_piles);
	num_shuffles--;
	if (num_shuffles > 0)
		shuffledList = do_pile(shuffledList, num_piles, num_shuffles);

	return shuffledList;

}

PyObject *do_mongean(PyObject *origList, int num_shuffles)
{

	if (num_shuffles == 0) { return origList; }

	srand((int)time(NULL));
	double length;
	PyObject * shuffledList;

	length = PyList_Size(origList);
	shuffledList = PyList_New((int)length);

	int i;
	int current;
	for(i = 0, current = length / 2; (current + (i + 1)) < length && (current - (i + 1)) > 0; i++)
	{

		printf("BEGIN CURRENT => %d\n", current);
		printf("ITERATION => %d\n", i);

		PyObject *temp = PyList_GetItem(origList, i);
		if (temp == NULL) {
			Py_DECREF(shuffledList);
			return NULL;
		}
		PyList_SET_ITEM(shuffledList, current , temp);
		Py_INCREF(temp);

		if (i % 2)  // even (back)
		{
			current += (i + 1);
		}
		else  // odd (front)
		{
			current -= (i + 1);
		}

		printf("END CURRENT => %d\n", current);
	}

	printf("Finished Loop");
	// recursively shuffle the desired amount
	num_shuffles--;
	if (num_shuffles > 0)
		shuffledList = do_mongean(shuffledList, num_shuffles);

	return shuffledList;

}

PyObject *do_overhand(PyObject *origList, int num_shuffles)
{

	if (num_shuffles == 0) { return origList; }

	srand((int)time(NULL));
	int o, s;
	double length;
	PyObject * shuffledList;

	length = PyList_Size(origList);
	shuffledList = PyList_New((int)length);

	// progress through shuffled list (backward)
	s = (int)length - 1;

	// progress through original list (forward)
	o = 0;

	// make sure we don't run off the end of the original list
	while ( o < length )
	{

		// random item number ceiling
		int ceiling;
		if (length > 50)
		{
			ceiling = 10;
		}
		else
		{
			ceiling = length > 10 ? (int)(.1 * length) : 1;
		}

		// get random number of items (min of 1), respecting the limit
		int num_items = (rand() % ceiling) + 1;

		double remaining = length - o;
		if (num_items > remaining)
		{
			num_items = remaining;
		}

		int i;
		for (i = 0; i < num_items; i++)
		{
			// add item to new shuffledList from the the origList
			// o + i = position in original list
			PyObject *temp = PyList_GetItem(origList, o + i);
			if (temp == NULL) {
				Py_DECREF(shuffledList);
				return NULL;
			}
			// s - (num_items - (i + 1)) = new position in shuffledList
			PyList_SET_ITEM(shuffledList, s - (num_items - (i + 1)) , temp);
			Py_INCREF(temp);
		}

		// move progress identifiers to next position
		s -= num_items;
		o += num_items;
	}

	// recursively shuffle the desired amount
	num_shuffles--;
	if (num_shuffles > 0)
		shuffledList = do_overhand(shuffledList, num_shuffles);

	return shuffledList;

}

PyObject *do_riffle(PyObject *origList, int num_shuffles)
{

	if (num_shuffles == 0) { return origList; }

	// how many times an item has been used consecutively from the same half
	int const max_streak = 10;
	int m, f, l, last_half_start, streak, *current_ptr;
	double length;
	srand((int)time(NULL));

	PyObject * shuffledList;

	length = (int)PyList_Size(origList);
	shuffledList = PyList_New((int)length);


	// initialize the current pointer
	current_ptr = (rand() % 2) ? &f : &l;

	/****
	 * Calculate the starting point for the second half of the list
	 * Add the half point to a random additional amount (maximum 10% of list)
	 */
	last_half_start = (int)(length / 2) + (rand() % (length > 10 ? (int)(.1 * length) : 2)) + 1;

	/**** Identifiers
	 * m = current total progress through shuffledList
	 * f = current progress across the first half
	 * l = current progress across the last half
	 * streak = count of how many times a card has been used consecutively from the same half
	 *
	 **** Conditions
	 * m < length = make sure we don't run off the end of shuffledArray
	 * l < length = make sure we don't run off the end of the last half
	 * f < last_half_start = make sure we don't run into the last half with the first half identifier
	 *
	 **** Increments
	 * m++ = progress through shuffledList
	 * *current_ptr++ = increment the currently select half's identifier
	 *
	 */
	for(m = 0, f = 0, l = last_half_start, streak = 0; m < length && l < length && f < last_half_start; m++, *current_ptr += 1)
	{
		// calculate remaining percentage of the list to process
		double remaining = 1 - m / length;
		// generate random double
		double test = rand() / (double)RAND_MAX;

		// if the random double is benieth the continually lowering remaining percentage
		// (lowering probability)
		if (test < remaining || streak > max_streak)
		{
			// switch to the other half
			current_ptr = (current_ptr == &f ? &l : &f);
			// reset streak
			streak = 0;
		}

		// add item to new shuffledList from the *current_ptr position in the origList
		PyObject *temp = PyList_GetItem(origList, *current_ptr);
		if (temp == NULL) {
			Py_DECREF(shuffledList);
			return NULL;
		}
		PyList_SET_ITEM(shuffledList, m, temp);
		Py_INCREF(temp);

		// increment streak
		streak += 1;
	}

	// change the pointer to the half that didn't cause the for to exit
	current_ptr = (current_ptr == &f ? &l : &f);

	// for the remaining half, add on to the end of the newly shuffledList
	// (one hand ran out before the other)
	while(m < (int)length)
	{
		PyObject *temp = PyList_GetItem(origList, *current_ptr);
		if (temp == NULL) {
			Py_DECREF(shuffledList);
			return NULL;
		}
		PyList_SET_ITEM(shuffledList, m, temp);
		Py_INCREF(temp);
		m++;
		*current_ptr += 1;
	}

	// recursively shuffle the desired amount
	num_shuffles--;
	if (num_shuffles > 0)
		shuffledList = do_riffle(shuffledList, num_shuffles);

	return shuffledList;

}

static PyObject *shuffle_pile(PyObject *self, PyObject *args)
{

	PyObject * origList;
	int num_piles;
	int num_shuffles = 1;

	// parse args to list
	if (! PyArg_ParseTuple( args, "Oi|i", &origList, &num_piles, &num_shuffles) )
	{
		return NULL;
	}

	return do_pile(origList, num_piles, num_shuffles);

}

static PyObject *shuffle_mongean(PyObject *self, PyObject *args)
{

	PyObject * origList;
	int num_shuffles = 1;

	// parse args to list
	if (! PyArg_ParseTuple( args, "O|i", &origList, &num_shuffles) )
	{
		return NULL;
	}

	return do_mongean(origList, num_shuffles);

}

static PyObject *shuffle_overhand(PyObject *self, PyObject *args)
{

	PyObject * origList;
	int num_shuffles = 1;

	// parse args to list
	if (! PyArg_ParseTuple( args, "O|i", &origList, &num_shuffles) )
	{
		return NULL;
	}

	return do_overhand(origList, num_shuffles);

}

static PyObject *shuffle_riffle(PyObject *self, PyObject *args)
{

	PyObject * origList;
	int num_shuffles = 1;

	// parse args to list
	if (! PyArg_ParseTuple( args, "O|i", &origList, &num_shuffles) )
	{
		return NULL;
	}

	return do_riffle(origList, num_shuffles);

}

static PyMethodDef ShuffleMethods[] = {
	{"riffle", shuffle_riffle, METH_VARARGS, "Simulate a Riffle Shuffle on a List."},
	{"overhand", shuffle_overhand, METH_VARARGS, "Simulate an Overhand Shuffle on a List."},
	{"mongean", shuffle_mongean, METH_VARARGS, "Simulate a Mongean Shuffle on a List."},
	{"pile", shuffle_pile, METH_VARARGS, "Simulate a Pile Shuffle on a List."},
	{NULL, NULL, 0, NULL}
};

void initshuffle(void){
	(void) Py_InitModule("shuffle", ShuffleMethods);
}