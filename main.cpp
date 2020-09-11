#include<iostream>
#include<cstdlib>
#include<memory>
#include<vector>
#include<map>
#include<stdlib.h>
#include<time.h>


#define PAGE_STREAM_SIZE  100
#define NOTHING_IN_FRAME  17

class Row {
    public:
        std::unique_ptr<unsigned int[]> row;
        Row(int size) {
            this->row = std::unique_ptr<unsigned int[]>(new unsigned int[size]);
            for (int i = 0; i < size; i++) {
                this->row[i] = NOTHING_IN_FRAME;
            }
        } 
};


int determineFurthestPage(unsigned int pageStream[], std::shared_ptr<Row>&, int pageStreamStart, int pageStreamLength, int activePagesLength);
int findFIFOPage(std::vector<std::shared_ptr<Row>>&, int activePagesLength);
int findLFUPage(std::map<int, int>, std::shared_ptr<Row>&, int rowLength);
void displayPageFrame(std::shared_ptr<Row>& currRow, int page, int rowLength);
int findEmptySlot(std::shared_ptr<Row>& row, int length);
bool pageInMemory(int page, std::shared_ptr<Row>&,  int length);
int findPageInProcessed(std::vector<std::shared_ptr<Row>>&, int page, int rowSize);

int main(){

    bool isValid = false;
    int numPages = 0;
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
    std::vector<std::shared_ptr<Row>>* pageFramesBelady = new std::vector<std::shared_ptr<Row>>();
    std::vector<std::shared_ptr<Row>>* pageFramesFIFO = new std::vector<std::shared_ptr<Row>>();
    std::vector<std::shared_ptr<Row>>* pageFramesLFU = new std::vector<std::shared_ptr<Row>>();
    std::map<int, int> mapMFU;


    
    
    //initialize empty row
    std::shared_ptr<Row> startBelady = std::shared_ptr<Row>(new Row(numPages));
    std::shared_ptr<Row> startFIFO = std::shared_ptr<Row>(new Row(numPages));
    std::shared_ptr<Row> startLFU = std::shared_ptr<Row>(new Row(numPages));
    pageFramesBelady->push_back(startBelady);
    pageFramesFIFO->push_back(startFIFO);
    pageFramesLFU->push_back(startLFU);

    //initialize random stream
    srand(time(NULL));
    
    for(int i =0; i < PAGE_STREAM_SIZE; i++){
    
       addrs[i] = rand() % 32768;
       int offset = addrs[i] % 4096;
       int pNum = (addrs[i] - offset)/4096;

       pageStream[i] = pNum;
    }
   
    int pageFaultsBelady = 0;
    int pageFaultsFIFO = 0;
    int pageFaultsLFU = 0;

    //Run through Belady's optimal algorithm first 

    std::cout << "**********************************" << '\n';
    std::cout << "*        Belady's Optimal        *" << '\n';
    std::cout << "**********************************" << '\n';
   
    for (int i = 0; i < PAGE_STREAM_SIZE; i++) {

        //only switch pages if the page is not in memory
        if (pageInMemory(pageStream[i], pageFramesBelady->back(), numPages)) {
            displayPageFrame(pageFramesBelady->back(), pageStream[i], numPages);

            continue;
        }


        int idx = findEmptySlot(pageFramesBelady->back(), numPages);
        if (idx >= 0) {
            pageFramesBelady->back()->row[idx] = pageStream[i];
            pageFaultsBelady++;

        }
        else {
           
            int frameToSwitch = determineFurthestPage(pageStream, pageFramesBelady->back(), i, PAGE_STREAM_SIZE, numPages);
            pageFramesBelady->back()->row[frameToSwitch] = pageStream[i];
            pageFaultsBelady++;

        }
      
        std::shared_ptr<Row> newRow = std::shared_ptr<Row>(new Row(numPages));
        for (int i = 0; i < numPages; i++) {
            newRow->row[i] = pageFramesBelady->back()->row[i];
        }
        pageFramesBelady->push_back(newRow);
     
        displayPageFrame(pageFramesBelady->back(), pageStream[i], numPages);
      
    }

    
    std::cout << "**********************************" << '\n';
    std::cout << "*              FIFO              *" << '\n';
    std::cout << "**********************************" << '\n';

    for (int i = 0; i < PAGE_STREAM_SIZE; i++) {

        //only switch pages if the page is not in memory
        if (pageInMemory(pageStream[i], pageFramesFIFO->back(), numPages)) {
            displayPageFrame(pageFramesFIFO->back(), pageStream[i], numPages);

            continue;
        }


        int idx = findEmptySlot(pageFramesFIFO->back(), numPages);
        if (idx >= 0) {
            pageFramesFIFO->back()->row[idx] = pageStream[i];
            pageFaultsFIFO++;

        }
        else {

            int frameToSwitch = findFIFOPage(*pageFramesFIFO, numPages);
            pageFramesFIFO->back()->row[frameToSwitch] = pageStream[i];
            pageFaultsFIFO++;

        }

        std::shared_ptr<Row> newRow = std::shared_ptr<Row>(new Row(numPages));
        for (int i = 0; i < numPages; i++) {
            newRow->row[i] = pageFramesFIFO->back()->row[i];
        }
        pageFramesFIFO->push_back(newRow);

        displayPageFrame(pageFramesFIFO->back(), pageStream[i], numPages);

    }

    std::cout << "**********************************" << '\n';
    std::cout << "*              LFU               *" << '\n';
    std::cout << "**********************************" << '\n';
    for (int i = 0; i < PAGE_STREAM_SIZE; i++) {

        //only switch pages if the page is not in memory
        if (pageInMemory(pageStream[i], pageFramesLFU->back(), numPages)) {
            displayPageFrame(pageFramesLFU->back(), pageStream[i], numPages);
            if (mapMFU[pageStream[i]] != NULL) {
                mapMFU[pageStream[i]] += 1;
            }
            continue;
        }


        int idx = findEmptySlot(pageFramesLFU->back(), numPages);
        if (idx >= 0) {
            pageFramesLFU->back()->row[idx] = pageStream[i];
            pageFaultsLFU++;
            if (mapMFU[pageStream[i]] != NULL) {
                mapMFU[pageStream[i]] += 1;
            }
            else {
                mapMFU[pageStream[i]] = 1;
            }
        }
        else {

            int frameToSwitch = findLFUPage(mapMFU, pageFramesLFU->back(), numPages);
            pageFramesLFU->back()->row[frameToSwitch] = pageStream[i];
            pageFramesLFU++;

            if (mapMFU[pageStream[i]] != NULL) {
                mapMFU[pageStream[i]] += 1;
            }
            else {
                mapMFU[pageStream[i]] = 1;
            }

        }

        std::shared_ptr<Row> newRow = std::shared_ptr<Row>(new Row(numPages));
        for (int i = 0; i < numPages; i++) {
            newRow->row[i] = pageFramesFIFO->back()->row[i];
        }
        pageFramesFIFO->push_back(newRow);

        displayPageFrame(pageFramesFIFO->back(), pageStream[i], numPages);

    }


    std::cout << "Number of page faults for Belady's optimal: " << pageFaultsBelady << '\n';
    std::cout << "Number of page faults for FIFO: " << pageFaultsFIFO << '\n';
    std::cout << "Number of page faults for LFU: " << pageFaultsLFU << '\n';


    return 0;
}
//finds the furthest page in the incoming page stream and swaps it out
int determineFurthestPage(unsigned int pageStream[], std::shared_ptr<Row>& activePages, int pageStreamStart, int pageStreamLength, int activePagesLength)
{
    int furthestPage = -1;
    int greatestDistance = -1;
    int greatestPageIdx = -1;


    for (int i = 0; i < activePagesLength; i++) {
        unsigned int currentPage = activePages->row[i];
        //starting at the current point in the page stream
        //find the page with the greatest distance from the point
        for (int j = pageStreamStart; j < pageStreamLength; j++) {
            unsigned int currPage = pageStream[j];
            if (currentPage == currPage) {
                int dist = j - pageStreamStart;
                if (dist > greatestDistance) {
                    greatestDistance = dist;
                    greatestPageIdx = i;
                    break;
                }
            }
        }

    }
    return greatestPageIdx;
}

//finds the given page which has been added to the pageFrames first
int findFIFOPage(std::vector<std::shared_ptr<Row>>& pageFrames, int activePagesLength)
{

    int greatestDistance = -1;
    int oldestPageIndex = -1;
    int count = 0;

    std::shared_ptr<Row> top = pageFrames.back();
    for (int i = 0; i < activePagesLength; i++) {
        int currentPage = top->row[i];
        int dist = findPageInProcessed(pageFrames, currentPage, activePagesLength);
        if (dist == greatestDistance) {
            //set to new distance
            greatestDistance = dist;
            oldestPageIndex = i;
        }
        else if (dist > greatestDistance) {
            greatestDistance = dist;
            oldestPageIndex = i;
        }

    }

   
    return oldestPageIndex;
    
}

int findLFUPage(std::map<int, int> m, std::shared_ptr<Row>& row, int rowLength)
{
    int leastUses = INT_MAX;
    int leastUsedIdx = -1;
    for (int i = 0; i < rowLength; i++) {
        int frame = row->row[i];
        if (m[frame] != NULL && m[frame] < leastUses) {
            leastUses = m[frame];
            leastUsedIdx = i;
        }
    }
    return leastUsedIdx;
}


void displayPageFrame(std::shared_ptr<Row>& currRow, int page, int rowLength){

    std::cout << page << "\t\t";
    
    for (int i = 0; i < rowLength; i++) {
        if (currRow->row[i] == NOTHING_IN_FRAME) {
            std::cout << 'x' << "  ";
        }
        else {
            std::cout << currRow->row[i] << "  ";
        }
    }
    std::cout << '\n';

}

int findEmptySlot(std::shared_ptr<Row>& row, int length)
{
    int i = 0;
    for (int i = 0; i < length; i++) {
        if (row->row[i] == NOTHING_IN_FRAME) {
            return i;
        }
    }
    return -1;
    

}

bool pageInMemory(int page , std::shared_ptr<Row>& row, int length)
{

   
    for (int i = 0; i < length; i++) {
        if (row->row[i] == page) {
            return true;
        }
    }
    return false;
}

int findPageInProcessed(std::vector<std::shared_ptr<Row>>& pFrames, int page, int rowSize)
{
    int dist = 0;
    for (int j = rowSize -1; j < 0; j--){
        for (int i = 0; i < rowSize; i++) {
            if (page == pFrames[j]->row[i]) {
                return dist;
            }
        }
        dist++;
    }
    return -1;
}

