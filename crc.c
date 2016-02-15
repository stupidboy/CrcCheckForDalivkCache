#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <malloc.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>
#include <errno.h>
#include <pthread.h>
#include <utils/Timers.h>
#define LOG_TAG "CRC"
#include <cutils/log.h>
#include <time.h>
#include <sys/time.h>

#define DEXPREOPT_PATH "/data/dalvik-cache/arm/"
#define Z_NULL 0
#define MAX_NAME 255
#define PATH_MAX 4096
#define DEFALUT_CPU_COUNT 4
#define CMP_FLAG_CHECK  0
#define CMP_FLAG_CREATE 1






static const int crc32c_table[256] = {
    0x00000000L, 0xF26B8303L, 0xE13B70F7L, 0x1350F3F4L,
    0xC79A971FL, 0x35F1141CL, 0x26A1E7E8L, 0xD4CA64EBL,
    0x8AD958CFL, 0x78B2DBCCL, 0x6BE22838L, 0x9989AB3BL,
    0x4D43CFD0L, 0xBF284CD3L, 0xAC78BF27L, 0x5E133C24L,
    0x105EC76FL, 0xE235446CL, 0xF165B798L, 0x030E349BL,
    0xD7C45070L, 0x25AFD373L, 0x36FF2087L, 0xC494A384L,
    0x9A879FA0L, 0x68EC1CA3L, 0x7BBCEF57L, 0x89D76C54L,
    0x5D1D08BFL, 0xAF768BBCL, 0xBC267848L, 0x4E4DFB4BL,
    0x20BD8EDEL, 0xD2D60DDDL, 0xC186FE29L, 0x33ED7D2AL,
    0xE72719C1L, 0x154C9AC2L, 0x061C6936L, 0xF477EA35L,
    0xAA64D611L, 0x580F5512L, 0x4B5FA6E6L, 0xB93425E5L,
    0x6DFE410EL, 0x9F95C20DL, 0x8CC531F9L, 0x7EAEB2FAL,
    0x30E349B1L, 0xC288CAB2L, 0xD1D83946L, 0x23B3BA45L,
    0xF779DEAEL, 0x05125DADL, 0x1642AE59L, 0xE4292D5AL,
    0xBA3A117EL, 0x4851927DL, 0x5B016189L, 0xA96AE28AL,
    0x7DA08661L, 0x8FCB0562L, 0x9C9BF696L, 0x6EF07595L,
    0x417B1DBCL, 0xB3109EBFL, 0xA0406D4BL, 0x522BEE48L,
    0x86E18AA3L, 0x748A09A0L, 0x67DAFA54L, 0x95B17957L,
    0xCBA24573L, 0x39C9C670L, 0x2A993584L, 0xD8F2B687L,
    0x0C38D26CL, 0xFE53516FL, 0xED03A29BL, 0x1F682198L,
    0x5125DAD3L, 0xA34E59D0L, 0xB01EAA24L, 0x42752927L,
    0x96BF4DCCL, 0x64D4CECFL, 0x77843D3BL, 0x85EFBE38L,
    0xDBFC821CL, 0x2997011FL, 0x3AC7F2EBL, 0xC8AC71E8L,
    0x1C661503L, 0xEE0D9600L, 0xFD5D65F4L, 0x0F36E6F7L,
    0x61C69362L, 0x93AD1061L, 0x80FDE395L, 0x72966096L,
    0xA65C047DL, 0x5437877EL, 0x4767748AL, 0xB50CF789L,
    0xEB1FCBADL, 0x197448AEL, 0x0A24BB5AL, 0xF84F3859L,
    0x2C855CB2L, 0xDEEEDFB1L, 0xCDBE2C45L, 0x3FD5AF46L,
    0x7198540DL, 0x83F3D70EL, 0x90A324FAL, 0x62C8A7F9L,
    0xB602C312L, 0x44694011L, 0x5739B3E5L, 0xA55230E6L,
    0xFB410CC2L, 0x092A8FC1L, 0x1A7A7C35L, 0xE811FF36L,
    0x3CDB9BDDL, 0xCEB018DEL, 0xDDE0EB2AL, 0x2F8B6829L,
    0x82F63B78L, 0x709DB87BL, 0x63CD4B8FL, 0x91A6C88CL,
    0x456CAC67L, 0xB7072F64L, 0xA457DC90L, 0x563C5F93L,
    0x082F63B7L, 0xFA44E0B4L, 0xE9141340L, 0x1B7F9043L,
    0xCFB5F4A8L, 0x3DDE77ABL, 0x2E8E845FL, 0xDCE5075CL,
    0x92A8FC17L, 0x60C37F14L, 0x73938CE0L, 0x81F80FE3L,
    0x55326B08L, 0xA759E80BL, 0xB4091BFFL, 0x466298FCL,
    0x1871A4D8L, 0xEA1A27DBL, 0xF94AD42FL, 0x0B21572CL,
    0xDFEB33C7L, 0x2D80B0C4L, 0x3ED04330L, 0xCCBBC033L,
    0xA24BB5A6L, 0x502036A5L, 0x4370C551L, 0xB11B4652L,
    0x65D122B9L, 0x97BAA1BAL, 0x84EA524EL, 0x7681D14DL,
    0x2892ED69L, 0xDAF96E6AL, 0xC9A99D9EL, 0x3BC21E9DL,
    0xEF087A76L, 0x1D63F975L, 0x0E330A81L, 0xFC588982L,
    0xB21572C9L, 0x407EF1CAL, 0x532E023EL, 0xA145813DL,
    0x758FE5D6L, 0x87E466D5L, 0x94B49521L, 0x66DF1622L,
    0x38CC2A06L, 0xCAA7A905L, 0xD9F75AF1L, 0x2B9CD9F2L,
    0xFF56BD19L, 0x0D3D3E1AL, 0x1E6DCDEEL, 0xEC064EEDL,
    0xC38D26C4L, 0x31E6A5C7L, 0x22B65633L, 0xD0DDD530L,
    0x0417B1DBL, 0xF67C32D8L, 0xE52CC12CL, 0x1747422FL,
    0x49547E0BL, 0xBB3FFD08L, 0xA86F0EFCL, 0x5A048DFFL,
    0x8ECEE914L, 0x7CA56A17L, 0x6FF599E3L, 0x9D9E1AE0L,
    0xD3D3E1ABL, 0x21B862A8L, 0x32E8915CL, 0xC083125FL,
    0x144976B4L, 0xE622F5B7L, 0xF5720643L, 0x07198540L,
    0x590AB964L, 0xAB613A67L, 0xB831C993L, 0x4A5A4A90L,
    0x9E902E7BL, 0x6CFBAD78L, 0x7FAB5E8CL, 0x8DC0DD8FL,
    0xE330A81AL, 0x115B2B19L, 0x020BD8EDL, 0xF0605BEEL,
    0x24AA3F05L, 0xD6C1BC06L, 0xC5914FF2L, 0x37FACCF1L,
    0x69E9F0D5L, 0x9B8273D6L, 0x88D28022L, 0x7AB90321L,
    0xAE7367CAL, 0x5C18E4C9L, 0x4F48173DL, 0xBD23943EL,
    0xF36E6F75L, 0x0105EC76L, 0x12551F82L, 0xE03E9C81L,
    0x34F4F86AL, 0xC69F7B69L, 0xD5CF889DL, 0x27A40B9EL,
    0x79B737BAL, 0x8BDCB4B9L, 0x988C474DL, 0x6AE7C44EL,
    0xBE2DA0A5L, 0x4C4623A6L, 0x5F16D052L, 0xAD7D5351L
};

struct file_src{
    char file_name[MAX_NAME];
    unsigned int crc32sum;
};
long current_time() {
   struct timespec t;
   t.tv_sec = t.tv_nsec = 0;
   clock_gettime(CLOCK_MONOTONIC, &t);
   return (t.tv_sec * 1000000000LL + t.tv_nsec)/1000000 ;
}
int g_cpucount = DEFALUT_CPU_COUNT;
pthread_mutex_t g_file_list_lock  = PTHREAD_MUTEX_INITIALIZER;
DIR *g_dir;
struct file_src * g_arry;
int g_files;
int g_dex_files_counts;
int g_current_fd;
int g_ret;
int g_taskdone;
int g_init = 0;
struct file_src * g_dex_files;

static int compute_crc32(int fd);
int crc_write_file(struct file_src arry);

bool is_file_odex(char * name){
     return ( strstr(name, ".dex") != NULL
          || strstr(name, ".odex") != NULL
        	|| strstr(name, ".oat") != NULL
        	|| strstr(name,".art"))
        	&& strstr(name, "crc32sum") == NULL
        	&& strstr(name,"system@") != NULL; // check system now
}
bool is_file_crcsum(char * name){
    return (strstr(name, ".crc32sum") != NULL);
}

// file queue:

int get_all_dex_files(DIR *d,bool exclude_crc) {
    g_dex_files = malloc(sizeof(struct file_src)*255);
    bzero(g_dex_files,sizeof(struct file_src)*255);
    struct dirent * file;
    int i = 0;
    while((file = readdir(d)) != NULL){
        if(is_file_odex(file->d_name) ||( !exclude_crc && is_file_crcsum(file->d_name))) {
            strcpy(g_dex_files[i++].file_name,file->d_name);
        }
    }
    ALOGE("dex file count :%d",i);
    return i;
}
void init_dex_files(DIR *d,int flag){
    g_dex_files_counts = flag == CMP_FLAG_CHECK ? get_all_dex_files(d, true): get_all_dex_files(d,false);
    g_current_fd = 0;
    g_init = 1;
}
struct file_src * get_next_file(DIR *d){
    struct file_src * file = NULL;
    char file_path[PATH_MAX];
    pthread_mutex_lock(&g_file_list_lock);
    if (g_current_fd < g_dex_files_counts){
        file = &g_dex_files[g_current_fd++];
    }
    pthread_mutex_unlock(&g_file_list_lock);
    if(file != NULL){
        //ALOGE("get file  count : %d : %s",g_current_fd,file->file_name);
    }else {
        ALOGE("get NULL....EOF");
    }
    return file;
}
void set_task_done() {
    pthread_mutex_lock(&g_file_list_lock);
    g_taskdone ++;
    pthread_mutex_unlock(&g_file_list_lock);
}
int get_task_done() {
    int count = 0;
    pthread_mutex_lock(&g_file_list_lock);
    count = g_taskdone;
    pthread_mutex_unlock(&g_file_list_lock);
    return g_cpucount == count;
}
void * comp_crc(void * args){
    struct file_src *file;
    int fd, temp;
    FILE * fp;
    unsigned int crc_temp;
    char file_path[PATH_MAX];
    char filename[PATH_MAX];
    char tempname[PATH_MAX];
    char crc32sum_path[PATH_MAX];
    char need_remove_path[PATH_MAX];
    int flag = 0;
    unsigned int crc_compute = 0;
    if(args != NULL){
        flag = *((int *)args);
    }
    ALOGE("cmp_crc ,flag = %d",flag);
    if(flag == CMP_FLAG_CHECK) {
        while((file = get_next_file(g_dir)) != NULL) {
            strcpy(file_path, DEXPREOPT_PATH);
            strcat(file_path, "/");
            strcat(file_path, file->file_name);
            fd = open(file_path, O_RDONLY);
            long now = current_time();
            crc_temp = compute_crc32(fd);
            ALOGE("check ->cmp dex: %s took %ld ms", file->file_name, current_time() - now);           
            for(temp=0; temp < g_files; temp++)
            {
            	  if(!strncmp(g_arry[temp].file_name, file->file_name, strlen(file->file_name)))
                {
                    if(crc_temp != g_arry[temp].crc32sum)
                    {
                    	  char  temp_filename[128];                    
                        sprintf(temp_filename, "%s%s", DEXPREOPT_PATH, file->file_name);
                        ALOGE("odex file : %s  crc not match!\n", file->file_name);
                        ALOGE("original crc : %u, present crc : %u !\n",g_arry[temp].crc32sum, crc_temp);
                        unlink(temp_filename);
                        sprintf(temp_filename,"%s.crc32sum",temp_filename);
                        unlink(temp_filename);
                        g_ret = -1;
                    }
                    break;
                }
                else
                {
                    //if(temp == g_files-1)
                        //ret = -1;
                }
            }
        }
    }
    else if (flag == CMP_FLAG_CREATE){
        while ((file = get_next_file(g_dir)) != NULL)
        {
            // scan dex files ,find those  haven't crc32sum.
            if (is_file_odex(file->file_name))
            {
                strcpy(file_path, DEXPREOPT_PATH);
                strcat(file_path, "/");
                strcat(file_path, file->file_name);
                strcpy(crc32sum_path, file_path);
                strcat(crc32sum_path, ".crc32sum");
                fd = open(file_path, O_RDONLY);
                if (fd == -1)
                {
                    ALOGE( "open %s failed: %d %s\n", file_path, errno, strerror(errno));
                    continue;
                }
                if ((fp = fopen(crc32sum_path, "r")) == NULL)
                {
                    long now = current_time();
                    struct file_src temp ;
                    crc_compute = compute_crc32(fd);
                    temp.crc32sum = crc_compute;
                    strcpy(temp.file_name, file->file_name);
                    crc_write_file(temp);
                    ALOGE("create -> %s took %ld ms", file->file_name, current_time() - now); 
                }
                close(fd);
            }
            //delete crc sum files which haven't owner
            else if(strstr(file->file_name, "dex.crc32sum") != NULL)
            {
                strcpy(filename, file->file_name);
                strcpy(need_remove_path, DEXPREOPT_PATH);
                strcat(need_remove_path, "/");
                *(filename+strlen(filename)-strlen(".crc32sum")) = '\0';
                strcpy(tempname, filename);
                strcat(need_remove_path, tempname);
                if (access(need_remove_path, F_OK) == -1)
                {
                    strcat(need_remove_path, ".crc32sum");
                    ALOGE("need_remove_path %s \n", need_remove_path);
                    remove(need_remove_path);
                }
            }
        }  	
     }
    set_task_done();
    return NULL;
}
void comp_crc_async(int flag) {
	  g_cpucount = sysconf(_SC_NPROCESSORS_CONF);
    int task_count = g_cpucount;
    init_dex_files(g_dir, flag);
    g_taskdone = 0;
    g_ret = 0;
    while(task_count --){
        pthread_t t ;
        pthread_create(&t, NULL, comp_crc, &flag);
        //pthread_join(t, NULL);
    }
    while(!get_task_done()){
        usleep(10*1000);
    }
    
}
void init() {
    
}

static int crc32(int crc, const char *data, unsigned int length)
{
    while (length--)
        crc = crc32c_table[(crc ^ *data++) & 0xFFL] ^ (crc >> 8);

    return crc;
}
static int compute_crc32(int fd)
{
    const int bufsize = 4*1024;
    unsigned int amt;

    char* buf = (char*)malloc(bufsize);
    int crc = crc32(0L, Z_NULL, 0);

    lseek(fd, 0, SEEK_SET);

    while ((amt = read(fd, buf, bufsize)) != 0)
    {
        crc = crc32(crc, (char*)buf, amt);
    }

    free(buf);
    return crc;
}

int remove_crc()
{
    char file_path[PATH_MAX];
    DIR *d;
    struct dirent *file;

    if (access(DEXPREOPT_PATH, F_OK) == -1)
    {
        return -1;
    }

    if(!(d = opendir(DEXPREOPT_PATH)))
    {
        return -1;
    }

    while ((file = readdir(d)) != NULL)
    {
        if (strstr(file->d_name, "crc32sum") != NULL)
        {
            strcpy(file_path, DEXPREOPT_PATH);
            strcat(file_path, "/");
            strcat(file_path, file->d_name);
            remove(file_path);
        }
    }
    fprintf(stderr, "remove crc files done....\n");
    return 0;
}

int crc_write_file(struct file_src arry)
{
    FILE *fp;
    char file_path[PATH_MAX];
    char crc_code[20];
    int ret;

    sprintf(crc_code,"%u", arry.crc32sum);			//get crc code
    strcpy(file_path, DEXPREOPT_PATH);
    strcat(file_path, "/");
    strcat(file_path, arry.file_name);
    strcat(file_path, ".crc32sum");				//creat crc32 file name

    if (access(file_path, F_OK) == -1)
    {
        if ((fp = fopen(file_path, "w+")) != NULL)
        {
            fputs(crc_code, fp);
            ret = 0;
        }
        else
            ret = -1;
        fclose(fp);
    }
    else	//crc file already exist
    {
    	  ALOGE("crc file already exist , we can't modify it here, sth hanppened ,wait for crc check.");
        ret = -1;
    }
    return ret;
}

int creat_crc()
{
    if (access(DEXPREOPT_PATH, F_OK) == -1)
    {
        return -1;
    }

    if(!(g_dir = opendir(DEXPREOPT_PATH)))
    {
        return -1;
    }
    long now = current_time();
    comp_crc_async(CMP_FLAG_CREATE);
    ALOGE("compute crc--> cost:%ld ", current_time()-now);
    closedir(g_dir);
    now = current_time();
    fprintf(stderr, "create crc files done. \n");
    return 0;
}

int get_crc(struct file_src *arry)
{
    DIR *d;
    FILE *fp;
    struct dirent *file;
    char file_path[PATH_MAX];
    int files=0,file_size;
    char crc_buf[20];

    if(!(d = opendir(DEXPREOPT_PATH)))
    {
        return -1;
    }

    while ((file = readdir(d)) != NULL)
    {
        if (strstr(file->d_name, "crc32sum") != NULL)
        {
            strcpy(file_path, DEXPREOPT_PATH);
            strcat(file_path, "/");
            strcat(file_path, file->d_name);
            if ((fp = fopen(file_path, "r")) != NULL)
            {
                file_size = fread(crc_buf, 1, 20, fp);
                crc_buf[file_size] = '\0';
                sscanf(crc_buf,"%u",&arry[files].crc32sum);
                strcpy(arry[files].file_name, file->d_name);
                files++;
            }
            else
            {
                files = -1;
                break;
            }
            fclose(fp);
        }
    }
    closedir(d);
    return files;
}

int check_crc()
{
    char file_path[PATH_MAX];
    struct dirent *file;
    int fd,temp;
    unsigned int crc_temp;
    ALOGE("checking crc begin...\n");
    g_arry = malloc(sizeof(struct file_src) *255);
    bzero(g_arry,sizeof(struct file_src) *255);
    if (!g_arry)
        return -1;

    if (access(DEXPREOPT_PATH, F_OK) == -1)
    {
        return -1;
    }
    g_files = get_crc(g_arry);
    if(g_files == -1)
        return -1;

    if(!(g_dir = opendir(DEXPREOPT_PATH)))
    {
        return -1;
    }
    comp_crc_async(CMP_FLAG_CHECK); 
    closedir(g_dir);
    free(g_arry);
    free(g_dex_files);
    fprintf(stderr, "check crc files done..,ret = %d \n", g_ret);
    return g_ret;
}



void useage()
{
    fprintf(stderr, "usage: crc <action>\n");
    fprintf(stderr, "<action> r : remove crc file\n");
    fprintf(stderr, "<action> c : creat crc file\n");
    fprintf(stderr, "<action> p : compare the crc result\n");
}

int crc_main(int argc, char *argv[])
{
    int ret = 0;
    if(argc<2)
    {
        useage();
        return -1;
    }
    long now = current_time();
    ALOGE("enter crc main");
    if(strcmp(argv[1], "r") == 0)
        ret = remove_crc();
    else if(strcmp(argv[1], "c") == 0)
        ret = creat_crc();
    else if(strcmp(argv[1], "p") == 0)
        ret = check_crc();
    else
    {
        useage();
        ret = -1;
    }
    if(ret != 0){
    	  ALOGE("exit crc main, sth bad happened:,cost %ld ms",current_time() - now);
        exit(10086);	
    }
    ALOGE("exit crc main,total cost : %ld ms ",current_time() - now);
    return ret;
}
