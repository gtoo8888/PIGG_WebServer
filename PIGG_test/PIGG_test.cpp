#include "PIGG_test.h"
#include <memory>
#include <cstring>
#include <stdio.h>
#include <unistd.h>     // sleep(1),usleep() 微秒级
#include <sys/stat.h>      // stat
#include <fstream>      
#include <unistd.h>      

namespace PIGG_init_test{

// PIGG_test::PIGG_test(){

// }
// PIGG_test::~PIGG_test(){

// }



void PIGG_test::test_shared_ptr(){    // 智能指针测试
    std::cout << "test_shared_ptr" << std::endl;

    std::shared_ptr<int> s_ptr = std::make_shared<int>(8);
    std::cout << *s_ptr << std::endl;
    std::cout << s_ptr.get() << std::endl;
    std::cout << s_ptr.use_count() << std::endl;
    std::cout << s_ptr.unique() << std::endl;
}

void PIGG_test::test_system_common(){// 系统命令的测试
    std::cout << "test_system_common" << std::endl;

    std::string folderPath = "lalalatest"; 
    std::string command;
    command = "touch " + folderPath;  
    system(command.c_str());

}

void PIGG_test::test_fflush(){  // 测试强制刷新
    char buff[64];

    memset(buff, '\0',sizeof(buff));
    fprintf(stdout, "启用全缓冲");
    setvbuf(stdout, buff, _IOFBF,1024); //全缓冲、线缓冲、无缓冲

    fprintf(stdout, "这里是runoob.com\n");
    fprintf(stdout, "该输出将保存到buff\n");
    fflush(stdout);

    fprintf(stdout, "这将在编程时出现\n");
    fprintf(stdout, "最后休眠五秒钟\n");

    sleep(5);
}

void PIGG_test::test_cppversion(){
    printf("cpp version : %ld\n",__cplusplus);
}

void PIGG_test::test_log(){
    LOG_DEBUG("LOG_DEBUG %s","test1");
    LOG_INFO("LOG_INFO %s", "test2");
    LOG_WARN("LOG_WARN %s", "test3");
    LOG_ERROR("LOG_ERROR %s", "test4");
}


// "r"	以“只读”方式打开文件。只允许读取，不允许写入。文件必须存在，否则打开失败。
// "w"	以“写入”方式打开文件。如果文件不存在，那么创建一个新文件；如果文件存在，那么清空文件内容（相当于删除原文件，再创建一个新文件）。
// "a"	以“追加”方式打开文件。如果文件不存在，那么创建一个新文件；如果文件存在，那么将写入的数据追加到文件的末尾（文件原有的内容保留）。
// "r+" 以“读写”方式打开文件。既可以读取也可以写入，也就是随意更新文件。文件必须存在，否则打开失败。
// "w+" 以“写入/更新”方式打开文件，相当于w和r+叠加的效果。既可以读取也可以写入，也就是随意更新文件。如果文件不存在，那么创建一个新文件；如果文件存在，那么清空文件内容（相当于删除原文件，再创建一个新文件）。
// "a+" 以“追加/更新”方式打开文件，相当于a和r+叠加的效果。既可以读取也可以写入，也就是随意更新文件。如果文件不存在，那么创建一个新文件；如果文件存在，那么将写入的数据追加到文件的末尾（文件原有的内容保留）。
void PIGG_test::test_fopen(){
    FILE *pFile;
    pFile=fopen("myfile.txt","w");  // 
    if(pFile!=NULL)
    {
        fputs("it's a fopen example",pFile);
        fclose(pFile);
    }
    
    // fopen不能有中间未创建的文件夹，需要检验文件路径是否存在
    FILE* PIGG_fp1 = fopen("./temp/ServerLog", "a");    // 如果中间文件夹temp没有创建，那么FILE为空，打开失败
    if(PIGG_fp1 == NULL){    // 如果为空就失败了
        printf("fopen(log_full_name; error!\n");
    }
    FILE* PIGG_fp2 = fopen(".ServerLog", "a");    // 直接指定一个文件，那就直接可以打开文件
    if(PIGG_fp2 == NULL){    
        printf("fopen(log_full_name; error!\n");
    }

}

// 检测文件是否存在的方法
inline bool exists_test0(const std::string& name){
    std::ifstream f(name.c_str());
    return f.good();
}

inline bool exists_test1(const std::string& name){
    if(FILE *file = fopen(name.c_str(),"r")){
        fclose(file);
        return true;
    }else{
        return false;
    }
}

inline bool exists_test2(const std::string& name){
    return (access(name.c_str(),F_OK) != -1);
}

inline bool exists_test3(const std::string& name){
    struct stat buffer;
    return (stat(name.c_str(),&buffer) == 0);
}

void PIGG_test::test_stat(){
    struct stat sb;
    char server_path[200];
    getcwd(server_path, 200);   // 得到当前工作目录，指的是当前执行文件的目录
    printf("%s\n",server_path);
    // const char* file_path = "/html_root";   // 这样读取当前文件夹下文件是错误的
    const char* file_path = "./html_root/judge.html";   
    printf("%s:%d\n",file_path,exists_test0(file_path));
    printf("%s:%d\n",file_path,exists_test1(file_path));
    printf("%s:%d\n",file_path,exists_test2(file_path));
    printf("%s:%d\n",file_path,exists_test3(file_path));
    if(stat(file_path,&sb) == -1){
        perror("stat");
    }
    switch (sb.st_mode & S_IFMT) //st_mode进行”&”操作，从而就可以得到某些特定的信息。
    {
        case S_IFBLK:  printf("块设备驱动\n");    
            break;
        case S_IFCHR:  printf("字符设备\n");      
            break;
        case S_IFDIR:  printf("目录\n");          
            break;
        case S_IFIFO:  printf("先入先出/管道\n");  
            break;
        case S_IFLNK:  printf("创建符号链接\n");                 
            break;
        case S_IFREG:  printf("普通文件\n");            
            break;
        case S_IFSOCK: printf("套接字\n");                  
            break;
        default:       printf("未知数?\n");                
            break;
    }

    printf("索引节点号:            %ld\n", (long) sb.st_ino);

    printf("Mode:                     %lo (octal)\n",(unsigned long) sb.st_mode);

    printf("链接数:               %ld\n", (long) sb.st_nlink);
    printf("所有权:                UID=%ld   GID=%ld\n",(long) sb.st_uid, (long) sb.st_gid);

    printf("首选I/O块大小: %ld bytes\n",(long) sb.st_blksize);
    printf("文件大小:                %lld bytes\n",(long long) sb.st_size);
    printf("块分配:         %lld\n",(long long) sb.st_blocks);

    printf("最后状态更改:       %s", ctime(&sb.st_ctime));
    printf("最后的文件访问:         %s", ctime(&sb.st_atime));
    printf("最后的文件修改:   %s", ctime(&sb.st_mtime));

}


void PIGG_test::test_snprintf(){
    char str[5];
    int ret = snprintf(str, 3, "%s","abcdefg"); // n 是要写入的最大输入，超过会被截断
    printf("%d\n",ret);
    printf("%s\n",str);

    char a[16];
    size_t i;

    i = snprintf(a, 13, "%012d", 12345);  // 第 1 种情况,12345复制到数组a中
    printf("i = %lu, a = %s\n", i, a);    // 输出：i = 12, a = 000000012345, 12位
    i = snprintf(a, 9, "%012d", 12345);   // 第 2 种情况，超出的截断，只复制1到数组a中
    printf("i = %lu, a = %s\n", i, a);    // 输出：i = 12, a = 00000001，9位
}



}