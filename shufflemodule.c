#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PyObject *do_shuffle(PyObject *origList, int num_shuffles)
{
	// how many times a card has been used consecutively from the same half
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
		shuffledList = do_shuffle(shuffledList, num_shuffles);

	return shuffledList;

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

	return do_shuffle(origList, num_shuffles);

}

static PyMethodDef ShuffleMethods[] = {
	{"riffle", shuffle_riffle, METH_VARARGS, "Simulate a Riffle Shuffle on a List."},
	{NULL, NULL, 0, NULL}
};

void initshuffle(void){
	(void) Py_InitModule("shuffle", ShuffleMethods);
}