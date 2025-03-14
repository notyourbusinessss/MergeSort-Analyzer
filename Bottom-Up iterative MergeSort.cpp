//thats a lot :,( but fun stuff is awaiting
#include <iostream>
#include <ctime>
#include <iomanip>
#include <thread>
#include <vector>
#include <array>
#include <sstream>
#include <string>
#include <fstream>
#include <mutex>
#include <algorithm> // for std::is_sorted 
#include <math.h>

//a value that can be changed accross all threads
std::atomic<int> activeThreads(0);

//tyoe of incriment default 4 can be changed with -I
int INCRIMENT_TYPE = 4;

//default make it 100000 can be changed with -sA
int STRESSTEST = 100000;

// Global atomic flag to signal threads to stop and it need to be atomic since we want it to work accross all threads 
std::atomic<bool> stopFlag(false);

// Global storage for vectors (needed for signal handling) this has to be general values its the only way i found to gently finish the program if ^C is ever pressed
std::vector<std::array<int,2>> itterativePoints, RevitterativePoints, AlmostitterativePoints, SorteditterativePoints,recursivePoints, RevrecursivePoints, AlmostrecursivePoints, SortedrecursivePoints;
std::vector<std::array<int,2>> Linear,nlogn,XX;

double amount = 0.01*STRESSTEST;
void updateAmount(){
    amount = 0.01*STRESSTEST;
}

void change(int &value){
    
    switch (INCRIMENT_TYPE)
    {
    case 1:
        ++value;
        break;
    case 2:
        value = value * 2;
        break;

    case 3:
        value = std::max(value * value,value +1);
        break;

    case 4:
        //one for every x
        value = value + (int)amount;
        break;
    
    default:
        break;
    }
}


//here is going to be our merge for the itterative merge sort
void merge(std::vector<int>& arr, int left, int mid, int right) {
    
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    // Create temporary vectors 
    // for left and right subarrays
    std::vector<int> arr1(n1), arr2(n2);
    
    // Copy data to temporary vectors
    for (int i = 0; i < n1; i++)
        arr1[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        arr2[j] = arr[mid + 1 + j];
    
    int i = 0;    
    int j = 0;    
    int k = left; 
    
    // Merge the temp vectors back into arr
    while (i < n1 && j < n2) {
        if (arr1[i] <= arr2[j]) {
            arr[k] = arr1[i];
            i++;
        } else {
            arr[k] = arr2[j];
            j++;
        }
        k++;
    }
    
    // Copy remaining elements of arr1[] if any
    while (i < n1) {
        arr[k] = arr1[i];
        i++;
        k++;
    }
    
    // Copy remaining elements of arr2[] if any
    while (j < n2) {
        arr[k] = arr2[j];
        j++;
        k++;
    }
}

//this is the itteration mergesort
void mergesort(std::vector<int> &array) {
    
    //here we can add smt like to make it faster 
    if(std::is_sorted(array.begin(), array.end())){
        return;
    }
    //if ever you dont want it just comment it out but it helps lower the 



    //set the size cause i dont want to have to rewrite array.size() each time
    int size = array.size();
    //if the array is lessthen 2 by default its already sorted
    if(size < 2){
        return;
    }
    //
    for(int windowSize = 1 ; windowSize < size; windowSize *=2){
        for(int leftStart = 0 ; leftStart < size -1; leftStart += 2*windowSize){
            int mid = std::min(leftStart+windowSize-1,size-1);
            int rightend = std::min(leftStart + 2*windowSize -1,size-1);

            merge(array,leftStart,mid,rightend);
        }
    }
}
void mergesortRE(std::vector<int>& arr, int left, int right){

    if (left >= right)
        return;

    
    int mid = left + (right - left) / 2;
    mergesortRE(arr, left, mid);
    mergesortRE(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

// so this isnt used since a table with 1000 values is a bit long but its here cause i used it to see if the graph aligned witht what was actual observed
std::string displayTable(const std::vector<std::array<int, 2>>& points) {
    std::ostringstream oss;
    
    oss << "\nPoint Table:\n";
    oss << "+----------+----------+\n";
    oss << "| X (#)    | Y (µs)   |\n";
    oss << "+----------+----------+\n";
    
    for (const auto& point : points) {
        oss << "| " << std::left << std::setw(8) << point[0] 
            << " | " << std::setw(8) << point[1] << " |\n";
    }
    
    oss << "+----------+----------+\n";
    
    return oss.str();
}
int maxi(const std::vector<std::array<int, 2>>& points){
    int max = 0;
    for(int i = 0 ; i <points.size(); i++){
        if(points[i][1] > max){
            max = points[i][1];
        }
    }
    return max;
    
}

std::string displayGraph(const std::vector<std::array<int, 2>>& points) {
    const int width = 100;
    const int height = 50;
    int size = points.size();
    const int arrange = std::max(STRESSTEST / width, 1); // Prevent divide by zero
    const int arrangeheight = std::max((maxi(points)+1) / height, 1) + 1;//i want the y's to be half abput 

    char grid[height][width];

    // Initialize grid with empty spaces
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            grid[i][j] = '.';
        }
    }

    // Place points on the grid
    for (const auto& point : points) {
        int x = point[0];
        int y = point[1];
        if (x >= 0 && x < width * arrange && y >= 0 && y < height * arrangeheight) {
            grid[y / arrangeheight][x / arrange] = 'X';
        }
    }

    std::stringstream ss;
    for (int i = height - 1; i >= 0; --i) { // go from top to bottom
        ss << std::setw(10) << i * arrangeheight << " ┫";
        for (int j = 0; j < width; ++j) {
            ss << grid[i][j] << ' ';
        }
        ss << '\n';
    }
    
    ss << "           ┗"; //need a lot o space eh
    for (int i = 0; i < width; i++) {
        ss << "┳━";
    }
    ss << "\n X"<<std::left<<std::setw(7)<<arrange<<" ->";
    for (int i = 0; i < 10; i++) {
        ss << std::left << std::setw(2) << i;
    }
    ss << " ...to" << width * arrange << "  \n";
    //i also want to know the highest point 
    ss << "The maximum is : "<<maxi(points) << " and the last value is : " << points.back()[1] << std::endl;

    return ss.str();
}
//this is to open a certain file, in this case its the .txt or something similar for the specific os 
void openFile(std::string filename){
    // Open the file based on OS with a larger window size
    #ifdef _WIN32
        system(("start notepad " + filename + " & timeout /t 1 & for /f \"tokens=2 delims==\" %A in ('wmic process where name=\"notepad.exe\" get ProcessId /format:value') do (wmic process where processid=%A call setwindowpos 0,0,1920,540)").c_str());
    #elif __APPLE__
        system(("open -a TextEdit " + filename + " && osascript -e 'tell application \"TextEdit\" to activate' && osascript -e 'tell application \"System Events\" to set size of front window of process \"TextEdit\" to {2500, 540}'").c_str());
    #elif __linux__
        system(("xdg-open " + filename + " & sleep 1 && wmctrl -r :ACTIVE: -e 0,0,0,1920,540").c_str());
    #endif
}
//this is the opposite ! its to close it !!! woaaawwww, very tidious...
void closeFile(std::string filename) {
    #ifdef _WIN32
        // Windows: Find Notepad process with specific file and close it
        system(("powershell -Command \"$p = Get-Process notepad | Where-Object { $_.MainWindowTitle -match '" + filename + "' }; if ($p) { Stop-Process -Id $p.Id -Force }\"").c_str());
    
    #elif __APPLE__
        // macOS: Uses AppleScript to close only the specific file in TextEdit
        system(("osascript -e 'tell application \"TextEdit\" to close (every document whose name contains \"" + filename + "\")'").c_str());

    #elif __linux__
        // Linux: Try closing only the window with the specific filename
        system(("wmctrl -c \"" + filename + "\"").c_str());
        system(("pkill -f \"" + filename + "\"").c_str()); // Alternative if wmctrl fails
    #endif
}
// Function to write the string to a text file, just a basic copy past honestly 
std::string saveToFile(const std::string& filename, const std::string& content) {
    closeFile(filename); //make sure its closed before jsut incase you left itopen so youll be bale to see the results this way
    
    //using the ofstream for files
    std::ofstream file(filename);
    if (file.is_open()) { //oopen the file 
        file << content;
        file.close();
    } else {//if you cant then smt is wrong cause honestlyy it should create the file if it doesnt already exist
        std::cerr << "Error: Could not open file for writing.\n";
    }
    return filename;
}



//this is the stress test, bassycally this is where we record and use the merge sorts
void StressTest(std::vector<std::array<int,2>> &pushInto,std::vector<int> Totest,bool recursive,int generation){
    //start the timer using a high resulution clock since were dealing with microseconds heree
    auto begIT = std::chrono::high_resolution_clock::now();
    if(recursive){
        mergesortRE(Totest,0,generation-1);
    }else{ mergesort(Totest);}
    //end it 
    auto endIT = std::chrono::high_resolution_clock::now();
    //and turn it into an int so we can push it into the vector
    int durationIT = std::chrono::duration_cast<std::chrono::microseconds>(endIT - begIT).count();
    //
    //herrrree
    std::array<int,2> pointIT = {generation,durationIT};
    pushInto.push_back(pointIT);
}

void NormalStressTest(std::vector<std::array<int,2>> &pushInto, int max,bool recursive){
    //this is to keep up with the active detached threads
    activeThreads++;
    // we keep up with the total time the stress testing takes
    auto begTotal = std::chrono::high_resolution_clock::now();
    for(int i = 0 ; i < max && !stopFlag.load() ; change(i)){
        std::vector<int> Totest;
        for(int j = 0 ; j < i ; ++j){
            Totest.push_back(rand()%i);
        }
        StressTest(pushInto,Totest,recursive,i);
    }
    auto endTotal = std::chrono::high_resolution_clock::now();
    int durationTotal= std::chrono::duration_cast<std::chrono::microseconds>(endTotal - begTotal).count();
    std::cout << "stressTest " <<(recursive? "Recursive" : "Iterrative")<<" Done in " << durationTotal << " | last took : " << pushInto.back()[1] << " for " << pushInto.back()[0] << " integers \n";
    activeThreads--;//when done 
}


//this is the same just we put inverse order into the array's
void stressTestRev(std::vector<std::array<int,2>> &pushInto, int max,bool recursive){
    activeThreads++;
    auto begTotal = std::chrono::high_resolution_clock::now();
    for(int i = 2 ; i < max && !stopFlag.load() ; change(i)){
        std::vector<int> Totest;
        for(int j = i ;  j > 0 ; --j){

            Totest.push_back(j);
        }
        StressTest(pushInto,Totest,recursive,i);
        
    }
    auto endTotal = std::chrono::high_resolution_clock::now();
    int durationTotal= std::chrono::duration_cast<std::chrono::microseconds>(endTotal - begTotal).count();
    //tell that its done and also show how long it took
    std::cout << "stressTestRev " <<(recursive? "Recursive" : "Iterrative")<<" Done in " << durationTotal << " | last took : " << pushInto.back()[1] << " for " << pushInto.back()[0] << " integers \n";
    activeThreads--;
}
//this one was a fun one
void stressTestAlmostSorted(std::vector<std::array<int,2>> &pushInto, int max,bool recursive){
    activeThreads++;
    auto begTotal = std::chrono::high_resolution_clock::now();
    //because you had to 
    for(int i = 2 ; i < max && !stopFlag.load(); change(i)){
        std::vector<int> Totest;
        //make a sorted list
        for(int j = 0 ;  j < i ; ++j){

            Totest.push_back(j);
        }
        //and then make it almost sorted by changing some placement of stuff, i meaaannnn 
        for(int j = 0 ; j < std::min(20,j/10);++j){
            int indexI = rand()%i;
            int indexJ = rand()%i;
            std::swap(Totest[indexI],Totest[indexJ]);
        }
        //and now test it 
        StressTest(pushInto,Totest,recursive,i);
    }
    //end timer 
    auto endTotal = std::chrono::high_resolution_clock::now();
    int durationTotal= std::chrono::duration_cast<std::chrono::microseconds>(endTotal - begTotal).count();
    //tell that its done and also show how long it took
    std::cout << "stressTestAlmostSorted " <<(recursive? "Recursive" : "Iterrative")<<" done in " << durationTotal << " | last took : " << pushInto.back()[1] << " for " << pushInto.back()[0] << " integers \n";
    activeThreads--;
}
//just make it sorted and test it
void stressTestSorted(std::vector<std::array<int,2>> &pushInto, int max,bool recursive){
    activeThreads++;
    auto begTotal = std::chrono::high_resolution_clock::now();
    for(int i = 2 ; i < max && !stopFlag.load(); change(i)){
        std::vector<int> Totest;
        for(int j = 0 ;  j < i ; ++j){

            Totest.push_back(j);
        }
        StressTest(pushInto,Totest,recursive,i);
    }
    auto endTotal = std::chrono::high_resolution_clock::now();
    int durationTotal= std::chrono::duration_cast<std::chrono::microseconds>(endTotal - begTotal).count();
    std::cout << "stressTestSorted " <<(recursive? "Recursive" : "Iterrative" ) <<" in " << durationTotal << " | last took : " << pushInto.back()[1] << " for " << pushInto.back()[0] << " integers \n";
    activeThreads--;
}

// Function to clean up and exit
void cleanupAndExit(int signum) {
    if(stopFlag.load()){
        exit(signum);
    }
    // Signal threads to stop
    stopFlag.store(true);
    std::cout << "\n\n^C detected. Cleaning up before exit, if you want to forcefully quit press ^C again or ^Z\n\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); //wait to make sure everything is indeed stopped
}
//this is for the argument so that if at place i its -p then the one after is going 
bool print(int argc, char* argv[],bool& ShowResults,int i){
    std::string input = argv[i];
    if(input == "-p"){
        input = argv[i+1];
        for(int index :input){
            input[index] = tolower(input[index]);
        }
        if(input =="false" || input =="f" ){
            ShowResults = false;
            std::cout << "The program will not open the .txt windows to show the results \n";
            
        }else if(input =="true" || input =="t"){
            ShowResults = true;
            std::cout << "The program will open the windows to show the results \n";
            
        }else{
            throw std::invalid_argument("-p can only be true or false, or t or f");
        }
        return true;
    }
    return false;
}
//this is for the argument so that if at place i its -sA then the next should be the value for STRESSAMOUNT
bool stressAmount(int argc, char* argv[],int i){
    std::string input = argv[i];
    if(input == "-sA"){
        int dude =atoi(argv[i+1]);
        STRESSTEST = dude;
        //update amount
        updateAmount();
        return true;
    }
    return false;
}
//this is to find if at position i the argument is -I and if it is the next one is the incriment type 
bool IncrimentType(int argc, char* argv[],int i){
    std::string input = argv[i];
    if(input == "-I"){
        int dude =atoi(argv[i+1]);
        if(dude < 5 && dude > 0){
            INCRIMENT_TYPE = dude;
            return true;
        }else if(dude == 0 || dude>4){
        
        stopFlag.store(true);
        throw std::invalid_argument("increment type must be between 1 and 4");
        exit(SIGINT);
    }
    }
    return false;
}
//this is just to print the help for the arguments you can put in 
void printHelp(){
    std::cout << " The program comes with 3 possible argument \n";
    std::cout << " \t-I Tells which increment type to use, Default is 4 \n \t\t type 1 : ++value\n \t\t type 2 : value = value * 2\n \t\t type 3 : value = value * value (Not very useful) \n \t\t type 4 : changes in order to always have only 100 points \n";
    std::cout << " \t-sA is used to change the maximum stress value or the largest array the program will have to sort, default is 100000.\n\t\t be careful : when changing the increment type you should also change this accordingly\n";
    std::cout << " \t-p is used to set if the programs opens the .txt windows or not after it is done running, default is true\n\n ";
}

int main(int argc, char* argv[]){
    //in case ^C is pressed make sure to terminate properly 
    signal(SIGINT, cleanupAndExit);

    //show the result is set on default to true but can be changed using -p
    bool ShowResults = true;
    //-help can only be after the ./program since it wont make it run instead itll give you information
    if(argc > 1 ){
        std::string line = argv[1];
        if(line == "-help"){
            printHelp();
            stopFlag.store(true);
            return 0;
        }
        int token = argv[1][0] - 48;
        if(token > 0 && token < 5){
            INCRIMENT_TYPE = token;
            std::cout << "Increment type : "<<INCRIMENT_TYPE << " chosen. \n";
        }
        if(argc >= 1){
            for(int i = 1 ; i < argc ; i++){
                if(print(argc, argv,ShowResults,i)){
                    //i++;
                    //continue;
                }
                if(stressAmount(argc, argv,i)){
                    //i++;
                    //continue;
                }
                if(IncrimentType(argc, argv,i)){
                    //i++;
                    //continue;
                }
            }
        }
    }
    
    int testUpTo = STRESSTEST;
    
    srand(time(NULL));
    
    for(int i = 0 ; i < STRESSTEST ; ++i){
        Linear.push_back({i,i});
        nlogn.push_back({i,(int)(i*log2(i))});
        XX.push_back({i,i*i});
    }

    std::vector<int> arrA ;
    std::vector<int> arrB ;
    for(int i = 0 ; i < 8 ; i ++){
        arrA.push_back(rand()%10);
        arrB.push_back(rand()%10);
    }

    std::cout << "\n\nproof that both mergesort works : ";
    std::cout << "initial arrays : \n\t Itterative : ";
    for(auto tok : arrA){
        std::cout << tok <<",";
    }
    std::cout << "\b\n\t Recursive  : ";
    for(auto tok : arrB){
        std::cout << tok <<",";
    }
    std::cout << "\b\n";
    mergesort(arrA);
    mergesortRE(arrB,0,arrB.size()-1);
    std::cout << "Sorted arrays : \n\t Itterative : ";
    for(auto tok : arrA){
        std::cout << tok <<",";
    }
    std::cout << "\b\n\t Recursive  : ";
    for(auto tok : arrB){
        std::cout << tok <<",";
    }
    std::cout << "\b\n\n";

    //if smt goes wrong or you want to stop listen for ^C to handle it
    std::cout << "Threads currently running independently... \n\n";

    


    //initialize the threads so that it doesnt have to wait for each of them to do stuff.
    std::thread stressAlmostItterative (stressTestAlmostSorted,std::ref(AlmostitterativePoints),testUpTo,false);
    std::thread stressAlmostRecursive (stressTestAlmostSorted,std::ref(AlmostrecursivePoints),testUpTo,true);
    //
    std::thread stressSortedItterative (stressTestSorted,std::ref(SorteditterativePoints),testUpTo,false);
    std::thread stressSortedRecursive (stressTestSorted,std::ref(SortedrecursivePoints),testUpTo,true);
    /**/
    
    //
    std::thread stressItterative (NormalStressTest,std::ref(itterativePoints),testUpTo,false);
    std::thread stressRecursive (NormalStressTest,std::ref(recursivePoints),testUpTo,true);
    //    
    
    std::thread stressRevItterative (stressTestRev,std::ref(RevitterativePoints),testUpTo,false);
    std::thread stressRevRecursive (stressTestRev,std::ref(RevrecursivePoints),testUpTo,true);
    //

    
    stressSortedItterative.detach();
    stressSortedRecursive.detach();
    
    
    stressAlmostItterative.detach();
    stressAlmostRecursive.detach();

    stressItterative.detach();
    stressRecursive.detach();
    
    stressRevItterative.detach();
    stressRevRecursive.detach();




    while(activeThreads != 0){

    }

    if(ShowResults){
        openFile( saveToFile("itterator.txt", /**/( INCRIMENT_TYPE == 4 ? displayTable(itterativePoints) + "Normal \n" : "") + displayGraph(itterativePoints) + "\nReversed Order \n" + displayGraph(RevitterativePoints) + "\nAlmost sorted \n" + displayGraph(AlmostitterativePoints) + "\nSorted \n" + displayGraph(SorteditterativePoints) +"\nexample of linear graph\n" + displayGraph(Linear)+"n\example of n*log(n) graph\n" + displayGraph(nlogn)+"\nn^2\n" + displayGraph(XX)/**/ ));
        openFile( saveToFile("recursive.txt", /**/( INCRIMENT_TYPE == 4 ? displayTable(recursivePoints)  + "Normal \n" : "") + displayGraph(recursivePoints) + "\nReversed Order \n" + displayGraph(RevrecursivePoints) + "\nAlmost sorted \n" + displayGraph(AlmostrecursivePoints)  + "\nSorted\n" +displayGraph(SortedrecursivePoints) +"\nexample of linear graph\n" + displayGraph(Linear)+"\nexample of n*log(n) graph\n" + displayGraph(nlogn)/**/));
    }
}