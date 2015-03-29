#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

// default data
#define DEF_SEP "\n"
#define DEF_ALP "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define DEF_PW_SIZE 8

// global data
static char* separator;
static char* base = NULL;
static char* alphabet;
static char* suffix;
static int alp_length;
static int pw_size;
static int static_size=0;
static int alp_size=0;
static int info=0;
static int one_time=0;
static double cmb=0;

static int def_sep=1;
static int def_alp=1;
static int def_pw_size=1;

// text generator
void generate_suffix(int nbl, int pos) {
	int i=0; double lim=pow(10,nbl);
	if (nbl != 1) {
		for (i=0; i<alp_length; i++) {
			suffix[pos] = alphabet[i];
			generate_suffix(nbl-1, pos+1);
			suffix[pos] = '\0';
		}
	}
	else {
		for (i=0; i<alp_length; i++) {
			suffix[pos] = alphabet[i];
			printf("%s%s%s", base?base:"", suffix, separator);
		}
	}
}

char* supLetter(char* src, int len, int lpos) {
	int i;
	char* ret = (char*)malloc((len-1)*sizeof(char));
	for (i=0; i<lpos; i++) ret[i] = src[i];
	for (i=lpos+1; i<len; i++) ret[i-1] = src[i];
	return ret;
}

int fact(int f) {
    if (f==0) return 1;
    return (f*fact(f-1));
}

void generate_suffix2(int nbl, int pos, char* letters) {
	// pour chaque lettres données
		// utiliser cette lettre en premier
		// puis pour chaque lettres données restantes
			// utiliser cette lettre en 2ème, ...
	int i, len = strlen(letters);
	char* rightLetters;
	if (nbl>1) {
		for (i=0; i<len; i++) {
			suffix[pos] = letters[i];
			rightLetters = supLetter(letters, len, i);
			generate_suffix2(nbl-1, pos+1, rightLetters);
			suffix[pos] = '\0';
		}
	}
	else {
		for (i=0; i<len; i++) {
			suffix[pos] = letters[i];
			printf("%s%s%s", base?base:"", suffix, separator);
		}
	}
}

void handler() {
	if (def_alp) free(alphabet);
	if (def_sep) free(separator);
	if (suffix) free(suffix);
	printf("\n");
	exit(0);
}

int main(int argc, char* argv[]) {
	int i=1, nbr_lettres = 1;
	struct sigaction action;
	action.sa_handler=(void*)handler;
	sigaction(SIGINT, &action, NULL);
	
	// arguments analysis
	while (i<argc) {
		if (strcmp(argv[i], "-b")==0 || strcmp(argv[i], "--base")==0) {
			base = argv[i+1];
			i++;
		}
		else if (strcmp(argv[i], "-a")==0 || strcmp(argv[i], "--alphabet")==0) {
			alphabet = argv[i+1];
			def_alp=0;
			i++;
		}
		else if (strcmp(argv[i], "-m")==0 || strcmp(argv[i], "--max-length")==0) {
			if (strcmp(argv[i+1],"a")==0) alp_size=1;
			pw_size = atoi(argv[i+1]);
			def_pw_size=0;
			i++;
		}
		else if (strcmp(argv[i], "-l")==0 || strcmp(argv[i], "--length")==0) {
			if (strcmp(argv[i+1],"a")==0) alp_size=1;
			static_size = atoi(argv[i+1]);
			def_pw_size=0;
			i++;
		}
		else if (strcmp(argv[i], "-s")==0 || strcmp(argv[i], "--separator")==0) {
			separator = argv[i+1];
			def_sep=0;
			i++;
		}
		else if (strcmp(argv[i], "-i")==0 || strcmp(argv[i], "--info")==0) {
			info=1;
		}
		else if (strcmp(argv[i], "-o")==0 || strcmp(argv[i], "--one-time")==0) {
			one_time=1;
		}
		
		// help of the program
		else if (strcmp(argv[i], "-h")==0 || strcmp(argv[i], "--help")==0) {
			printf("usage: %s [-h, --help]\n", argv[0]);
			printf("    [-a, --alphabet \"ABC\"]   Set the alphabet to use\n");
			printf("    [-b, --base \"BASE\"]      Set the prefix to use\n");
			printf("    [-l, --length N]         Set a static word length\n");
			printf("    [-m, --max-length M]     Set a progressive word length\n");
			printf("    [-o, --one-time]         Use only one time each alphabet's character\n");
			printf("    [-s, --separator \"SEP\"]  Set the word separator to use\n");
			printf("    [-i, --info]             Print info, not words\n\n");
			printf("    if ':num:' is given with -a, alphabet is 0123456789\n");
			printf("    if 'a' is given with -l or -m, length is alphabet length\n\n");
			printf("examples:\n");
			printf("    %s -a \"aAeEiIuUoOyY $:;/\" -m 3 -s \"  |  \"\n", argv[0]);
			printf("    %s -b \"password\" -a \"0123456789\" -l 2\n\n", argv[0]);
			printf("default options:\n");
			printf("    * alphabet is\n\t\t\"%s\"\n", DEF_ALP);
			printf("    * base is empty\n");
			printf("    * progressive length expansion from 1 to %d (-m %d)\n", DEF_PW_SIZE, DEF_PW_SIZE);
			printf("    * separator is \"%s\"\n", DEF_SEP);
			printf("    * if both -m and -l, works with -l (static length)\n");
			return 0;
		}
		i++;
	}
	
	// use default data ?
	if (def_sep) {
		separator = (char*)malloc((strlen(DEF_SEP)+1)*sizeof(char));
		assert(separator!=NULL);
		sprintf(separator, DEF_SEP);
	}
	
	if (def_alp) {
		alphabet = (char*)malloc((strlen(DEF_ALP)+1)*sizeof(char));
		assert(alphabet!=NULL);
		sprintf(alphabet, DEF_ALP);
	}
	
	if (def_pw_size)
		pw_size = DEF_PW_SIZE;
	
	// calculate length
	if (strcmp(alphabet, ":num:")==0) {
		alp_length = 10;
		alphabet = (char*)malloc(11*sizeof(char));
		assert(alphabet!=NULL);
		sprintf(alphabet, "0123456789");
		alphabet[10] = '\0';
	}
	else alp_length = strlen(alphabet);
	
	if (alp_size) {
		if (static_size) static_size = alp_length;
		else pw_size = alp_length;
	}
	
	// initialize suffix
	suffix = (char*)malloc(pw_size*sizeof(char));
	assert(suffix!=NULL);
	suffix[0] = '\0';
	
	// print information
	if (info) {
		if (one_time) {
			if (static_size > alp_length) static_size = alp_length;
			if (static_size) cmb = fact(alp_length) / fact(alp_length-static_size);
			else while (nbr_lettres <= pw_size) {
				cmb += fact(alp_length) / fact(alp_length-nbr_lettres);
				nbr_lettres++;
			}
			printf("Number of possible combinations : %.f", cmb);
		}
		else {
			if (static_size) cmb = pow((double)alp_length, (double)static_size);
			else while (nbr_lettres <= pw_size) {
				cmb += pow((double)alp_length, (double)nbr_lettres);
				nbr_lettres++;
			}
			printf("Number of possible combinations : %.f", cmb);
		}
	} 
	
	// password generation
	else {
		if (one_time) {
			// with static length
			if (static_size)
				generate_suffix2(static_size, 0, alphabet);
			
			// with progressive length (starting from 1)
			else while (nbr_lettres <= pw_size) {
				generate_suffix2(nbr_lettres, 0, alphabet);
				nbr_lettres++;
			}
		}
		else {
			// with static length
			if (static_size)
				generate_suffix(static_size, 0);
			
			// with progressive length (starting from 1)
			else while (nbr_lettres <= pw_size) {
				generate_suffix(nbr_lettres, 0);
				nbr_lettres++;
			}
		}
	}
	
	// free memory
	handler();
	return 0;
}

