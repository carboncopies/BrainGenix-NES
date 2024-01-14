#!/bin/python3

import Test1
import Test2
import Test3

import os



def FormatResult(_Time:float, _Name:str):
    return f"|{_Name}|{_Time}|\n"


def ExecuteRenderBenchmarks():
    TableResults:str = "Render Benchmarks\n|Test | Resulting Time|\n|--------------|--------------|\n"
    TableResults += FormatResult(Test1.GetTime(), "Test 1")    
    TableResults += FormatResult(Test2.GetTime(), "Test 2")    
    TableResults += FormatResult(Test3.GetTime(), "Test 3")    
    return TableResults



def Main():

    print(ExecuteRenderBenchmarks())
    os.system("rm -rf *.png")




if __name__ == "__main__":
    Main()