#define BOLD "\033[1m"
#define RESET_FORMAT "\033[0m"
#define CYAN "\033[36m"
#define RED "\033[31m"
#define HEADER_FORMAT "\033[48;5;255m\033[38;5;232m\033[1m" //white bg black fg bold, format is 8 bit
//format specification: FG --> \033[38;5;<rgb color>m] BG --> replace 38 with 48