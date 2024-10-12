//
// Created by jonathan on 9/24/24.
//

#include "../include/filter.h"

typedef char (*filter_func)(char);

char upper_filter(char c)
{
    if(c >= 'a' && c <= 'z')
    {
        return (char)(c - ('a' - 'A'));
    }
    return c;
}

char lower_filter(char c)
{
    if(c >= 'A' && c <= 'Z')
    {
        return (char)(c + ('a' - 'A'));
    }
    return c;
}

char null_filter(char c)
{
    return c;
}
