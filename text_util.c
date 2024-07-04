#include "text_util.h"

char *trim_whitespace(char *str) {

    char *ptr_leading;
    char *ptr_trailing;

    if (str == NULL || *str == '\0') return NULL;

    ptr_leading = str;
    ptr_trailing = str + strlen(str) - 1;

    /* As long is the first character is a space, move forward */
    while (isspace(*ptr_leading))
        ptr_leading++;

    /* At this point, ptr points to the first non space character in the string
         If that character is the end of the string, return it.
    */

    if (*ptr_leading == '\0') return ptr_leading;

    /* Now we trim the trailing whitespace
         The last character in the array besides the null terminator is located at index 'strlen(str) -2'
    */

    while (isspace(*ptr_trailing)) {
        *ptr_trailing = '\0';
        ptr_trailing--;
    }

    return ptr_leading;

}