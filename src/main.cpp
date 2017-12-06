#include <iostream>
#include "includes.hpp"
#include "tracing.hpp"
#include "default_setup.hpp"

int main(int argc, char** argv)
{
    cout << "\nVisualization of WiFi coverage" << endl;

    Timer t; // measure runtime
    t.start();

    if (argc == 2){
        cout << "Check on config file..." << endl; 

    }else{
        cout << "No config! Using default parameters." <<endl;
        default_run();
    }    

    t.check("\nCPU runtime");
    
    return 0;
}