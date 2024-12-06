#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static char NUM_CHAR[] = "0123456789ABCDEF";
int printf(const char *fmt, ...) {
  va_list ap;
  va_start(ap,fmt);

  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  int len=0;
  char buf[128];
  va_list ap;
  va_start(ap,fmt);
  while(*fmt!='\0'){
    if(*fmt=='%'){
      fmt++;
      if(*fmt=='d'){
        int num_len=0;
        int q=va_arg(ap,int);
        if(q==0) out[len++]='0';
        if(q<0) out[len++]='-';
        for(;q!=0;q=q/10){
          buf[num_len++]=NUM_CHAR[q%10];
        }
        for(int i=num_len-1;i>=0;i--){
          out[len++]=buf[i];
        }
      }
      else if(*fmt=='s'){
        char* s = va_arg(ap, char*);
				for(int i = 0; s[i] != '\0'; i++) 
						out[len++] = s[i];
      }
      fmt++;
    }else{
       out[len++]=*fmt++;
    }
  }
  out[len]='\0';
  return len;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
