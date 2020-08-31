#include<iostream>
#include<cstdlib>

#define PAGE_STREAM_SIZE  100

int determineFurthestPage(unsigned int pageStream[], unsigned int activePages[], int pageStreamStart);
void resetPageFrames(unsigned int** pageFrames, int width, int height);
void displayPageFrame(unsigned int** pageFrames, int width, int height);

int main(){

    bool isValid = false;
    unsigned int numPages = 0;
    unsigned int addrs[PAGE_STREAM_SIZE];
    unsigned int pageStream[PAGE_STREAM_SIZE];
   
    do{
        try{
          std::cout << "Enter the number of pages (2-7): ";
          std::cin>>numPages;
          isValid =true;
        } catch(...){
            isValid = false;
        }
    } while(!isValid && numPages < 2 || numPages > 7);

    //create allocate enough space in the array for 
    //for each page to be run in the array
    unsigned int **pageFrames = new unsigned int*[numPages];
    for(int i = 0; i < numPages; i++){
        pageFrames[i] = new unsigned int[PAGE_STREAM_SIZE];
    }

    //initialize random stream
    for(int i =0; i < PAGE_STREAM_SIZE; i++){
    
       addrs[i] = rand() % 32768;
       int offset = addrs[i] % 4096;
       int pNum = (addrs[i] - offset)/4096;

       pageStream[i] = pNum;
    }
    
    std::cout << "address\t\t" << "page\t" << "offset\n";
    for(int i : addrs){
       int offset = i % 4096;
       int pNum = (i - offset)/4096;
       std::cout <<i << "\t\t" << pNum << '\t' << offset << '\n';
    }


    //free up memory
    for(int i = 0; i < numPages; i++){
        delete[] pageFrames[i];
    }
    delete[] pageFrames;


    return 0;
}

void resetPageFrames(unsigned int** pageFrames, int width, int height){
    for(int i = 0; i < width; i++){
        for(int j = 0; j< height; j++){
            pageFrames[i][j] = NULL;
        }
    }
}

void displayPageFrame(unsigned int** pageFrames, int width, int height){
     for(int i = 0; i < width; i++){
        for(int j = 0; j< height; j++){
            if(pageFrames[i][j] == NULL){
                std::cout<< ' ';
            } else {
                std::cout << pageFrames[i][j] << ' ';
            }
        }
        std::cout<< '\n';
    }
}


int determineFurthestPage(unsigned int pageStream[], unsigned int activePages[], int pageStreamStart){
    
    int pageStreamLength = sizeof(pageStream)/sizeof(pageStream[0]);
    int activePagesLength = sizeof(activePages)/sizeof(activePages[0]);
    int furthestPage = -1;
    int greatestDistance = -1;
    int greatestPageIdx = -1;


    for(int i =0; i < activePagesLength; i++){
        unsigned int currentPage = activePages[i];
        //starting at the current point in the page stream
        //find the page with the greatest distance from the point
        for(int j=pageStreamStart; j< pageStreamLength; j++){
            unsigned int currPage = pageStream[j];
            if(currentPage == currPage){
                int dist = j - pageStreamStart;
                if(dist > greatestDistance){
                    greatestDistance = dist;
                    greatestPageIdx = i;
                    break;
                }
            }
        }

    }
    return greatestPageIdx;
}