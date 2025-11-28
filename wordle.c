#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define GREEN		"\x1b[32m"
#define YELLOW  	"\x1b[33m"
#define GRAY  		"\033[90m"
#define RESET   	"\x1b[0m"

#define MAX_WORDS 53402

typedef struct {
	int word_sz;
	int max_attempts;
	char word[32];
	char lang[3];
	char dict[MAX_WORDS][32];
	int dict_count;
} Config;

Config g_config = {5, 6};

char keyboard[26] = {'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'Z', 'X', 'C', 'V', 'B', 'N', 'M'};
int used_keys[26] = {0};
char** guesses;

void print_title() {
	printf("\n");
	printf("                                         █████  ████                      \n");
	printf("                                        ▒▒███  ▒▒███                      \n");
	printf("  █████ ███ █████  ██████  ████████   ███████   ▒███   ██████      ██████ \n");
	printf(" ▒▒███ ▒███▒▒███  ███▒▒███▒▒███▒▒███ ███▒▒███   ▒███  ███▒▒███    ███▒▒███\n");
	printf("  ▒███ ▒███ ▒███ ▒███ ▒███ ▒███ ▒▒▒ ▒███ ▒███   ▒███ ▒███████    ▒███ ▒▒▒ \n");
	printf("  ▒▒███████████  ▒███ ▒███ ▒███     ▒███ ▒███   ▒███ ▒███▒▒▒     ▒███  ███\n");
	printf("   ▒▒████▒████   ▒▒██████  █████     ▒▒████████ █████▒▒██████  ██▒▒██████ \n");
	printf("    ▒▒▒▒ ▒▒▒▒     ▒▒▒▒▒▒  ▒▒▒▒▒       ▒▒▒▒▒▒▒▒ ▒▒▒▒▒  ▒▒▒▒▒▒  ▒▒  ▒▒▒▒▒▒  \n");
	printf("\n");
	printf("                             Made with <3 by                              \n");
	printf("                  https://github.com/enricocristaudo                      \n");
	printf("\n");
}

void clear_screen() {
    #ifdef _WIN32
        system("cls");
    #else
        printf("\033[H\033[J");
    #endif
}

void parse_arguments(int argc, char *argv[]) {
	memcpy(g_config.lang, "en", 3);

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-s") == 0) {
			if (i + 1 < argc) {
				g_config.word_sz = atoi(argv[i + 1]);
				i++;
			} else {
				fprintf(stderr, "Warning: flag -s specified without value. Default usage.\n");
			}
		}

		else if (strcmp(argv[i], "-a") == 0) {
			if (i + 1 < argc) {
				g_config.max_attempts = atoi(argv[i + 1]);
				i++;
			} else {
				fprintf(stderr, "Warning: flag -a specified without value. Default usage.\n");
			}
		}

		else if (strcmp(argv[i], "-it") == 0) {
			memcpy(g_config.lang, "it", 3);
		}

		else if (strcmp(argv[i], "-en") == 0) {
			memcpy(g_config.lang, "en", 3);
		}
	}
	guesses = (char**)malloc(g_config.max_attempts * sizeof(char*));
	for (int i=0; i<g_config.max_attempts; i++) {
		char *empty = (char*)malloc(sizeof(char) * g_config.word_sz + 1);
		memset(empty, ' ', g_config.word_sz);
		empty[g_config.word_sz] = '\0';
		guesses[i] = empty;
	 }
}

void str_to_upper(char *str) {
	for(int i=0; str[i] != '\0'; i++) {
		if(str[i] >= 'a' && str[i] <= 'z') {
			str[i] = str[i] - 'a' + 'A';
		}
	}
}

int print_matchings(int index) {
	int ws = g_config.word_sz;
	int guessed[g_config.max_attempts];

	for (int k = 0; k<g_config.max_attempts; k++) {
		guessed[k] = 0;
		char *guess = guesses[k];

		printf("┌");
		for(int i=0; i<ws-1; i++) printf("───┬");
		printf("───┐\n");
		printf("│");
		for(int i=0; i<ws; i++) {
			if(guess[i] == g_config.word[i]) {
				guessed[index]++;
				printf(GREEN" %c "RESET, guess[i]);
				used_keys[guess[i] - 'A'] = 1;
			} else if (strchr(g_config.word, guess[i]) != NULL){
				printf(YELLOW" %c "RESET, guess[i]);
				if (used_keys[guess[i] - 'A'] != 1) used_keys[guess[i] - 'A'] = 2;
			} else {
				printf(" %c ", guess[i]);
				used_keys[guess[i] - 'A'] = 3;
			}
			if(i < ws - 1) printf("│");
		}
		printf("│");

		printf("\n└");
		for(int i=0; i<ws-1; i++) printf("───┴");
		printf("───┘\n");
	}

	return guessed[index] == ws;
}

void print_keyboard() {
	for (int i=0; i<26; i++) {
		if(i == 10) printf("\n ");
		if(i == 19) printf("\n   ");
		if(used_keys[keyboard[i] - 'A'] == 3) printf(GRAY"%c "RESET, keyboard[i]);
		else if(used_keys[keyboard[i] - 'A'] == 2) printf(YELLOW"%c "RESET, keyboard[i]);
		else if(used_keys[keyboard[i] - 'A'] == 1) printf(GREEN"%c "RESET, keyboard[i]);
		else printf("%c ", keyboard[i]);
	}
	printf("\n");
}

void save_guess(char *guess, int i) {
	char *new_guess = (char*)malloc(sizeof(char) * g_config.word_sz + 1);
	strcpy(new_guess, guess);
	guesses[i] = new_guess;
}

void load_dictionary() {
	char filename[50];
   sprintf(filename, "words/%s/%d.txt", g_config.lang, g_config.word_sz);

	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "Error: cannot open file words.txt\n");
		exit(1);
	}

	char buffer[100];
	g_config.dict_count = 0;

	while (g_config.dict_count < MAX_WORDS && fgets(buffer, sizeof(buffer), fp) != NULL) {
		buffer[strcspn(buffer, "\n")] = 0;
		strcpy(g_config.dict[g_config.dict_count], buffer);
		g_config.dict_count++;
	}

	fclose(fp);
}

void pick_secret_word() {
    int random_index = rand() % g_config.dict_count;
    strcpy(g_config.word, g_config.dict[random_index]);
}

int is_valid_guess(char *guess) {
    for (int i = 0; i < g_config.dict_count; i++) {
        if (strcmp(guess, g_config.dict[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {

	srand(time(NULL));

	parse_arguments(argc, argv);
	clear_screen();
	print_title();

	load_dictionary();
	pick_secret_word();

	for(int i=0; i<g_config.max_attempts; i++) {

		printf("\nGUESS #%d/%d:\n", i+1, g_config.max_attempts);
		printf(">>> ");

		char guess[g_config.word_sz + 1];
		scanf("%s", guess);
		str_to_upper(guess);

		if((int)strlen(guess) != g_config.word_sz) {
			clear_screen();
			print_matchings(i);
			print_keyboard();
			printf("Error: Word must be %d characters long.\n", g_config.word_sz);
			i--;
			continue;
		}

		if (is_valid_guess(guess) == 0) {
      	clear_screen();
			print_matchings(i);
			print_keyboard();
			printf("Word not found.\n");
			i--;
			continue;
      }
		save_guess(guess, i);
		clear_screen();

		if(print_matchings(i)) {
			printf("\nCongratulations! You guessed!\n");
			return 0;
		}
		print_keyboard();

	}

	printf("\nYou lost :(\n");
	printf("Word was: %s\n\n", g_config.word);
}
