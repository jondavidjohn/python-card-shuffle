#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PyObject *do_shuffle(PyObject *origList, int num_shuffles)
{
	int const MAX_STREAK = 10;
	int m, f, l, end_range, streak, *current_ptr;
	double length;
	srand((int)time(NULL));

	PyObject * shuffledList;

	length = (int)PyList_Size(origList);
	current_ptr = (rand() % 2) ? &f : &l;
	end_range = (int)(length / 2) + (rand() % (length > 10 ? (int)(.1 * length) : 2));
	shuffledList = PyList_New((int)length);

	for(m = 0, f = 0, l = (end_range + 1), streak = 0; m < length && l < length && f < end_range + 1; m++, *current_ptr += 1)
	{
		double remaining = 1 - m / length;
		double test = rand() / (double)RAND_MAX;

		if (test < remaining || streak > MAX_STREAK)
		{
			current_ptr = (current_ptr == &f ? &l : &f);
			streak = 0;
		}

		PyObject *temp = PyList_GetItem(origList, *current_ptr);
		if (temp == NULL) {
			Py_DECREF(shuffledList);
			return NULL;
		}

		PyList_SET_ITEM(shuffledList, m, temp);
		Py_INCREF(temp);
		streak += 1;
	}

	// change the pointer to the one that didn't cause the for to exit
	current_ptr = (current_ptr == &f ? &l : &f);


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