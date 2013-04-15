#ifndef LIXIN_H
#define LIXIN_H

#define PARAM_NONE 		0
#define PARAM_A 		1
#define PARAM_L 		2
#define PARAM_AL 		3
#define PARAM_I 		4
#define PARAM_IA 	 	5
#define PARAM_S 		8
#define PARAM_R 		9
#define MAXROWLEN 	 	80
#define MASK 			07000

#define S_ISU(m) 		(((m) & MASK) == S_ISUID)
#define S_ISG(m) 		(((m) & MASK) == S_ISGID)
#define S_ISS(m) 		(((m) & MASK) == S_ISVTX)

int 	row_leave;
int 	file_maxlen;
//int 	MAXROWLEN;

//int  gettermsize( void );
void my_err ( const char *, int );
void display_dir( int, char * );
void display( int, char * );
void display_single( char * );
void display_attribute( struct stat, char * );
void display_inode( struct stat, char * );
void display_blksize( struct stat, char * );
void display_recursive( int, struct stat, char *, char * );

#endif
