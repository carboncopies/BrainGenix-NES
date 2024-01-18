#!/bin/python3

import Test0
import Test1
import Test2
import Test3
import Test4




def FormatResult(_Time:float, _Name:str):
    return f"|{_Name}|{_Time}|\n"


def ExecuteRenderBenchmarks():
    TableResults:str = "Render Benchmarks\n|Test | Resulting Time|\n|--------------|--------------|\n"
    TableResults += FormatResult(Test0.GetTime(), "Test 0")    
    TableResults += FormatResult(Test1.GetTime(), "Test 1")    
    TableResults += FormatResult(Test2.GetTime(), "Test 2")    
    TableResults += FormatResult(Test3.GetTime(), "Test 3")    
    # TableResults += FormatResult(Test4.GetTime(), "Test 4")    
    return TableResults



def Main():

    print(ExecuteRenderBenchmarks())




if __name__ == "__main__":
    Main()