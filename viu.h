/* 
    Header file for ViUnproved.
*/

/* Constants for syntax highlighting.*/
#define HL_NORMAL 0
#define HL_NONPRINT 1
#define HL_COMMENT 2   
#define HL_MLCOMMENT 3 
#define HL_KEYWORD1 4
#define HL_KEYWORD2 5
#define HL_STRING 6
#define HL_NUMBER 7
#define HL_MATCH 8      
#define HL_HIGHLIGHT_STRINGS (1<<0)
#define HL_HIGHLIGHT_NUMBERS (1<<1)
#define HLDB_ENTRIES (sizeof(HLDB)/sizeof(HLDB[0]))
#define VIU_QUERY_LEN 256

/* Set up necessary buffers. */
#define ebuf_INIT {NULL,0}
#define FIND_RESTORE_HL do { \
    if (saved_hl) { \
        memcpy(viu.row[saved_hl_line].hl,saved_hl, viu.row[saved_hl_line].rsize); \
        saved_hl = NULL; \
    } \
} while (0)

/* Editor syntax structure to keep track of files.*/
struct editorSyntax {
    char **filematch;  
    char **keywords;
    char singleline_comment_start[2]; 
    char multiline_comment_start[3];
    char multiline_comment_end[3];
    int flags;
};

/* Structure with which to index each line of the file row-by-row. */
typedef struct erow {
    int idx;            /* Zero-indexed row number. */
    int size;           /* Row size, not including null terminator. */
    int rsize;          /* The size of the row. */
    char *chars;        /* Content of the row. */
    char *render;       /* Tab-rendered content. */
    unsigned char *hl;  /* Highlighting type*/
    int hl_oc;          /* If the row had an open comment at the end of the last
                           syntax highlighting check. */
} erow;

/* Structure for colors of syntax. */
typedef struct hlcolor {
    int r,g,b;
} hlcolor;

/* Structure for different settings for editor configuraiton. */
struct editorConfig {
    int cx;                         /* Cursor x position. */
    int cy;                         /* Cursor y position. */
    int rowoff;                     /* Row offset. */
    int coloff;                     /* Column offset. */
    int screenrows;                 /* Number of rows displayed. */
    int screencols;                 /* Number of columns displayed. */
    int numrows;                    /* Total number of rows. */
    int rawmode;                    /* If raw mode is enabled or not. */
    erow *row;                      /* Tracks row.*/
    int unsaved;                    /* If file has unsaved changes. */
    char *filename;                 /* Name of file currently open. */
    char statusmsg[80];             /* Status message */
    time_t statusmsg_time;          /* Time associated with status message */
    struct editorSyntax *syntax;    /* Syntax highlight. */
};

/* Editor configuration structure for the current session.*/
static struct editorConfig viu;


/* Syntax highlighting framework. A lot of this was taken pretty directly
   from the tutorial example code above. */
enum KEY_ACTION{
        KEY_NULL = 0,       /* NULL */
        CTRL_C = 3,         /* Ctrl-c */
        CTRL_D = 4,         /* Ctrl-d */
        CTRL_F = 6,         /* Ctrl-f */
        CTRL_H = 8,         /* Ctrl-h */
        TAB = 9,            /* Tab */
        CTRL_L = 12,        /* Ctrl+l */
        ENTER = 13,         /* Enter */
        CTRL_Q = 17,        /* Ctrl-q */
        CTRL_S = 19,        /* Ctrl-s */
        CTRL_U = 21,        /* Ctrl-u */
        ESC = 27,           /* Escape */
        BACKSPACE =  127,   /* Backspace */
        ARROW_LEFT = 1000,
        ARROW_RIGHT,
        ARROW_UP,
        ARROW_DOWN,
        DEL_KEY,
        HOME_KEY,
        END_KEY,
        PAGE_UP,
        PAGE_DOWN
};

/* General status method. */
void setStatus(const char *fmt, ...);
/* Extensions associated with files we can do syntax highlighting for.
   We're pretty biased towards C, surprisngly. */
char *C_HL_extensions[] = {".c",".cpp",NULL};
/* Keywords for C programs that we can keep track of. */
char *C_HL_keywords[] = {
        "switch","if","while","for","break","continue","return","else",
        "struct","union","typedef","static","enum","class",
        "int|","long|","double|","float|","char|","unsigned|","signed|",
        "void|",NULL
};
/* Syntax structure for keeping track of highlighting. */
struct editorSyntax HLDB[] = {
    {
        C_HL_extensions,
        C_HL_keywords,
        "//","/*","*/",
        HL_HIGHLIGHT_STRINGS | HL_HIGHLIGHT_NUMBERS
    }
};
/* Buffer to track escape sequences. */
struct ebuf {
    char *b;
    int len;
};
/* Save state to restore at exit. */
static struct termios orig_termios;
