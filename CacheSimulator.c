#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

typedef struct lineStruct{
  int turns;//turns since use...necessary for LRU
  int tag;
  char valid;//1 if valid, 0 otherwise. C "version" of a bool
} line;

long s,e,b;
FILE *traces = NULL;

void getArgs(int argc, char** argv){
  //get option flags etc
  extern char *optarg;
  int getop;
  while ((getop= getopt (argc, argv, "s:E:b:t:v")) != -1)//source: GNU foundation getopt package
  {
    switch (getop)
      {
      case 's':
        s = strtol(optarg,NULL, 10);
      case 'E':
        e = strtol(optarg,NULL,10);
      case 'b':
        b = strtol(optarg,NULL,10);
      case 't':
        traces=fopen(optarg,"r");
      }
  }

}
int main(int argc, char** argv)
{
  //***Initially necessary definitions
  int i;
  s=e=b=0;//as defined by convention

  getArgs(argc, argv);
  //***Now that we have s,E,b, we can initialize everything for our simulation

  int blockMask = 1;
  for(i=1; i<b; i++){
    blockMask |= blockMask <<1;
  }
  int setMask=blockMask+1;
  for(i=1; i<s; i++){
    setMask |= setMask <<1;
  }

  line* lines = malloc(sizeof(line)*(1<<s)*e);//all lines will be in the same array
  for(i=0;i<(1<<s)*e;i++){//each set is e lines long
    lines[i] = (line){ 0,0,0};
  }

  unsigned long long int address =0;//account for large addressses
  unsigned int size=0;//turns out to be irrelevant
  int misses=0,hits=0,evictions=0,setBits=0,tagBits=0,MRU=0,LRi=0;
  char* inp = malloc(sizeof(char)*5);
  char found,allValid;//use as bools

  //***Begin iterating
  while(fscanf(traces,"%s %llx,%x",inp,&address,&size)>0){
    if(inp[0]=='I'){ //ignore instruction loads
      continue;
    }

    setBits = (address&setMask) >> b;
    tagBits = (address & ~(setMask|blockMask)) >> (s+b);
    i=0;
    allValid=1;
    found=0;

    while( (!found) && i<e ){ //e*setBits = beginning of set # setBits in array
      if(lines[(e*setBits)+i].valid  ){
        if(lines[(e*setBits)+i].tag==tagBits){
          hits++;
          found = 1; //True
          MRU=(e*setBits)+i; //most recently used...all others get turns++
        }
      }
      else{
        allValid=0;
      }
      i++;
    }
    //handle situations
    if(allValid && !found){//evict LRU
      LRi=e*setBits;//Least Recent index

      for(i=0;i<e;i++){
        if(lines[(e*setBits)+i].turns>lines[LRi].turns){
          LRi=(e*setBits)+i; //find which has not been used in most turns
        }
      }
      //eviction below
      lines[LRi]=(line){0,tagBits,1};
      MRU=LRi;
      evictions++;
      misses++;
    }
    if(!allValid && !found){//write to the empty line which exists
      i=0;
      while(i<e){
        if(!lines[(e*setBits)+i].valid){
          lines[(e*setBits)+i]=(line){0,tagBits,1};
          misses++;
          MRU=(e*setBits)+i;
          break;
        }
        i++;
      }
    }
    if(inp[0]=='M'){ //each modify is a normal operation,then a guaranteed hit
      hits++;
    }

    //conclude by updating turn counts
    for(i=0;i<(1<<s)*e;i++){
      if(i==MRU){
        lines[i].turns=0;
      }
      else{
        lines[i].turns++;
      }
    }
  }

  //conclude
  printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);
  return 0;
}
