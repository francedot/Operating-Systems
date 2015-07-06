//invio segnali
int kill(int pid, int sig);
pid>0: l’intero è il pid dell’unico processo destinatario
pid=0: il segnale è spedito a tutti i processi appartenenti al
	   gruppo del mittente
pid < -1: il segnale è spedito a tutti i processi con groupId uguale
al valore assoluto di pid
pid== -1: vari comportamenti possibili (Posix non specifica)

//creazione file e apertura in write mode (ritorna fd)
int creat(char nome[] , int mode);

//ritorna byte da inizio. SEEK_SET SEEK_CUR SEEK_END
lseek(int fd , int offset_in_byte , int origine);

//aumenta numero di link. ritorna 0 se ok -1 se fallisce
int link(char *oldname , char *newname);

//decrementa link. se link_num in i-node è 0 il file è eliminato
int unlink(char *name);

//verifica i diritti utente. Fa uso solo del real UID del processo (non usa effective UID)
//amode: 04 read access , 02 write access , 01 execute access , 00 extistence
//ritorna -1 se fallisce
int access(char* pathname , int amode);

//per leggere gli attributi di un file
//path: nome del file , buf puntatore a struct di tipo stat (contiene attributi del file)
//ritorna un numero negativo in caso di errore
#include <sys/stat.h>
int stat(const char *path , struct stat *buf);

//uso:
struct stat {
	dev_t	st_dev;		/* ID of device containing file */
	ino_t	st_ino;		/* i-number */
	mode_t 	st_mode;	/* protection */
	nlink_t	st_nlink;	/* number of hard links */
	uid_t	st_uid;		/* user ID of owner */
	gid_t	st_gid;		/* group ID of owner */
	dev_t	st_rdev;	/* device ID (if special file) */
	off_t	st_size;	/* total size, in bytes */
	blksize_t st_blksize; 	/* blocksize for file system I/O */
	blkcnt_t st_blocks;	/* number of blocks allocated */
	time_t	st_atime;	/* time of last access */
	time_t	st_mtime;	/* time of last modification */
	time_t	st_ctime;	/* time of last status change */
};
//macro per st_mode:
- S_ISREG(mode): è un file regolare? (flag S_IFREG)
- S_ISDIR(mode): è una directory? (flag S_IFDIR)
- S_ISCHR(mode): è un dispositivo a caratteri (file speciale)? (flag S_IFCHR)
- S_ISBLK(mode): è un dispositivo a blocchi (file speciale)? (flag S_IFBLK)

//esempio d'uso
struct stat sb;
if (stat(argv[1], &sb) == -1)
{
	perror("stat");
	exit(EXIT_FAILURE);
}

if(S_ISREG(sb.st_mode)) printf("file ordinario\n");
if (S_ISBLK(sb.st_mode)) printf("block device\n");
if (S_ISCHR(sb.st_mode)) printf("character device\n");
if (S_ISDIR(sb.st_mode)) printf("directory\n");
printf("I-number:\t%ld\n", (long)  sb.st_ino);
printf("Mode:\t%lo  (octal)\n", (unsigned  long)  sb.st_mode);
printf("numero di link:\t%ld\n", (long)  sb.st_nlink);
printf("Proprietario:\tUID=%ld    GID=%ld\n", (long)  sb.st_uid, (long)  sb.st_gid);
printf("I/O  block  size:\t %ld  bytes\n", (long)  sb.st_blksize);
printf("dimensione del file:\t%ld  bytes\n", (long) sb.st_size);
printf("Blocchi allocati: \t%ld\n”,(long )  sb.st_blocks); 

// modifica bit di protezione
int chmod (char *pathname , char *newmode);

// modifica proprietario e gruppo
// owner: UID nuovo proprietario , group: GID nuovo gruppo
int chown (char *pathname , int owner , int group);

// gestione direttori
#include <dirent. h>
//cambio di direttorio (ritorna -1 se fallisce)
int chdir(char *dirname);

//aprire cartella (NULL se fallisce)
DIR *opendir (char *nomedir);

//chiude cartella (ritorna -1 se fallisce)
int closedir (DIR *dir);

//lettura di un direttorio (ritorna -1 se fallisce)
#include <sys/types.h>
#include <dirent.h>
struct dirent *descr;
descr = readdir (DIR *dir);

struct  dirent{
		long  d_ino;  /*  inumber  */
		off_t  d_off; /* offset del prossimo */
	unsigned  short  d_reclen; /* lunghezza del record */
	unsigned  short  d_namelen; /* lunghezza del nome */
	char  *d_name;  /*  nome del file */
}

//crea cartella (ritorna -1 se fallisce)
int mkdir(char *pathname , int mode);