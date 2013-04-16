#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/limits.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <sys/ioctl.h>

#include "lixin.h"

/*int gettermsize( void )
{
	struct winsize *ws;

	ws=(struct winsize*)malloc(sizeof(struct winsize));
	memset(ws,0x00,sizeof(struct winsize));
	ioctl(STDIN_FILENO,TIOCGWINSZ,ws);	
	return ws->ws_row;
}*/

void my_err( const char *err_string, int line )
{
	fprintf( stderr, "line: %d ", line );
	perror( err_string );
	exit( 1 );
}

void display_dir( int flag_param, char *path )
{
	DIR 	*dir; 		// 存放目录信息
	struct dirent 	*ptr; 	
	int 	count = 0; 		// 记录文件数
	char 	filenames[255][PATH_MAX + 1]; 	// 存放所有文件名
	char 	temp[PATH_MAX + 1]; 
	struct stat 	buf;

	file_maxlen = 0;
	row_leave = MAXROWLEN;

	// 获取该目录下文件总数和最长的文件名
	dir = opendir( path );
	if (dir == NULL)
	  my_err( "opendir", __LINE__ );
	while ((ptr = readdir( dir )) != NULL ) {
		if (file_maxlen < strlen( ptr->d_name ) )
		  file_maxlen = strlen( ptr->d_name);
		count++;
	}
	closedir( dir );

	if (count > 256)
	  my_err( "too many files under this directory\n", __LINE__ );

	int 	i = 0;
	int  	j = 0;
	int 	len = strlen( path );
	// 获取该目录下的所有文件名
	dir = opendir( path );
	for (; i < count; i++) {
		ptr = readdir( dir );
		if (ptr == NULL)
		  my_err( "readdir", __LINE__ );
		strncpy( filenames[i], path, len );
		filenames[i][len] = '\0';
		strncat( filenames[i], ptr->d_name, strlen( ptr->d_name ) );
		filenames[i][len + strlen( ptr->d_name )] = '\0';
	}
	closedir( dir );

	if (flag_param != PARAM_Q) {
		// 对文件名进行排序
		for (i = 0; i < count - 1; i++)
		  for (j = 0; j < count - i - 1; j++ ) {
			  if (strcmp( filenames[j], filenames[j + 1]) > 0) {
				  strncpy( temp, filenames[j], strlen( filenames[j] ) );
				  temp[strlen(filenames[j])] = '\0';
				  strncpy( filenames[j], filenames[j + 1], 
							  strlen( filenames[j + 1] ) );
				  filenames[j][strlen(filenames[j + 1] )] = '\0';
				  strncpy( filenames[j + 1], temp, strlen( temp ) );
				  filenames[j + 1][strlen( temp )] = '\0';
			  }
		  }
	}

	if (flag_param == PARAM_R) {
		printf( "%s:\n", path );
	}
	// 打印每个文件名
	int t = 0;
	int r = 0;
	char r_sort[255][PATH_MAX + 1];
	for (i = 0; i < count; i++) {
		t = 0;
		for (j = 0; j < strlen(filenames[i]); j++) {
			if (filenames[i][j] == '/') {
				t = 0;
				continue;
			}
			temp[t] = filenames[i][j];
			t++;
		}
		temp[t] = '\0';

		for (r = 0; r < t; r++) {
			r_sort[count - i - 1][r] = temp[r];
		}
		if ((flag_param != PARAM_A) && (flag_param != PARAM_AL) && (flag_param != PARAM_IA)) {
			if (temp[0] == '.') {
				continue;
			}
		}
		if ((flag_param == PARAM_R) || (flag_param == PARAM_Q)) {
			display_single( filenames[i] );
		}
		else {
			display(flag_param, filenames[i]);
		}
	}

	if (flag_param == PARAM_RR) {
		for (i = 0; i < count; i++) {
			if (r_sort[i][0] == '.')
			  continue;
			display_single(r_sort[i]);
		}
	}

	// 如果命令行没有-l，打印一个换行符
	if (( flag_param & PARAM_L ) == 0)
	  printf( "\n" );

	// 如果有参数-Ｒ,则递归打印目录
	if (flag_param == PARAM_R) {
		for (i = 0; i < count; i++) {
			int k = 0;

			if (lstat(filenames[i], &buf) == -1) {
				my_err("lstat", __LINE__);
			}

			// 如果是目录则递归调用display_dir
			if (S_ISDIR(buf.st_mode)) {
				int path_len = strlen(filenames[i]);
				// 解析出最后一个文件名
				for (j = 0; j < path_len; j++) {
					if (filenames[i][j] == '/') {
						k = 0;
						continue;
					}
					temp[k] = filenames[i][j];
					k++;
				}
				temp[k] = '\0';

				if (temp[0] == '.') {
					if (temp[1] == '\0')
						continue;
					else if (temp[1] == '.')
					  if (temp[2] == '\0')
						continue;
				}

				path_len = strlen(filenames[i]);
				if (filenames[i][path_len - 1] != '/') {
					filenames[i][path_len] = '/';
					filenames[i][path_len + 1] = '\0';
				}
				else {
					filenames[i][path_len] = '\0';
				}
				path_len = strlen(filenames[i]);
				display_dir(flag_param, filenames[i]);
			}
		}
	}
}

void display_attribute( struct stat buf, char * name )
{
	char 	buf_time[32];
	struct passwd 	*psd; 	// 存放文件所有者打用户名
	struct group 	*grp; 	// 存放文件所有者所属的组名

	// 获取并打印文件类型
	if (S_ISLNK(buf.st_mode)) {
		printf( "l" );
	} else if (S_ISREG(buf.st_mode)) {
		printf( "-" );
	} else if (S_ISDIR(buf.st_mode)) {
		printf( "d" );
	} else if (S_ISCHR(buf.st_mode)) {
		printf( "c" );
	} else if (S_ISBLK(buf.st_mode)) {
		printf( "b" );
	} else if (S_ISFIFO(buf.st_mode)) {
		printf( "f" );
	} else if (S_ISSOCK(buf.st_mode)) {
		printf( "s" );
	} 

	int bit = 0;
	int i;
	// 获取并打印文件所有者的权限
	/*	for (i = 0; i < 9; i++) {
		bit = buf.st_mode & ( 1 << ( 8 - i ) );
		if (0 == bit) {
		printf( "-" );
		} else {
		if (0 == ( i % 3 )) {
		printf( "r" );
		} else if (1 == ( i % 3 )) {
		printf( "w" );
		} else if (2 == ( i % 3 )) {
		if (S_ISU(buf.st_mode))
		printf( "s" );
		else if ( S_ISG(buf.st_mode))
		printf( "S" );
		else if ( S_ISS(buf.st_mode))
		printf( "t" );
		else
		printf( "x" );
		}
		}
		}*/
	if (buf.st_mode & S_IRUSR)
	  printf( "r" );
	else 
	  printf( "-" );
	if (buf.st_mode & S_IWUSR)
	  printf( "w" );
	else
	  printf( "-" );
	if (buf.st_mode & S_IXUSR) {
		if (S_ISU(buf.st_mode))
		  printf( "s" );
		else 
		  printf( "x" );
	}
	else 
	  printf( "-" );
	
	if (buf.st_mode & S_IRGRP)
	  printf( "r" );
	else 
	  printf( "-" );
	if (buf.st_mode & S_IWGRP)
	  printf( "w" );
	else 
	  printf( "-" );
	if (buf.st_mode & S_IXGRP) {
		if (S_ISG(buf.st_mode))
		  printf( "S" );
		else
		  printf( "x" );
	}
	else 
	  printf( "-" );

	if (buf.st_mode & S_IROTH)
	  printf( "r" );
	else 
	  printf( "-" );
	if (buf.st_mode & S_IWOTH)
	  printf( "w" );
	else 
	  printf( "-" );
	if (buf.st_mode & S_IXOTH) {
		if (S_ISS(buf.st_mode))
		  printf( "t" );
		else
		  printf( "x" );
	}
	else 
	  printf( "-" );

	// 根据uid与gid获取文件所有者打用户名与组名
	psd = getpwuid( buf.st_uid );
	grp = getgrgid( buf.st_gid );
	printf( "%2d ", buf.st_nlink );
	printf( "%-6s", psd->pw_name );
	printf( "%-6s", grp->gr_name );

	printf( "%6d", buf.st_size );
	strcpy( buf_time, ctime( &buf.st_mtime ) );
	buf_time[strlen(buf_time) - 1] = '\0';
	printf( " %s", buf_time );
}

// 打印文件的inode号
void display_inode( struct stat buf, char *name )
{
	int 	i = 0;
	int 	len = 0;
	row_leave = MAXROWLEN;

	// 如果剩余长度不足以打印文件名则换行
	if (row_leave < file_maxlen) {
		printf( "\n" );
		row_leave = MAXROWLEN;
	}

	len = strlen( name );
	len = file_maxlen - len;
	printf( "%8d ", buf.st_ino );
	row_leave -= 8;
	display_single( name );
	
//	for(i = 0; i < len; i++) {
//		printf(" ");
//	}
//	printf( " " );
//	row_leave -= (file_maxlen + 1);
}

void display( int flag, char * pathname )
{
	struct stat 	buf; 	// 存放文件信息
	char 	name[NAME_MAX + 1];

	// 从路径中解析出文件名
	int i = 0;
	int j = 0;
	for (; i < strlen( pathname ); i++) {
		if (pathname[i] == '/') {
			j = 0;
			continue;
		}
		name[j] = pathname[i];
		j++;
	}
	name[j] = '\0';

	// 用lstat以方便解析链接文件
	if (lstat( pathname, &buf ) == -1) {
		my_err( "lstat", __LINE__ );
	}

	switch (flag) {
		case PARAM_NONE: 	// 没有任何选项
			if (name[0] != '.') {
				display_single( pathname );
			}
			break;
		case PARAM_I:
			if (name[0] != '.') {
				display_inode( buf, pathname );
			}
			break;
		case PARAM_IA:
			display_inode( buf, pathname );
			break;
		case PARAM_A:
			display_single( pathname );
			break;
		case PARAM_L:
			if (name[0] != '.' ) {
				display_attribute( buf, name );
				printf( " %-s\n", name );
			}
			break;
		case PARAM_AL:
			display_attribute( buf, name );
			printf( " %-s\n", name );
			break;
		case PARAM_S:
			if (name[0] != '.') {
				display_blksize( buf, name );
			}
			break;

		default:
			break;
	}
}

// 打印文件的块大小
void display_blksize( struct stat buf, char * name )
{
	int i = 0;
	int len = 0;

	if (row_leave < file_maxlen) {
		printf( "\n" );
		row_leave = MAXROWLEN;
	}

	len = strlen( name );
	len = file_maxlen - len;
	printf( "%d ", buf.st_blocks/2);
	display_single( name );
}

// 打印单个文件名
void display_single( char * name )
{
	int i = 0;
	int j = 0;
	char filename[255];
	int len = 0;
	struct stat buf;
//	row_leave = MAXROWLEN;


	for (i = 0; i <strlen(name); i++) {
		if (name[i] == '/') {
			j = 0;
			continue;
		}
		filename[j] = name[i];
		j++;
	}
	filename[j] = '\0';

	if (lstat( name, &buf ) == -1)
	  my_err( "lstat", __LINE__ );
	
	if (row_leave < file_maxlen) {
		printf( "\n" );
		row_leave = MAXROWLEN;
	}

	len = strlen( filename );
	len = file_maxlen - len;

	if (S_ISDIR(buf.st_mode)) {
		printf("\033[1;34;1m%s\033[0m", filename);
	}
	else if (S_ISLNK(buf.st_mode)) {
		printf("\033[1;36;1m%s\033[0m", filename);
	}
	else if (S_ISCHR(buf.st_mode) || S_ISBLK(buf.st_mode)) {
		printf("\033[1;33;1m%s\033[0m", filename);
	}
	else if (S_ISFIFO(buf.st_mode)) {
		printf("\033[1;37;1m%s\033[0m", filename);
	}
	else if (buf.st_mode & 0111) {
		printf("\033[1;32;1m%s\033[0m", filename);
	}
	else {
		printf( "%s", filename );
	}

	for (i = 0; i < len; i++)
	  printf( " " );
	printf( " " );
	row_leave -= ( file_maxlen + 1 );
}
