#include<stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>

#define DEBUGx

const char * WORD_FILE_NAME = "./data/words.txt";
const char * INPUT_FILE_NAME = "./data/input.txt";


int main(int argc, char* argv[]){

    if(argc != 4){
        fprintf(stdout, "usage : ./palindrome <thread number> <input file> <output file>\n");
        return 1;
    }

    /*
    1. 대칭인지 확인할 단어를 받아온다.
    2. 사전에서 단어를 찾아온다. -> 배열에 받아온다. 그러고 나서  아니 ㅅㅂ 근데 걍 단어 찾는거를 thread로 돌려야하나?
    3. 

    시간이 걸리는 시나리오
    1. 단어 자체를 뒤집는 과정 -> 단어 수개
    2. word 목록에서 단어가 있는지 찾는 과정 (단어 하나에서) -> 몇 만개
    3. input 파일에서 단어를 찾는 과정 -> 몇 만개


    */ 
    // word 파일에서 단어 개수 읽어온다. 
    // input 파일에서 input 개수 읽어오기
    // word 목록 다 읽어오기
    // input 목록 모두 읽기
    int dictionaryWordCount = 0;
    int inputWordCount = 0;

    FILE* dictionaryFp;
    FILE* inputFp;

    char ** dicionaryWords;
    char ** inputWords;

    char buffer[100];

    if (1)
    {
        // 임시로 개수 늘리기
        if ((dictionaryFp = fopen(WORD_FILE_NAME, "r")) == NULL)
        {
            fprintf(stdout, "word file not found!\n");
            return 1;
        }

        while (fscanf(dictionaryFp, "%s", buffer) != EOF)
            dictionaryWordCount += 1;

        fclose(dictionaryFp);

        if ((inputFp = fopen(INPUT_FILE_NAME, "r")) == NULL)
        {
            fprintf(stdout, "word file not found!\n");
            return 1;
        }

        while (fscanf(inputFp, "%s", buffer) != EOF)
            inputWordCount += 1;

        fclose(inputFp);
    }

    dicionaryWords = (char**)malloc(sizeof(char*) * dictionaryWordCount);
    for(int i = 0; i < dictionaryWordCount; i++)
        dicionaryWords[i] = (char*)malloc(sizeof(char) * 100);


    inputWords = (char**)malloc(sizeof(char*) * inputWordCount);
    for(int i = 0; i < inputWordCount; i++)
        inputWords[i] = (char*)malloc(sizeof(char) * 100);

    if (1)
    {
        if ((dictionaryFp = fopen(WORD_FILE_NAME, "r")) == NULL)
        {
            fprintf(stdout, "word file not found!\n");
            return 1;
        }
        for(int i = 0; i < dictionaryWordCount; i++)
            fscanf(dictionaryFp, "%s", dicionaryWords[i]);
        
        fclose(dictionaryFp);

        if ((inputFp = fopen(INPUT_FILE_NAME, "r")) == NULL)
        {
            fprintf(stdout, "word file not found!\n");
            return 1;
        }
        for(int i = 0; i < inputWordCount; i++)
            fscanf(inputFp, "%s", inputWords[i]);
        fclose(inputFp);
    }
    // 파일 불러오는데 성공

    #ifdef DEBUG
    for(int i = 0; i < inputWordCount; i++)
        fprintf(stdout, "%s\n", inputWords[i]);

    fprintf(stdout,"dictionary word count! : %d\n", dictionaryWordCount);
    #endif
    // todo for 문 돌려서 단어 분석

   //#pragma omp parallel for


    return 0;
}