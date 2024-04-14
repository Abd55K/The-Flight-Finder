#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

//=======================//
// Implemented Functions //
//=======================//
template<int MAX_SIZE>
int HashTable<MAX_SIZE>::PRIMES[3] = {102523, 100907, 104659};

template<int MAX_SIZE>
void HashTable<MAX_SIZE>::PrintLine(int tableIndex) const
{
    const HashData& data = table[tableIndex];

    // Using printf here it is easier to format
    if(data.sentinel == SENTINEL_MARK)
    {
        printf("[%03d]         : SENTINEL\n", tableIndex);
    }
    else if(data.sentinel == EMPTY_MARK)
    {
        printf("[%03d]         : EMPTY\n", tableIndex);
    }
    else
    {
        printf("[%03d] - [%03d] : ", tableIndex, data.lruCounter);
        printf("(%-5s) ", data.isCostWeighted ? "True" : "False");
        size_t sz = data.intArray.size();
        for(size_t i = 0; i < sz; i++)
        {
            if(i % 2 == 0)
                printf("[%03d]", data.intArray[i]);
            else
                printf("/%03d/", data.intArray[i]);

            if(i != sz - 1)
                printf("-->");
        }
        printf("\n");
    }
}

template<int MAX_SIZE>
void HashTable<MAX_SIZE>::PrintTable() const
{
    printf("____________________\n");
    printf("Elements %d\n", elementCount);
    printf("[IDX] - [LRU] | DATA\n");
    printf("____________________\n");
    for(int i = 0; i < MAX_SIZE; i++)
    {
        PrintLine(i);
    }
}

//=======================//
//          TODO         //
//=======================//
template<int MAX_SIZE>
int HashTable<MAX_SIZE>::Hash(int startInt, int endInt, bool isCostWeighted)
{
    /* TODO */
    int cost;
    
    if(isCostWeighted) cost=1;
    else cost=0;
    
    return PRIMES[0]*startInt+PRIMES[1]*endInt+PRIMES[2]*cost;
}

template<int MAX_SIZE>
HashTable<MAX_SIZE>::HashTable()
{
    /* TODO */
    elementCount = 0;
    
    for(int i=0;i<MAX_SIZE;i++){
        table[i].lruCounter = 0;
        table[i].sentinel = EMPTY_MARK;
    }
}

template<int MAX_SIZE>
int HashTable<MAX_SIZE>::Insert(const std::vector<int>& intArray, bool isCostWeighted)
{
    /* TODO */
    if(intArray.size()<1) throw InvalidTableArgException();
    
    int result=0, h = Hash(intArray[0], intArray[intArray.size()-1], isCostWeighted) % MAX_SIZE, q, i;
    
    for(q = h, i=0; table[q].sentinel == OCCUPIED_MARK; ++i ,q = (h + i*i) % MAX_SIZE){
        if(table[q].startInt == intArray[0] && table[q].endInt == intArray[intArray.size()-1] && table[q].isCostWeighted == isCostWeighted){
            result=table[q].lruCounter;
            table[q].lruCounter++;
            return result;
        }
    }
    
    if(elementCount > MAX_SIZE/2) throw TableCapFullException(elementCount);
    
    elementCount++;
    table[q].startInt = intArray[0];
    table[q].endInt = intArray[intArray.size()-1];
    table[q].lruCounter++;
    
    for(i=0;i<intArray.size();i++) table[q].intArray.push_back(intArray[i]);
    
    table[q].isCostWeighted = isCostWeighted;
    table[q].sentinel = OCCUPIED_MARK;
    
    return result;
}

template<int MAX_SIZE>
bool HashTable<MAX_SIZE>::Find(std::vector<int>& intArray,
                               int startInt, int endInt, bool isCostWeighted,
                               bool incLRU)
{
    /* TODO */
    int h = Hash(startInt, endInt, isCostWeighted) % MAX_SIZE;
    
    for(int q = h, i=0; table[q].sentinel != EMPTY_MARK; ++i ,q = (h + i*i) % MAX_SIZE){
        if(table[q].startInt == startInt && table[q].endInt == endInt && table[q].isCostWeighted == isCostWeighted && table[q].sentinel != SENTINEL_MARK){
            if(incLRU) table[q].lruCounter++;
            for(int j = 0; j < table[q].intArray.size(); j++) intArray.push_back(table[q].intArray[j]);
            return true;
        }
    }
    
    return false;
}

template<int MAX_SIZE>
void HashTable<MAX_SIZE>::InvalidateTable()
{
    /* TODO */
    for(int i = 0; i < MAX_SIZE; i++){
        table[i].sentinel = EMPTY_MARK;
        table[i].lruCounter = 0;
    }
    
    elementCount = 0;
}

template<int MAX_SIZE>
void HashTable<MAX_SIZE>::GetMostInserted(std::vector<int>& intArray) const
{
    /* TODO */
    int theIndex, maxCount=0, i;
    
    for(i = 0; i < MAX_SIZE; i++){
        if(table[i].sentinel != OCCUPIED_MARK) continue;
        
        if(table[i].lruCounter > maxCount){
            maxCount = table[i].lruCounter;
            theIndex = i;
        }
    }
    
    intArray.clear();
    for(i=0;i<table[theIndex].intArray.size();i++) intArray.push_back(table[theIndex].intArray[i]);
}

template<int MAX_SIZE>
void HashTable<MAX_SIZE>::Remove(std::vector<int>& intArray,
                                 int startInt, int endInt, bool isCostWeighted)
{
    /* TODO */
    int h = Hash(startInt, endInt, isCostWeighted) % MAX_SIZE;
    
    for(int q = h, i=0; table[q].sentinel != EMPTY_MARK; ++i ,q = (h + i*i) % MAX_SIZE){
        if(table[q].sentinel == SENTINEL_MARK) continue;
        
        if(table[q].startInt == startInt && table[q].endInt == endInt && table[q].isCostWeighted == isCostWeighted){
            table[q].sentinel = SENTINEL_MARK;
            for(int j = 0; j < table[q].intArray.size(); j++) intArray.push_back(table[q].intArray[j]);
            elementCount--;
            return;
        }
    }
}

template<int MAX_SIZE>
void HashTable<MAX_SIZE>::RemoveLRU(int lruElementCount)
{
    /* TODO */
    int i;
    MinPairHeap<int, int> Heap;
    
    for(i=0;i<MAX_SIZE;i++){
        if(table[i].sentinel == OCCUPIED_MARK){
            Heap.push(Pair<int, int> {table[i].lruCounter, i});
        }
    }
    
    while(lruElementCount){
        i=Heap.top().value;
        
        table[i].sentinel = SENTINEL_MARK;
        
        Heap.pop();
        elementCount--;
        lruElementCount--;
    }
}

template<int MAX_SIZE>
void HashTable<MAX_SIZE>::PrintSortedLRUEntries() const
{
    /* TODO */
    int i;
    MaxPairHeap<int, int> Heap;
    
    for(i=0;i<MAX_SIZE;i++){
        if(table[i].sentinel == OCCUPIED_MARK){
            Heap.push(Pair<int, int> {table[i].lruCounter, i});
        }
    }
    
    while(!Heap.empty()){
        i=Heap.top().value;
        
        PrintLine(i);
        
        Heap.pop();
    }
}

#endif // HASH_TABLE_HPP