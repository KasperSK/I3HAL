 #include <stdio.h>
 #include <unistd.h>
 #include <fcntl.h>
 #include <errno.h>
 
 int main(void){
    char var, temp;
    int fd, w, c;
    char *v = &var;
    
    printf("Write 1 to turn LED3 on and write 0 to turn LED3 off\n");
    scanf(" %c", &var);
    
    fd = open("/sys/class/leds/led3/brightness", O_WRONLY);
    
    w = write( fd, v , 1);
    
    c = close(fd);
    
 }