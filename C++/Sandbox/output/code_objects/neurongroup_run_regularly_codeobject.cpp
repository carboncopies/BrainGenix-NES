#include "code_objects/neurongroup_run_regularly_codeobject.h"
#include "objects.h"
#include "brianlib/common_math.h"
#include "brianlib/stdint_compat.h"
#include<cmath>
#include<ctime>
#include<iostream>
#include<fstream>
#include<climits>
#include <opencv4/opencv2/core/core.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>

////// SUPPORT CODE ///////
namespace {
        
    double* get_frame(bool new_frame)
    {
        // The following initializations will only be executed once
        static cv::VideoCapture source("/home/tliao/Documents/BrainGenix-NES/sandbox/Megamind.avi");
        static cv::Mat frame;
        static double* grayscale_frame = (double*)malloc(VIDEO_WIDTH*VIDEO_HEIGHT*sizeof(double));
        if (new_frame)
        {
            source >> frame;
            double mean_value = 0;
            for (int row=0; row<VIDEO_HEIGHT; row++)
                for (int col=0; col<VIDEO_WIDTH; col++)
                {
                    const double grayscale_value = (frame.at<cv::Vec3b>(row, col)[0] +
                                                    frame.at<cv::Vec3b>(row, col)[1] +
                                                    frame.at<cv::Vec3b>(row, col)[2])/(3.0*128);
                    mean_value += grayscale_value / (VIDEO_WIDTH * VIDEO_HEIGHT);
                    grayscale_frame[row*VIDEO_WIDTH + col] = grayscale_value;
                }
            // subtract the mean
            for (int i=0; i<VIDEO_HEIGHT*VIDEO_WIDTH; i++)
                grayscale_frame[i] -= mean_value;
        }
        return grayscale_frame;
    }
    double video_input(const int x, const int y)
    {
        // Get the current frame (or a new frame in case we are asked for the first
        // element
        double *frame = get_frame(x==0 && y==0);
        return frame[y*VIDEO_WIDTH + x];
    }
    template < typename T1, typename T2 > struct _higher_type;
    template < > struct _higher_type<int,int> { typedef int type; };
    template < > struct _higher_type<int,long> { typedef long type; };
    template < > struct _higher_type<int,long long> { typedef long long type; };
    template < > struct _higher_type<int,float> { typedef float type; };
    template < > struct _higher_type<int,double> { typedef double type; };
    template < > struct _higher_type<int,long double> { typedef long double type; };
    template < > struct _higher_type<long,int> { typedef long type; };
    template < > struct _higher_type<long,long> { typedef long type; };
    template < > struct _higher_type<long,long long> { typedef long long type; };
    template < > struct _higher_type<long,float> { typedef float type; };
    template < > struct _higher_type<long,double> { typedef double type; };
    template < > struct _higher_type<long,long double> { typedef long double type; };
    template < > struct _higher_type<long long,int> { typedef long long type; };
    template < > struct _higher_type<long long,long> { typedef long long type; };
    template < > struct _higher_type<long long,long long> { typedef long long type; };
    template < > struct _higher_type<long long,float> { typedef float type; };
    template < > struct _higher_type<long long,double> { typedef double type; };
    template < > struct _higher_type<long long,long double> { typedef long double type; };
    template < > struct _higher_type<float,int> { typedef float type; };
    template < > struct _higher_type<float,long> { typedef float type; };
    template < > struct _higher_type<float,long long> { typedef float type; };
    template < > struct _higher_type<float,float> { typedef float type; };
    template < > struct _higher_type<float,double> { typedef double type; };
    template < > struct _higher_type<float,long double> { typedef long double type; };
    template < > struct _higher_type<double,int> { typedef double type; };
    template < > struct _higher_type<double,long> { typedef double type; };
    template < > struct _higher_type<double,long long> { typedef double type; };
    template < > struct _higher_type<double,float> { typedef double type; };
    template < > struct _higher_type<double,double> { typedef double type; };
    template < > struct _higher_type<double,long double> { typedef long double type; };
    template < > struct _higher_type<long double,int> { typedef long double type; };
    template < > struct _higher_type<long double,long> { typedef long double type; };
    template < > struct _higher_type<long double,long long> { typedef long double type; };
    template < > struct _higher_type<long double,float> { typedef long double type; };
    template < > struct _higher_type<long double,double> { typedef long double type; };
    template < > struct _higher_type<long double,long double> { typedef long double type; };
    template < typename T1, typename T2 >
    static inline typename _higher_type<T1,T2>::type
    _brian_mod(T1 x, T2 y)
    {{
        return x-y*floor(1.0*x/y);
    }}
    template < typename T1, typename T2 >
    static inline typename _higher_type<T1,T2>::type
    _brian_floordiv(T1 x, T2 y)
    {{
        return floor(1.0*x/y);
    }}
    #ifdef _MSC_VER
    #define _brian_pow(x, y) (pow((double)(x), (y)))
    #else
    #define _brian_pow(x, y) (pow((x), (y)))
    #endif

}

////// HASH DEFINES ///////



void _run_neurongroup_run_regularly_codeobject()
{
    using namespace brian;


    ///// CONSTANTS ///////////
    const size_t _numrow = 380160;
const size_t _numI = 380160;
const size_t _numcolumn = 380160;
    ///// POINTERS ////////////
        
    int32_t* __restrict  _ptr_array_neurongroup_row = _array_neurongroup_row;
    double* __restrict  _ptr_array_neurongroup_I = _array_neurongroup_I;
    int32_t* __restrict  _ptr_array_neurongroup_column = _array_neurongroup_column;


    //// MAIN CODE ////////////
    // scalar code
    const size_t _vectorisation_idx = -1;
        


    const int _N = 380160;
    
    for(int _idx=0; _idx<_N; _idx++)
    {
        // vector code
        const size_t _vectorisation_idx = _idx;
                
        const int32_t row = _ptr_array_neurongroup_row[_idx];
        const int32_t column = _ptr_array_neurongroup_column[_idx];
        double I;
        I = video_input(column, row);
        _ptr_array_neurongroup_I[_idx] = I;

    }

}


