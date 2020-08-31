#include<iostream>
#include<cstdlib>
#include<vector>
#include<memory>

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
void displayPageFrame(std::shared_ptr<Row>& currRow, int page, int rowLength);
int findEmptySlot(std::shared_ptr<Row>& row, int length);
bool pageInMemory(int page, std::shared_ptr<Row>&,  int length);

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
    std::vector<std::shared_ptr<Row>> pageFrames;
    
    //initialize empty row
    std::shared_ptr<Row> r = std::shared_ptr<Row>(new Row(numPages));
    pageFrames.push_back(r);

    //initialize random stream
    for(int i =0; i < PAGE_STREAM_SIZE; i++){
    
       addrs[i] = rand() % 32768;
       int offset = addrs[i] % 4096;
       int pNum = (addrs[i] - offset)/4096;

       pageStream[i] = pNum;
    }
   
    int currentRow = 0;
    int pageFaults = 0;
    for (int i = 0; i < PAGE_STREAM_SIZE; i++) {

        //only switch pages if the page is not in memory
        if (pageInMemory(pageStream[i], pageFrames.back(), numPages)) {
            displayPageFrame(pageFrames.back(), pageStream[i], numPages);

            continue;
        }


        int idx = findEmptySlot(pageFrames.back(), numPages);
        if (idx >= 0) {
            pageFrames.back()->row[idx] = pageStream[i];

        }
        else {
           
            int frameToSwitch = determineFurthestPage(pageStream, pageFrames.back(), i, PAGE_STREAM_SIZE, numPages);
            pageFrames.back()->row[frameToSwitch] = pageStream[i];


        }
      
        std::shared_ptr<Row> newRow = std::shared_ptr<Row>(new Row(numPages));
        for (int i = 0; i < numPages; i++) {
            newRow->row[i] = pageFrames.back()->row[i];
        }
        pageFrames.push_back(newRow);
     
        displayPageFrame(pageFrames.back(), pageStream[i], numPages);
      
    }

    std::cout << "Number of page faults for Belady: " << pageFaults;
    pageFrames.clear();


    return 0;
}

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

void displayPageFrame(std::shared_ptr<Row>& currRow, int page, int rowLength){

    std::cout << page << "  ";
    
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

