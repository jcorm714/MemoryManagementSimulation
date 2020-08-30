#include<iostream>
#include<cstdlib>

#define PAGE_STREAM_SIZE  100

int main(){

    bool isValid = false;
    unsigned int numPages = 0;
    unsigned int addrs[PAGE_STREAM_SIZE];
    do{
        try{
          std::cout << "Enter the number of pages (2-7): ";
          std::cin>>numPages;
          isValid =true;
        } catch(...){
            isValid = false;
        }
    } while(!isValid && numPages < 2 || numPages > 7);


    //initialize random stream
    for(int i =0; i < PAGE_STREAM_SIZE; i++){
       addrs[i] = rand() % 32768;
    }
    
    std::cout << "address\t\t" << "page\t" << "offset\n";
    for(int i : addrs){
       int offset = i % 4096;
       int pNum = (i - offset)/4096;
       std::cout <<i << "\t\t" << pNum << '\t' << offset << '\n';
    }


    return 0;
}