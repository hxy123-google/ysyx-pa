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
  int len=0;
  char buf[128];
  while(*fmt!='\0'){
    if(*fmt=='%'){
      fmt++;
      switch(*fmt){
        case 'd':
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
        fmt++;
        break;
        case 's':
          char* s = va_arg(ap, char*);
				  for(int i = 0; s[i] != '\0'; i++) 
						out[len++] = s[i];
          fmt++;
          break;
        case 'u':
						int unum32_len = 0;
						uint32_t unum32 = va_arg(ap, uint32_t);
						if(unum32 == 0) 
							out[len++] = '0';
						else {
							while(unum32 > 0) {
								buf[unum32_len++] = '0' + (unum32 % 10);
								unum32 /= 10;
							}
						}
						for(int i = unum32_len - 1; i >= 0; i--)
							out[len++] = buf[i];
            fmt++;
						break;
					case 'c':
						char c = (char)va_arg(ap, int);
						out[len++] = c;
            fmt++;
						break;
      }
      
    }else{
       out[len++]=*fmt++;
    }
  }
  out[len]='\0';
  return len;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
	va_start(ap, fmt);
	int res = vsprintf(out, fmt, ap);
	va_end(ap);
	return res;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
