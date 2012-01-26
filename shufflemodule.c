#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PyObject *do_pile(PyObject *origList, int num_piles, int num_shuffles)
{
	double length = PyList_Size(origList);
	int i;

	// init empty list to return
	PyObject * shuffledList = PyList_New(0);

	// init piles
	PyObject **pilesArray = malloc(num_piles * sizeof(PyObject *));
	for(i = 0; i < num_piles; i++)
	{
		pilesArray[i] = PyList_New(0);
	}

	// deal items into respective piles
	for(i = 0; i < length; i++)
	{
		int pile = i % num_piles;

		// Retrieve Object
		PyObject *temp = PyList_GetItem(origList, i);
		if (temp == NULL)
		{
			Py_DECREF(shuffledList);
			for(i = 0; i < num_piles; i++)
			{
				Py_DECREF(pilesArray[i]);
			}
			free(pilesArray);
			return NULL;
		}

		// append the item
		int test = PyList_Append(pilesArray[pile], temp);
		if (test != 0)
		{
			Py_DECREF(shuffledList);
			for(i = 0; i < num_piles; i++)
			{
				Py_DECREF(pilesArray[i]);
			}
			free(pilesArray);
			return NULL;
		}
		Py_INCREF(temp);

	}

	// Reverse Piles
	for(i = 0; i < num_piles; i++)
	{
		int test = PyList_Reverse(pilesArray[i]);
		if (test != 0)
		{
			Py_DECREF(shuffledList);
			for(i = 0; i < num_piles; i++)
			{
				Py_DECREF(pilesArray[i]);
			}
			free(pilesArray);
			return NULL;
		}
	}

	// Build final list
	for(i = 0; i < num_piles; i++)
	{
		int pile_size = (int)PyList_Size(pilesArray[i]);
		int j;
		for(j = 0; j < pile_size; j++)
		{
			PyObject *temp = PyList_GetItem(pilesArray[i], j);
			if (temp == NULL)
			{
				Py_DECREF(shuffledList);
				for(i = 0; i < num_piles; i++)
				{
					Py_DECREF(pilesArray[i]);
				}
				free(pilesArray);
				return NULL;
			}

			// append the item
			int test = PyList_Append(shuffledList, temp);
			if (test != 0)
			{
				Py_DECREF(shuffledList);
				for(i = 0; i < num_piles; i++)
				{
					Py_DECREF(pilesArray[i]);
				}
				free(pilesArray);
				return NULL;
			}
			Py_INCREF(temp);
		}
	}

	// free uneeded memory
	for(i = 0; i < num_piles; i++)
	{
		Py_DECREF(pilesArray[i]);
	}
	free(pilesArray);

	// recursively shuffle the desired amount
	num_shuffles--;
	if (num_shuffles > 0)
	{
		shuffledList = do_pile(shuffledList, num_piles, num_shuffles);
	}

	return shuffledList;
}

PyObject *do_mongean(PyObject *origList, int num_shuffles)
{
	double length = PyList_Size(origList);
	PyObject * shuffledList = PyList_New((int)length);

	int i, current;
	for(i = 0, current = (int)length / 2; i < length; i++)
	{
		PyObject *temp = PyList_GetItem(origList, i);
		if (temp == NULL)
		{
			Py_DECREF(shuffledList);
			return NULL;
		}
		PyList_SET_ITEM(shuffledList, current , temp);
		Py_INCREF(temp);

		// Calc position of next item
		if (i % 2)  // even (back)
		{
			current += (i + 1);
		}
		else  // odd (front)
		{
			current -= (i + 1);
		}
	}

	// recursively shuffle the desired amount
	num_shuffles--;
	if (num_shuffles > 0)
	{
		shuffledList = do_mongean(shuffledList, num_shuffles);
	}

	return shuffledList;
}

PyObject *do_overhand(PyObject *origList, int num_shuffles)
{
	srand((int)time(NULL));
	double length = PyList_Size(origList);
	PyObject * shuffledList = PyList_New((int)length);

	// progress through shuffled list (backward)
	int s = (int)length - 1;

	// progress through original list (forward)
	int o = 0;

	// make sure we don't run off the end of the original list
	while ( o < length )
	{
		// random item number ceiling
		int ceiling = length > 4 ? (int)(.2 * length) : 1;

		// get random number of items (min of 1), respecting the limit
		int num_items = (rand() % ceiling) + 1;

		int remaining = (int)length - o;
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
			if (temp == NULL)
			{
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
	{
		shuffledList = do_overhand(shuffledList, num_shuffles);
	}

	return shuffledList;
}

PyObject *do_riffle(PyObject *origList, int num_shuffles)
{
	// how many times an item has been used consecutively from the same half
	int const max_streak = 10;

	srand((int)time(NULL));
	double length = (int)PyList_Size(origList);
	PyObject * shuffledList = PyList_New((int)length);

	/****
	 * Calculate the starting point for the second half of the list
	 * Add the half point to a random additional amount (maximum 10% of list)
	 */
	int last_half_start = (int)(length / 2) + (rand() % (length > 10 ? (int)(.1 * length) : 2)) + 1;

	/**** Identifiers
	 * m = current total progress through shuffledList
	 * f = current progress across the first half
	 * l = current progress across the last half
	 * streak = count of how many times a card has been used consecutively from the same half
	 *
	 **** Conditions
	 * m < length = make sure we don't run off the end of shuffledList
	 * l < length = make sure we don't run off the end of the last half
	 * f < last_half_start = make sure we don't run into the last half with the first half identifier
	 *
	 **** Increments
	 * m++ = progress through shuffledList
	 * *current_ptr++ = increment the currently select half's identifier
	 *
	 */
	int m, f, l, streak;
	// initialize the current pointer
	int *current_ptr = (rand() % 2) ? &f : &l;
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
		if (temp == NULL)
		{
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
		if (temp == NULL)
		{
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
	{
		shuffledList = do_riffle(shuffledList, num_shuffles);
	}

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