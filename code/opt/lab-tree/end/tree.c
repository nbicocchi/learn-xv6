#include "types.h"
#include "fcntl.h"
#include "fs.h"
#include "stat.h"
#include "user.h"


#define NULL   ((void*)0)
#define FALSE  (0)
#define TRUE   (1)

#define PATH_SEPARATOR	"/"


struct tree_part{
      char* name;
      struct tree_part *child;
      struct tree_part *next;
};

struct tree_part* create_tree(char *, char *);
void print_tree(struct tree_part *, int);
static struct tree_part* tree_alloc(char *);
static int getcwd(char *resultPath);
static char *goUp(int ino, char *ancestorPath, char *resultPath);
static int dirlookup(int fd, int ino, char *p);

static int getcwd(char *resultPath)
{
    resultPath[0] = '\0';

    char ancestorPath[512];
    strcpy(ancestorPath, ".");

    struct stat st;
    if (stat(ancestorPath, &st) < 0)
	return FALSE;

    char *p = goUp(st.ino, ancestorPath, resultPath);
    if (p == NULL)
	return FALSE;
    if (resultPath[0] == '\0')
	strcpy(resultPath, PATH_SEPARATOR);
    return TRUE;
}

static char *goUp(int ino, char *ancestorPath, char *resultPath)
{
    strcpy(ancestorPath + strlen(ancestorPath), PATH_SEPARATOR "..");
    struct stat st;
    if (stat(ancestorPath, &st) < 0)
	return NULL;

    if (st.ino == ino) {
	// No parent directory exists: must be the root.
	return resultPath;
    }

    char *foundPath = NULL;
    int fd = open(ancestorPath, O_RDONLY);
    if (fd >= 0) {
	char *p = goUp(st.ino, ancestorPath, resultPath);
	if (p != NULL) {
	    strcpy(p, PATH_SEPARATOR);
	    p += sizeof(PATH_SEPARATOR) - 1;

	    // Find current directory.
	    if (dirlookup(fd, ino, p))
		foundPath = p + strlen(p);
	}
	close(fd);
    }
    return foundPath;
}

static int dirlookup(int fd, int ino, char *p)
{
    struct dirent de;
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
	if (de.inum == 0)
	    continue;
	if (de.inum == ino) {
	    memmove(p, de.name, DIRSIZ);
	    p[DIRSIZ] = '\0';
	    return TRUE;
	}
    }
    return FALSE;
}


int main(int argc, char** argv)
{
	struct tree_part* root;
	char full_path[256];
	int is_absolute = FALSE;


	if(argc>2)
	{
		printf(2, "\n USAGE : tree [PATH]\n");
		exit();
	}
	
	getcwd(full_path);
	if(argc > 1){
		if(strcmp(&argv[1][0], PATH_SEPARATOR) == 0) {
			is_absolute = TRUE;
		} else {
			is_absolute = FALSE;
		}
	}
	root = create_tree(argc > 1 ? argv[1]:".", is_absolute ? argv[1]:full_path);
	print_tree(root, 0);

exit();
}

struct tree_part* create_tree(char * n_name, char* path)
{
	char full_path[256];
	struct stat st;
	int fd;
	struct dirent de;

	strcpy(full_path, path);
	if(strcmp(&full_path[strlen(full_path) - 1],PATH_SEPARATOR) != 0) strcat(full_path, "/");
	if(strcmp(n_name, "/") != 0) strcat(full_path, n_name);
	fd = open(full_path, O_RDONLY);
	if(fd < 0){
		printf(2, "Error opening %s%s\n", path, n_name);
		exit();
	}

	if(fstat(fd, &st) < 0){
		printf(2, "Error fstat in file %s\n", n_name);
		exit();
	}
	struct tree_part* node = tree_alloc(n_name);
	if(st.type == T_DIR) {
		struct tree_part* previous = node;
		while(read(fd, &de, sizeof(struct dirent)) == sizeof(de)) {
			if(strlen(de.name) > 0 && strcmp(de.name,".") != 0 && strcmp(de.name,"..") != 0 ) {
				if(previous == node) {
					previous = node->child = create_tree(de.name, full_path);
				} else {
					previous = previous->next = create_tree(de.name, full_path);
				}
			}
		}
	} 

	close(fd);

	return node;
}

static struct tree_part* tree_alloc(char* n)
{
	struct tree_part *temp=(struct tree_part *)malloc(sizeof(struct tree_part ));
	temp->name = (char*)malloc(sizeof(char)*(strlen(n)+1));
	strcpy(temp->name, n);
	temp->next = NULL;
	temp->child = NULL;
return temp;
}


void print_tree(struct tree_part *root, int depth)
{
	int i;
	char * dashes ="|-----",*spaces="|     ";


	for(i = 0;i < (depth-1); i++)
		printf(1, "%s", spaces);
	if(depth > 0)
		printf(1, "%s", dashes);


	printf(1, "%s\n", root->name);
	if(root->child != NULL) {
		print_tree(root->child, depth+1);
	} 
	while((root = root->next) != NULL) {
		for(i = 0; i < depth-1; i++)
			printf(1, "%s", spaces);
		if(depth > 0)
			printf(1, "%s", dashes);
		printf(1, "%s\n", root->name);
		if(root->child != NULL) {
			print_tree(root->child, depth+1);
		}
	}
}



