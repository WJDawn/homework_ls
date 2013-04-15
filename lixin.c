#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <linux/limits.h>

#include "lixin.h"

int main( int argc, char **argv )
{
	// 获取终端宽度
// 	MAXROWLEN = gettermsize();

	row_leave = MAXROWLEN;
	if ( argc < 1 )
	  my_err( "argc", __LINE__ );

	char 	path[PATH_MAX + 1]; 	// 存放文件绝对路径以及文件名
	char 	param[32]; 		// 保存选项信息
	int 	flag_param = PARAM_NONE;  	// 保存选项类型
	struct stat 	buf;
	int  	i = 0;
	int 	num = 0;
	int  	k = 0;
	int 	j = 0;

	// 命令行参数的解析
	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			for (k = 1; k < strlen( argv[i]); k++, j++)
			  param[j] = argv[i][k];
		num++;
		}
	}

	// 对除过支持的选项之外的选项报错
	for (i = 0; i < j; i++) {
		if (param[i] == 'a') {
			flag_param += PARAM_A;
			continue;
		}
		else if (param[i] == 'l') {
			flag_param += PARAM_L;
			continue;
		}
		else if (param[i] == 'i') {
			flag_param += PARAM_I;
			continue;
		}
		else if (param[i] == 's') {
			flag_param += PARAM_S;
			continue;
		}
		else if (param[i] == 'R') {
			flag_param += PARAM_R;
			continue;
		}
		else {
			printf( "invalid option!\n" );
			exit( 1 );
		}
	}
	param[j] = '\0';

	// 如果没有输入文件名或目录，就显示当前目录
	if ((num + 1) == argc) {
		strncpy( path, "./", 2 );
		path[2] = '\0';
		display_dir( flag_param, path );
		return 0;
	}

	i = 1;
	do {
		// 如果不是目标文件名或目录，解析下一个命令行参数
		if (argv[i][0] == '-') {
			++i;
			continue;
		} else {
			strncpy( path, argv[i], strlen( argv[i] ) );

			// 如果目标文件或目录不存在，报错并退出程序
			if (stat( path, &buf) == -1)
			  my_err( "stat", __LINE__ );

			if (S_ISDIR(buf.st_mode)) { 	// argv[i]是一个目录
				// 如果目录的最后一个字符不是‘/’，就加上‘/’
				if (path[strlen( argv[i] ) - 1] != '/') {
					path[strlen( argv[i])] = '/';
					path[strlen( argv[i]) + 1] = '\0';
				} 
				else
				  path[strlen( argv[i] )] = '\0';

				display_dir( flag_param, path );
				i++;
			}
			else { 		// argv[i]是一个文件
				display( flag_param, path );
				i++;
			}
		}
	} while ( i < argc );

	return 0;
}
