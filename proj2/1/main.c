#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#include <stdbool.h>

#define DEBUGx
#define MAX_SIZE 100
const char *WORD_FILE_NAME = "./data/words.txt";

int main(int argc, char *argv[])
{

    if (argc != 4)
    {
        fprintf(stdout, "usage : ./hw1 <thread number> <input file> <output file>\n");
        return 1;
    }

    char file_directory1[128] = "./data/";
    char file_directory2[128] = "./data/";

    int thread_number = atoi(argv[1]);
    const char *INPUT_FILE_NAME = strcat(file_directory1 ,argv[2]);
    const char *OUTPUT_FILE_NAME = strcat(file_directory2 ,argv[3]);

    if (!thread_number)
    {
        fprintf(stdout, "thread number must be integer!\n");
        return -1;
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

    FILE *dictionaryFp;
    FILE *inputFp;
    FILE *outputFp;

    char **dicionaryWords;
    char **inputWords;
    bool *result;
    double start; 
    double end; 
    

    char buffer[MAX_SIZE];

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
            fprintf(stdout, "input file not found!\n");
            return 1;
        }

        while (fscanf(inputFp, "%s", buffer) != EOF)
            inputWordCount += 1;

        fclose(inputFp);
    }

    dicionaryWords = (char **)malloc(sizeof(char *) * dictionaryWordCount);
    for (int i = 0; i < dictionaryWordCount; i++)
        dicionaryWords[i] = (char *)malloc(sizeof(char) * MAX_SIZE);

    inputWords = (char **)malloc(sizeof(char *) * inputWordCount);
    for (int i = 0; i < inputWordCount; i++)
        inputWords[i] = (char *)malloc(sizeof(char) * MAX_SIZE);

    result = (bool *)malloc(sizeof(bool) * inputWordCount);

    if (1)
    {
        if ((dictionaryFp = fopen(WORD_FILE_NAME, "r")) == NULL)
        {
            fprintf(stdout, "word file not found!\n");
            return 1;
        }
        for (int i = 0; i < dictionaryWordCount; i++)
            fscanf(dictionaryFp, "%s", dicionaryWords[i]);

        fclose(dictionaryFp);

        if ((inputFp = fopen(INPUT_FILE_NAME, "r")) == NULL)
        {
            fprintf(stdout, "word file not found!\n");
            return 1;
        }
        for (int i = 0; i < inputWordCount; i++)
            fscanf(inputFp, "%s", inputWords[i]);
        fclose(inputFp);
    }
    // 파일 불러오는데 성공

#ifdef DEBUG
    for (int i = 0; i < inputWordCount; i++)
        fprintf(stdout, "%s\n", inputWords[i]);

    fprintf(stdout, "dictionary word count! : %d\n", dictionaryWordCount);
#endif
    
    // thread 개수 설정
    start = omp_get_wtime();
    
    #pragma omp parallel for num_threads(thread_number)
    for (int i = 0; i < inputWordCount; i++)
    {
        char buffer[MAX_SIZE];
        int count = strlen(inputWords[i]);
        strncpy(buffer, inputWords[i], 100);

        for (int j = 0; j < count / 2; j++)
        {
            char temp = buffer[j];
            buffer[j] = buffer[count - j - 1];
            buffer[count - j - 1] = temp;
        }
        buffer[count] = 0; // null 문자 처리

        // 그 자체로 팰린드롬이면 통과
        //fprintf(stdout, "%s\n", buffer);
        if (strncmp(inputWords[i], buffer, MAX_SIZE) == 0)
        {
            result[i] = true;
            continue;
        }
        int flag = 0;
        for (int j = 0; j < dictionaryWordCount; j++)
        {
            if (strncmp(buffer, dicionaryWords[j], MAX_SIZE) == 0)
            {
                flag = 1;
                break;
            }
        }
        // 사전에 존재하면 true
        if (flag)
            result[i] = true;
    }
    end = omp_get_wtime();

    if ((outputFp = fopen(OUTPUT_FILE_NAME, "w")) == NULL)
    {
        fprintf(stdout, "ouput file not found!\n");
        return 1;
    }
    //fprintf(stdout ,"%f %f\n", start, end);
    fprintf(stdout, "elapsed time %f seconds\n", end - start);

    for (int i = 0; i < inputWordCount; i++)
    {
        if (result[i])
            fprintf(outputFp, "YES!\n");
        else
            fprintf(outputFp, "NO!\n");
    }
    
    
    
    
    for (int i = 0; i < dictionaryWordCount; i++)
        free(dicionaryWords[i]);
    free(dicionaryWords);
    
    for (int i = 0; i < inputWordCount; i++)
        free(inputWords[i]);
    free(inputWords);

    free(result);
    
    fclose(outputFp);

    return 0;
}