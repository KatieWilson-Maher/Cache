#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include<stdlib.h>
#include<stdbool.h>

// struct to represent a value and a unit (ex. for RAM, 4G)
struct size{
	int value;
	char unit[3];
};

// Places tag in cache
void placeInCache(int start, int end, int tag, int * cache, int mapType, int setNum){
	int cacheLine;
	if(mapType == 1){
		cache[setNum] = tag;
	}
	if(mapType == 2 || mapType == 3){
		// formula from Geeks4Geeks "Generating
		// Random Numbers in a range in C"
		//num = (rand()% (upper-lower + 1)) + lower
		cacheLine = (rand()% (end-start+1))+start;
	}
	cache[cacheLine] = tag;
}

// Checks for tag between given lines
bool checkLines(int tag, int * cache, int start, int end){
	bool inLines = false;
	for(int i = start; i<end; i++){
		if(cache[i] == tag){
			inLines = true;
		}
	}
	return inLines;
}

// Determines a cache hit or cache miss
bool isHit(int tag, int setNum, int mapType, int k, int * cache, int cacheLen){
	if(mapType == 1){
		return cache[setNum] == tag;
	}
	if(mapType == 2){
		return checkLines(tag, cache, 0, cacheLen);
	}
	if(mapType == 3){
		return checkLines(tag, cache, (setNum*k), ((setNum+1)*k));
	}
}

// Calculates the number of line in the cache
int linesInCache(int cachePower, int blockPower){
	int lineAmt = cachePower-blockPower;
	return lineAmt;
}

// Gets tag and set number for a given address
int * getTagSetNum(unsigned int address, int r, int w){
	printf("%u\n",address);
	int blockNum = address >> w;
	printf("%u\n", blockNum);
	int setNum = blockNum & (int)((pow(2, r))-1);
	int tag = blockNum >> r;
	unsigned int *tagSetNum=(unsigned int *) malloc(2*sizeof(int));
	tagSetNum[0] = tag;
	tagSetNum[1] = setNum;
	return tagSetNum;
}

// Prints an array
void printArray(int *array, int size){
	printf("{");
	for(int i=0; i<size-1; i++){
		printf(" %d, ", array[i]);
	}
	printf(" %d", array[size-1]);
	printf("}");
}

// Modified geeksforgeeks Program to find whether a no is a power of 2,
// returns that power
double powerOfTwo(int num){
	double ceiling1 = ceil(log2(num));
	double floor1 = floor(log2(num));
	if(num != 0 && ceiling1 == floor1){
		return ceiling1;
	}
}

// Takes units and converts to 2 to the power of something in bytes
// ex. M -> 2^10
double unitToBytes(char unit[]){

	if ((strcasecmp(unit, "k") == 0) || (strcasecmp(unit, "kb") == 0)){
		return 10;
	}
	else if((strcasecmp(unit, "m") == 0) || (strcasecmp(unit, "mb") == 0)){
		return 20;
	}
	else if((strcasecmp(unit, "g") == 0) || (strcasecmp(unit, "gb") == 0)){
		return 30;
	}
	else if((strcasecmp(unit, "b") == 0)){
		return 0;
	}
	//TO DO: add more units
}

// Gets what power of 2 a number is(ex. 64 -> 2^6 -> returns 6)
int bytePower(struct size memorySize){

	return (int)powerOfTwo(memorySize.value) + (int) unitToBytes(memorySize.unit);
}

// Formats the address as {tagLength, rLength, wLength}
int* addressFormat(int addrLen, int cacheLen, int blockLen, int mapping, int k){
		int *addressFormat;
		printArray(addressFormat, 3);
		addressFormat[2] = blockLen;
		int cacheLine = cacheLen - blockLen;
		if(mapping == 1){
			addressFormat[1] = cacheLine ;
		}
		else if (mapping ==2){
			addressFormat[1] = 0;
		}
		else{
			addressFormat[1]= (cacheLine) - (log2(k));	
		}
		addressFormat[0] = addrLen - blockLen - addressFormat[1];
		return addressFormat;
}

int main(){
	//get RAM size from user and calculate address length
	struct size ramSize;
	printf("Enter a RAM unit: ");
	scanf("%s", ramSize.unit);
	printf("Enter a RAM size: ");
	scanf("%d", &ramSize.value);
	int addrLen;
	addrLen = bytePower(ramSize);
	
	//get cache size from user and calculate to what power of 2 the
	//cache is in bytes
	struct size cacheSize;
	printf("Enter a cache unit: ");
	scanf("%s", cacheSize.unit);
	printf("Enter a cache size: ");
	scanf("%d", &cacheSize.value);
	int cachePower;
	cachePower = bytePower(cacheSize);
	
	//get block size from user and calculate w
	struct size blockSize;
	printf("Enter a block unit: ");
	scanf("%s", blockSize.unit);
	printf("Enter a block size: ");
	scanf("%d", &blockSize.value);
	int w;
	w = bytePower(blockSize);
	
	// Get map type from user
	int mapType;
	printf("\nDirect - 1\nAssociative - 2\nSet Associative - 3\n");
	printf("Enter a type of mapping: ");
	scanf("%d", &mapType);
	
	// Get address format
	int *addrFormat;	
	int k = 0;
	if(mapType == 3){
		printf("k: ");
		scanf("%d", &k);
	}
	int lineAmt = pow(2,(linesInCache(cachePower, w)));
	// fails here:
	addressFormat(addrLen, lineAmt, blockSize.value, mapType, k);
	
	// Create array of tags to represent cache, make each line -1.
	int tags[lineAmt];
	int tagsSize = (sizeof(tags))/(sizeof(tags[0]));
	for(int i = 0; i<tagsSize; i++){
		tags[i] = -1;
	}
	
	// Get address from user
	double ramSpots = pow(2, addrLen);
	int address;
	printf("\nGive an address between 0 and %f: ", ramSpots);
	scanf("%d", &address);
	
	// Get tag and set number
	unsigned int *tagSetNum = getTagSetNum(address, addrFormat[1], w); 
	int tag = tagSetNum[0];
	int setNum = tagSetNum[1];
	
	// Check for cache hit or miss
	bool isHit = isHit(tag, setNum, mapType, k, tags, lineAmt);
	
	// If mapping type is direct, check for cache hit or miss.
	// If cache miss, place tag of address on proper line.
	if(mapType == 1){
		if(isHit == false){
			placeInCache(0, 0, tag, tags, mapType, setNum);
		}
	}
	
	// If mapping type is associative, check for cache hit or miss.
	// If cache miss, find an empty line to store the tag of the
	// address.
	// If no lines are empty, replace a random cache line with the
	// tag of the address.
	if(mapType == 2){
		if(isHit == false){
			bool lineFound = false;
			i = 0;
			while((i<length) && (lineFound == false)){
				if(cache[i] == -1){
					lineFound == true;
				}
				i = i+1;
			}
			else{
				i = i+1;
			}
		}
		if(lineFound == true){
			cache[i] = tag;
		}
		else{
			placeInCache(0, lineAmt, tag, tags, mapType, setNum);
		}
	}
	// If mapping type is set associative, check for cache hit or miss.
	// If cache miss, find an empty line within the set to store the 		// tag of the address.
	// If no lines are empty, replace a random cache line in the 
	// proper set with the tag of the address.
	if(mapType == 3){
		if (isHit == false){
			int start = (setNum*k);
			int end = start + k;
			i = start;
			lineFound = false;
			while((i<end) && (lineFound == false)){
				if(tags[i] == -1){
					lineFound = true;
				}
				else{
					i = i+1;
				}
			}	
			if(lineFound == true){
				tags[i] = tag;
			}
			else{
				placeInCache(start, end, tag, tags, mapType, setNum);
			}
		}
	}
}




