/* string.c - common string routines */

/*
 * Copyright (c) 2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#define _MLIBC_RESTRICT

/**
 *
 * @brief Copy a string
 *
 * @return pointer to destination buffer <d>
 */

char *strcpy(char *_MLIBC_RESTRICT d, const char *_MLIBC_RESTRICT s)
{
	char *dest = d;

	while (*s != '\0') {
		*d = *s;
		d++;
		s++;
	}

	*d = '\0';

	return dest;
}

/**
 *
 * @brief Copy part of a string
 *
 * @return pointer to destination buffer <d>
 */

char *strncpy(char *_MLIBC_RESTRICT d, const char *_MLIBC_RESTRICT s, size_t n)
{
	char *dest = d;

	while ((n > 0) && *s != '\0') {
		*d = *s;
		s++;
		d++;
		n--;
	}

	while (n > 0) {
		*d = '\0';
		d++;
		n--;
	}

	return dest;
}

/**
 *
 * @brief String scanning operation
 *
 * @return pointer to 1st instance of found byte, or NULL if not found
 */

char *strchr(const char *s, int c)
{
	char tmp = (char) c;

	while ((*s != tmp) && (*s != '\0'))
		s++;

	return (*s == tmp) ? (char *) s : NULL;
}

/**
 *
 * @brief String scanning operation
 *
 * @return pointer to last instance of found byte, or NULL if not found
 */

char *strrchr(const char *s, int c)
{
	char *match = NULL;

	do {
		if (*s == (char)c) {
			match = (char *)s;
		}
	} while (*s++);

	return match;
}

/**
 *
 * @brief Get string length
 *
 * @return number of bytes in string <s>
 */

size_t strlen(const char *s)
{
	size_t n = 0;

	while (*s != '\0') {
		s++;
		n++;
	}

	return n;
}

/**
 *
 * @brief Compare two strings
 *
 * @return negative # if <s1> < <s2>, 0 if <s1> == <s2>, else positive #
 */

int strcmp(const char *s1, const char *s2)
{
	while ((*s1 == *s2) && (*s1 != '\0')) {
		s1++;
		s2++;
	}

	return *s1 - *s2;
}

/**
 *
 * @brief Compare part of two strings
 *
 * @return negative # if <s1> < <s2>, 0 if <s1> == <s2>, else positive #
 */

int strncmp(const char *s1, const char *s2, size_t n)
{
	while ((n > 0) && (*s1 == *s2) && (*s1 != '\0')) {
		s1++;
		s2++;
		n--;
	}

	return (n == 0) ? 0 : (*s1 - *s2);
}

char *strcat(char *_MLIBC_RESTRICT dest, const char *_MLIBC_RESTRICT src)
{
	strcpy(dest + strlen(dest), src);
	return dest;
}

char *strncat(char *_MLIBC_RESTRICT dest, const char *_MLIBC_RESTRICT src,
	      size_t n)
{
	char *orig_dest = dest;
	size_t len = strlen(dest);

	dest += len;
	while ((n-- > 0) && (*src != '\0')) {
		*dest++ = *src++;
	}
	*dest = '\0';

	return orig_dest;
}

/**
 *
 * @brief Compare two memory areas
 *
 * @return negative # if <m1> < <m2>, 0 if <m1> == <m2>, else positive #
 */
int memcmp(const void *m1, const void *m2, size_t n)
{
	const char *c1 = m1;
	const char *c2 = m2;

	if (!n)
		return 0;

	while ((--n > 0) && (*c1 == *c2)) {
		c1++;
		c2++;
	}

	return *c1 - *c2;
}

/**
 *
 * @brief Copy bytes in memory with overlapping areas
 *
 * @return pointer to destination buffer <d>
 */

void *memmove(void *d, const void *s, size_t n)
{
	char *dest = d;
	const char *src  = s;

	if ((size_t) (dest - src) < n) {
		/*
		 * The <src> buffer overlaps with the start of the <dest> buffer.
		 * Copy backwards to prevent the premature corruption of <src>.
		 */

		while (n > 0) {
			n--;
			dest[n] = src[n];
		}
	} else {
		/* It is safe to perform a forward-copy */
		while (n > 0) {
			*dest = *src;
			dest++;
			src++;
			n--;
		}
	}

	return d;
}

/**
 *
 * @brief Copy bytes in memory
 *
 * @return pointer to start of destination buffer
 */

void *memcpy(void *_MLIBC_RESTRICT d, const void *_MLIBC_RESTRICT s, size_t n)
{
	/* attempt word-sized copying only if buffers have identical alignment */

	unsigned char *d_byte = (unsigned char *)d;
	const unsigned char *s_byte = (const unsigned char *)s;

	if ((((unsigned int)d ^ (unsigned int)s_byte) & 0x3) == 0) {

		/* do byte-sized copying until word-aligned or finished */

		while (((unsigned int)d_byte) & 0x3) {
			if (n == 0) {
				return d;
			}
			*(d_byte++) = *(s_byte++);
			n--;
		};

		/* do word-sized copying as long as possible */

		unsigned int *d_word = (unsigned int *)d_byte;
		const unsigned int *s_word = (const unsigned int *)s_byte;

		while (n >= sizeof(unsigned int)) {
			*(d_word++) = *(s_word++);
			n -= sizeof(unsigned int);
		}

		d_byte = (unsigned char *)d_word;
		s_byte = (unsigned char *)s_word;
	}

	/* do byte-sized copying until finished */

	while (n > 0) {
		*(d_byte++) = *(s_byte++);
		n--;
	}

	return d;
}

/**
 *
 * @brief Set bytes in memory
 *
 * @return pointer to start of buffer
 */

void *memset(void *buf, int c, size_t n)
{
	/* do byte-sized initialization until word-aligned or finished */

	unsigned char *d_byte = (unsigned char *)buf;
	unsigned char c_byte = (unsigned char)c;

	while (((unsigned int)d_byte) & 0x3) {
		if (n == 0) {
			return buf;
		}
		*(d_byte++) = c_byte;
		n--;
	};

	/* do word-sized initialization as long as possible */

	unsigned int *d_word = (unsigned int *)d_byte;
	unsigned int c_word = (unsigned int)(unsigned char)c;

	c_word |= c_word << 8;
	c_word |= c_word << 16;

	while (n >= sizeof(unsigned int)) {
		*(d_word++) = c_word;
		n -= sizeof(unsigned int);
	}

	/* do byte-sized initialization until finished */

	d_byte = (unsigned char *)d_word;

	while (n > 0) {
		*(d_byte++) = c_byte;
		n--;
	}

	return buf;
}

/**
 *
 * @brief Scan byte in memory
 *
 * @return pointer to start of found byte
 */

void *memchr(const void *s, int c, size_t n)
{
	if (n != 0) {
		const unsigned char *p = s;

		do {
			if (*p++ == (unsigned char)c) {
				return ((void *)(p - 1));
			}

		} while (--n != 0);
	}

	return NULL;
}

/*
 * Find the first occurrence of find in s.
 */
char *
strstr(const char *s, const char *find)
{
	char c, sc;
	size_t len;

	c = *find++;
	if (c != 0) {
		len = strlen(find);
		do {
			do {
				sc = *s++;
				if (sc == 0)
				return NULL;
			} while (sc != c);
		} while (strncmp(s, find, len) != 0);
	s--;
	}
	return (char *)s;
}

int
strncasecmp(const char *s1, const char *s2, size_t n)
{
	unsigned char c = 1;

	for (; c && n != 0; n--) {
		unsigned char lower1, lower2;

		c = *s1++;
		lower1 = tolower(c);
		lower2 = tolower(*s2++);

		if (lower1 != lower2) {
			return (lower1 > lower2) - (lower1 < lower2);
		}
	}

	return 0;
}

/*
 * Convert a string to a long integer.
 *
 * Ignores `locale' stuff.  Assumes that the upper and lower case
 * alphabets and digits are each contiguous.
 */
long strtol(const char *nptr, char **endptr, register int base)
{
	register const char *s = nptr;
	register unsigned long acc;
	register int c;
	register unsigned long cutoff;
	register int neg = 0, any, cutlim;

	/*
	 * Skip white space and pick up leading +/- sign if any.
	 * If base is 0, allow 0x for hex and 0 for octal, else
	 * assume decimal; if base is already 16, allow 0x.
	 */
	do {
		c = *s++;
	} while (isspace(c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else if (c == '+')
		c = *s++;
	if ((base == 0 || base == 16) &&
	    c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0) {
		base = c == '0' ? 8 : 10;
	}

	/*
	 * Compute the cutoff value between legal numbers and illegal
	 * numbers.  That is the largest legal value, divided by the
	 * base.  An input number that is greater than this value, if
	 * followed by a legal input character, is too big.  One that
	 * is equal to this value may be valid or not; the limit
	 * between valid and invalid numbers is then based on the last
	 * digit.  For instance, if the range for longs is
	 * [-2147483648..2147483647] and the input base is 10,
	 * cutoff will be set to 214748364 and cutlim to either
	 * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
	 * a value > 214748364, or equal but the next digit is > 7 (or 8),
	 * the number is too big, and we will return a range error.
	 *
	 * Set any if any `digits' consumed; make it negative to indicate
	 * overflow.
	 */
	cutoff = neg ? -(unsigned long)LONG_MIN : LONG_MAX;
	cutlim = cutoff % (unsigned long)base;
	cutoff /= (unsigned long)base;
	for (acc = 0, any = 0;; c = *s++) {
		if (isdigit(c)) {
			c -= '0';
		} else if (isalpha(c)) {
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		} else {
			break;
		}
		if (c >= base) {
			break;
		}
		if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) {
			any = -1;
		} else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	if (any < 0) {
		acc = neg ? LONG_MIN : LONG_MAX;
		errno = ERANGE;
	} else if (neg)
		acc = -acc;
	if (endptr != NULL) {
		*endptr = (char *)(any ? s - 1 : nptr);
	}
	return acc;
}

/*
 * Convert a string to an unsigned long integer.
 *
 * Ignores `locale' stuff.  Assumes that the upper and lower case
 * alphabets and digits are each contiguous.
 */
unsigned long strtoul(const char *nptr, char **endptr, register int base)
{
	register const char *s = nptr;
	register unsigned long acc;
	register int c;
	register unsigned long cutoff;
	register int neg = 0, any, cutlim;

	/*
	 * See strtol for comments as to the logic used.
	 */
	do {
		c = *s++;
	} while (isspace(c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else if (c == '+')
		c = *s++;
	if ((base == 0 || base == 16) &&
	    c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0) {
		base = c == '0' ? 8 : 10;
	}
	cutoff = (unsigned long)ULONG_MAX / (unsigned long)base;
	cutlim = (unsigned long)ULONG_MAX % (unsigned long)base;
	for (acc = 0, any = 0;; c = *s++) {
		if (isdigit(c)) {
			c -= '0';
		} else if (isalpha(c)) {
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		} else {
			break;
		}
		if (c >= base) {
			break;
		}
		if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) {
			any = -1;
		} else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	if (any < 0) {
		acc = ULONG_MAX;
		errno = ERANGE;
	} else if (neg) {
		acc = -acc;
	}
	if (endptr != NULL) {
		*endptr = (char *)(any ? s - 1 : nptr);
	}
	return acc;
}

int atoi(const char *s)
{
	int n = 0;
	int neg = 0;

	while (isspace(*s)) {
		s++;
	}
	switch (*s) {
	case '-':
		neg = 1;
		s++;
		break;	/* artifact to quiet coverity warning */
	case '+':
		s++;
	}
	/* Compute n as a negative number to avoid overflow on INT_MIN */
	while (isdigit(*s)) {
		n = 10*n - (*s++ - '0');
	}
	return neg ? n : -n;
}
