#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

//////////////////////////////////////////////////////////
//														//
//						Grupo 11						//
//														//
//				Rodrigo Monteiro - 73701				//
//				Guilherme Ferreira - 69316				//
//														//
//////////////////////////////////////////////////////////

#define NFILES 5
#define NRUNIGRAMAS 200
#define NRBIGRAMAS 5000
#define NRTRIGRAMAS 10000
#define SIZEOFSENTENCE 750 
#define LOG_PROB 1
#define LOG_PROB_TESTS 0

typedef struct{
	
	int nUnigramas;
    int nBigramas;
    int nTrigramas;

    char **unigramas;
    char **bigramas;
    char **trigramas;

    int *unigramasCount;
    int *bigramasCount;
    int *trigramasCount;

    double *unigramasProb;
    double *bigramasProb;
    double *trigramasProb;
    
//////////////////////////////////////////////////////
//	 probabilities smoothed using add1-smoothing	//
//////////////////////////////////////////////////////
    
    double *bigramasProbSmooth;
    double *trigramasProbSmooth;

} fileCounts;

int DEBUG;
int CREATE_TEST_FILES;
int CLEAN;
int OUTPUT;
int SMOOTH;

void init();
void normalize();
void readFromFiles();

void countNGrams();
int existe(char *elemento, char **estrutura, int size);
void writeCountToFiles();

void calcProbs();
void writeProbsToFiles();

void processSentence();
int min(int currentFile, int *estruturaCount, int size);
void returnOutput();

void clean();
void writeIOTestFiles();

char **filenames;
char **filenamesNOR;

int *numberofSentences;
int **sizeofSentences;
int *sizeofFiles;
char **fileVector;
char ***sentencesVector;

char *sentence;
char *output;
char *sentenceFilename;
char *sentenceFilenameNOR;
char *inputVector;
int inputSize;

double result[NFILES];

// Contagens de unigramas bigramas e trigramas

fileCounts **counts;

int main(int argc, char *argv[]) {

	DEBUG = 0;
	CREATE_TEST_FILES = 0;
	CLEAN = 0;

	if(argc > 1) {
		
		if(!strcmp(argv[1], "--clean")){
				CLEAN = 1;
				clean();
				return 0;
		}

		if(DEBUG) printf("RUNNING...\n\n");
		if(DEBUG) printf("ATTEMPTING TO PROCESS FILE: %s\n", argv[1]);

		sentence = (char*) malloc(SIZEOFSENTENCE * sizeof(char));
		output = (char*) malloc(SIZEOFSENTENCE * sizeof(char));

		strncpy(sentence, argv[1], SIZEOFSENTENCE);

		FILE *file = fopen(sentence, "r");

		if (file == NULL) {
			printf("Requested file does not exist! Exiting ...\n");
			return 0;
		}

		else if(DEBUG) printf("SUCCESSFULLY OPENED FILE: %s\n\n", argv[1]);

		if(argc > 2){
			if(!strcmp(argv[2], "--clean")){
				CLEAN = 1;
				clean();
				return 0;
			}

			else{
				SMOOTH = 1;

				if(!strcmp(argv[2], "--debug"))  DEBUG = 1;

				else if(!strcmp(argv[2], "--files")) CREATE_TEST_FILES = 1;

				else{
					strncpy(output, argv[2], SIZEOFSENTENCE);
					OUTPUT = 1;
				}

				if(argc > 3){
					if(!strcmp(argv[3], "--debug")) DEBUG = 1;
					if(!strcmp(argv[3], "--files")) CREATE_TEST_FILES = 1;
					if(!strcmp(argv[3], "--nosmooth")) SMOOTH = 0;
				}

				if(argc > 4){
					if(!strcmp(argv[4], "--debug")) DEBUG = 1;
					if(!strcmp(argv[4], "--files")) CREATE_TEST_FILES = 1;
					if(!strcmp(argv[4], "--nosmooth")) SMOOTH = 0;
				}
			}
		}

		if(!CLEAN){
			init();
			normalize();
			readFromFiles();
			writeIOTestFiles();
			countNGrams();
			writeCountToFiles();
			calcProbs();
			writeProbsToFiles();
			processSentence();
			returnOutput();

			return 0;
		}

		if(CLEAN) clean();
	}

	else{

		if(DEBUG) printf("RUNNING WITH NO ARGUMENTS...\n\n");
		init();
		normalize();
		readFromFiles();
		writeIOTestFiles();
		countNGrams();
		writeCountToFiles();
		calcProbs();
		writeProbsToFiles();
	}

	return 0;
}

void init(){
	
	int i, j;
	counts = (fileCounts**) malloc(sizeof(fileCounts*) * NFILES);

	for(i = 0; i < NFILES; i++){
		counts[i] = (fileCounts*) malloc(sizeof(fileCounts));

		counts[i]->nUnigramas = 0;
		counts[i]->nBigramas = 0;
		counts[i]->nTrigramas = 0;

		counts[i]->unigramas = (char**) malloc(sizeof(char*) * NRUNIGRAMAS);
    	counts[i]->bigramas = (char**) malloc(sizeof(char*) * NRBIGRAMAS);
    	counts[i]->trigramas = (char**) malloc(sizeof(char*) * NRTRIGRAMAS);

    	counts[i]->unigramasCount = (int*) malloc(sizeof(int) * NRUNIGRAMAS);
    	counts[i]->bigramasCount = (int*) malloc(sizeof(int) * NRBIGRAMAS);
	    counts[i]->trigramasCount = (int*) malloc(sizeof(int) * NRTRIGRAMAS);

	    counts[i]->unigramasProb = (double*) malloc(sizeof(double) * NRUNIGRAMAS);
    	counts[i]->bigramasProb = (double*) malloc(sizeof(double) * NRBIGRAMAS);
	    counts[i]->trigramasProb = (double*) malloc(sizeof(double) * NRTRIGRAMAS);
	    
    	counts[i]->bigramasProbSmooth = (double*) malloc(sizeof(double) * NRBIGRAMAS);
	    counts[i]->trigramasProbSmooth = (double*) malloc(sizeof(double) * NRTRIGRAMAS);
		
		for(j = 0; j < NRUNIGRAMAS; j++){
			counts[i]->unigramas[j] = (char*) malloc(5 * sizeof(char));
			counts[i]->unigramasCount[j] = 0;
			counts[i]->unigramasProb[j] = 0.0;
			memset(counts[i]->unigramas[j], '\0', 5 * sizeof(char));
		}

		j = 0;

		for(j = 0; j < NRBIGRAMAS; j++){
			counts[i]->bigramas[j] = (char*) malloc(5 * sizeof(char));
			counts[i]->bigramasCount[j] = 0;
			counts[i]->bigramasProb[j] = 0.0;
			counts[i]->bigramasProbSmooth[j] = 0.0;
			memset(counts[i]->bigramas[j], '\0', 5 * sizeof(char));
		}

		j = 0;

		for(j = 0; j < NRTRIGRAMAS; j++){
			counts[i]->trigramas[j] = (char*) malloc(5 * sizeof(char));
			counts[i]->trigramasCount[j] = 0;
			counts[i]->trigramasProb[j] = 0.0;
			counts[i]->trigramasProbSmooth[j] = 0.0;
			memset(counts[i]->trigramas[j], '\0', 5 * sizeof(char));
		}
	}
}

void normalize(){

    char command[100];
    int i;

    filenames = (char **) malloc(NFILES * sizeof(char*));
    filenamesNOR = (char **) malloc(NFILES * sizeof(char*));
    
    for(i = 0; i < NFILES; i++){
		filenames[i] = (char*) malloc(20 * sizeof(char));
		filenamesNOR[i] = (char*) malloc(20 * sizeof(char));
    }

    sprintf(command, "rm -r ../corpora/normalized/");
    system(command);
    strcpy(command, "");
    sprintf(command, "mkdir ../corpora/normalized/");
    system(command);
    strcpy(command, "");
    if(DEBUG) printf("Removed normalized directory and files.\n\n");	
    

    for(i = 1; i < NFILES + 1; i++){
		sprintf(filenames[i-1], "lingua%d.txt", i); 
		sprintf(filenamesNOR[i-1], "lingua%dNOR.txt", i);
		sprintf(command, "sh normalizer.sh ../corpora/%s ../corpora/normalized/%s", filenames[i-1], filenamesNOR[i-1]);
		if(DEBUG) printf("Normalized Corpus lingua%d.txt. Generated lingua%dNOR.txt.\n", i, i);
		system(command);
		strcpy(command, "");	
    }    
    
    if(DEBUG) printf("\nFinished Normalizing Corpora.\n\n");
}

void readFromFiles(){

    char command[100];
    char ch;
    char previousChar;
    int lastSentence = 0;
    int i, j = 0;
    int fileVectorPos = 0;
    int currentFile;

    if(DEBUG) printf("Began reading from files at 'corpora/normalized/'\n\n");	

    char *filename = (char*) malloc(sizeof(char) * 100);

    FILE **openedFiles = (FILE **) malloc(sizeof(FILE*) * NFILES);

    fileVector = (char**) malloc(NFILES * sizeof(char*));
    sizeofFiles = (int*) malloc(NFILES * sizeof(int));

    sentencesVector = (char ***) malloc(NFILES * sizeof(char**));
    sizeofSentences = (int**) malloc(NFILES * sizeof(int*));
    numberofSentences = (int*) malloc(NFILES * sizeof(int));

    for(currentFile = 0; currentFile < NFILES; currentFile++){ 

    	numberofSentences[currentFile] = 0;

    	strcpy(filename, "../corpora/normalized/");
    	strcat(filename, filenamesNOR[currentFile]);
    
	    openedFiles[currentFile] = fopen(filename, "r");
	    
	    if (openedFiles[currentFile] == NULL) { 
	      printf("Erro na abertura do ficheiro %s\n", filename);
	    }

	    else if(DEBUG) { printf("Opened file %s\n", filename); }

	    // Calcula tamanho do ficheiro de input 

	    fseek(openedFiles[currentFile], 0L, SEEK_END);
	    size_t filesize = ftell(openedFiles[currentFile]);
	    fseek(openedFiles[currentFile], 0L, SEEK_SET);

	    if(DEBUG) printf("Current File size: %d\n", filesize);
	    
	    // Aloca vector unidimensional para receber dados do ficheiro 

	    fileVector[currentFile] = (char *) malloc((filesize+1) * sizeof(char));
	    memset(fileVector[currentFile], '\0', (filesize+1) * sizeof(char));
	    if(DEBUG) printf("Malloc and Memset successfull for fileVector[%d]!\n", currentFile);
	    
	    // Lê do ficheiro para o vector 

	    fileVectorPos = 0;
	    ch = '.';
	    previousChar = '.';

	    while(ch != EOF){
	    	ch = fgetc(openedFiles[currentFile]);

		    fileVector[currentFile][fileVectorPos] = ch;
		    fileVectorPos ++;
		      
		    if((ch == '<') && (previousChar == '<')){
		    	numberofSentences[currentFile] ++;
	      	}
	      	previousChar = ch;
	    }

	    sizeofFiles[currentFile] = fileVectorPos;

	    // DEBUG : prints the normalized text copied from file to vector

	    if(DEBUG) printf("\nNumber of Sentences on Current File: %d\n\n", numberofSentences[currentFile]);

	    // Aloca matriz de frases (tamanho numberofSentences) 

	    sizeofSentences[currentFile] = (int *) malloc((numberofSentences[currentFile] * 1.5) * sizeof(int));
	    sentencesVector[currentFile] = (char **) malloc((numberofSentences[currentFile] * 1.5) * sizeof(char*));
	    
	    // Detecta e segmenta frases 

	    int beggining = 0;
	    int end = 1;
	    int stop = 0;
	    int lastSentenceWritten = 0;

	    for(i = 0; i < fileVectorPos-1; i++){
	    	if(end < fileVectorPos-1){

	    		if((fileVector[currentFile][beggining] != '<') | (fileVector[currentFile][beggining+1] != '<')){
	    			printf("ERROR: Sentence doesn't begin with a < symbol!\n");
	    			break;
	    		}

				if(fileVector[currentFile][i] == '>'){
					end = i;
		    		stop = 1;
		    	}

		    	if(i == fileVectorPos - 2){
		    		end = i;
		    		stop = 1;
		    	}

		    	if(stop == 1){
		    		sizeofSentences[currentFile][lastSentenceWritten] = (end-beggining + 2);
		    		sentencesVector[currentFile][lastSentenceWritten] = (char *) malloc((end-beggining + 3) * sizeof(char));
		    		memset(sentencesVector[currentFile][lastSentenceWritten], '\0', (end-beggining + 3) * sizeof(char));

		    		strncpy(sentencesVector[currentFile][lastSentenceWritten], fileVector[currentFile] + beggining, (end - beggining + 2));

		    		lastSentenceWritten ++;

		    		beggining = end + 2;
		    		end = beggining;
		    	}

		    	else end ++;
		    	stop = 0;
	    	}
	    }

	    numberofSentences[currentFile] = lastSentenceWritten;
	    lastSentenceWritten = 0;
	   	
		fclose(openedFiles[currentFile]);
		if(DEBUG) printf("Read %s.\n", filenamesNOR[currentFile]);
 	}

 	if(DEBUG) printf("\nFinished reading from files.\n\n");	
}

void countNGrams(){

	if(DEBUG) printf("Began Counting N-Grams.\n");
	
	int currentFile = 0;
	int currentSentence = 0;
	int currentChar = 0;
	int secondChar = 1;
	int thirdChar = 2;

	char *unigrama = (char*) malloc(5 * sizeof(char));
	char *bigrama = (char*) malloc(5 * sizeof(char));
	char *trigrama = (char*) malloc(5 * sizeof(char));

	memset(unigrama, '\0', 5 * sizeof(char));
	memset(bigrama, '\0', 5 * sizeof(char));
	memset(trigrama, '\0', 5 * sizeof(char));

	for(currentFile = 0; currentFile < NFILES; currentFile++){
		for(currentSentence = 0; currentSentence < numberofSentences[currentFile]; currentSentence++){
			for(currentChar = 0; currentChar < sizeofSentences[currentFile][currentSentence]-1; currentChar++, secondChar++, thirdChar++){

				memset(unigrama, '\0', 5 * sizeof(char));
				strncpy(unigrama, sentencesVector[currentFile][currentSentence] + currentChar, 1);

				int pos;
				pos = existe(unigrama, counts[currentFile]->unigramas, counts[currentFile]->nUnigramas);

				if(pos == -1){
					if(counts[currentFile]->nUnigramas == NRUNIGRAMAS){
						printf("Numero de Unigramas limite atingido no ficheiro %d\n", currentFile);
					}

					else{
						strncpy(counts[currentFile]->unigramas[counts[currentFile]->nUnigramas], unigrama, 1);
						counts[currentFile]->unigramasCount[counts[currentFile]->nUnigramas] ++;
						counts[currentFile]->nUnigramas ++;
					}
				}

				else{
					counts[currentFile]->unigramasCount[pos] ++;
				}
				
				int posSecond;

				memset(bigrama, '\0', 5 * sizeof(char));
				strncpy(bigrama, sentencesVector[currentFile][currentSentence] + currentChar, 2);

				posSecond = existe(bigrama, counts[currentFile]->bigramas, counts[currentFile]->nBigramas);

				if(posSecond == -1){
					if(counts[currentFile]->nBigramas == NRBIGRAMAS - 1){
						printf("Numero de Bigramas limite atingido no ficheiro %d\n", currentFile);
					}

					else{
						strncpy(counts[currentFile]->bigramas[counts[currentFile]->nBigramas], bigrama, 2);
						counts[currentFile]->bigramasCount[counts[currentFile]->nBigramas] ++;
						counts[currentFile]->nBigramas ++;
					}
				}

				else{
					counts[currentFile]->bigramasCount[posSecond] ++;
				}

				int posThird;

				memset(trigrama, '\0', 5 * sizeof(char));
				strncpy(trigrama, sentencesVector[currentFile][currentSentence] + currentChar, 3);

				posThird = existe(trigrama, counts[currentFile]->trigramas, counts[currentFile]->nTrigramas);

				if(posThird == -1){
					if(counts[currentFile]->nTrigramas == NRTRIGRAMAS - 1){
						printf("Numero de Trigramas limite atingido no ficheiro %d\n", currentFile);
					}

					else{
						strncpy(counts[currentFile]->trigramas[counts[currentFile]->nTrigramas], trigrama, 3);
						counts[currentFile]->trigramasCount[counts[currentFile]->nTrigramas] ++;
						counts[currentFile]->nTrigramas ++;
					}
				}

				else{
					counts[currentFile]->trigramasCount[posThird] ++;
				}
			}
		}
	}

	if(DEBUG) printf("Finished Counting N-Grams.\n\n");
}

int existe(char *elemento, char **estrutura, int size){

	int existe = -1; //-1 -> não existe
	int currentPos;

	for(currentPos = 0; currentPos <= size; currentPos++){
		if(!strcmp(estrutura[currentPos], elemento)){
			existe = currentPos;
			break;
		}
	}
	return existe;
}

void writeCountToFiles(){

	int currentFile = 0;
	int currentSentence = 0;
	int currentChar = 0;
	char command[100];
	char filename[100];

	char *filenameUni = (char*) malloc(sizeof(char) * 200);
	char *filenameBi = (char*) malloc(sizeof(char) * 200);
	char *filenameTri = (char*) malloc(sizeof(char) * 200);
	char *uni = (char*) malloc(sizeof(char) * 200);
	char *bi = (char*) malloc(sizeof(char) * 200);
	char *tri = (char*) malloc(sizeof(char) * 200);

	memset(filenameUni, '\0', 200 * sizeof(char));
	memset(filenameBi, '\0', 200 * sizeof(char));
	memset(filenameTri, '\0', 200 * sizeof(char));
	memset(uni, '\0', 200 * sizeof(char));
	memset(bi, '\0', 200 * sizeof(char));
	memset(tri, '\0', 200 * sizeof(char));

	char *linha = (char*) malloc(sizeof(char) * 200);
	memset(linha, '\0', 200 * sizeof(char));

	sprintf(command, "rm -r ../corpora/counts/");
    system(command);
    sprintf(command, "mkdir ../corpora/counts/");
    system(command);

    if(DEBUG) printf("Began Writing Count Files at '/corpora/count/'\n\n");

    FILE *fileUni = (FILE*) malloc(sizeof(FILE));
    FILE *fileBi = (FILE*) malloc(sizeof(FILE));
    FILE *fileTri = (FILE*) malloc(sizeof(FILE));
    
	for(currentFile = 0; currentFile < NFILES; currentFile++){ 

		fileUni = (FILE*) realloc(fileUni, sizeof(FILE));		
		fileBi = (FILE*) realloc(fileBi, sizeof(FILE));
		fileTri = (FILE*) realloc(fileTri, sizeof(FILE));

		int i;

		strcpy(filenameUni, "../corpora/counts/");
		strcpy(uni, ".unigrama");
    	strcat(filenameUni, filenames[currentFile]);
    	strcat(filenameUni, uni);

    	fileUni = fopen(filenameUni, "w+"); 

    	for(i = 0; i < counts[currentFile]->nUnigramas; i++){
	        sprintf(linha,"%-25s %-10d\n", counts[currentFile]->unigramas[i], counts[currentFile]->unigramasCount[i]);
	        fputs(linha, fileUni);
    	}

    	memset(linha, '\0', 200 * sizeof(char));

    	fclose(fileUni);
    	fileUni = NULL;

    	if(DEBUG) printf("Generated Count File: %s\n", filenameUni);

		strcpy(filenameBi, "../corpora/counts/");
		strcpy(bi, ".bigrama");
    	strcat(filenameBi, filenames[currentFile]);
    	strcat(filenameBi, bi);

    	fileBi = fopen(filenameBi, "w+"); 

		for(i = 0; i < counts[currentFile]->nBigramas; i++){
	        sprintf(linha,"%-25s %-10d\n", counts[currentFile]->bigramas[i], counts[currentFile]->bigramasCount[i]);
	        fputs(linha, fileBi);
    	}

    	memset(linha, '\0', 200 * sizeof(char));

    	fclose(fileBi);
    	fileBi = NULL;

    	if(DEBUG) printf("Generated Count File: %s\n", filenameBi);

		strcpy(filenameTri, "../corpora/counts/");
		strcpy(tri, ".trigrama");
    	strcat(filenameTri, filenames[currentFile]);
    	strcat(filenameTri, tri);

    	fileTri = fopen(filenameTri, "w+"); 
    	if(DEBUG) printf("Generated Count File: %s\n", filenameTri);

    	for(i = 0; i < counts[currentFile]->nTrigramas; i++){
	        sprintf(linha,"%-25s %-10d\n", counts[currentFile]->trigramas[i], counts[currentFile]->trigramasCount[i]);
	        fputs(linha, fileTri);
    	}

    	memset(linha, '\0', 200 * sizeof(char));

    	fclose(fileTri);
    	fileTri = NULL;
    }

    if(DEBUG) printf("\nFinished Writing Count Files.\n\n");
}

void calcProbs(){

	if(DEBUG) printf("Began Calculating Probabilities.\n");

	int V[NFILES];
	int currentFile;
	int currentUnigram;
	int currentBigram;
	int currentTrigram;

	char* unigramStr = (char*) malloc(sizeof(char) * 5);
	memset(unigramStr, '\0', 5 * sizeof(char));

	char* bigramStr = (char*) malloc(sizeof(char) * 5);
	memset(bigramStr, '\0', 5 * sizeof(char));

	int existeUni;
	int existeBi;

	for(currentFile = 0; currentFile < NFILES; currentFile++){
		V[currentFile] = 0;

		for(currentUnigram = 0; currentUnigram < counts[currentFile]->nUnigramas; currentUnigram ++){
			V[currentFile] += counts[currentFile]->unigramasCount[currentUnigram];
		}

		currentUnigram = 0;

		for(currentUnigram = 0; currentUnigram < counts[currentFile]->nUnigramas; currentUnigram ++){
			if(LOG_PROB) counts[currentFile]->unigramasProb[currentUnigram] = exp((log((double)counts[currentFile]->unigramasCount[currentUnigram]) - log((double) V[currentFile])));
			else counts[currentFile]->unigramasProb[currentUnigram] = (double)counts[currentFile]->unigramasCount[currentUnigram] /(double)V[currentFile];
		}

		for(currentBigram = 0; currentBigram < counts[currentFile]->nBigramas; currentBigram ++){
			memset(unigramStr, '\0', 5 * sizeof(char));
			strncpy(unigramStr, counts[currentFile]->bigramas[currentBigram], 1);
			
			existeUni = existe(unigramStr, counts[currentFile]->unigramas, counts[currentFile]->nUnigramas);

			if(LOG_PROB) {
				counts[currentFile]->bigramasProb[currentBigram] = exp((log((double)counts[currentFile]->bigramasCount[currentBigram]) - log((double)counts[currentFile]->unigramasCount[existeUni])));
				if(SMOOTH) counts[currentFile]->bigramasProbSmooth[currentBigram] = exp((log((double)counts[currentFile]->bigramasCount[currentBigram] + 1) - log((double)counts[currentFile]->unigramasCount[existeUni] + V[currentFile])));
			}
			else {
				counts[currentFile]->bigramasProb[currentBigram] = (double)counts[currentFile]->bigramasCount[currentBigram] / (double)counts[currentFile]->unigramasCount[existeUni];
				counts[currentFile]->bigramasProbSmooth[currentBigram] = (double)(counts[currentFile]->bigramasCount[currentBigram] + 1) / (double)(counts[currentFile]->unigramasCount[existeUni] + V[currentFile]);
				
			}
		}	
		
		for(currentTrigram = 0; currentTrigram < counts[currentFile]->nTrigramas; currentTrigram ++){
			memset(bigramStr, '\0', 5 * sizeof(char));
			strncpy(bigramStr, counts[currentFile]->trigramas[currentTrigram], 2);
			
			existeBi = existe(bigramStr, counts[currentFile]->bigramas, counts[currentFile]->nBigramas);

			if(LOG_PROB) {
				counts[currentFile]->trigramasProb[currentTrigram] = exp((log((double) counts[currentFile]->trigramasCount[currentTrigram]) - log((double)counts[currentFile]->bigramasCount[existeBi])));
				if(SMOOTH) counts[currentFile]->trigramasProbSmooth[currentTrigram] = exp((log((double) counts[currentFile]->trigramasCount[currentTrigram] + 1) - log((double)counts[currentFile]->bigramasCount[existeBi] + V[currentFile])));
			}
			else {
				counts[currentFile]->trigramasProb[currentTrigram] = (double)counts[currentFile]->trigramasCount[currentTrigram] / (double)counts[currentFile]->bigramasCount[existeBi];
				if(SMOOTH) counts[currentFile]->trigramasProbSmooth[currentTrigram] = (double)(counts[currentFile]->trigramasCount[currentTrigram] + 1) / (double)(counts[currentFile]->bigramasCount[existeBi] + V[currentFile]);
			}
		}
	}

	if(DEBUG) printf("Finished Calculating Probabilities.\n\n");
}

void writeProbsToFiles(){

	int currentFile = 0;
	int currentSentence = 0;
	int currentChar = 0;
	char command[100];
	char filename[100];

	char *filenameUni = (char*) malloc(sizeof(char) * 200);
	char *filenameBi = (char*) malloc(sizeof(char) * 200);
	char *filenameTri = (char*) malloc(sizeof(char) * 200);
	
	char *filenameBiSmooth = (char*) malloc(sizeof(char) * 200);
	char *filenameTriSmooth = (char*) malloc(sizeof(char) * 200);
	
	char *uni = (char*) malloc(sizeof(char) * 200);
	char *bi = (char*) malloc(sizeof(char) * 200);
	char *tri = (char*) malloc(sizeof(char) * 200);
	
	char *biSmooth = (char*) malloc(sizeof(char) * 200);
	char *triSmooth = (char*) malloc(sizeof(char) * 200);

	memset(filenameUni, '\0', 200 * sizeof(char));
	memset(filenameBi, '\0', 200 * sizeof(char));
	memset(filenameTri, '\0', 200 * sizeof(char));
	
	memset(filenameBiSmooth, '\0', 200 * sizeof(char));
	memset(filenameTriSmooth, '\0', 200 * sizeof(char));
	
	memset(uni, '\0', 200 * sizeof(char));
	memset(bi, '\0', 200 * sizeof(char));
	memset(tri, '\0', 200 * sizeof(char));
	
	memset(biSmooth, '\0', 200 * sizeof(char));
	memset(triSmooth, '\0', 200 * sizeof(char));

	char *linha = (char*) malloc(sizeof(char) * 200);
	memset(linha, '\0', 200 * sizeof(char));

	sprintf(command, "rm -r ../corpora/probs/");
    system(command);
    sprintf(command, "mkdir ../corpora/probs/");
    system(command);
    
    if(SMOOTH){
    	sprintf(command, "mkdir ../corpora/probs/smoothed/");
    	system(command);
    }

    if(DEBUG) printf("Began Writing Probs Files at '/corpora/probs/'\n\n");

    FILE *fileUni = (FILE*) malloc(sizeof(FILE));
    FILE *fileBi = (FILE*) malloc(sizeof(FILE));
    FILE *fileTri = (FILE*) malloc(sizeof(FILE));
    
    FILE *fileBiSmooth = (FILE*) malloc(sizeof(FILE));
    FILE *fileTriSmooth = (FILE*) malloc(sizeof(FILE));

    char unigramaStr[50];
    char bigramaStr[50];
    char trigramaStr[50];
    char probabilidadeStr[50];

    strcpy(unigramaStr, "unigrama");
    strcpy(bigramaStr, "bigrama");
    strcpy(trigramaStr, "trigrama");
    strcpy(probabilidadeStr, "log-probabilidade");
    
	for(currentFile = 0; currentFile < NFILES; currentFile++){ 

		fileUni = (FILE*) realloc(fileUni, sizeof(FILE));		
		fileBi = (FILE*) realloc(fileBi, sizeof(FILE));
		fileTri = (FILE*) realloc(fileTri, sizeof(FILE));
				
		fileBiSmooth = (FILE*) realloc(fileBi, sizeof(FILE));
		fileTriSmooth = (FILE*) realloc(fileTri, sizeof(FILE));

		int i;

		strcpy(filenameUni, "../corpora/probs/");
		strcpy(uni, "probs_unigramas_");
		strcat(filenameUni, uni);
    	strcat(filenameUni, filenames[currentFile]);

    	fileUni = fopen(filenameUni, "w+"); 

    	sprintf(linha,"%-25s %-25s\n", unigramaStr, probabilidadeStr);
	    fputs(linha, fileUni);

    	for(i = 0; i < counts[currentFile]->nUnigramas; i++){
	        sprintf(linha,"%-25s %2.12f\n", counts[currentFile]->unigramas[i], counts[currentFile]->unigramasProb[i]);
	        fputs(linha, fileUni);
    	}

    	memset(linha, '\0', 200 * sizeof(char));

    	fclose(fileUni);
    	fileUni = NULL;

    	if(DEBUG) printf("Generated Probs File: %s\n", filenameUni);

		strcpy(filenameBi, "../corpora/probs/");
		strcpy(bi, "probs_bigramas_");
		strcat(filenameBi, bi);
    	strcat(filenameBi, filenames[currentFile]);
    	
    	strcpy(filenameBiSmooth, "../corpora/probs/smoothed/");
		strcpy(biSmooth, "probs_bigramas_smoothed_");
		strcat(filenameBiSmooth, biSmooth);
    	strcat(filenameBiSmooth, filenames[currentFile]);

    	fileBi = fopen(filenameBi, "w+"); 
    	if(SMOOTH) fileBiSmooth = fopen(filenameBiSmooth, "w+"); 

    	sprintf(linha,"%-25s %-25s\n", bigramaStr, probabilidadeStr);
	    fputs(linha, fileBi);
	    if(SMOOTH) fputs(linha, fileBiSmooth);

		for(i = 0; i < counts[currentFile]->nBigramas; i++){
	        sprintf(linha,"%-25s %2.12f\n", counts[currentFile]->bigramas[i], counts[currentFile]->bigramasProb[i]);
	        fputs(linha, fileBi);
	        
	        memset(linha, '\0', 200 * sizeof(char));
	        
	        if(SMOOTH){
	        	sprintf(linha,"%-25s %2.12f\n", counts[currentFile]->bigramas[i], counts[currentFile]->bigramasProbSmooth[i]);
	        	fputs(linha, fileBiSmooth);
	        }
    	}

    	memset(linha, '\0', 200 * sizeof(char));
    	if(SMOOTH) fclose(fileBiSmooth);
    	fclose(fileBi);
    	fileBi = NULL;

    	if(DEBUG) printf("Generated Probs File: %s\n", filenameBi);
    	if(DEBUG && SMOOTH) printf("Generated Smoothed Probs File: %s\n", filenameBiSmooth);

		strcpy(filenameTri, "../corpora/probs/");
		strcpy(tri, "probs_trigramas_");
		strcat(filenameTri, tri);
    	strcat(filenameTri, filenames[currentFile]);
    	
    	strcpy(filenameTriSmooth, "../corpora/probs/smoothed/");
		strcpy(triSmooth, "probs_trigramas_smoothed_");
		strcat(filenameTriSmooth, triSmooth);
    	strcat(filenameTriSmooth, filenames[currentFile]);

    	fileTri = fopen(filenameTri, "w+"); 
    	if(SMOOTH) fileTriSmooth = fopen(filenameTriSmooth, "w+"); 

    	sprintf(linha,"%-25s %-25s\n", trigramaStr, probabilidadeStr);
	    fputs(linha, fileTri);
	    if(SMOOTH) fputs(linha, fileTriSmooth);

    	for(i = 0; i < counts[currentFile]->nTrigramas; i++){
	        sprintf(linha,"%-25s %2.12f\n", counts[currentFile]->trigramas[i], counts[currentFile]->trigramasProb[i]);
	        fputs(linha, fileTri);
	        
	        memset(linha, '\0', 200 * sizeof(char));
	        
	        if(SMOOTH){
	        	sprintf(linha,"%-25s %2.12f\n", counts[currentFile]->trigramas[i], counts[currentFile]->trigramasProbSmooth[i]);
	        	fputs(linha, fileTriSmooth);
	        }
    	}

    	memset(linha, '\0', 200 * sizeof(char));
    	
    	if(DEBUG) printf("Generated Probs File: %s\n", filenameTri);
    	if(DEBUG) printf("Generated Smoothed Probs File: %s\n", filenameTriSmooth);
		
		if(SMOOTH) fclose(fileTriSmooth);
    	fclose(fileTri);
    	fileTri = NULL;
    }

    if(DEBUG) ("\nFinished Writing Probs Files.\n\n");
}

void processSentence(){
	
	if(DEBUG) printf("Began Pre-Processing Input Sentence.\n");

	int sentencePos, inputVectorPos, currentFile, currentChar;
	char ch = '.';
	char command[100];
	
	FILE *sentenceFile;
	sentenceFile = (FILE*) malloc(sizeof(FILE));
	
	inputVector = (char*) malloc(SIZEOFSENTENCE * sizeof(char));
	memset(inputVector, '\0', SIZEOFSENTENCE * sizeof(char));

	sentenceFilenameNOR = (char*) malloc(sizeof(char) * 100);

	strcpy(sentenceFilenameNOR, sentence);
	strcat(sentenceFilenameNOR, "_normalized.txt");

	sprintf(command, "sh normalizer.sh %s %s", sentence, sentenceFilenameNOR);
	system(command);

	if(DEBUG) printf("Normalized Input Sentence '%s'\n", sentence);
	
	sentenceFile = fopen(sentenceFilenameNOR, "r");

	if(sentenceFile == NULL){
		printf("ERROR: CAN'T ACCESS NORMALIZED SENTENCE FILE!\n");
		exit(0);
	}

	inputVectorPos = 0;
	
	while(ch != EOF){
		ch = fgetc(sentenceFile);

		inputVector[inputVectorPos] = ch;
		inputVectorPos ++;
	}

	inputSize = inputVectorPos;
	
	fclose(sentenceFile);

	if(DEBUG) printf("Finished Pre-Processing Input Sentence.\n\n");

	// Cálculos de probabilidades

	char *currentBigram = (char*) malloc(5 * sizeof(char));
	memset(currentBigram, '\0', 5 * sizeof(char));
	char *currentTrigram = (char*) malloc(5 * sizeof(char));
	memset(currentTrigram, '\0', 5 * sizeof(char));

	int existeBigrama;
	int existeTrigrama;
	double countUni = 1.0;
	double countBi = 1.0;
	double countTri = 1.0;
	double countPrevious = 0.0;
	double temp = 0.0;

	for(currentFile = 0; currentFile < NFILES; currentFile++){
		
		result[currentFile] = 0.0;

		for(currentChar = 0; currentChar < inputSize - 3; currentChar ++){
			
			memset(currentBigram, '\0', 5 * sizeof(char));
			strncpy(currentBigram, inputVector + currentChar, 2);
			memset(currentTrigram, '\0', 5 * sizeof(char));
			strncpy(currentTrigram, inputVector + currentChar, 3);

			countUni = 0.0;
			countBi = 0.0;
			countTri = 0.0;

			temp = 0.0;

			existeBigrama = existe(currentBigram, counts[currentFile]->bigramas, counts[currentFile]->nBigramas);
			existeTrigrama = existe(currentTrigram, counts[currentFile]->trigramas, counts[currentFile]->nTrigramas);

			if(existeBigrama == - 1) countBi = (double)1.0;
			else countBi = (double)counts[currentFile]->bigramasCount[existeBigrama];

			if(existeTrigrama == - 1) countTri = (double)1.0;
			else countTri = (double)counts[currentFile]->trigramasCount[existeTrigrama];

			if(LOG_PROB_TESTS){ // p1 x p2 x p3 = exp(log(p1) + log(p2) + log(p3))
				temp = (double) log((double) exp(log(countTri) - log(countBi)));
				if (result[currentFile] == 0) result[currentFile] = (double)temp;
				else result[currentFile] += (double) temp;
			}

			else{
				temp = (double) ((double)countTri / (double)countBi);
				if (result[currentFile] == 0) result[currentFile] = temp;
				else result[currentFile] *= (double) temp;
			}
		}
		
			currentChar = 0;
			countUni = 0.0;
			countBi = 0.0;
			countTri = 0.0;

			temp = 0.0;
	}
}

void returnOutput(){

	double total = 0.0;
	int currentFile;
	char *filename = (char*) malloc(50* sizeof(char));
	char *command = (char*) malloc(50* sizeof(char));
	char *linha = (char*) malloc(200* sizeof(char));
	char *lingua = (char*) malloc(50* sizeof(char));

	char *linguaStr = (char*) malloc(50* sizeof(char));
	char *probabilidadeStr = (char*) malloc(50* sizeof(char));

	for(currentFile = 0; currentFile < NFILES; currentFile++){
		if(LOG_PROB_TESTS) total += (double) exp(result[currentFile]);
		else total += (double) result[currentFile];
	}

	for(currentFile = 0; currentFile < NFILES; currentFile++){
		if(DEBUG && LOG_PROB_TESTS) printf("total: %2.10f | exp[currentFile]: %2.10f\n", total, exp(result[currentFile]));
		result[currentFile] /= (double) total;
		result[currentFile] *= (double) 100.0; // percentagem
		if(DEBUG) printf("Probabidade estimada da frase estar na Lingua %d: %2.10f%%\n", currentFile+1, result[currentFile]);
	}

	if(DEBUG) printf("\n");

	/*sprintf(command, "rm -r ../results/");
    system(command);
    sprintf(command, "mkdir ../results/");
    system(command);*/

	FILE *file;
	file = (FILE*) malloc(sizeof(FILE));

	if(!OUTPUT) strcpy(filename, "../results/Resultado.txt");
	else strcpy(filename, output);

	file = fopen(filename, "w+");

	strcpy(linguaStr, "Lingua");
	strcpy(probabilidadeStr, "Probabilidade");

	sprintf(linha,"%-30s %-30s\n", linguaStr, probabilidadeStr);
	fputs(linha, file);

	char linguaName[NFILES][20];
	strcpy(linguaName[0], "Portugues");
	strcpy(linguaName[1], "Alemao");
	strcpy(linguaName[2], "Frances");
	strcpy(linguaName[3], "Italiano");
	strcpy(linguaName[4], "Ingles");

	for(currentFile = 0; currentFile < NFILES; currentFile++){
	    memset(linha, '\0', 200 * sizeof(char));
	    memset(lingua, '\0', 50 * sizeof(char));
		sprintf(lingua, "%d - %s:", currentFile + 1, linguaName[currentFile]);
	    sprintf(linha,"%-30s %3.10f%%\n", lingua, result[currentFile]);
	    fputs(linha, file);
	}

	if(DEBUG){
		printf("NORMALIZED INPUT FILES AT ../corpora/normalized/\n");
		printf("NORMALIZED INPUT SENTENCE AT ../corpora/normalized/\n");
		printf("N-GRAM COUNTS AT ../corpora/counts/\n");
		if(SMOOTH) printf("N-GRAM SMOOTHED COUNTS AT ../corpora/counts/smoothed/\n");
		printf("N-GRAM PROBABILITIES AT ../corpora/counts/probs/\n");
		if(SMOOTH) printf("N-GRAM PROBABILITIES AFTER SMOOTHING AT ../corpora/counts/probs/smoothed/\n");
		printf("RESULTS SAVED AT %s\n", filename);
	}
}

void writeIOTestFiles(){

	int currentFile = 0;
	int currentSentence = 0;
	int currentChar = 0;
	char command[100];
	char filename[100];
	char ch = 'a';

	FILE *file;
	file = (FILE*) malloc(sizeof(FILE));

	if(CREATE_TEST_FILES){
		
		if(DEBUG) printf("Began Writing Reading/Writing Debug Files at '/corpora/test/'\n");

		sprintf(command, "rm -r ../corpora/test/");
    	system(command);
    	sprintf(command, "mkdir ../corpora/test/");
    	system(command);

		for(currentFile = 0; currentFile < NFILES; currentFile++){ 

			file = (FILE*) realloc(file, sizeof(FILE));			
			
			strcpy(filename, "../corpora/test/");
			strcat(filename, "test_");
    		strcat(filename, filenamesNOR[currentFile]);

    		file = fopen(filename, "w+"); 

    		int fileVectorPos = 0;
		    int i;
 
			for(currentSentence = 0; currentSentence < numberofSentences[currentFile]; currentSentence++){
				for(currentChar = 0; currentChar < sizeofSentences[currentFile][currentSentence]; currentChar++){

		    		ch = sentencesVector[currentFile][currentSentence][currentChar];
			    	fputc(ch, file);
			    	fileVectorPos ++;
			    }
		    }

		    fclose(file);
		    file = NULL;
		}

		if(DEBUG) printf("Finished Writing Reading/Writing Debug Files\n\n");
	}
}

void clean(){

	char command[100];

	if(DEBUG) printf("CLEANING...\n");
	sprintf(command, "rm -r ../corpora/test/");
    system(command);
    if(DEBUG) printf("REMOVED TEST DIRECTORY\n");
    sprintf(command, "rm -r ../corpora/normalized/");
    system(command);
    if(DEBUG) printf("REMOVED NORMALIZED DIRECTORY\n");
    sprintf(command, "rm -r ../corpora/counts/");
    system(command);
    if(DEBUG) printf("REMOVED COUNTS DIRECTORY\n");
    sprintf(command, "rm -r ../corpora/probs/");
    system(command);
    if(DEBUG) printf("REMOVED PROBS DIRECTORY\n");
    if(DEBUG && SMOOTH) printf("REMOVED SMOOTHED PROBS DIRECTORY\n");
    sprintf(command, "rm -r ../results/");
    system(command);
    if(DEBUG) printf("REMOVED RESULTS DIRECTORY\n");
    sprintf(command, "rm -r ../sentences/de/*normalized.txt");
    system(command);
    sprintf(command, "rm -r ../sentences/fr/*normalized.txt");
    system(command);
    sprintf(command, "rm -r ../sentences/it/*normalized.txt");
    system(command);
    sprintf(command, "rm -r ../sentences/en/*normalized.txt");
    system(command);
    sprintf(command, "rm -r ../sentences/pt/*normalized.txt");
    system(command);
    if(DEBUG) printf("REMOVED NORMALIZED INPUTS\n");
}
