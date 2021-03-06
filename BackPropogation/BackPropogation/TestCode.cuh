#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/complex.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <thrust/transform.h>
#include <thrust/transform_reduce.h>
#include <thrust/iterator/zip_iterator.h>
#include <thrust/iterator/permutation_iterator.h>
#include <thrust/execution_policy.h>
#include <thrust/copy.h>
#ifndef weight_type
#define weight_type double
#endif

#ifndef __FUNCTORS__H__INCLUDED__
#include "Recurrent_Functors.cuh"
#define __FUNCTORS__H__INCLUDED__
#endif

using namespace std;
namespace testing{


	inline void testOutputToTarget(thrust::device_vector<weight_type> weights, thrust::device_vector<weight_type> values, thrust::device_vector<weight_type> output){

	}


	

	template <typename T>
	void outputToFile(thrust::device_vector<T> values, string title){
		outputToFile(values, title, "tests/test1.txt");
	}

	template <typename T>
	void outputToFile(thrust::device_vector<T> values, string title, string file_name){
		static bool opened_once = false;
		std::ofstream outputfile;
		outputfile.precision(30);
		if (opened_once){
			outputfile.open(file_name, std::ios::app | std::ios::ate);
		}
		else{
			outputfile.open(file_name, ios::trunc);
		}

		if (outputfile.is_open()){
			outputfile << title << endl;
			thrust::copy(values.begin(), values.end(), std::ostream_iterator<T>(outputfile, "\n"));
			outputfile << endl << endl;
		}
		outputfile.close();
		opened_once = true;

	}

	template <typename T, typename Iterator>
	void outputToFile(Iterator begin, int length, string title){
		outputToFile<T>(begin, length, title, "test/test2.txt");
	}

	template <typename T, typename Iterator>
	void outputToFile(Iterator begin, int length, string title, string file_name){
		static bool opened_once = false;
		std::ofstream outputfile;
		outputfile.precision(30);
		if (opened_once){
			outputfile.open(file_name, std::ios::app | std::ios::ate);
		}
		else{
			outputfile.open(file_name, ios::trunc);
		}

		if (outputfile.is_open()){
			outputfile << title << endl;
			thrust::copy(begin, begin + length, std::ostream_iterator<T>(outputfile, "\n"));
			outputfile << endl << endl;
		}
		outputfile.close();
		opened_once = true;

	}

	template <typename T>
	void outputArrayToFile(T* out, int size, string file_name){
		static bool opened_once = false;
		std::ofstream outputfile;
		outputfile.precision(30);
		if (opened_once){
			outputfile.open(file_name, std::ios::app | std::ios::ate);
		}
		else{
			outputfile.open(file_name, ios::trunc);
		}

		if (outputfile.is_open()){
			for (int i = 0; i < size; i++){
				outputfile << out[i] << ",";
			}

			outputfile << endl;

			outputfile.close();
			opened_once = true;
		}
	}


	template <typename T>
	void outputArrayToFile(T** out, int size2,int size, string file_name){
		static bool opened_once = false;
		std::ofstream outputfile;
		outputfile.precision(30);
		if (opened_once){
			outputfile.open(file_name, std::ios::app | std::ios::ate);
		}
		else{
			outputfile.open(file_name, ios::trunc);
		}

		if (outputfile.is_open()){
			for (int j = 0; j < size2; j++){
				for (int i = 0; i < size; i++){
					outputfile << out[j][i] << ",";
				}
			}
			outputfile << endl;

			outputfile.close();
			opened_once = true;
		}
	}


	template<typename T>
	void outputVectorToFile(std::vector<T> vec, string title, string file_name){
		static bool opened_once = false;
		std::ofstream outputfile;
		outputfile.precision(30);
		if (opened_once){
			outputfile.open(file_name, std::ios::app | std::ios::ate);
		}
		else{
			outputfile.open(file_name, ios::trunc);
		}

		if (outputfile.is_open()){
			outputfile << title << endl;
			std::copy(vec.begin(), vec.end(), std::ostream_iterator<T>(outputfile, "\n"));
			outputfile << endl << endl;
		}
		outputfile.close();
		opened_once = true;
	}
}

namespace value_testing{
	//Find the mean square error
	template <typename T, typename Iterator>
	T getMeanSquareErrorResults(Iterator _pred_begin, Iterator _pred_end, Iterator _real_begin, Iterator _real_end){
		thrust::transform(_real_begin, _real_end, _pred_begin, _real_begin, functors::mean_square_error<weight_type>());
		return thrust::reduce(_real_begin, _real_end,(weight_type)0);
		//Reduce the results
		/*return thrust::reduce(
			thrust::make_transform_iterator(
			thrust::make_zip_iterator(
			thrust::make_tuple(
			_real_begin,
			_pred_begin
			)
			),
			functors::mean_square_error_special<weight_type>()
			),
			thrust::make_transform_iterator(
			thrust::make_zip_iterator(
			thrust::make_tuple(
			_real_end,
			_pred_end
			)
			),
			functors::mean_square_error_special<weight_type>()
			)
		);*/

		
	}

	template <typename T, typename Iterator>
	void getMeanSquareError(Iterator _pred_begin, Iterator _pred_end, Iterator _real_begin, Iterator _real_end, host_vector<T> &storage){
		storage[0] = getMeanSquareErrorResults<T>(_pred_begin, _pred_end, _real_begin, _real_end) / (storage.size() - 1);
		//Copy the values
		thrust::copy(_real_begin, _real_end, storage.begin() + 1);
	}

	template <typename T, typename Iterator>
	void getMeanSquareErrorSum(Iterator _pred_begin, Iterator _pred_end, Iterator _real_begin, Iterator _real_end, host_vector<T> &storage){
		host_vector<weight_type> temp = host_vector<weight_type>(storage.size()-1);
		storage[0] += (getMeanSquareErrorResults<T>(_pred_begin, _pred_end, _real_begin, _real_end) / temp.size());
		thrust::copy(_real_begin, _real_end, temp.begin());
		thrust::transform(thrust::host, temp.begin(), temp.end(), storage.begin() + 1, storage.begin() + 1, _1 + _2);
	}
}