//
// Created by jonathan on 9/24/24.
//

#ifndef COMP_3980_ASSIGN_1_FILTER_H
#define COMP_3980_ASSIGN_1_FILTER_H

typedef char (*filter_func)(char);

char upper_filter(char c);

char lower_filter(char c);

char null_filter(char c);

#endif    // COMP_3980_ASSIGN_1_FILTER_H
