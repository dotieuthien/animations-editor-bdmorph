#ifndef CHOLMOD_VECTOR_H
#define CHOLMOD_VECTOR_H

#include <cholmod.h>
#include "cholmod_common.h"
#include <stdio.h>


class CholmodVector
{
public:
	CholmodVector(unsigned int len) :
		values(cholmod_zeros(len, 1, CHOLMOD_REAL, cholmod_get_common())), cm(cholmod_get_common())
	{}

	CholmodVector() : values(NULL), cm(cholmod_get_common()){}

	void resize(unsigned int newSize)
	{
		cholmod_free_dense(&values,cm);
		values = cholmod_zeros(newSize, 1, CHOLMOD_REAL, cm);
	}

	unsigned int size() const
	{
		return values->nrow;
	}

	void setData(cholmod_dense* new_values)
	{
		cholmod_free_dense(&values, cm);
		values = new_values;
	}

	void add(const CholmodVector &other)
	{
		double *values1 = getValues();
		const double *values2 = other.getValues();

		for (unsigned int i = 0 ;  i < size() ;i++)
			values1[i] += values2[i];
	}

	void sub(const CholmodVector &other)
	{
		double *values1 = getValues();
		const double *values2 = other.getValues();

		for (unsigned int i = 0 ;  i < size() ;i++)
			values1[i] = values1[i] - values2[i];
	}

	double& operator[] (int index)
	{
		return getValues()[index];
	}

	const double& operator[] (int index) const
	{
		return getValues()[index];
	}

	CholmodVector operator*(const double other) const
	{
		const double *values1 = getValues();
		CholmodVector result(size());

		for (unsigned int i = 0 ; i < size() ; i++)
			result[i] = values1[i] * other;

		return result;
	}


	void display(const char* var, FILE* file)
	{
		double *values1 = getValues();

		fprintf(file, "%s = [", var);
		bool first = true;

		for (unsigned int i = 0 ; i < size() ; i++) {
			if (first) {
				first = false;
				fprintf(file, "%25.20e", values1[i]);
			} else
				fprintf(file, "; %25.20e", values1[i]);
		}

		fprintf(file, "];\n");
	}

	operator cholmod_dense*() { return values; }

	double* getValues()  { return (double*)values->x; }
	const double* getValues() const  { return (double*)values->x; }

	~CholmodVector() { cholmod_free_dense(&values,cm); }
public:
	cholmod_dense* values;
	cholmod_common *cm;
};

#endif
