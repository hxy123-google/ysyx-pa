#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  if(s==NULL) return 0;
  size_t len=0;
  while(*s!='\0'){
    s++;
    len++;
  }
  return len;
}

char *strcpy(char *dst, const char *src) {
  if (src == NULL || dst == NULL) {   
    return dst;
  }
  char * first=dst;
  while(*src!='\0'){
    *dst=*src;
    dst++;
    src++;
  }
  *dst='\0';
  return first;
}

char *strncpy(char *dst, const char *src, size_t n) {
  panic("Not implemented");
}

char *strcat(char *dst, const char *src) {
  char * first=dst;
  while(*dst!='\0') dst++;
  strcpy(dst,src);
  return first;
}

int strcmp(const char *s1, const char *s2) {
  while(*s1!='\0'&&*s2!='\0'&&*s1==*s2){
    s1++;
    s2++;
  }
  if(*s1=='\0'&&*s2!='\0') return 1;
  if(*s1!='\0'&&*s2=='\0') return -1;
  if(*s1>*s2) return 1;
  if(*s1<*s2) return -1;
   return 0;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  while(n--)
	{
		if(*s1 > *s2)
			return 1;
		if(*s1 < *s2)
			return -1;
		s1++;
		s2++;
	}
	return 0;
}

void *memset(void *s, int c, size_t n) {
  char *ch = (char *) s;
  while(n-- > 0)
	  *ch++ = c;
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  if(dst < src)
  {
	  char *d = (char *) dst;
	  char *s = (char *) src;
	  while(n--)
	  {
		  *d = *s;
		  d++;
		  s++;
	  }
  }
  else
  {
	  char *d = (char *) dst + n - 1;
	  char *s = (char *) src + n - 1;
	  while(n--)
	  {
		  *d = *s;
		  d--;
		  s--;
	  }
  }
  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  char *d = (char *) out;
  char *s = (char *) in;
  while(n--)
  {
	  *d = *s;
	  d++;
	  s++;
  }
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  char * S1 = (char *)s1;
  char * S2 = (char *)s2;
  while(n--)
  {
	  if(*S1 > *S2)
		  return 1;
	  if(*S1 < *S2)
		  return -1;
	  S1++;
	  S2++;
  }
  return 0;
}

#endif
